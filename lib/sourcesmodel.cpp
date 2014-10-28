/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "sourcesmodel.h"

#include <KConfig>
#include <KDirWatch>
#include <KSharedConfig>

#include <QModelIndex>
#include <QSet>

using namespace Milou;

SourcesModel::SourcesModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_size(0)
{
    m_manager = new Plasma::RunnerManager(this);
    connect(m_manager, SIGNAL(matchesChanged(QList<Plasma::QueryMatch>)),
            this, SLOT(slotMatchesChanged(QList<Plasma::QueryMatch>)));

    KDirWatch* watch = KDirWatch::self();
    connect(watch, SIGNAL(created(QString)), this, SLOT(reloadConfiguration()));
    connect(watch, SIGNAL(dirty(QString)), this, SLOT(reloadConfiguration()));
    watch->addFile(QStandardPaths::locate(QStandardPaths::ConfigLocation, "krunnerrc"));

    m_resetTimer.setSingleShot(true);
    m_resetTimer.setInterval(500);
    connect(&m_resetTimer, SIGNAL(timeout()), this, SLOT(slotResetTimeout()));
}

SourcesModel::~SourcesModel()
{
}

QHash<int, QByteArray> SourcesModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles.insert(TypeRole, "type");
    roles.insert(SubtextRole, "subtext");
    roles.insert(PreviewTypeRole, "previewType");
    roles.insert(PreviewUrlRole, "previewUrl");
    roles.insert(PreviewLabelRole, "previewLabel");

    return roles;
}

Plasma::QueryMatch SourcesModel::fetchMatch(int row) const
{
    foreach (const QString& type, m_types) {
        const TypeData data = m_matches.value(type);
        if (row < data.shown.size()) {
            return data.shown[row];
        }
        else {
            row -= data.shown.size();
            if (row < 0) {
                break;
            }
        }
    }

    return Plasma::QueryMatch(0);
}

QVariant SourcesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_size)
        return QVariant();

    Plasma::QueryMatch m = fetchMatch(index.row());
    Q_ASSERT(m.runner());

    switch(role) {
        case Qt::DisplayRole:
            return m.text();

        case Qt::DecorationRole:
            return m.icon();

        case TypeRole:
            return m.matchCategory();

        case SubtextRole: {
            if (m_duplicates.value(m.text()) > 1) {
                return m.subtext();
            } else {
                return QString();
            }
        }

            /*
        case PreviewTypeRole:
            return m.previewType();

        case PreviewUrlRole:
            return m.previewUrl();

        case PreviewLabelRole:
            return m.previewLabel();
            */
    }

    return QVariant();
}

int SourcesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_size;
}

QString SourcesModel::queryString() const
{
    return m_queryString;
}

int SourcesModel::queryLimit() const
{
    return m_queryLimit;
}

QString SourcesModel::runner() const
{
    return m_runner;
}

void SourcesModel::setRunner(const QString& runner)
{
    m_runner = runner;
}

void SourcesModel::setQueryLimit(int limit)
{
    m_queryLimit = limit;
    /*
    foreach (AbstractSource* source, m_sources)
        source->setQueryLimit(limit);
    */
}

void SourcesModel::setQueryString(const QString& str)
{
    if (str.trimmed() == m_queryString.trimmed()) {
        return;
    }

    m_queryString = str;
    if (m_queryString.isEmpty()) {
        clear();
        return;
    }

    m_modelPopulated = false;
    m_manager->setSingleModeRunnerId(m_runner);
    m_manager->setSingleMode(!m_runner.isEmpty());
    m_manager->launchQuery(m_queryString, m_runner);

    // We avoid clearing the model instantly, and instead wait for the results
    // to show up, and only then do we clear the model. In the event
    // where there are no results, we wait for a predefined time before
    // clearing the model
    m_resetTimer.start();
}

void SourcesModel::slotResetTimeout()
{
    if (!m_modelPopulated) {
        clear();
    }
}

void SourcesModel::slotMatchesChanged(const QList<Plasma::QueryMatch>& l)
{
    beginResetModel();
    m_matches.clear();
    m_size = 0;
    m_types.clear();
    m_duplicates.clear();

    QList<Plasma::QueryMatch> list(l);
    qSort(list);

    QListIterator<Plasma::QueryMatch> iter(list);
    iter.toBack();

    while (iter.hasPrevious()) {
        const Plasma::QueryMatch match = iter.previous();
        slotMatchAdded(match);
    }

    // Sort the result types. We give the results which contain the query
    // text in the user visible string a higher preference than the ones
    // that do not
    // The rest are given the same preference as given by the runners.
    QSet<QString> higherTypes;
    for (QString type: m_types) {
        const TypeData td = m_matches.value(type);
        for (const Plasma::QueryMatch& match : td.shown) {
            if (match.text().contains(m_queryString, Qt::CaseInsensitive)) {
                QString matchType = match.matchCategory();
                higherTypes << matchType;
            }
        }
    }

    auto sortFunc = [&](const QString& l, const QString& r) {
        bool lHigher = higherTypes.contains(l);
        bool rHigher = higherTypes.contains(r);

        if (lHigher == rHigher) {
            return false;
        }
        else {
            return lHigher;
        }
    };
    qStableSort(m_types.begin(), m_types.end(), sortFunc);

    m_modelPopulated = true;
    endResetModel();
}

//
// Tries to make sure that all the types have the same number
// of visible items
//
void SourcesModel::slotMatchAdded(const Plasma::QueryMatch& m)
{
    if (m_queryString.isEmpty())
        return;

    QString matchType = m.matchCategory();

    if (!m_types.contains(matchType)) {
        m_types << matchType;
    }

    if (m_size == m_queryLimit) {
        int maxShownItems = 0;
        QString maxShownType;
        foreach (const QString& type, m_types) {
            TypeData data = m_matches.value(type);
            if (data.shown.size() >= maxShownItems) {
                maxShownItems = data.shown.size();
                maxShownType = type;
            }
        }

        if (maxShownType == matchType) {
            m_matches[matchType].hidden.append(m);
            return;
        }

        // Remove the last shown row from maxShownType
        // and add it to matchType
        int removeRowPos = fetchRowCount(maxShownType);
        removeRowPos += m_matches[maxShownType].shown.size() - 1;

        //beginRemoveRows(QModelIndex(), removeRowPos, removeRowPos);
        Plasma::QueryMatch transferMatch = m_matches[maxShownType].shown.takeLast();
        m_matches[maxShownType].hidden.append(transferMatch);
        m_size--;
        m_duplicates[transferMatch.text()]--;
        //endRemoveRows();

        int insertPos = fetchRowCount(matchType) + m_matches[matchType].shown.size();
        //beginInsertRows(QModelIndex(), insertPos, insertPos);
        m_matches[matchType].shown.append(m);
        m_size++;
        m_duplicates[m.text()]++;
        //endInsertRows();
    }
    else {
        int pos = 0;
        foreach (const QString& type, m_types) {
            pos += m_matches.value(type).shown.size();
            if (type == matchType) {
                break;
            }
        }

        //beginInsertRows(QModelIndex(), pos, pos);
        m_matches[matchType].shown.append(m);
        m_size++;
        m_duplicates[m.text()]++;
        //endInsertRows();
    }
}

int SourcesModel::fetchRowCount(const QString& type) const
{
    int c = 0;
    foreach (const QString& t, m_types) {
        if (t == type)
            break;

        c += m_matches.value(t).shown.size();
    }

    return c;
}

void SourcesModel::clear()
{
    beginResetModel();
    m_matches.clear();
    m_size = 0;
    m_duplicates.clear();
    m_queryString.clear();
    m_manager->reset();
    endResetModel();
}

void SourcesModel::run(int index)
{
    Plasma::QueryMatch match = fetchMatch(index);
    m_manager->run(match);
}

void SourcesModel::reloadConfiguration()
{
    KSharedConfig::openConfig("krunnerrc")->reparseConfiguration();
    m_manager->reloadConfiguration();
}

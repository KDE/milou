/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#include "sourcesmodel.h"

#include <KConfig>
#include <KDirWatch>
#include <KSharedConfig>

#include <QAction>
#include <QMimeData>
#include <QModelIndex>
#include <QSet>

using namespace Milou;

SourcesModel::SourcesModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_size(0)
{
    m_manager = new Plasma::RunnerManager(this);
    connect(m_manager, &Plasma::RunnerManager::matchesChanged, this, &SourcesModel::slotMatchesChanged);

    KDirWatch *watch = KDirWatch::self();
    connect(watch, &KDirWatch::created, this, &SourcesModel::slotSettingsFileChanged);
    connect(watch, &KDirWatch::dirty, this, &SourcesModel::slotSettingsFileChanged);
    watch->addFile(QStandardPaths::locate(QStandardPaths::ConfigLocation, QStringLiteral("krunnerrc")));

    m_resetTimer.setSingleShot(true);
    m_resetTimer.setInterval(500);
    connect(&m_resetTimer, &QTimer::timeout, this, &SourcesModel::slotResetTimeout);
}

SourcesModel::~SourcesModel()
{
}

QHash<int, QByteArray> SourcesModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles.insert(TypeRole, "type");
    roles.insert(SubtextRole, "subtext");
    roles.insert(ActionsRole, "actions");
    roles.insert(DuplicateRole, "isDuplicate");
    roles.insert(PreviewTypeRole, "previewType");
    roles.insert(PreviewUrlRole, "previewUrl");
    roles.insert(PreviewLabelRole, "previewLabel");

    return roles;
}

Plasma::QueryMatch SourcesModel::fetchMatch(int row) const
{
    for (const QString &type : qAsConst(m_types)) {
        const TypeData data = m_matches.value(type);
        if (row < data.shown.size()) {
            return data.shown[row];
        } else {
            row -= data.shown.size();
            if (row < 0) {
                break;
            }
        }
    }

    return Plasma::QueryMatch(nullptr);
}

QVariant SourcesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_size) {
        return QVariant();
    }

    Plasma::QueryMatch m = fetchMatch(index.row());
    Q_ASSERT(m.runner());

    switch (role) {
    case Qt::DisplayRole:
        return m.text();

    case Qt::DecorationRole:
        if (!m.iconName().isEmpty()) {
            return m.iconName();
        }

        return m.icon();

    case TypeRole:
        return m.matchCategory();

    case SubtextRole:
        return m.subtext();

    case ActionsRole: {
        const auto &actions = m_manager->actionsForMatch(m);
        if (actions.isEmpty()) {
            return QVariantList();
        }

        QVariantList actionsList;
        actionsList.reserve(actions.size());

        for (QAction *action : actions) {
            actionsList.append(QVariant::fromValue(action));
        }

        return actionsList;
    }
    case DuplicateRole:
        return m_duplicates.value(m.text());

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

int SourcesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

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

#if KRUNNER_BUILD_DEPRECATED_SINCE(5, 81)
QString SourcesModel::runner() const
{
    return m_runner;
}

void SourcesModel::setRunner(const QString &runner)
{
    if (m_runner != runner) {
        m_runner = runner;

        m_manager->setSingleModeRunnerId(m_runner);
        m_manager->setSingleMode(!m_runner.isEmpty());

        Q_EMIT runnerChanged();
    }
}

QString SourcesModel::runnerName() const
{
    auto *singleRunner = m_manager->singleModeRunner();
    if (!singleRunner) {
        return QString();
    }

    return singleRunner->name();
}

QIcon SourcesModel::runnerIcon() const
{
    auto *singleRunner = m_manager->singleModeRunner();
    if (!singleRunner) {
        return QIcon();
    }

    return singleRunner->icon();
}
#endif

void SourcesModel::setQueryLimit(int limit)
{
    m_queryLimit = limit;
    /*
    foreach (AbstractSource* source, m_sources)
        source->setQueryLimit(limit);
    */
}

void SourcesModel::setQueryString(const QString &str)
{
    if (str.trimmed() == m_queryString.trimmed()) {
        return;
    }

    m_queryString = str;
    if (m_queryString.isEmpty()) {
        clear();
        return;
    }

    // We avoid clearing the model instantly, and instead wait for the results
    // to show up, and only then do we clear the model. In the event
    // where there are no results, we wait for a predefined time before
    // clearing the model
    m_resetTimer.start();

    m_modelPopulated = false;
    m_manager->launchQuery(m_queryString, m_runner);
}

void SourcesModel::slotResetTimeout()
{
    if (!m_modelPopulated) {
        // The old items are still shown, get rid of them
        beginResetModel();
        m_matches.clear();
        m_size = 0;
        m_duplicates.clear();
        endResetModel();
    }
}

void SourcesModel::slotMatchesChanged(const QList<Plasma::QueryMatch> &l)
{
    // We do reset handling ourselves, so ignore clears if the reset timer
    // is supposed to handle them (see setQueryString)
    if (l.length() == 0 && m_resetTimer.isActive() && !m_modelPopulated) {
        return;
    }

    beginResetModel();
    m_matches.clear();
    m_size = 0;
    m_types.clear();
    m_duplicates.clear();

    QList<Plasma::QueryMatch> list(l);
    std::sort(list.begin(), list.end());

    for (auto it = list.crbegin(), end = list.crend(); it != end; ++it) {
        slotMatchAdded(*it);
    }

    // Sort the result types. We give the results which contain the query
    // text in the user visible string a higher preference than the ones
    // that do not
    // The rest are given the same preference as given by the runners.
    const QString simplifiedQuery = m_queryString.simplified();
    const auto words = simplifiedQuery.splitRef(QLatin1Char(' '), Qt::SkipEmptyParts);

    QSet<QString> higherTypes;
    for (const QString &type : qAsConst(m_types)) {
        const TypeData td = m_matches.value(type);

        for (const Plasma::QueryMatch &match : td.shown) {
            const QString text = match.text().simplified();
            bool containsAll = true;

            for (const auto &word : words) {
                if (!text.contains(word, Qt::CaseInsensitive)) {
                    containsAll = false;
                    break;
                }
            }

            // Maybe we should be giving it a higher type based on the number of matched
            // words in the text?
            if (containsAll) {
                higherTypes << match.matchCategory();
            }
        }
    }

    auto sortFunc = [&](const QString &l, const QString &r) {
        bool lHigher = higherTypes.contains(l);
        bool rHigher = higherTypes.contains(r);

        if (lHigher == rHigher) {
            return false;
        } else {
            return lHigher;
        }
    };
    std::stable_sort(m_types.begin(), m_types.end(), sortFunc);

    m_modelPopulated = true;
    endResetModel();
}

//
// Tries to make sure that all the types have the same number
// of visible items
//
void SourcesModel::slotMatchAdded(const Plasma::QueryMatch &m)
{
    if (m_queryString.isEmpty()) {
        return;
    }

    QString matchType = m.matchCategory();

    if (!m_types.contains(matchType)) {
        m_types << matchType;
    }

    if (m_size == m_queryLimit) {
        int maxShownItems = 0;
        QString maxShownType;
        for (const QString &type : qAsConst(m_types)) {
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
        Plasma::QueryMatch transferMatch = m_matches[maxShownType].shown.takeLast();
        m_matches[maxShownType].hidden.append(transferMatch);
        m_size--;
        m_duplicates[transferMatch.text()]--;
    }

    m_matches[matchType].shown.append(m);
    m_size++;
    m_duplicates[m.text()]++;
}

void SourcesModel::slotSettingsFileChanged(const QString &path)
{
    if (!path.endsWith(QLatin1String("krunnerrc"))) {
        return;
    }

    reloadConfiguration();
}

void SourcesModel::clear()
{
    beginResetModel();
    m_matches.clear();
    m_size = 0;
    m_duplicates.clear();
    m_queryString.clear();
    m_manager->reset();
    m_manager->matchSessionComplete();
    endResetModel();
}

bool SourcesModel::run(int index)
{
    Plasma::QueryMatch match = fetchMatch(index);
    Q_ASSERT(match.runner());

    if (match.type() == Plasma::QueryMatch::InformationalMatch) {
        QString info = match.data().toString();
        int editPos = info.length();

        if (!info.isEmpty()) {
            // FIXME: pretty lame way to decide if this is a query prototype
            // Copied from kde4 krunner interface.cpp
            if (match.runner() == nullptr) {
                // lame way of checking to see if this is a Help Button generated match!
                int index = info.indexOf(QStringLiteral(":q:"));

                if (index != -1) {
                    editPos = index;
                    info.replace(QStringLiteral(":q:"), QString());
                }
            }

            Q_EMIT updateSearchTerm(info, editPos);
            return false;
        }
    }

    m_manager->run(match);
    return true;
}

bool SourcesModel::runAction(int index, int actionIndex)
{
    Plasma::QueryMatch match = fetchMatch(index);
    Q_ASSERT(match.runner());

    const auto &actions = m_manager->actionsForMatch(match);
    if (actionIndex < 0 || actionIndex >= actions.count()) {
        return false;
    }

    QAction *action = actions.at(actionIndex);
    match.setSelectedAction(action);
    m_manager->run(match);
    return true;
}

void SourcesModel::reloadConfiguration()
{
    KSharedConfig::openConfig(QStringLiteral("krunnerrc"))->reparseConfiguration();
    m_manager->reloadConfiguration();
}

QMimeData *SourcesModel::getMimeData(int index) const
{
    Plasma::QueryMatch match = fetchMatch(index);
    Q_ASSERT(match.runner());

    // we're returning a parent-less QObject from a Q_INVOKABLE
    // which means the QML engine will take care of deleting it eventually
    QMimeData *mimeData = m_manager->mimeDataForMatch(match);

    return mimeData;
}

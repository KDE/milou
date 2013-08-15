/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Vishesh Handa <me@vhanda.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "sourcesmodel.h"
#include "nepomuksource.h"
#include "plasmarunnersource.h"
#include "applicationsource.h"

#include <KDebug>

SourcesModel::SourcesModel(QObject* parent)
: QAbstractListModel(parent)
{
    PlasmaRunnerSource* plasmaRunnerSource = new PlasmaRunnerSource(this);
    connect(plasmaRunnerSource, SIGNAL(matchAdded(Match)), this, SLOT(slotMatchAdded(Match)));

    ApplicationSource* appSource = new ApplicationSource(this);
    connect(appSource, SIGNAL(matchAdded(Match)), this, SLOT(slotMatchAdded(Match)));

    AbstractSource* nepomukSource = new NepomukSource(this);
    connect(nepomukSource, SIGNAL(matchAdded(Match)), this, SLOT(slotMatchAdded(Match)));

    m_sources << appSource;
    m_sources << nepomukSource;
    m_sources << plasmaRunnerSource;

    QHash<int, QByteArray> roles = roleNames();
    roles.insert(TypeRole, "type");

    setRoleNames(roles);
    loadSettings();
}

SourcesModel::~SourcesModel()
{
}

void SourcesModel::loadSettings()
{
    QList<MatchType*> allTypes;
    foreach(AbstractSource* source, m_sources)
        allTypes << source->types();

    foreach(MatchType* type, allTypes)
        m_typeNameMap.insert(type, type->name());

    KConfig config("nepomukfinderrc");
    KConfigGroup generalGroup = config.group("General");
    int numTypes = generalGroup.readEntry("NumTypes", 0);

    if (numTypes != allTypes.size()) {
        generalGroup.writeEntry("NumTypes", allTypes.size());

        for(int i=0; i<allTypes.size(); i++) {
            MatchType* type = allTypes[i];
            KConfigGroup group = config.group("Type-" + QString::number(i));

            group.writeEntry("Name", type->name());
            group.writeEntry("Icon", type->icon());
            group.writeEntry("Enabled", type->shown());

            m_types << type->name();
        }
    }
    else {
        kDebug() << "Loading the settings";
        for(int i=0; i<allTypes.size(); i++) {
            MatchType* type = allTypes[i];
            m_types << type->name();

            KConfigGroup group = config.group("Type-" + QString::number(i));

            QString name = group.readEntry("Name", QString());
            bool shown = group.readEntry("Enabled", true);

            // Update allTypes
            foreach(MatchType* type, allTypes) {
                if (type->name() == name) {
                    kDebug() << type->name() << shown;
                    type->setShown(shown);
                }
            }
        }
    }
}

Match SourcesModel::fetchMatch(int row) const
{
    foreach(const QString& type, m_types) {
        TypeData data = m_matches.value(type);
        if (row < data.shown.size())
            return data.shown[row];
        else
            row -= data.shown.size();
    }

    return Match(0);
}

QVariant SourcesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_size)
        return QVariant();

    Match m = fetchMatch(index.row());
    switch(role) {
        case Qt::DisplayRole:
            return m.text();

        case Qt::DecorationRole:
            return m.icon();

        case TypeRole:
            return m_typeNameMap.value(m.type());
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

void SourcesModel::setQueryLimit(int limit)
{
    m_queryLimit = limit;
    foreach (AbstractSource* source, m_sources)
        source->setQueryLimit(limit);
}

void SourcesModel::setQueryString(const QString& str)
{
    if (str.trimmed() == m_queryString.trimmed()) {
        return;
    }

    clear();
    m_queryString = str;

    foreach (AbstractSource* source, m_sources) {
        source->query(str);
    }
}

//
// Tries to make sure that all the types have the same number
// of visible items
//
void SourcesModel::slotMatchAdded(const Match& m)
{
    if (m_queryString.isEmpty())
        return;

    const QString matchType = m_typeNameMap.value(m.type());
    //Q_ASSERT(m.source()->types().contains(matchType));

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

        beginRemoveRows(QModelIndex(), removeRowPos, removeRowPos);
        Match transferMatch = m_matches[maxShownType].shown.takeLast();
        m_matches[maxShownType].hidden.append(transferMatch);
        endRemoveRows();

        int insertPos = fetchRowCount(matchType) + m_matches[matchType].shown.size();
        beginInsertRows(QModelIndex(), insertPos, insertPos);
        m_matches[matchType].shown.append(m);
        endInsertRows();
    }
    else {
        int pos = 0;
        foreach (const QString& type, m_types) {
            pos += m_matches.value(type).shown.size();
            if (type == matchType) {
                break;
            }
        }

        beginInsertRows(QModelIndex(), pos, pos);
        m_matches[matchType].shown.append(m);
        m_size++;
        endInsertRows();
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
    m_queryString.clear();
    endResetModel();
}

void SourcesModel::run(int index)
{
    Match match = fetchMatch(index);
    AbstractSource* source = match.source();
    if (source) {
        source->run(match);
    }
}

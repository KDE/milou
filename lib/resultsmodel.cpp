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

#include "resultsmodel.h"
#include "queryrunnable.h"

#include <KDebug>
#include <KIcon>

#include <Soprano/QueryResultIterator>

#include <Nepomuk2/Query/FileQuery>
#include <Nepomuk2/Query/QueryParser>
#include <Nepomuk2/Query/LiteralTerm>
#include <Nepomuk2/Query/ResourceTypeTerm>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>

#include <Soprano/Model>
#include <Soprano/Vocabulary/NAO>

using namespace Nepomuk2;
using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

ResultsModel::ResultsModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_queryLimit(0)
{
    qRegisterMetaType<Query::Result>("Query::Result");

    QHash<int, QByteArray> roles = roleNames();
    roles.insert(UrlRole, "url");
    roles.insert(CreatedRole, "created");
    roles.insert(ModifiedRole, "modified");

    setRoleNames(roles);

    m_queryTypes << NFO::Audio();
    m_queryTypes << NFO::Image();
    m_queryTypes << NFO::Document();
    m_queryTypes << NFO::Video();
}

int ResultsModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_results.uniqueKeys().size();
    }

    if (parent.row() >= m_queryTypes.size() || parent.column() != 0) {
        return 0;
    }

    if (parent.internalId() != -1)
        return 0;

    const QUrl type = m_queryTypes[parent.row()];
    return m_results.value(type).size();
}

int ResultsModel::columnCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return 1;

    // Is this required?
    if (!hasIndex(parent.row(), parent.column(), parent.parent())) {
        return 0;
    }

    return 1;
}

QModelIndex ResultsModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column != 0) {
        return QModelIndex();
    }

    if (!parent.isValid()) {
        if (0 <= row && row < m_results.size()) {
            return createIndex(row, 0, -1);
        }
        return QModelIndex();
    }
    else {
        int parentRow = parent.row();
        int count = m_results.value(m_queryTypes[parentRow]).size();
        if (row >= count)
            return QModelIndex();

        return createIndex(row, 0, parentRow);
    }

}

QModelIndex ResultsModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    int parentRow = child.internalId();
    if (parentRow == -1)
        return QModelIndex();

    return createIndex(parentRow, 0, -1);
}

QVariant ResultsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // Top level labels
    if (!index.parent().isValid()) {
        int row = index.row();
        if (row < 0 || row >= m_results.size())
            return QVariant();

        // FIXME: Maybe this should have better labels?
        if( role == Qt::DisplayRole ) {
            return Types::Class(m_queryTypes[row]).label();
        }
        return QVariant();
    }

    else {
        const QUrl type = m_queryTypes.value(index.parent().row());
        const QList<Query::Result> resultList = m_results.value(type);
        const Query::Result result = resultList.at(index.row());
        const Resource res = result.resource();

        switch (role) {
        case Qt::DisplayRole:
            return res.genericLabel();

        case Qt::DecorationRole: {
            QString iconName = res.genericIcon();
            if( !iconName.isEmpty() ) {
                return KIcon( iconName );
            }
            else {
                QIcon icon = Types::Class(res.type()).icon();
                if( !icon.isNull() )
                    return icon;
                else
                    return QVariant();
            }
        }

        case UrlRole:
            return res.property(NIE::url()).toUrl();

        case ModifiedRole:
            return res.property(NIE::lastModified()).toDateTime();

        case CreatedRole:
            return res.property(NIE::created()).toDateTime();

        default:
            return QVariant();
        }
    }
}

QVariant ResultsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

QString ResultsModel::queryString()
{
    return m_queryString;
}

void ResultsModel::setQueryString(const QString& text)
{
    if (text.trimmed() == m_queryString.trimmed()) {
        return;
    }
    m_queryString = text;

    QHash<QueryRunnable*, QUrl>::iterator it = m_queryTypeMap.begin();
    for(; it != m_queryTypeMap.end(); it++) {
        it.key()->stop();
    }
    m_queryTypeMap.clear();

    beginResetModel();
    m_results.clear();
    endResetModel();

    if( text.length() < 4 ) {
        return;
    }

    kDebug() << text;

    QStringList strList = text.split(' ');
    QString searchString;
    foreach(const QString& str, strList) {
        searchString += str + "* ";
    }

    Query::LiteralTerm literalTerm( searchString );

    // Types
    foreach(const QUrl& type, m_queryTypes) {
        Query::ResourceTypeTerm typeTerm(type);

        Query::FileQuery query(literalTerm && typeTerm);
        query.setLimit(m_queryLimit);

        m_queryTypeMap.insert(newQueryTask(query), type);
    }
}

void ResultsModel::slotQueryResult(QueryRunnable* runnable, const Query::Result& result)
{
    QUrl type = m_queryTypeMap.value(runnable);
    int parentRow = m_queryTypes.indexOf(type);

    int insertPos = m_results.value(type).size();
    if (!insertPos) {
        beginInsertRows(QModelIndex(), m_results.size(), m_results.size());
        m_results.insert(type, QList<Query::Result>());
        endInsertRows();

        if (m_results.size() == 1) {
            emit listingStarted(m_queryString);
        }
    }

    beginInsertRows(createIndex(parentRow, 0, -1), insertPos, insertPos);
    m_results[type].append(result);
    endInsertRows();

    //kDebug() << (void*)runnable << type << result.resource().uri();
}

void ResultsModel::slotQueryFinished(QueryRunnable* runnable)
{
    //kDebug() << (void*)runnable;
    m_queryTypeMap.remove(runnable);

    if (m_queryTypeMap.isEmpty()) {
        emit listingFinished(m_queryString);
    }
}

int ResultsModel::queryLimit()
{
    return m_queryLimit;
}

void ResultsModel::setQueryLimit(int limit)
{
    m_queryLimit = limit;
}

QueryRunnable* ResultsModel::newQueryTask(const Query::Query& query)
{
    QueryRunnable* task = new QueryRunnable( query );
    connect(task, SIGNAL(queryResult(QueryRunnable*,Query::Result)),
            this, SLOT(slotQueryResult(QueryRunnable*,Query::Result)));
    connect(task, SIGNAL(finished(QueryRunnable*)), this, SLOT(slotQueryFinished(QueryRunnable*)));

    QThreadPool::globalInstance()->start(task);

    return task;
}

void ResultsModel::clear()
{
    beginResetModel();
    m_results.clear();
    m_queryTypeMap.clear();
    m_queryString.clear();
    endResetModel();
}




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

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Vocabulary/NIE>

#include <Soprano/Model>
#include <Soprano/Vocabulary/NAO>

using namespace Nepomuk2;
using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

ResultsModel::ResultsModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_queryTask(0)
    , m_queryLimit(0)
{
    qRegisterMetaType<Query::Result>("Query::Result");

    QHash<int, QByteArray> roles = roleNames();
    roles.insert(UrlRole, "url");

    setRoleNames(roles);
}

int ResultsModel::rowCount(const QModelIndex& parent) const
{
    return m_results.size();
}

QVariant ResultsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() > m_results.size())
        return QVariant();

    const Query::Result result = m_results.at(index.row());
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

QString ResultsModel::queryString()
{
    return m_queryString;
}

void ResultsModel::setQueryString(const QString& text)
{
    if( m_queryTask ) {
        m_queryTask->stop();
        m_queryTask = 0;
    }

    if( text.length() < 4 ) {
        beginResetModel();
        m_results.clear();
        endResetModel();
        return;
    }

    kDebug() << text;

    QStringList strList = text.split(' ');
    QString searchString;
    foreach(const QString& str, strList) {
        searchString += str + "* ";
    }

    Nepomuk2::Query::LiteralTerm literalTerm( searchString );
    Nepomuk2::Query::FileQuery query( literalTerm );
    query.setLimit( m_queryLimit );

    QString sparqlQuery = query.toFileQuery().toSparqlQuery();
    kDebug() << sparqlQuery;

    m_queryTask = new QueryRunnable( query );
    connect(m_queryTask, SIGNAL(queryResult(Query::Result)), this, SLOT(slotQueryResult(Query::Result)));
    connect(m_queryTask, SIGNAL(finished(QueryRunnable*)), this, SLOT(slotQueryFinished(QueryRunnable*)));

    QThreadPool::globalInstance()->start(m_queryTask);

    beginResetModel();
    m_results.clear();
    endResetModel();
}

void ResultsModel::slotQueryResult(const Query::Result& result)
{
    kDebug() << result.resource().uri();

    beginInsertRows(QModelIndex(), m_results.size(), m_results.size());
    m_results << result;
    endInsertRows();
}

void ResultsModel::slotQueryFinished(QueryRunnable* runnable)
{
    //Q_ASSERT(runnable == m_queryTask);
    m_queryTask = 0;
}

int ResultsModel::queryLimit()
{
    return m_queryLimit;
}

void ResultsModel::setQueryLimit(int limit)
{
    m_queryLimit = limit;
}


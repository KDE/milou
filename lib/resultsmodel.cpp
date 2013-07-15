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

using namespace Nepomuk2;
using namespace Nepomuk2::Vocabulary;

ResultsModel::ResultsModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_query(0)
    , m_queryLimit(0)
{
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
    if( m_query ) {
        m_query->close();
        delete m_query;
        m_query = 0;
    }

    if( text.length() < 4 ) {
        beginResetModel();
        m_results.clear();
        endResetModel();
        return;
    }

    kDebug() << text;

    Nepomuk2::Query::LiteralTerm literalTerm( text + "*" );
    Nepomuk2::Query::Query query( literalTerm );
    query.setLimit( m_queryLimit );

    QString sparqlQuery = query.toFileQuery().toSparqlQuery();
    kDebug() << sparqlQuery;

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    m_query = Soprano::Util::AsyncQuery::executeQuery(model, sparqlQuery, Soprano::Query::QueryLanguageSparql);

    connect(m_query, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)), this, SLOT(slotNextReady(Soprano::Util::AsyncQuery*)));
    connect(m_query, SIGNAL(finished(Soprano::Util::AsyncQuery*)), this, SLOT(slotFinished(Soprano::Util::AsyncQuery*)));

    beginResetModel();
    m_results.clear();
    endResetModel();
}

void ResultsModel::slotNextReady(Soprano::Util::AsyncQuery* query)
{
    if( query->next() ) {
        const QUrl uri = query->binding(0).uri();
        Query::Result result(Resource::fromResourceUri( uri ));

        beginInsertRows(QModelIndex(), m_results.size(), m_results.size());
        m_results << result;
        kDebug() << uri;
        endInsertRows();
    }
}

void ResultsModel::slotFinished(Soprano::Util::AsyncQuery* query)
{
    Q_ASSERT(m_query == query);
    m_query = 0;
}

int ResultsModel::queryLimit()
{
    return m_queryLimit;
}

void ResultsModel::setQueryLimit(int limit)
{
    m_queryLimit = limit;
}


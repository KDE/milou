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

#include "queryrunnable.h"

#include <Nepomuk2/Query/ResultIterator>
#include <Nepomuk2/Resource>

#include <QScopedPointer>

using namespace Nepomuk2;

QueryRunnable::QueryRunnable(const Query::Query& query)
    : m_query(query)
    , m_stop(false)
{
    qRegisterMetaType<Nepomuk2::Query::Result>("Nepomuk2::Query::Result");
}

QueryRunnable::QueryRunnable(const QString& query, const Query::RequestPropertyMap& map)
    : m_sparqlQuery(query)
    , m_requestPropMap(map)
    , m_stop(false)
{
    qRegisterMetaType<Nepomuk2::Query::Result>("Nepomuk2::Query::Result");
}

void QueryRunnable::run()
{
    QScopedPointer<Query::ResultIterator> it;

    if (!m_sparqlQuery.isEmpty())
        it.reset(new Query::ResultIterator(m_sparqlQuery, m_requestPropMap));
    else
        it.reset(new Query::ResultIterator(m_query));

    while (it->next() && !m_stop) {
        emit queryResult(this, it->current());
    }

    emit finished(this);
}

void QueryRunnable::stop()
{
    m_stop = true;
}

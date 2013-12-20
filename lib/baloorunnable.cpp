/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2013  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "baloorunnable.h"

#include <baloo/query.h>
#include <baloo/resultiterator.h>

using namespace Milou;

BalooRunnable::BalooRunnable(const QString& search,
                             const QHash<MatchType*, QString> matchTypeHash,
                             const QList<MatchType*>& types,
                             int limit,
                             QObject* parent)
    : QObject(parent)
    , m_searchString(search)
    , m_matchTypeHash(matchTypeHash)
    , m_types(types)
    , m_limit(limit)
    , m_stop(false)
{
    qRegisterMetaType<Baloo::Result>("Baloo::Result");
}

void BalooRunnable::run()
{
    Q_FOREACH (MatchType* type, m_types) {
        if (m_stop)
            return;

        Baloo::Query query;
        query.addType(m_matchTypeHash.value(type));
        query.setSearchString(m_searchString);
        query.setLimit(m_limit);

        Baloo::ResultIterator it = query.exec();
        while (it.next()) {
            if (m_stop)
                return;

            Q_EMIT queryResult(type, it.result());
        }
    }
}

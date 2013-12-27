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

#ifndef MILOU_CONTEXT_H
#define MILOU_CONTEXT_H

#include <QString>
#include <QList>

#include "match.h"

class Context {
public:
    Context();

    void setQuery(const QString& query);
    QString query() const;

    /**
     * Gives the list of types that the Source should take into consideration
     * when returning results.
     *
     * Results which are not one of these types will be ignored, so it is
     * best to not generate them
     */
    QList<MatchType*> types() const;
    void setTypes(const QList<MatchType*>& types);

private:
    QString m_query;
    QList<MatchType*> m_types;
};

#endif

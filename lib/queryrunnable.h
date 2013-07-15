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

#ifndef NEPOMUK2_QUERYRUNNABLE_H
#define NEPOMUK2_QUERYRUNNABLE_H

#include <QRunnable>
#include <Nepomuk2/Query/Query>
#include <Nepomuk2/Query/Result>

namespace Nepomuk2 {

class QueryRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    QueryRunnable(const Query::Query& query);
    virtual void run();

    void stop();

signals:
    void queryResult(const Query::Result& result);
    void finished(QueryRunnable* queryRunnable);

private:
    Query::Query m_query;
    bool m_stop;
};

}

#endif // NEPOMUK2_QUERYRUNNABLE_H

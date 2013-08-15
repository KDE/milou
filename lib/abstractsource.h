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

#ifndef ABSTRACTSOURCE_H
#define ABSTRACTSOURCE_H

#include <QObject>
#include <QUrl>

#include "match.h"

class AbstractSource : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSource(QObject* parent = 0);
    virtual ~AbstractSource();

    virtual void query(const QString& string) = 0;
    virtual void run(const Match& match);

    void setQueryLimit(int limit);
    int queryLimit();

    /**
     * Returns a list of types that the source can return. These
     * sources should checked before each query.
     */
    QList<MatchType*> types();
    void setTypes(const QList<MatchType*> types);

    /**
     * Stop the last running query. By default this function
     * does not do anything
     */
    virtual void stop();

protected:
    void addMatch(const Match& match);

signals:
    void matchAdded(const Match& match);

private:
    int m_queryLimit;
    QList<MatchType*> m_types;
};

#endif // ABSTRACTSOURCE_H

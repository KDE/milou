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

#ifndef BALOORUNNABLE_H
#define BALOORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include <baloo/result.h>
#include "abstractsource.h"

namespace Milou {

class BalooRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    BalooRunnable(const QString& search,
                  const QHash<MatchType*, QString> matchTypeHash,
                  const QList<MatchType*>& types,
                  int limit,
                  QObject* parent = 0);

    virtual void run();

    void stop() { m_stop = true; }

Q_SIGNALS:
    void queryResult(Milou::MatchType* type, const Baloo::Result& result);
    void queryFinished();

private:
    QString m_searchString;
    QHash<MatchType*, QString> m_matchTypeHash;
    QList<MatchType*> m_types;
    int m_limit;

    QAtomicInt m_stop;
};

}
#endif // BALOORUNNABLE_H

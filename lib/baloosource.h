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

#ifndef BALOOSOURCE_H
#define BALOOSOURCE_H

#include "abstractsource.h"
#include "baloorunnable.h"

#include <QThreadPool>
#include <baloo/query.h>

class BalooSource : public AbstractSource
{
    Q_OBJECT
public:
    explicit BalooSource(QObject* parent = 0);
    virtual ~BalooSource();

    virtual void query(const QString& string);
    virtual void run(const Match& match);
    virtual void stop();

public slots:
    void slotQueryResult(MatchType* type, const Baloo::Result& result);

private:
    Milou::BalooRunnable* m_runnable;
    QThreadPool* m_threadPool;

    MatchType* m_audioType;
    MatchType* m_videoType;
    MatchType* m_documentType;
    MatchType* m_imageType;
    MatchType* m_folderType;
    MatchType* m_emailType;

    QHash<MatchType*, QString> m_typeHash;

    Baloo::Query fetchQueryForType(const QString& text, MatchType* type);
};

#endif // BALOOSOURCE_H

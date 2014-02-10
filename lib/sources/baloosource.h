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

class BalooSource : public Milou::AbstractSource
{
    Q_OBJECT
public:
    explicit BalooSource(QObject* parent = 0);
    virtual ~BalooSource();

    virtual void query(const Milou::Context& context);
    virtual void run(const Milou::Match& match);
    virtual void stop();

public slots:
    void slotQueryResult(Milou::MatchType* type, const Baloo::Result& result);
    void slotQueryFinished();

private:
    Milou::BalooRunnable* m_runnable;
    QThreadPool* m_threadPool;

    Milou::MatchType* m_audioType;
    Milou::MatchType* m_videoType;
    Milou::MatchType* m_documentType;
    Milou::MatchType* m_imageType;
    Milou::MatchType* m_folderType;
    Milou::MatchType* m_emailType;

    QHash<Milou::MatchType*, QString> m_typeHash;

    Baloo::Query fetchQueryForType(const QString& text, Milou::MatchType* type);
};

#endif // BALOOSOURCE_H

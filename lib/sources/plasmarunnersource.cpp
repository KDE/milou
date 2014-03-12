/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "plasmarunnersource.h"
#include <KDebug>
#include <KLocalizedString>
#include <QIcon>
#include <kplugininfo.h>

PlasmaRunnerSource::PlasmaRunnerSource(QObject* parent): AbstractSource(parent)
{
    m_manager = new Plasma::RunnerManager(this);
    m_manager->setAllowedRunners(QStringList() << "bookmarks");

    connect(m_manager, SIGNAL(matchesChanged(QList<Plasma::QueryMatch>)),
            this, SLOT(slotMatchesChanged(QList<Plasma::QueryMatch>)));

    m_bookmarkType = new Milou::MatchType(i18n("Bookmarks"), "bookmarks");

    QList<Milou::MatchType*> types;
    types << m_bookmarkType;

    setTypes(types);
}

void PlasmaRunnerSource::query(const Milou::Context& context)
{
    qDeleteAll(m_mapping);
    m_mapping.clear();

    m_manager->launchQuery(context.query());
}

void PlasmaRunnerSource::slotMatchesChanged(const QList< Plasma::QueryMatch >& matches)
{
    foreach(const Plasma::QueryMatch& plasmaMatch, matches) {
        Milou::Match match(this);
        match.setText(plasmaMatch.text());
        match.setIcon(plasmaMatch.icon().name());
        match.setType(m_bookmarkType);
        // The Bookmark runner stores the url in the data as a string
        match.setPreviewUrl(plasmaMatch.data().toString());
        match.setPreviewType(QLatin1String("html"));

        uint id = qHash(plasmaMatch.text() + plasmaMatch.subtext());
        match.setData(id);

        addMatch(match);
        m_mapping.insert(id, new Plasma::QueryMatch(plasmaMatch));
    }
}

void PlasmaRunnerSource::run(const Milou::Match& match)
{
    uint id = match.data().toUInt();
    if (m_mapping.contains(id)) {
        Plasma::QueryMatch* plasmaMatch = m_mapping.value(id);
        m_manager->run(*plasmaMatch);
    }
}

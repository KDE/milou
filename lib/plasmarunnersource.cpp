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

#include "plasmarunnersource.h"
#include <KDebug>
#include <QIcon>
#include <kplugininfo.h>

PlasmaRunnerSource::PlasmaRunnerSource(QObject* parent): AbstractSource(parent)
{
    m_manager = new Plasma::RunnerManager(this);
    m_manager->setAllowedRunners(QStringList() << "services");

    connect(m_manager, SIGNAL(matchesChanged(QList<Plasma::QueryMatch>)),
            this, SLOT(slotMatchesChanged(QList<Plasma::QueryMatch>)));
}

QStringList PlasmaRunnerSource::types()
{
    return QStringList() << "Application";
}

void PlasmaRunnerSource::query(const QString& string)
{
    qDeleteAll(m_mapping.values());
    m_mapping.clear();

    m_manager->launchQuery(string);
}

void PlasmaRunnerSource::slotMatchesChanged(const QList< Plasma::QueryMatch >& matches)
{
    foreach(const Plasma::QueryMatch& plasmaMatch, matches) {
        Match match(this);
        match.setText(plasmaMatch.text());
        match.setIcon(plasmaMatch.icon().name());
        match.setType(QLatin1String("Application"));

        uint id = qHash(plasmaMatch.text() + plasmaMatch.subtext());
        match.setData(id);

        addMatch(match);
        m_mapping.insert(id, new Plasma::QueryMatch(plasmaMatch));
    }
}

void PlasmaRunnerSource::run(const Match& match)
{
    uint id = match.data().toUInt();
    if (m_mapping.contains(id)) {
        Plasma::QueryMatch* plasmaMatch = m_mapping.value(id);
        m_manager->run(*plasmaMatch);
    }
}

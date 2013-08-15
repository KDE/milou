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

#include "applicationsource.h"

#include <KService>
#include <KServiceTypeTrader>
#include <KRun>

ApplicationSource::ApplicationSource(QObject* parent): AbstractSource(parent)
{
    m_applicationType = new MatchType("Application", "bah");
    m_kcmType = new MatchType("System Settings", "preferences-desktop");

    QList<MatchType*> types;
    types << m_applicationType << m_kcmType;

    setTypes(types);
}

void ApplicationSource::query(const QString& string)
{
    if (string.size() < 3)
        return;

    QString queryStr = QString("exist Exec and ('%1' ~~ Name)").arg(string);

    if (m_applicationType->isShown()) {
        KService::List services = KServiceTypeTrader::self()->query("Application", queryStr);

        foreach (const KService::Ptr& service, services) {
            if (service->noDisplay() || service->property("NotShowIn", QVariant::String) == "KDE")
                continue;

            Match match(this);
            match.setType(m_applicationType);
            match.setText(service->name());
            match.setIcon(service->icon());
            match.setData(service->storageId());

            addMatch(match);
        }
    }

    if (m_kcmType->isShown()) {
        KService::List services = KServiceTypeTrader::self()->query("KCModule", queryStr);
        foreach (const KService::Ptr& service, services) {
            if (service->noDisplay() || service->property("NotShowIn", QVariant::String) == "KDE")
                continue;

            Match match(this);
            match.setType(m_kcmType);
            match.setText(service->name());
            match.setIcon(service->icon());
            match.setData(service->storageId());

            addMatch(match);
        }
    }
}

void ApplicationSource::run(const Match& match)
{
    KService::Ptr service = KService::serviceByStorageId(match.data().toString());
    if (service) {
        KRun::run(*service, KUrl::List(), 0);
    }
}


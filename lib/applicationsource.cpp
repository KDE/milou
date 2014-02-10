/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Vishesh Handa <me@vhanda.in>
 *
 * Code adapted from kde-workspace/plasma/generic/runners/servicerunner.cpp
 * Copyright (C) 2006 Aaron Seigo <aseigo@kde.org>
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
#include <QSet>

ApplicationSource::ApplicationSource(QObject* parent): AbstractSource(parent)
{
    //TODO: Find an applications icon!
    m_applicationType = new Milou::MatchType(i18n("Applications"), "bah");
    m_kcmType = new Milou::MatchType(i18n("System Settings"), "preferences-desktop");

    QList<Milou::MatchType*> types;
    types << m_applicationType << m_kcmType;

    setTypes(types);
}

void ApplicationSource::query(const Milou::Context& context)
{
    const QString term = context.query();
    if (!term.size())
        return;

    bool showApplications = context.types().contains(m_applicationType);
    bool showKcms = context.types().contains(m_kcmType);

    QSet<QString> seen;
    QString queryStr = QString("exist Exec and ('%1' ~~ Name)").arg(term);

    KService::List services = KServiceTypeTrader::self()->query("Application", queryStr);
    services += KServiceTypeTrader::self()->query("KCModule", queryStr);

    foreach (const KService::Ptr& service, services) {
        if (service->noDisplay() || service->property("NotShowIn", QVariant::String) == "KDE")
            continue;

        Milou::Match match(this);
        bool isKcm = service->serviceTypes().contains("KCModule");
        if (isKcm) {
            if (!showKcms)
                continue;

            match.setType(m_kcmType);
        }
        else {
            if (!showApplications)
                continue;

            match.setType(m_applicationType);
        }

        match.setText(service->name());
        match.setIcon(service->icon());
        match.setData(service->storageId());
        match.setPreviewType("application");
        match.setPreviewUrl(service->entryPath());

        addMatch(match);
        seen.insert(service->storageId());
        seen.insert(service->exec());
    }

    // If the term length is < 3, no real point searching the Keywords and GenericName
    if (term.length() < 3) {
        queryStr = QString("exist Exec and ( (exist Name and '%1' ~~ Name) or ('%1' ~~ Exec) )").arg(term);
    } else {
        // Search for applications which are executable and the term case-insensitive matches any of
        // * a substring of one of the keywords
        // * a substring of the GenericName field
        // * a substring of the Name field
        // Note that before asking for the content of e.g. Keywords and GenericName we need to ask if
        // they exist to prevent a tree evaluation error if they are not defined.
        queryStr = QString("exist Exec and ( (exist Keywords and '%1' ~subin Keywords) or (exist GenericName and '%1' ~~ GenericName) or (exist Name and '%1' ~~ Name) or ('%1' ~~ Exec) )").arg(term);
    }

    services = KServiceTypeTrader::self()->query("Application", queryStr);
    services += KServiceTypeTrader::self()->query("KCModule", queryStr);
    foreach (const KService::Ptr &service, services) {
        if (service->noDisplay()) {
            continue;
        }

        const QString id = service->storageId();
        const QString name = service->desktopEntryName();
        const QString exec = service->exec();

        if (seen.contains(id) || seen.contains(exec)) {
            continue;
        }

        // If the term was < 3 chars and NOT at the beginning of the App's name or Exec, then
        // chances are the user doesn't want that app.
        if (term.size() < 3 && !(name.startsWith(term) || exec.startsWith(term))) {
            continue;
        }

        Milou::Match match(this);
        bool isKcm = service->serviceTypes().contains("KCModule");
        if (isKcm) {
            if (!showKcms)
                continue;

            match.setType(m_kcmType);
        }
        else {
            if (!showApplications)
                continue;

            match.setType(m_applicationType);
        }
        match.setText(service->name());
        match.setIcon(service->icon());
        match.setData(service->storageId());
        match.setPreviewType("application");
        match.setPreviewUrl(service->entryPath());

        addMatch(match);
        seen.insert(service->storageId());
        seen.insert(service->exec());
    }
}

void ApplicationSource::run(const Milou::Match& match)
{
    KService::Ptr service = KService::serviceByStorageId(match.data().toString());
    if (service) {
        KRun::run(*service, KUrl::List(), 0);
    }
}


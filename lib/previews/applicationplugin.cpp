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

#include "applicationplugin.h"

#include <KGlobal>
#include <KStandardDirs>
#include <KDebug>

#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDateTime>
#include <QFileInfo>

#include <KLocale>
#include <KService>
#include <KGlobal>
#include <KMimeType>

ApplicationPlugin::ApplicationPlugin(QObject* parent, const QVariantList&): PreviewPlugin(parent)
{
}

void ApplicationPlugin::generatePreview()
{
    QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.milou/contents/ui/previews/Audio.qml");

    QDeclarativeComponent* component = new QDeclarativeComponent(context()->engine(), qmlFile, this);
    if (component->status() == QDeclarativeComponent::Error) {
        kError() << component->errorString();
        return;
    }

    QDeclarativeItem* item = qobject_cast<QDeclarativeItem*>(component->create());

    const QString entryPath = url().toLocalFile();
    KService::Ptr service = KService::serviceByDesktopPath(entryPath);
    if (service.isNull()) {
        return;
    }

    item->setProperty("title", service->name());
    //item->setProperty("keys", QVariant::fromValue(keys));
    //item->setProperty("values", QVariant::fromValue(values));
    //item->setProperty("length", keys.length());
    item->setProperty("length", 0);
    item->setProperty("iconName", service->icon());

    emit previewGenerated(item);
}


MILOU_EXPORT_PREVIEW(ApplicationPlugin, "milouapplicationplugin", "milou")

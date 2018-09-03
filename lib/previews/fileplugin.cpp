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

#include "fileplugin.h"

#include <KGlobal>
#include <KStandardDirs>
#include <QDebug>

#include <QDeclarativeComponent>
#include <QFileInfo>

#include <KLocale>
#include <KMimeType>

FilePlugin::FilePlugin(QObject* parent, const QVariantList&): PreviewPlugin(parent)
{
}

void FilePlugin::generatePreview()
{
    QFileInfo fileInfo(url().toLocalFile());

    QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.milou/contents/ui/previews/Audio.qml");

    QDeclarativeComponent* component = new QDeclarativeComponent(context()->engine(), qmlFile, this);
    if (component->status() == QDeclarativeComponent::Error) {
        qWarning() << component->errorString();
        return;
    }

    QDeclarativeItem* item = qobject_cast<QDeclarativeItem*>(component->create());

    QStringList keys;
    keys << i18n("Modified:") << i18n("Size:");

    QStringList values;
    values << KGlobal::locale()->formatDateTime(fileInfo.lastModified(), KLocale::FancyShortDate);
    values << KGlobal::locale()->formatByteSize(fileInfo.size());

    item->setProperty("title", fileInfo.fileName());
    item->setProperty("keys", QVariant::fromValue(keys));
    item->setProperty("values", QVariant::fromValue(values));
    item->setProperty("length", keys.length());

    KMimeType::Ptr mime = KMimeType::findByUrl(url());
    if (!mime.isNull())
        item->setProperty("iconName", mime->iconName());

    emit previewGenerated(item);
}


MILOU_EXPORT_PREVIEW(FilePlugin, "miloufileplugin", "milou")

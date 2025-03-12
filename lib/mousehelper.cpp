/*
 * SPDX-FileCopyrightText: 2017 Peifeng Yu <aetf@unlimitedcodeworks.xyz>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "mousehelper.h"

#include <QCursor>
#include <QMimeData>
#include <QUrl>

using namespace Milou;

MouseHelper::MouseHelper(QObject *parent)
    : QObject(parent)
{
}

MouseHelper::~MouseHelper()
{
}

QVariantMap MouseHelper::generateMimeDataMap(QMimeData *data) const
{
    QVariantMap dataMap;

    QStringList formats = data->formats();
    auto it = std::find(formats.cbegin(), formats.cend(), QLatin1String("text/plain"));
    if (it != formats.cend()) {
        dataMap.insert(*it, data->text());
        formats.erase(it);
    }

    it = std::find(formats.cbegin(), formats.cend(), QLatin1String("text/html"));
    if (it != formats.cend()) {
        dataMap.insert(*it, data->html());
        formats.erase(it);
    }

    it = std::find(formats.cbegin(), formats.cend(), QLatin1String("text/uri-list"));
    if (it != formats.cend()) {
        const QList<QUrl> urls = data->urls();
        dataMap.insert(*it, QVariantList(urls.cbegin(), urls.cend()));
    }

    it = std::find_if(formats.cbegin(), formats.cend(), [](const QString &format) {
        return format.startsWith(QLatin1String("image/"));
    });
    if (it != formats.cend()) {
        dataMap.insert(*it, data->imageData());
        formats.erase(it);
    }

    for (const QString &format : std::as_const(formats)) {
        dataMap.insert(format, data->data(format));
    }

    return dataMap;
}

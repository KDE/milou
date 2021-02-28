/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#include "textplugin.h"

#include <QFile>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QTextStream>

TextPlugin::TextPlugin(QObject *parent, const QVariantList &)
    : PreviewPlugin(parent)
{
}

void TextPlugin::generatePreview()
{
    QFile file(url().toLocalFile());
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, //
                                          QStringLiteral("plasma/plasmoids/org.kde.milou/contents/ui/previews/TextPreview.qml"));
    QQmlComponent component(context()->engine(), path);

    QQuickItem *item = qobject_cast<QQuickItem *>(component.create());

    QTextStream stream(&file);
    const QString text = stream.readAll();

    item->setProperty("text", text);
    item->setWidth(700);
    item->setHeight(1000);

    Q_EMIT previewGenerated(item);
}

MILOU_EXPORT_PREVIEW(TextPlugin, "miloutextplugin")

#include "textplugin.moc"

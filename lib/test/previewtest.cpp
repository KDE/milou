/*
 * This file is part of the KDE Baloo Project
 * SPDX-FileCopyrightText: 2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#include <QGuiApplication>

#include <QCommandLineParser>
#include <QDebug>
#include <QMimeDatabase>
#include <QQmlEngine>
#include <QTimer>

#include "preview.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("fileName"), QStringLiteral("The file path"));
    parser.process(app);

    if (parser.positionalArguments().size() != 1) {
        parser.showHelp(1);
    }

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData("import QtQuick.Controls 1.2\nApplicationWindow { visible: true \n}", QUrl());

    QObject *obj = component.create();
    QQuickItem *parentItem = obj->property("contentItem").value<QQuickItem *>();

    const QString url = parser.positionalArguments().constFirst();
    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(url).name();

    qDebug() << url << mimetype;

    auto previewItem = new Milou::Preview(parentItem);
    previewItem->setUrl(url);
    previewItem->setMimetype(mimetype);

    QTimer::singleShot(0, previewItem, SLOT(refresh()));

    return app.exec();
}

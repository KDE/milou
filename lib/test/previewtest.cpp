/*
 * This file is part of the KDE Baloo Project
 * Copyright (C) 2014  Vishesh Handa <me@vhanda.in>
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

#include <QGuiApplication>

#include <QQmlEngine>
#include <QCommandLineParser>
#include <QMimeDatabase>
#include <QTimer>
#include <QDebug>

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

    QObject* obj = component.create();
    QQuickItem* parentItem = obj->property("contentItem").value<QQuickItem*>();

    QString url = parser.positionalArguments().first();
    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(url).name();

    qDebug() << url << mimetype;

    auto previewItem = new Milou::Preview(parentItem);
    previewItem->setUrl(url);
    previewItem->setMimetype(mimetype);

    QTimer::singleShot(0, previewItem, SLOT(refresh()));

    return app.exec();
}

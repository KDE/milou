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

#include "textplugin.h"

#include <QFile>
#include <QQmlEngine>
#include <QQmlComponent>

TextPlugin::TextPlugin(QObject* parent, const QVariantList&)
    : PreviewPlugin(parent)
{
}

void TextPlugin::generatePreview()
{
    QFile file(url().toLocalFile());
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData("import QtQuick 2.0\nText { text: \"Hello world!\" }", QUrl());

    QQuickItem *item = qobject_cast<QQuickItem *>(component.create());
    emit previewGenerated(item);

    /*
    QTextStream stream(&file);
    const QString text = stream.readAll();

    QTextEdit* textEdit = new QTextEdit(0);
    textEdit->setText(text);
    textEdit->setReadOnly(true);
    textEdit->setWordWrapMode(QTextOption::WordWrap);

    highlight(textEdit->document());

    // Maybe the height should be reduced based on the contents?
    textEdit->resize(384, 384);
    emit previewGenerated(textEdit);
    */
}

MILOU_EXPORT_PREVIEW(TextPlugin, "miloutextplugin")

#include "textplugin.moc"


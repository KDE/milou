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

#include "textplugin.h"

#include <QFile>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTextStream>

TextPlugin::TextPlugin(QObject* parent, const QVariantList&)
    : PreviewPlugin(parent)
{
}

void TextPlugin::generatePreview(const QUrl& url, const QString& mimetype)
{
    Q_UNUSED(mimetype);

    QFile file(url.toLocalFile());
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QTextStream stream(&file);
    const QString text = stream.readAll();

    QTextEdit* textEdit = new QTextEdit(0);
    textEdit->setText(text);
    textEdit->setReadOnly(true);
    textEdit->setWordWrapMode(QTextOption::WordWrap);

    emit previewGenerated(textEdit);
}

MILOU_EXPORT_PREVIEW(TextPlugin, "miloutextplugin")
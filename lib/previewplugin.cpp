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

#include "previewplugin.h"

#include <QTextCursor>

using namespace Milou;

PreviewPlugin::PreviewPlugin(QObject *parent)
    : QObject(parent)
    , m_context(nullptr)
{
}

PreviewPlugin::~PreviewPlugin()
{
}

void PreviewPlugin::setUrl(const QUrl &url)
{
    m_url = url;
}

QUrl PreviewPlugin::url() const
{
    return m_url;
}

void PreviewPlugin::setMimetype(const QString &mimetype)
{
    m_mimetype = mimetype;
}

QString PreviewPlugin::mimetype() const
{
    return m_mimetype;
}

void PreviewPlugin::setHighlight(const QString &term)
{
    m_highlight = term;
}

QString PreviewPlugin::highlight() const
{
    return m_highlight;
}

void PreviewPlugin::setContext(QQmlContext *context)
{
    m_context = context;
}

QQmlContext *PreviewPlugin::context()
{
    Q_ASSERT(m_context);
    return m_context;
}

void PreviewPlugin::highlight(const QTextDocument *doc) const
{
    QTextCursor cursor;
    const QStringList highlights = highlight().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    for (const QString &text : highlights) {
        while (1) {
            cursor = doc->find(text, cursor);
            if (cursor.isNull()) {
                break;
            }

            QString selection = cursor.selectedText();

            QTextCharFormat format = cursor.charFormat();
            format.setBackground(QBrush(Qt::yellow));

            cursor.removeSelectedText();
            cursor.insertText(selection, format);
        }
    }
}

bool PreviewPlugin::onHighDPI() const
{
    return true;
}

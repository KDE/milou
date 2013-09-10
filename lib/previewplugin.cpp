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

#include "previewplugin.h"

#include <QTextDocument>
#include <QTextCursor>

using namespace Milou;

PreviewPlugin::PreviewPlugin(QObject* parent)
    : QObject(parent)
    , m_context(0)
{
}

PreviewPlugin::~PreviewPlugin()
{
}

void PreviewPlugin::setUrl(const KUrl& url)
{
    m_url = url;
}

KUrl PreviewPlugin::url() const
{
    return m_url;
}

void PreviewPlugin::setMimetype(const QString& mimetype)
{
    m_mimetype = mimetype;
}

QString PreviewPlugin::mimetype() const
{
    return m_mimetype;
}

void PreviewPlugin::setHighlight(const QString& term)
{
    m_highlight = term;
}

QString PreviewPlugin::highlight() const
{
    return m_highlight;
}

void PreviewPlugin::setContext(QDeclarativeContext* context)
{
    m_context = context;
}

QDeclarativeContext* PreviewPlugin::context()
{
    return m_context;
}

void PreviewPlugin::highlight(const QTextDocument* doc) const
{
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QBrush(Qt::yellow));

    QTextCursor cursor;
    while (1) {
        cursor = doc->find(highlight(), cursor);
        if (cursor.isNull())
            break;

        QString selection = cursor.selectedText();
        cursor.removeSelectedText();
        cursor.insertText(selection, highlightFormat);
    }
}


/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

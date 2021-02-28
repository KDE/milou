/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#include "preview.h"
#include "previewplugin.h"

#include <KService>
#include <KServiceTypeTrader>
#include <QDebug>

#include <QTimer>

#include <QQmlContext>

using namespace Milou;

Preview::Preview(QQuickItem *parent)
    : QQuickItem(parent)
    , m_loaded(false)
    , m_declarativeItem(nullptr)
    , m_filePlugin(nullptr)
{
    // setFlag(QGraphicsItem::ItemHasNoContents, false);

    m_plugins = allPlugins();
    for (PreviewPlugin *plugin : qAsConst(m_plugins)) {
        connect(plugin, &PreviewPlugin::previewGenerated, this, &Preview::slotPreviewGenerated);

        if (plugin->mimetypes().contains(QStringLiteral("file"))) {
            m_filePlugin = plugin;
        }
    }

    // When the object is created, it doesn't have a QQmlContext
    QTimer::singleShot(0, this, &Preview::setPluginContexts);
}

void Preview::setPluginContexts()
{
    QQmlContext *context = qmlContext(parentItem());
    for (PreviewPlugin *plugin : qAsConst(m_plugins)) {
        plugin->setContext(context);
    }
}

Preview::~Preview()
{
}

void Preview::refresh()
{
    if (m_oldUrl == m_url && m_oldMimetype == m_mimetype) {
        if (m_declarativeItem) {
            Q_EMIT loadingFinished();
        }
        return;
    }

    m_loaded = false;

    bool foundPlugin = false;
    QUrl url = QUrl::fromLocalFile(m_url);
    for (PreviewPlugin *plugin : qAsConst(m_plugins)) {
        const QStringList mimeTypes = plugin->mimetypes();
        for (const QString &mime : mimeTypes) {
            if (m_mimetype.startsWith(mime)) {
                plugin->setUrl(url);
                plugin->setMimetype(m_mimetype);
                plugin->setHighlight(m_highlight);
                plugin->generatePreview();

                foundPlugin = true;
                break;
            }
        }
    }

    if (!foundPlugin && m_filePlugin) {
        m_filePlugin->setUrl(url);
        m_filePlugin->setMimetype(m_mimetype);
        m_filePlugin->setHighlight(m_highlight);
        m_filePlugin->generatePreview();
    }
}

void Preview::slotPreviewGenerated(QQuickItem *item)
{
    clear();

    m_declarativeItem = item;
    item->setParentItem(this);

    setWidth(item->width());
    setHeight(item->height());

    m_loaded = true;
    Q_EMIT loadingFinished();
}

void Preview::clear()
{
    if (m_declarativeItem) {
        m_declarativeItem->deleteLater();
        m_declarativeItem = nullptr;
    }
}

QString Preview::mimetype() const
{
    return m_mimetype;
}

void Preview::setMimetype(const QString &mime)
{
    if (m_mimetype != mime) {
        m_oldMimetype = m_mimetype;
        m_mimetype = mime;
    }
}

void Preview::setUrl(const QString &url)
{
    if (m_url != url) {
        m_oldUrl = m_url;
        m_url = url;
    }
}

QString Preview::url() const
{
    return m_url;
}

void Preview::setHighlight(const QString &highlight)
{
    m_highlight = highlight;
}

QString Preview::highlight() const
{
    return m_highlight;
}

QList<PreviewPlugin *> Preview::allPlugins()
{
    KService::List serviceList = KServiceTypeTrader::self()->query(QStringLiteral("MilouPreviewPlugin"));
    QList<PreviewPlugin *> plugins;

    KService::List::const_iterator it;
    for (it = serviceList.constBegin(); it != serviceList.constEnd(); it++) {
        KService::Ptr service = *it;

        QString error;
        PreviewPlugin *p = service->createInstance<PreviewPlugin>(this, QVariantList(), &error);
        if (!p) {
            qWarning() << "Could not create PreviewPlugin:" << service->library();
            qWarning() << error;
            continue;
        }

        plugins << p;
    }

    return plugins;
}

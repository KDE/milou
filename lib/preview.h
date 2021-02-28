/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#ifndef PREVIEW_H
#define PREVIEW_H

#include "milou_export.h"
#include <QQuickItem>

namespace Milou
{
class PreviewPlugin;

class MILOU_EXPORT Preview : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString mimetype READ mimetype WRITE setMimetype)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(QString highlight READ highlight WRITE setHighlight)
    Q_PROPERTY(bool loaded READ loaded)

public:
    explicit Preview(QQuickItem *parent = nullptr);
    ~Preview() override;

    QString mimetype() const;
    void setMimetype(const QString &mime);

    QString url() const;
    void setUrl(const QString &url);

    QString highlight() const;
    void setHighlight(const QString &highlight);

    bool loaded() const
    {
        return m_loaded;
    }

Q_SIGNALS:
    void loadingFinished();

public Q_SLOTS:
    void refresh();
    void clear();

private Q_SLOTS:
    void slotPreviewGenerated(QQuickItem *item);

    void setPluginContexts();

private:
    bool m_loaded;
    QString m_mimetype;
    QString m_url;
    QString m_highlight;

    QString m_oldMimetype;
    QString m_oldUrl;

    QQuickItem *m_declarativeItem;

    QList<Milou::PreviewPlugin *> m_plugins;
    QList<Milou::PreviewPlugin *> allPlugins();
    PreviewPlugin *m_filePlugin;
};

}

#endif // PREVIEW_H

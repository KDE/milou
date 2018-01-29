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

#ifndef PREVIEW_H
#define PREVIEW_H

#include <QQuickItem>
#include "milou_export.h"

namespace Milou {

class PreviewPlugin;

class MILOU_EXPORT Preview : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString mimetype READ mimetype WRITE setMimetype)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(QString highlight READ highlight WRITE setHighlight)
    Q_PROPERTY(bool loaded READ loaded)

public:
    Preview(QQuickItem* parent = 0);
    ~Preview() override;

    QString mimetype() const;
    void setMimetype(const QString& mime);

    QString url() const;
    void setUrl(const QString& url);

    QString highlight() const;
    void setHighlight(const QString& highlight);

    bool loaded() const { return m_loaded; }

signals:
    void loadingFinished();

public slots:
    void refresh();
    void clear();

private slots:
    void slotPreviewGenerated(QQuickItem* item);

    void setPluginContexts();

private:
    bool m_loaded;
    QString m_mimetype;
    QString m_url;
    QString m_highlight;

    QString m_oldMimetype;
    QString m_oldUrl;

    QQuickItem* m_declarativeItem;

    QList<Milou::PreviewPlugin*> m_plugins;
    QList<Milou::PreviewPlugin*> allPlugins();
    PreviewPlugin* m_filePlugin;
};

}

#endif // PREVIEW_H

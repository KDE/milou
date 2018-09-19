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

#ifndef MILOU_PREVIEWPLUGIN_H
#define MILOU_PREVIEWPLUGIN_H

#include <KService>
#include <QQuickItem>
#include "milou_export.h"

#include <QTextDocument>

namespace Milou {

class MILOU_EXPORT PreviewPlugin : public QObject
{
    Q_OBJECT
public:
    explicit PreviewPlugin(QObject* parent);
    ~PreviewPlugin() override;

    /**
     * Return a list of mimetype which this plugin supports. These mimetypes
     * can also just be the starting substring of the actual mimetype.
     * Eg - "text/" or "audio/"
     */
    virtual QStringList mimetypes() = 0;

    /**
     * Generates a preview widget/graphicsItem which is then returned via
     * the appropriate signal
     *
     * \sa previewGenerated
     */
    virtual void generatePreview() = 0;

    /**
     * Retrieve the context, this allows you to create
     * your own preview plugins in QML
     */
    QQmlContext* context();

    void setContext(QQmlContext* context);

    /**
     * The url of the preview being generated
     */
    QUrl url() const;
    void setUrl(const QUrl& url);

    /**
     * The mimetype of the url for whcih the preview
     * should be generated
     */
    QString mimetype() const;
    void setMimetype(const QString& mimetype);

    /**
     * A term that should be highlighted in the preview
     * that is generated
     */
    QString highlight() const;
    void setHighlight(const QString& term);

Q_SIGNALS:
    void previewGenerated(QQuickItem* graphicsItem);

protected:
    /**
     * Highlights all the occurrences of highlight in the document
     */
    void highlight(const QTextDocument* doc) const;

    bool onHighDPI() const;
private:
    QQmlContext* m_context;

    QUrl m_url;
    QString m_highlight;
    QString m_mimetype;
};

}

/**
 * Export a Milou Preview Generator
 *
 * \param classname The name of the subclass to export
 * \param libname The name of the library which should export the extractor
 */
#define MILOU_EXPORT_PREVIEW( classname, libname )    \
    K_PLUGIN_FACTORY(factory, registerPlugin<classname>();)

#endif // MILOU_PREVIEWPLUGIN_H

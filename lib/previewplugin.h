/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#ifndef MILOU_PREVIEWPLUGIN_H
#define MILOU_PREVIEWPLUGIN_H

#include "milou_export.h"
#include <KService>
#include <QQuickItem>

#include <QTextDocument>

namespace Milou
{
class MILOU_EXPORT PreviewPlugin : public QObject
{
    Q_OBJECT
public:
    explicit PreviewPlugin(QObject *parent);
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
    QQmlContext *context();

    void setContext(QQmlContext *context);

    /**
     * The url of the preview being generated
     */
    QUrl url() const;
    void setUrl(const QUrl &url);

    /**
     * The mimetype of the url for whcih the preview
     * should be generated
     */
    QString mimetype() const;
    void setMimetype(const QString &mimetype);

    /**
     * A term that should be highlighted in the preview
     * that is generated
     */
    QString highlight() const;
    void setHighlight(const QString &term);

Q_SIGNALS:
    void previewGenerated(QQuickItem *graphicsItem);

protected:
    /**
     * Highlights all the occurrences of highlight in the document
     */
    void highlight(const QTextDocument *doc) const;

    bool onHighDPI() const;

private:
    QQmlContext *m_context;

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
#define MILOU_EXPORT_PREVIEW(classname, libname) K_PLUGIN_FACTORY(factory, registerPlugin<classname>();)

#endif // MILOU_PREVIEWPLUGIN_H

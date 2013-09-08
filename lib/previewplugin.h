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

#ifndef PREVIEWPLUGIN_H
#define PREVIEWPLUGIN_H

#include <QUrl>
#include <QDeclarativeItem>

#include <KService>
#include "milou_export.h"

namespace Milou {

class MILOU_EXPORT PreviewPlugin : public QObject
{
    Q_OBJECT
public:
    explicit PreviewPlugin(QObject* parent);
    virtual ~PreviewPlugin();

    /**
     * Return a list of mimetype which this plugin supports. These mimetypes
     * can also just be the starting substring of the actual mimetype.
     * Eg - "text/" or "audio/"
     */
    virtual QStringList mimetypes() = 0;

    /**
     * Generates a preview widget/graphicsItem which is then returned via
     * the appropriate signal
     */
    virtual void generatePreview(const QUrl& url, const QString& mimetype) = 0;

signals:
    void previewGenerated(QWidget* widget);
    void previewGenerated(QDeclarativeItem* graphicsItem);
};

}
/**
 * Export a Milou Preview Generator
 *
 * \param classname The name of the subclass to export
 * \param libname The name of the library which should export the extractor
 */
#define MILOU_EXPORT_PREVIEW( classname, libname )    \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();) \
K_EXPORT_PLUGIN(factory(#libname))

#endif // PREVIEWPLUGIN_H

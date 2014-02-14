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

#include "imageplugin.h"
#include <QPainter>

#include <KDebug>

ImagePlugin::ImagePlugin(QObject* parent, const QVariantList& ): PreviewPlugin(parent)
{

}

void ImagePlugin::generatePreview()
{
    KFileItemList itemList;
    itemList << KFileItem(url(), mimetype(), mode_t());

    QStringList enabledPlugins;
    enabledPlugins << "imagethumbnail" << "jpegthumbnail" << "directorythumbnail"
                   << "videopreview" << "ffmpegthumbs";

    QSize size(512, 512);
    if (mimetype() == QLatin1String("inode/directory")) {
        size = QSize(256, 256);
    }
    KIO::PreviewJob* job = new KIO::PreviewJob(itemList, size, &enabledPlugins);
    job->setScaleType(KIO::PreviewJob::ScaledAndCached);

    connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)),
            this, SLOT(slotGotPreview(KFileItem,QPixmap)));
    connect(job, SIGNAL(finished(KJob*)),
            this, SLOT(slotJobFinished(KJob*)));
    job->start();
}

namespace {
    class ImageItem: public QDeclarativeItem {

    public:
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* ) {
            if (!m_pixmap.isNull()) {
                painter->drawPixmap(x(), y(), width(), height(), m_pixmap);
            }
        }
        QPixmap m_pixmap;
    };
}

void ImagePlugin::slotGotPreview(const KFileItem&, const QPixmap& pixmap)
{
    ImageItem* declarativeItem = new ImageItem();
    declarativeItem->m_pixmap = pixmap;
    declarativeItem->setFlag(QGraphicsItem::ItemHasNoContents, false);
    declarativeItem->setWidth(pixmap.width());
    declarativeItem->setHeight(pixmap.height());

    emit previewGenerated(declarativeItem);
}

void ImagePlugin::slotJobFinished(KJob* job)
{
    if (job->error())
        kError() << job->errorString();
}


MILOU_EXPORT_PREVIEW(ImagePlugin, "milouimageplugin", "milou")

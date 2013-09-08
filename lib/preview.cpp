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

#include "preview.h"
#include "previews/textview.h"

#include <KService>
#include <KDebug>
#include <kparts/mainwindow.h>

#include <QPainter>
#include <QGraphicsProxyWidget>
#include <QTextEdit>
#include <QTimer>

Preview::Preview(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , m_loaded(false)
    , m_widget(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

Preview::~Preview()
{
    delete m_widget;
}

void Preview::paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget)
{
    Q_UNUSED(item);
    Q_UNUSED(widget);

    if (!m_pixmap.isNull())
        painter->drawPixmap(x(), y(), width(), height(), m_pixmap);
}

void Preview::refresh()
{
    if (m_mimetype.startsWith("image/")) {
        // Create a preview job and draw the pixmap!
        KFileItemList itemList;

        KUrl url(m_url);
        itemList << KFileItem(url, m_mimetype, mode_t());

        KIO::PreviewJob* job = new KIO::PreviewJob(itemList, QSize(256, 256));

        connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)),
                this, SLOT(slotGotPreview(KFileItem,QPixmap)));

        m_loaded = false;
        job->start();
    }
    else if (m_mimetype == QLatin1String("application/pdf")
             || m_mimetype == QLatin1String("application/epub+zip")
             || m_mimetype == QLatin1String("application/x-mobipocket-ebook"))
    {
        // FIXME: You will need to create your own config file, so that the last accessed page
        //        is not opened
        KService::Ptr service = KService::serviceByDesktopName("okular_part");
        if (service) {
            KParts::ReadOnlyPart* part = service->createInstance<KParts::ReadOnlyPart>(this, QVariantList() << QString("ViewerWidget"));
            part->openUrl(KUrl(m_url));

            QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget(this);
            proxyWidget->setWidget(part->widget());

            part->widget()->resize(256, 256);

            m_loaded = true;
            emit loadingFinished();
        }
        else {
            kDebug() << "Could not load okular service!";
        }
    }
    else if (m_mimetype.startsWith("text")) {
        m_widget = new TextView(KUrl(m_url).toLocalFile(), 0);

        QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget(this);
        proxyWidget->setWidget(m_widget);
        proxyWidget->resize(256, 256);

        m_loaded = true;
        emit loadingFinished();
    }
    else {
        emit loadingFailed();
    }

}

void Preview::slotGotPreview(const KFileItem& item, const QPixmap& pixmap)
{
    Q_UNUSED(item);

    m_pixmap = pixmap;
    update();

    m_loaded = true;
    emit loadingFinished();
}

QString Preview::mimetype()
{
    return m_mimetype;
}

void Preview::setMimetype(const QString& mime)
{
    bool changed = false;

    if (m_mimetype != mime) {
        m_mimetype = mime;
        changed = true;
    }

    if(changed && !m_url.isEmpty())
        refresh();
}

void Preview::setUrl(const QString& url)
{
    bool changed = false;

    if (m_url != url) {
        m_url = url;
        changed = true;
    }

    if(changed && !m_mimetype.isEmpty())
        refresh();
}

QString Preview::url()
{
    return m_url;
}

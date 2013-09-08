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

#ifndef PREVIEW_H
#define PREVIEW_H

#include <QDeclarativeItem>
#include "milou_export.h"

#include <kio/previewjob.h>

class MILOU_EXPORT Preview : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString mimetype READ mimetype WRITE setMimetype)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(bool loaded READ loaded)
public:
    Preview(QDeclarativeItem* parent = 0);

    virtual void paint(QPainter* , const QStyleOptionGraphicsItem* , QWidget* );

    QString mimetype();
    void setMimetype(const QString& mime);

    QString url();
    void setUrl(const QString& url);

    bool loaded() const { return m_loaded; }
signals:
    void loadingFinished();
    void loadingFailed();

public slots:
    void refresh();

private slots:
    void slotGotPreview(const KFileItem& item, const QPixmap& pixmap);

private:
    QWidget* createPdfWidget();

    bool m_loaded;
    QString m_mimetype;
    QString m_url;

    QPixmap m_pixmap;
};

#endif // PREVIEW_H

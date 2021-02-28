/***************************************************************************
 *   SPDX-FileCopyrightText: 2013 Eike Hein <hein@kde.org>
 *   SPDX-License-Identifier: GPL-2.0-or-later
 ***************************************************************************/

#ifndef DRAGHELPER_H
#define DRAGHELPER_H

#include <QIcon>
#include <QObject>

#include "milou_export.h"

class QQuickItem;
class QMimeData;

namespace Milou
{
class MILOU_EXPORT DragHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int dragIconSize READ dragIconSize WRITE setDragIconSize NOTIFY dragIconSizeChanged)

public:
    explicit DragHelper(QObject *parent = nullptr);
    ~DragHelper() override;

    int dragIconSize() const;
    void setDragIconSize(int size);

    Q_INVOKABLE bool isDrag(int oldX, int oldY, int newX, int newY) const;
    // TODO KF6 merge the two
    Q_INVOKABLE void startDrag(QQuickItem *item, QMimeData *mimeData, const QIcon &icon = QIcon());
    Q_INVOKABLE void startDrag(QQuickItem *item, QMimeData *mimeData, const QString &iconName);

Q_SIGNALS:
    void dragIconSizeChanged();
    void dropped();

private:
    int m_dragIconSize;
    Q_INVOKABLE void doDrag(QQuickItem *item, QMimeData *mimeData, const QIcon &icon = QIcon());
};

}

#endif

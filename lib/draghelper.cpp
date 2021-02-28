/***************************************************************************
 *   SPDX-FileCopyrightText: 2013 Eike Hein <hein@kde.org>
 *   SPDX-License-Identifier: GPL-2.0-or-later
 ***************************************************************************/

#include "draghelper.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QQuickItem>

using namespace Milou;

DragHelper::DragHelper(QObject *parent)
    : QObject(parent)
    , m_dragIconSize(32)
{
}

DragHelper::~DragHelper()
{
}

int DragHelper::dragIconSize() const
{
    return m_dragIconSize;
}

void DragHelper::setDragIconSize(int size)
{
    if (m_dragIconSize != size) {
        m_dragIconSize = size;

        Q_EMIT dragIconSizeChanged();
    }
}

bool DragHelper::isDrag(int oldX, int oldY, int newX, int newY) const
{
    return ((QPoint(oldX, oldY) - QPoint(newX, newY)).manhattanLength() >= QApplication::startDragDistance());
}

void DragHelper::startDrag(QQuickItem *item, QMimeData *mimeData, const QIcon &icon)
{
    // This allows the caller to return, making sure we don't crash if
    // the caller is destroyed mid-drag
    // clang-format off
    QMetaObject::invokeMethod(this, "doDrag", Qt::QueuedConnection,
        Q_ARG(QQuickItem*, item), Q_ARG(QMimeData*, mimeData), Q_ARG(QIcon, icon));
    // clang-format on
}

void DragHelper::startDrag(QQuickItem *item, QMimeData *mimeData, const QString &iconName)
{
    startDrag(item, mimeData, QIcon::fromTheme(iconName));
}

void DragHelper::doDrag(QQuickItem *item, QMimeData *mimeData, const QIcon &icon)
{
    QDrag *drag = new QDrag(item);
    drag->setMimeData(mimeData);

    if (!icon.isNull()) {
        drag->setPixmap(icon.pixmap(m_dragIconSize, m_dragIconSize));
    }

    drag->exec();

    Q_EMIT dropped();
}

/*
 * SPDX-FileCopyrightText: 2017 Peifeng Yu <aetf@unlimitedcodeworks.xyz>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "mousehelper.h"

#include <QCursor>

using namespace Milou;

MouseHelper::MouseHelper(QObject *parent)
    : QObject(parent)
{
}

MouseHelper::~MouseHelper()
{
}

QPointF MouseHelper::globalMousePosition() const
{
    return QCursor::pos();
}

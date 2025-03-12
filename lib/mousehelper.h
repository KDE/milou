/*
 * SPDX-FileCopyrightText: 2017 Aetf <aetf@unlimitedcodeworks.xyz>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef MOUSEHELPER_H
#define MOUSEHELPER_H

#include <QObject>
#include <QPoint>
#include <QVariantMap>

class QMimeData;

namespace Milou
{
class MouseHelper : public QObject
{
    Q_OBJECT

public:
    explicit MouseHelper(QObject *parent = nullptr);
    ~MouseHelper() override;

    /*
     * Converts QMimeData to QVariantMap
     * @internal will be removed after https://codereview.qt-project.org/c/qt/qtdeclarative/+/491548
     */
    Q_INVOKABLE QVariantMap generateMimeDataMap(QMimeData *data) const;
};

}
#endif // MOUSEHELPER_H

/**
 * SPDX-FileCopyrightText: 2022 Kai Uwe Broulik <kde@broulik.de>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "milou_export.h"

#include <QObject>
#include <QPointer>
#include <QQuickItem>
#include <QUrl>

namespace Milou
{
class MILOU_EXPORT FileMenu : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QQuickItem *visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)
    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

public:
    explicit FileMenu(QObject *parent = nullptr);
    ~FileMenu() override;

    QUrl url() const;
    void setUrl(const QUrl &url);
    Q_SIGNAL void urlChanged(const QUrl &url);

    QQuickItem *visualParent() const;
    void setVisualParent(QQuickItem *visualParent);
    Q_SIGNAL void visualParentChanged();

    bool valid() const;
    Q_SIGNAL void validChanged(bool valid);

    bool visible() const;
    Q_SIGNAL void visibleChanged();

    Q_INVOKABLE void open(int x = -1, int y = -1);

Q_SIGNALS:
    void actionTriggered(QAction *action);

private:
    QUrl m_url;
    QPointer<QQuickItem> m_visualParent;
    bool m_valid = false;
    bool m_visible = false;
};

} // namespace Milou

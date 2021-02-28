/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#ifndef _MILOU_QML_PLUGINS_H
#define _MILOU_QML_PLUGINS_H

#include <QQmlExtensionPlugin>

class QmlPlugins : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void registerTypes(const char *uri) override;
};

#endif // _MILOU_QML_PLUGINS_H

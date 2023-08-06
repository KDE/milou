// SPDX-FileCopyrightText: 2023 Alexander Lohnau <alexander.lohnau@gmx.de>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include <QMimeData>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>

#include "mousehelper.h"
#include "resultsmodel.h"

class MilouQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri) override
    {
        qmlRegisterType<Milou::ResultsModel>(uri, 0, 3, "ResultsModel");
        qmlRegisterSingletonType<Milou::MouseHelper>(uri, 0, 1, "MouseHelper", [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new Milou::MouseHelper();
        });
        qmlRegisterAnonymousType<QMimeData>(uri, 0);
    }
};

#include "plugin.moc"

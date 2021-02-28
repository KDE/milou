/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#include "qmlplugins.h"

#include "draghelper.h"
#include "mousehelper.h"
#include "preview.h"
#include "resultsmodel.h"
#include "sourcesmodel.h"

#include <QMimeData>
#include <QQmlEngine>

void QmlPlugins::initializeEngine(QQmlEngine *, const char *)
{
}

void QmlPlugins::registerTypes(const char *uri)
{
    qmlRegisterType<Milou::SourcesModel>(uri, 0, 1, "SourcesModel");
    qmlRegisterType<Milou::ResultsModel>(uri, 0, 3, "ResultsModel");
    qmlRegisterType<Milou::Preview>(uri, 0, 1, "Preview");
    qmlRegisterType<Milou::DragHelper>(uri, 0, 2, "DragHelper");
    qmlRegisterSingletonType<Milou::MouseHelper>(uri, 0, 1, "MouseHelper", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return new Milou::MouseHelper();
    });
    qmlRegisterAnonymousType<QMimeData>(uri, 0);
}

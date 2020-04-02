/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "qmlplugins.h"

#include "sourcesmodel.h"
#include "resultsmodel.h"
#include "preview.h"
#include "draghelper.h"
#include "mousehelper.h"

#include <QQmlEngine>
#include <QMimeData>

void QmlPlugins::initializeEngine(QQmlEngine *, const char *)
{
}

void QmlPlugins::registerTypes(const char *uri)
{
    qmlRegisterType<Milou::SourcesModel> (uri, 0, 1, "SourcesModel");
    qmlRegisterType<Milou::ResultsModel>(uri, 0, 3, "ResultsModel");
    qmlRegisterType<Milou::Preview> (uri, 0, 1, "Preview");
    qmlRegisterType<Milou::DragHelper> (uri, 0, 2, "DragHelper");
    qmlRegisterSingletonType<Milou::MouseHelper> (uri, 0, 1, "MouseHelper",
                                                  [](QQmlEngine*, QJSEngine*) -> QObject* {
        return new Milou::MouseHelper();
    });
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    qmlRegisterAnonymousType<QMimeData>(uri, 0);
#else
    qmlRegisterType<QMimeData>();
#endif
}


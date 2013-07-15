/*
    Copyright (C) 2013 Vishesh Handa <me@vhanda.in>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef QML_PLUGINS_H
#define QML_PLUGINS_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

class QmlPlugins : public QDeclarativeExtensionPlugin
{
public:
    virtual void initializeEngine(QDeclarativeEngine *engine, const char *uri);
    virtual void registerTypes(const char *uri);
};

#endif // QML_PLUGINS_H

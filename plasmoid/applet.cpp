/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Vishesh Handa <me@vhanda.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "applet.h"

#include <KGlobal>
#include <KStandardDirs>
#include <KConfigDialog>

#include <QDeclarativeContext>
#include <QDeclarativeEngine>

Applet::Applet(QObject* parent, const QVariantList& args): PopupApplet(parent, args)
{
    setPopupIcon("nepomuk");
}

Applet::~Applet()
{
}

void Applet::init()
{
    m_declarativeWidget = new Plasma::DeclarativeWidget(this);

    QDeclarativeContext* rootContext = m_declarativeWidget->engine()->rootContext();
    rootContext->setContextProperty("plasmoid", this);

    QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.nepomuk.finder/contents/ui/main.qml");
    m_declarativeWidget->setQmlPath(qmlFile);

    Plasma::Applet::init();
}

QGraphicsWidget* Applet::graphicsWidget()
{
    return m_declarativeWidget;
}

void Applet::createConfigurationInterface(KConfigDialog* parent)
{
//    parent->addPage(widget, i18n("General"), icon());
    Plasma::Applet::createConfigurationInterface(parent);
}

void Applet::popupEvent(bool show)
{
    emit popupEventSignal(show);
}



#include "applet.moc"

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
#include "configwidget.h"

#include <KGlobal>
#include <KStandardDirs>
#include <KConfigDialog>
#include <KLocale>

#include <QDeclarativeContext>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QDesktopWidget>

Applet::Applet(QObject* parent, const QVariantList& args): PopupApplet(parent, args)
{
    setPopupIcon("nepomuk");
    KGlobal::locale()->insertCatalog("milou");
}

Applet::~Applet()
{
}

void Applet::init()
{
    m_declarativeWidget = new Plasma::DeclarativeWidget(this);

    QDeclarativeContext* rootContext = m_declarativeWidget->engine()->rootContext();
    rootContext->setContextProperty("plasmoid", this);

    QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.milou/contents/ui/main.qml");
    m_declarativeWidget->setQmlPath(qmlFile);

    Plasma::Applet::init();
}

QGraphicsWidget* Applet::graphicsWidget()
{
    return m_declarativeWidget;
}

void Applet::createConfigurationInterface(KConfigDialog* parent)
{
    ConfigWidget* widget = new ConfigWidget();
    parent->addPage(widget, i18n("Search Results"), icon());

    connect(widget, SIGNAL(changed()), parent, SLOT(settingsModified()));

    // The order matters. The config widget should be saved first
    connect(parent, SIGNAL(applyClicked()), widget, SLOT(saveSettings()));
    connect(parent, SIGNAL(okClicked()), widget, SLOT(saveSettings()));
    connect(parent, SIGNAL(applyClicked()), this, SIGNAL(settingsChanged()));
    connect(parent, SIGNAL(okClicked()), this, SIGNAL(settingsChanged()));
}

void Applet::popupEvent(bool show)
{
    emit popupEventSignal(show);
}

bool Applet::isTopEdge() const
{
    return location() == Plasma::TopEdge;
}

bool Applet::isBottomEdge() const
{
    return location() == Plasma::BottomEdge;
}

bool Applet::isShown() const
{
    return isPopupShowing();
}

// Copied adapted from kde-workspace/libs/plasmagenericshell/widgetexplorer/widgetexplorer.cpp
QPoint Applet::tooltipPosition(QGraphicsObject* item, int tipWidth, int tipHeight)
{
    if (!item) {
        return QPoint();
    }

    // Find view
    if (!item->scene()) {
        return QPoint();
    }

    QList<QGraphicsView*> views = item->scene()->views();
    if (views.isEmpty()) {
        return QPoint();
    }

    QGraphicsView *view = 0;
    if (views.size() == 1) {
        view = views[0];
    } else {
        QGraphicsView *found = 0;
        QGraphicsView *possibleFind = 0;

        foreach (QGraphicsView *v, views) {
            if (v->sceneRect().intersects(item->sceneBoundingRect()) ||
                v->sceneRect().contains(item->scenePos())) {
                if (v->isActiveWindow()) {
                    found = v;
                } else {
                    possibleFind = v;
                }
            }
        }
        view = found ? found : possibleFind;
    }

    if (!view) {
        return QPoint();
    }

    // Compute tip pos
    QRect itemRect(
        view->mapToGlobal(view->mapFromScene(item->scenePos())),
                   item->boundingRect().size().toSize());

    const int margin = 10;

    QPoint pos;
    pos.setY(itemRect.top() - tipHeight/2 + itemRect.height()/2);
    pos.setX(itemRect.right() + margin);

    // Choose left edge if no space on right edge
    const QRect avail = QApplication::desktop()->availableGeometry(view);
    if (pos.x() + tipWidth > avail.right()) {
        // HACK: The tipWidth doesn't seem to accurately reflect the actual width
        //       Hence the extra -25 to make it look good.
        pos.setX(itemRect.left() - tipWidth - margin - 25);
    }

    // HACK: The +30 is to account for other margins
    QRect finalRect(pos.x(), pos.y(), tipWidth, tipHeight + 30);

    if (finalRect.bottom() > avail.bottom()) {
        int diff = finalRect.bottom() - avail.bottom();
        finalRect.moveTop(finalRect.top() - diff);
    }

    return QPoint(finalRect.x(), finalRect.y());
}

#include "applet.moc"

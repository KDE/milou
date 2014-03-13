/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013-2014 Vishesh Handa <me@vhanda.in>
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

import QtQuick 2.1
import QtQuick.Layouts 1.1

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.qtextracomponents 2.0 as QtExtra
import org.kde.milou 0.1 as Milou

import "../code/globals.js" as Globals

Item {
    id: mainWidget
    Layout.minimumWidth: Globals.PlasmoidWidth
    Layout.maximumWidth: Globals.PlasmoidWidth
    Layout.minimumHeight: wrapper.minimumHeight + wrapper.anchors.topMargin + wrapper.anchors.bottomMargin
    Layout.maximumHeight: Layout.minimumHeight

    function isBottomEdge() {
        return plasmoid.location == PlasmaCore.BottomEdge;
    }

    // The wrapper just exists for giving an appropriate top/bottom margin
    // when it is placed on the top/bottom edge of the screen
    Item {
        id: wrapper

        //
        // The +20 is so that the minimumHeight is always atleast 20+. If the height
        // is too small then Plasma will ignore the fact that it is a PopupApplet
        // and put the SearchField directly in the panel.
        //
        property int minimumHeight: listView.count ? listView.contentHeight + searchField.height + 5
                                                   : searchField.height + 20
        property int maximumHeight: minimumHeight

        anchors {
            fill: parent
            topMargin: isBottomEdge() ? 0 : 7
            bottomMargin: isBottomEdge() ? 7 : 0
        }

        SearchField {
            id: searchField

            anchors {
                left: parent.left
                right: parent.right
            }
            onSearchTextChanged: {
                listView.setQueryString(text)
            }
        }

        ResultsView {
            id: listView

            anchors {
                left: parent.left
                right: parent.right

                // vHanda: Random number - Is there some way to use consisten margins?
                //         PlasmaCore.FrameSvg does have margins, but one needs to construct
                //          a PlasmaCore.FrameSvg for that
                topMargin: isBottomEdge() ? 0 : 5
                bottomMargin: isBottomEdge() ? 5 : 0
            }
        }

        Component.onCompleted: {
            // plasmoid.popupEvent.connect(setTextFieldFocus)
            //
            // The focus is not always set correctly. The hunch is that this
            // function is called before the popup is actually visible and
            // therfore the setFocus call does not do anything. So, we are using
            // a small timer and calling the setTextFieldFocus function again.
            //
            // FIXME: Figure out why these fail!!
            //plasmoid.popupEvent.connect(theFocusDoesNotAlwaysWorkTimer.start)
            //plasmoid.settingsChanged.connect(loadSettings)

            if (!isBottomEdge()) {
                // Normal view
                searchField.anchors.top = wrapper.top
                listView.anchors.top = searchField.bottom
                listView.anchors.bottom = wrapper.bottom
            }
            else {
                // When on the bottom
                listView.anchors.top = wrapper.top
                listView.anchors.bottom = searchField.top
                searchField.anchors.bottom = wrapper.bottom
            }
        }

        Timer {
            id: theFocusDoesNotAlwaysWorkTimer
            interval: 100
            repeat: false

            onTriggered: {
                wrapper.setTextFieldFocus(plasmoid.isShown())
            }
        }

        function setTextFieldFocus(shown) {
            searchField.setFocus();
            searchField.selectAll();

            if (!shown)
                listView.clearPreview();
        }

        function loadSettings() {
            listView.loadSettings()
        }
    }
}

/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013-2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.plasmoid

import org.kde.plasma.core as PlasmaCore
import org.kde.milou as Milou

import "globals.js" as Globals

PlasmoidItem {
    id: mainWidget

    switchWidth: Globals.SwitchWidth
    switchHeight: Globals.SwitchWidth

    Layout.minimumWidth: Globals.PlasmoidWidth
    Layout.maximumWidth: Globals.PlasmoidWidth
    Layout.minimumHeight: fullRepresentationItem?.minimumHeight ?? 0
    Layout.maximumHeight: fullRepresentationItem?.maximumHeight ?? 0

    function isBottomEdge(): bool {
        return Plasmoid.location === PlasmaCore.Types.BottomEdge;
    }

    fullRepresentation: Item {
        id: wrapper

        property alias searchField: searchField
        property alias listView: listView

        property int minimumHeight: listView.count > 0
            ? listView.contentHeight + searchField.height + 5
            : searchField.height

        property int maximumHeight: minimumHeight

        anchors.fill: parent

        SearchField {
            id: searchField

            anchors {
                left: parent.left
                right: parent.right
            }
            onSearchTextChanged: {
                listView.setQueryString(text)
            }
            onClose: mainWidget.expanded = false
        }

        Milou.ResultsView {
            id: listView

            // in case is expanded
            clip: true

            anchors {
                left: parent.left
                right: parent.right
            }

            reversed: mainWidget.isBottomEdge()

            onActivated: {
                searchField.text = "";
                mainWidget.expanded = false;
            }

            onUpdateQueryString: (text, cursorPosition) => {
                searchField.text = text;
                searchField.cursorPosition = cursorPosition;
            }
        }

        Component.onCompleted: {
            //plasmoid.settingsChanged.connect(loadSettings)

            if (!mainWidget.isBottomEdge()) {
                // Normal view
                searchField.anchors.top = wrapper.top
                listView.anchors.top = searchField.bottom
                listView.anchors.bottom = wrapper.bottom
            } else {
                // When on the bottom
                listView.anchors.top = wrapper.top
                listView.anchors.bottom = searchField.top
                searchField.anchors.bottom = wrapper.bottom
            }
        }
    }

    Timer {
        id: theFocusDoesNotAlwaysWorkTimer
        interval: 100
        repeat: false

        onTriggered: {
            mainWidget.setTextFieldFocus();
        }
    }

    function setTextFieldFocus() {
        mainWidget.fullRepresentationItem.searchField.setFocus();
        mainWidget.fullRepresentationItem.searchField.selectAll();
    }

    function loadSettings() {
        mainWidget.fullRepresentationItem.listView.loadSettings();
    }

    onExpandedChanged: {
        setTextFieldFocus();
        //
        // The focus is not always set correctly. The hunch is that this
        // function is called before the popup is actually visible and
        // therfore the setFocus call does not do anything. So, we are using
        // a small timer and calling the setTextFieldFocus function again.
        //
        theFocusDoesNotAlwaysWorkTimer.start()
    }
}

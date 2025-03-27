/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013-2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.milou as Milou
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

import "globals.js" as Globals

PlasmoidItem {
    id: mainWidget

    switchWidth: Globals.SwitchWidth
    switchHeight: Globals.SwitchWidth

    function isBottomEdge(): bool {
        return Plasmoid.location === PlasmaCore.Types.BottomEdge;
    }

    fullRepresentation: ColumnLayout {
        id: wrapper

        property alias searchField: searchField
        property alias listView: listView

        property int minimumHeight: listView.count > 0
            ? listView.contentHeight + searchField.height + spacing
            : searchField.height

        property int maximumHeight: minimumHeight

        Layout.minimumWidth: Globals.PlasmoidWidth
        Layout.maximumWidth: Globals.PlasmoidWidth
        Layout.minimumHeight: minimumHeight
        Layout.maximumHeight: maximumHeight

        spacing: Kirigami.Units.smallSpacing

        SearchField {
            id: searchField

            Layout.fillWidth: true

            onSearchTextChanged: {
                listView.setQueryString(text)
            }
            onClose: mainWidget.expanded = false
        }

        Milou.ResultsView {
            id: listView
            queryField: searchField.queryField

            // in case is expanded
            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true

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

        function changeLocation(): void {
            if (mainWidget.isBottomEdge()) {
                // Search field is at the bottom
                children = [listView, searchField];
            } else {
                // Search field is on top
                children = [searchField, listView];
            }
        }

        Component.onCompleted: {
            Plasmoid.locationChanged.connect(changeLocation);
            changeLocation();
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

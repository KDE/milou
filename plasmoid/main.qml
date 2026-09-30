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

    readonly property bool isBottomEdge: Plasmoid.location === PlasmaCore.Types.BottomEdge

    switchWidth: Globals.SwitchWidth
    switchHeight: Globals.SwitchWidth

    fullRepresentation: ColumnLayout {
        id: wrapper

        property alias searchField: searchField
        property alias listView: listView

        property int minimumHeight: implicitHeight
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

            Keys.forwardTo: listView.count > 0 ? [queryField, listView] : queryField
            Component.onCompleted: {
                queryField.Keys.onUpPressed.connect(event => {
                    listView.currentIndex = mainWidget.isBottomEdge ? 1 : listView.count - 1
                    event.accepted = listView.count === 0 // pass to KeyNavigation if we have results
                })
                queryField.KeyNavigation.up = listView

                queryField.Keys.onDownPressed.connect(event => {
                    listView.currentIndex = mainWidget.isBottomEdge ? listView.count - 1 : 1
                    event.accepted = listView.count === 0 // pass to KeyNavigation if we have results
                })
                queryField.KeyNavigation.down = listView
            }
        }

        LayoutItemProxy {
            target: searchField
            visible: !mainWidget.isBottomEdge
        }

        Milou.ResultsView {
            id: listView
            queryField: searchField.queryField

            // in case is expanded
            clip: true
            activeFocusOnTab: count > 0
            keyNavigationWraps: false

            Layout.fillWidth: true
            Layout.fillHeight: true
            implicitHeight: contentHeight

            reversed: mainWidget.isBottomEdge

            onActivated: {
                searchField.text = "";
                mainWidget.expanded = false;
            }

            onUpdateQueryString: (text, cursorPosition) => {
                searchField.text = text;
                searchField.cursorPosition = cursorPosition;
            }
        }

        LayoutItemProxy {
            target: searchField
            visible: mainWidget.isBottomEdge
        }
    }

    function resetFocusAndCurrentItem() {
        mainWidget.fullRepresentationItem.searchField.setFocus();
        mainWidget.fullRepresentationItem.searchField.selectAll();
        mainWidget.fullRepresentationItem.listView.currentIndex = 0;
    }

    onExpandedChanged: expanded => {
        if (expanded) {
            // callLater as the window is not visible yet. We can't use the usual trick
            // of doing it on !expanded, as the text selection doesn't persist
            Qt.callLater(resetFocusAndCurrentItem);
        }
    }
}

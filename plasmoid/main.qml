/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013-2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */
pragma ComponentBehavior: Bound

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

        property alias searchField: searchFieldItem
        property alias listView: listView

        property int minimumHeight: implicitHeight
        property int maximumHeight: minimumHeight

        Layout.minimumWidth: Globals.PlasmoidWidth
        Layout.maximumWidth: Globals.PlasmoidWidth
        Layout.minimumHeight: minimumHeight
        Layout.maximumHeight: maximumHeight

        spacing: Kirigami.Units.smallSpacing

        SearchField {
            id: searchFieldItem

            Layout.fillWidth: true

            onSearchTextChanged: {
                listView.setQueryString(text)
            }
            onClose: mainWidget.expanded = false

            Keys.forwardTo: listView.count > 0 ? [queryField, listView] : queryField
            Component.onCompleted: {
                queryField.Keys.onUpPressed.connect(event => {
                    if (event.modifiers == Qt.ControlModifier) {
                        event.accepted = false
                        listView.navigationKeyHandler(event)
                        return;
                    }
                    listView.currentIndex = mainWidget.isBottomEdge ? 1 : listView.count - 1
                    event.accepted = listView.count === 0 // pass to KeyNavigation if we have results
                })
                queryField.KeyNavigation.up = listView

                queryField.Keys.onDownPressed.connect(event => {
                    if (event.modifiers == Qt.ControlModifier) {
                        event.accepted = false
                        listView.navigationKeyHandler(event)
                        return;
                    }
                    listView.currentIndex = mainWidget.isBottomEdge ? listView.count - 1 : 1
                    event.accepted = listView.count === 0 // pass to KeyNavigation if we have results
                })
                queryField.KeyNavigation.down = listView

                queryField.Keys.onPressed.connect(event => {
                    if (listView.count > 0 && event.modifiers | Qt.ControlModifier && (event.key == Qt.Key_J || event.key == Qt.Key_K)) {
                        event.accepted = true
                        if (event.key == Qt.Key_K) {
                            listView.currentIndex = mainWidget.isBottomEdge ? 1 : listView.count - 1
                            listView.forceActiveFocus(Qt.BacktabFocusReason)
                        } else {
                            listView.currentIndex = mainWidget.isBottomEdge ? listView.count - 1 : 1
                            listView.forceActiveFocus(Qt.BacktabFocusReason)
                        }
                    }
                })
            }
        }

        Keys.onPressed: event => {
            if (event.modifiers | Qt.ControlModifier && (event.key == Qt.Key_J || event.key == Qt.Key_K)) {
                searchFieldItem.queryField.forceActiveFocus(event.key == Qt.Key_K ? Qt.BacktabFocusReason : Qt.TabFocusReason)
            }
        }

        LayoutItemProxy {
            target: searchFieldItem
            visible: !mainWidget.isBottomEdge
        }

        Milou.ResultsView {
            id: listView
            queryField: searchFieldItem.queryField

            // in case is expanded
            clip: true
            activeFocusOnTab: count > 0
            keyNavigationWraps: false

            Layout.fillWidth: true
            Layout.fillHeight: true
            implicitHeight: contentHeight

            reversed: mainWidget.isBottomEdge

            onActivated: {
                searchFieldItem.text = "";
                mainWidget.expanded = false;
            }

            onUpdateQueryString: (text, cursorPosition) => {
                searchFieldItem.text = text;
                searchFieldItem.queryField.cursorPosition = cursorPosition;
            }
        }

        LayoutItemProxy {
            target: searchFieldItem
            visible: mainWidget.isBottomEdge
        }

        function resetFocusAndCurrentItem() {
            wrapper.searchField.setFocus();
            wrapper.searchField.selectAll();
            wrapper.listView.currentIndex = 0;
        }

        Connections {
            target: mainWidget
            function onExpandedChanged(expanded) {
                if (expanded) {
                    // callLater as the window is not visible yet. We can't use the usual trick
                    // of doing it on !expanded, as the text selection doesn't persist
                    Qt.callLater(wrapper.resetFocusAndCurrentItem);
                }
            }
        }
    }
}

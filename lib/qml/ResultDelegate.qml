/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013-2014 Vishesh Handa <me@vhanda.in>
 * Copyright (C) 2015 Kai Uwe Broulik <kde@privat.broulik.de>
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

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kquickcontrolsaddons 2.0 as QtExtra
import "globals.js" as Globals

MouseArea {
    id: resultDelegate

    property variant theModel: model

    readonly property bool isCurrent: ListView.isCurrentItem // cannot properly Connect {} to this

    property int activeAction: -1

    property string typeText: {
        var currentType = model.type
        var reversed = resultDelegate.ListView.view.reversed
        var nextIndex = model.index + (reversed ? 1 : -1)
        var nextType = resultDelegate.ListView.view.model.getType(nextIndex)

        if (nextType != currentType) {
            return String(currentType)
        } else {
            return ""
        }
    }

    onIsCurrentChanged: {
        if (!isCurrent) {
            activeAction = -1
        }
    }

    function activateNextAction() {
        if (activeAction === actionsRepeater.count - 1) { // last action, do nothing
            return false
        }
        ++activeAction
        return true
    }

    function activatePreviousAction() {
        if (activeAction < 0) { // no action, do nothing
            return false
        }
        --activeAction
        return true
    }

    function activateLastAction() {
        activeAction = actionsRepeater.count - 1
    }

    width: listItem.implicitWidth
    height: listItem.implicitHeight

    hoverEnabled: true
    onEntered: {
        listView.currentIndex = index
    }

    PlasmaComponents.Label {
        id: typeText
        text: resultDelegate.typeText
        color: theme.textColor
        opacity: 0.5

        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight

        width: Globals.CategoryWidth - Globals.CategoryRightMargin
        anchors {
            left: parent.left
            verticalCenter: listItem.verticalCenter
        }
    }

    PlasmaComponents.ListItem {
        id: listItem
        enabled: true

        onClicked: {
            listView.currentIndex = model.index
            listView.runCurrentIndex()
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: Globals.CategoryWidth
            }
            height: typePixmap.height

            RowLayout {
                anchors {
                    left: parent.left
                    right: actionsRow.left
                    rightMargin: units.smallSpacing
                }

                QtExtra.QIconItem {
                    id: typePixmap
                    width: Globals.IconSize
                    height: Globals.IconSize

                    icon: model.decoration
                    smooth: true
                }

                PlasmaComponents.Label {
                    id: displayLabel
                    text: String(typeof modelData !== "undefined" ? modelData : model.display)

                    height: typePixmap.height

                    elide: Text.ElideMiddle
                    maximumLineCount: 1

                    Layout.maximumWidth: parent.width - displayLabel.x
                }

                PlasmaComponents.Label {
                    id: subtextLabel
                    text: model.isDuplicate > 1 ? String(model.subtext) : ""

                    color: theme.textColor
                    opacity: 0.3

                    height: typePixmap.height

                    elide: Text.ElideRight
                    maximumLineCount: 1
                    Layout.fillWidth: true
                }
            }

            Row {
                id: actionsRow
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                Repeater {
                    id: actionsRepeater
                    model: typeof actions !== "undefined" ? actions : 0

                    PlasmaComponents.ToolButton {
                        width: height
                        height: listItem.height
                        visible: modelData.visible
                        enabled: modelData.enabled
                        tooltip: modelData.text
                        checkable: checked
                        checked: resultDelegate.activeAction === index

                        PlasmaCore.IconItem {
                            anchors.centerIn: parent
                            width: Globals.IconSize
                            height: Globals.IconSize
                            // ToolButton cannot cope with QIcon
                            source: modelData.icon
                            active: parent.hovered || parent.checked
                        }

                        onClicked: listView.runAction(index)
                    }
                }
            }
        }
    }
}

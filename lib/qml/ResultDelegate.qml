/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013-2014 Vishesh Handa <me@vhanda.in>
 * Copyright (C) 2015-2016 Kai Uwe Broulik <kde@privat.broulik.de>
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
    readonly property bool sectionHasChanged: typeof reversed !== "undefined" && (
                                                  (reversed && ListView.section != ListView.nextSection)
                                                    || (!reversed && ListView.section != ListView.previousSection)
                                                  )

    property int activeAction: -1

    property string typeText: sectionHasChanged ? ListView.section : ""
    property var additionalActions: typeof actions !== "undefined" ? actions : []

    property bool __pressed: false
    property int __pressX: -1
    property int __pressY: -1

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

    acceptedButtons: Qt.LeftButton
    hoverEnabled: true
    onEntered: {
        listView.currentIndex = index
    }

    onPressed: {
        __pressed = true;
        __pressX = mouse.x;
        __pressY = mouse.y;
    }

    onReleased: {
        if (__pressed) {
            listView.currentIndex = model.index
            listView.runCurrentIndex()
        }

        __pressed = false;
        __pressX = -1;
        __pressY = -1;
    }

    onPositionChanged: {
        if (__pressX != -1 && typeof dragHelper !== "undefined" && dragHelper.isDrag(__pressX, __pressY, mouse.x, mouse.y)) {
            var mimeData = ListView.view.model.getMimeData(index);
            if (mimeData) {
                dragHelper.startDrag(root, mimeData, model.decoration);
                __pressed = false;
                __pressX = -1;
                __pressY = -1;
            }
        }
    }

    onContainsMouseChanged: {
        if (!containsMouse) {
            __pressed = false;
            __pressX = -1;
            __pressY = -1;
        }
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

        // fake pressed look
        checked: resultDelegate.pressed
        separatorVisible: resultDelegate.sectionHasChanged
                       && !resultDelegate.isCurrent
                       && (index === 0 || resultDelegate.ListView.view.currentIndex !== (index - 1))

        Item {
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: Globals.CategoryWidth
            }
            height: Math.max(typePixmap.height, displayLabel.height, subtextLabel.height)

            RowLayout {
                anchors {
                    left: parent.left
                    right: actionsRow.left
                    rightMargin: units.smallSpacing
                }

                PlasmaCore.IconItem {
                    id: typePixmap
                    Layout.preferredWidth: Globals.IconSize
                    Layout.preferredHeight: Globals.IconSize
                    Layout.fillHeight: true
                    source: model.decoration
                    usesPlasmaTheme: false
                    animated: false
                }

                PlasmaComponents.Label {
                    id: displayLabel
                    text: String(typeof modelData !== "undefined" ? modelData : model.display)

                    height: undefined

                    elide: Text.ElideMiddle
                    wrapMode: Text.NoWrap
                    verticalAlignment: Text.AlignVCenter

                    Layout.maximumWidth: parent.width - displayLabel.x
                }

                PlasmaComponents.Label {
                    id: subtextLabel
                    text: model.isDuplicate > 1 || resultDelegate.isCurrent ? String(model.subtext || "") : ""

                    color: theme.textColor
                    opacity: 0.3

                    height: undefined

                    elide: Text.ElideMiddle
                    wrapMode: Text.NoWrap
                    verticalAlignment: Text.AlignVCenter
                    Layout.fillWidth: true
                }
            }

            Row {
                id: actionsRow
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: resultDelegate.isCurrent

                Repeater {
                    id: actionsRepeater
                    model: resultDelegate.additionalActions

                    PlasmaComponents.ToolButton {
                        width: height
                        height: listItem.height
                        visible: modelData.visible || true
                        enabled: modelData.enabled || true
                        tooltip: modelData.text || ""
                        checkable: checked
                        checked: resultDelegate.activeAction === index

                        PlasmaCore.IconItem {
                            anchors.centerIn: parent
                            width: Globals.IconSize
                            height: Globals.IconSize
                            // ToolButton cannot cope with QIcon
                            source: modelData.icon || ""
                            active: parent.hovered || parent.checked
                        }

                        onClicked: resultDelegate.ListView.view.runAction(index)
                    }
                }
            }
        }
    }
}

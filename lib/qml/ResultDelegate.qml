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
import org.kde.kquickcontrolsaddons 2.0 as QtExtra
import "globals.js" as Globals

MouseArea {
    id: resultDelegate
    property variant theModel: model

    width: listItem.implicitWidth
    height: listItem.implicitHeight

    hoverEnabled: true
    onEntered: {
        listView.currentIndex = index
    }

    function fetchTypeText() {
        var currentType = model.type
        var reversed = resultDelegate.ListView.view.reversed
        var nextIndex = model.index + (reversed ? 1 : -1)
        var nextType = resultDelegate.ListView.view.model.getType(nextIndex)

        if (nextType != currentType)
            return currentType
        else
            return ""
    }

    PlasmaComponents.Label {
        id: typeText
        text: String(fetchTypeText())
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

        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: Globals.CategoryWidth
            height: typePixmap.height

            QtExtra.QIconItem {
                id: typePixmap
                width: Globals.IconSize
                height: Globals.IconSize

                icon: model.decoration
                smooth: true
            }

            PlasmaComponents.Label {
                id: displayLabel
                text: String(model.display)

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
    }
}

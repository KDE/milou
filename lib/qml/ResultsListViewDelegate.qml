/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013-2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

import QtQuick 2.1
import QtQuick.Layouts 1.1

import org.kde.plasma.components 2.0 as PlasmaComponents // for ListItem
import org.kde.plasma.components 3.0 as PlasmaComponents3
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

    PlasmaComponents.ListItem {
        id: listItem
        enabled: true

        onClicked: {
            listView.currentIndex = model.index
            listView.runCurrentIndex()
        }

        RowLayout {
            spacing: 7

            QtExtra.QIconItem {
                id: typePixmap
                width: Globals.IconSize
                height: Globals.IconSize

                icon: model.decoration
                smooth: true
            }

            ColumnLayout {
                Layout.fillWidth: true

                PlasmaComponents3.Label {
                    id: displayLabel
                    text: String(model.display)

                    elide: Text.ElideMiddle
                    maximumLineCount: 1

                    Layout.maximumWidth: parent.width - displayLabel.x
                }

                PlasmaComponents3.Label {
                    id: subtextLabel
                    text: String(model.subtext)

                    opacity: 0.3

                    elide: Text.ElideRight
                    maximumLineCount: 1
                    Layout.fillWidth: true
                }
            }
        }
    }
}

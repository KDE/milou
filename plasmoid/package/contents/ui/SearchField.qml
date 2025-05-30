/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013-2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

import QtQuick

import org.kde.plasma.components as PlasmaComponents3
import "globals.js" as Globals

/*
 * The SearchField is a simple text field widget. The only complex part
 * is the internal timer to reduce the number of textChanged signals
 * using searchTextChanged.
 */
Item {
    id: root

    property alias text: textField.text
    readonly property var queryField: textField

    signal searchTextChanged()
    signal close()

    implicitHeight: textField.height
    implicitWidth: Globals.PlasmoidWidth

    PlasmaComponents3.TextField {
        id: textField
        clearButtonShown: true
        placeholderText: i18n("Search…")
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        focus: true
        Keys.forwardTo: listView

        // We do not want to send the text instantly as that would result
        // in too many queries. Therefore we add a small 200msec delay
        Timer {
            id: timer
            interval: 200
            onTriggered: root.searchTextChanged()
        }

        onTextChanged: timer.restart()
    }

    function selectAll(): void {
        textField.selectAll();
    }

    function setFocus(): void {
        textField.focus = true;
    }

    Keys.onEscapePressed: event => {
        if (textField.text !== "") {
            textField.text = "";
        } else {
            close();
        }
    }
}

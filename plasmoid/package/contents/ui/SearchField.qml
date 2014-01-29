import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra
import org.kde.milou 0.1 as Milou

import "../code/globals.js" as Globals

Item {
    signal textChanged()
    property alias text: textField.text

    height: childrenRect.height

    PlasmaComponents.Label {
        id: searchText
        anchors {
            left: parent.left
            right: textField.left
            top: parent.top

            rightMargin: 10
        }
        horizontalAlignment: Text.AlignRight
        width: Globals.CategoryComponentWidth
        text: i18n("Search")
    }

    PlasmaComponents.TextField {
        id: textField
        clearButtonShown: true
        anchors {
            right: parent.right
            top: parent.top
        }
        // vHanda: If you know a better way of aligning everything, feel free to change this
        width: Globals.PlasmoidWidth - Globals.CategoryComponentWidth

        focus: true

        Keys.forwardTo: listView

        Timer {
            id: timer
            interval: 200
            onTriggered: textChanged()
        }

        onTextChanged: timer.restart()
    }

    function selectAll() {
        textField.selectAll()
    }

    function setFocus() {
        textField.focus = true
    }
}

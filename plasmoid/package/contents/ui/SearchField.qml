import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import "../code/globals.js" as Globals

/*
 * The SearchField is a simple "   Search |___input___|" widget.
 * The only complex part is aligning the "Search" text on the right
 * and an internal timer to reduce the number of textChanged signals
 */
Item {
    signal textChanged()
    property alias text: textField.text

    height: childrenRect.height
    width: Globals.PlasmoidWidth

    PlasmaComponents.Label {
        id: searchText
        anchors {
            left: parent.left
            top: parent.top
        }
        // We cannot use anchors.rightMargin as this has an anchor on
        // left, so the rightMargin has no effect.
        // Because of this we also need to apply an appropriate leftMargin
        // on the textField below
        width: Globals.CategoryWidth - Globals.CategoryRightMargin

        horizontalAlignment: Text.AlignRight
        text: i18n("Search")
    }

    PlasmaComponents.TextField {
        id: textField
        clearButtonShown: true
        anchors {
            left: searchText.right
            right: parent.right
            top: parent.top

            leftMargin: Globals.CategoryRightMargin
        }

        focus: true
        Keys.forwardTo: listView

        // We do not want to send the text instantly as that would result
        // in too many queries. Therefore we add a small 200msec delay
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

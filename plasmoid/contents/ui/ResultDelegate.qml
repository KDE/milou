import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1 as QtExtra

PlasmaComponents.ListItem {
    id: resultDelegate
    enabled: true

    // FIXME
    // This has been done so that the current item looks selected
    // We need a proper fix in PlasmaComponents.ListItem to show the currentItem
    // as selected
    opacity: ListView.isCurrentItem ? 0.5 : 1

    // Try to find some signal called activated? and use that?
    onClicked: {
        Qt.openUrlExternally(model.url)
    }
    Keys.onReturnPressed: {
        Qt.openUrlExternally(model.url)
    }

    QtExtra.QIconItem {
        id: typePixmap
        width: 16
        height: 16

        icon: model.decoration
        smooth: true

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: 80
        }
    }

    PlasmaComponents.Label {
        id: displayLabel
        text: model.display
        wrapMode: Text.Wrap

        width: parent.width
        height: 16

        elide: Text.ElideRight
        maximumLineCount: 1

        anchors {
            left: typePixmap.right
            leftMargin: 5
            verticalCenter: parent.verticalCenter
            right: parent.right
        }
    }
}

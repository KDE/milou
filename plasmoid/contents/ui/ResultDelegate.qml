import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1 as QtExtra

PlasmaComponents.ListItem {
    id: resultDelegate
    enabled: true

    onClicked: {
        Qt.openUrlExternally(model.url)
    }

    QtExtra.QIconItem {
        id: typePixmap
        width: 24
        height: 24

        icon: model.decoration
        smooth: true

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
    }

    PlasmaComponents.Label {
        id: displayLabel
        text: model.display
        wrapMode: Text.WordWrap

        anchors {
            left: typePixmap.right
            leftMargin: 5
            verticalCenter: parent.verticalCenter
            right: parent.right
        }
    }
}

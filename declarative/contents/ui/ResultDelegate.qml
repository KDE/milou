import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1 as QtExtra

PlasmaComponents.ListItem {
    id: resultDelegate
    enabled: true

    onClicked: {
        plasmoid.run(model.index)
    }

    QtExtra.QIconItem {
        id: typePixmap
        width: height
        height: parent.height

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

        anchors {
            left: typePixmap.right
            leftMargin: 5
            verticalCenter: parent.verticalCenter
        }
    }
}

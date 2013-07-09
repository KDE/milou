import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra

Item {
    id: mainWidget
    property int minimumWidth: 200
    property int minimumHeight: 500

    PlasmaComponents.TextField {
        id: searchField
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        focus: true
        placeholderText: i18n("Search..")

        onTextChanged: {
            finderApplet.setSearchText( text )
        }
    }

    PlasmaExtras.ScrollArea {
        anchors {
            top: searchField.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        flickableItem: ListView {
            id: listView
            clip: true

            model: resultsModel
            delegate: ResultDelegate {}

            boundsBehavior: Flickable.StopAtBounds
        }
    }

    Component.onCompleted: {
        plasmoid.aspectRatioMode = IgnoreAspectRatio;
        plasmoid.popupIcon = "nepomuk";
    }
}

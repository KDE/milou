import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1 as QtExtra

Item {
    id: mainWidget

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
            plasmoid.setSearchText( text )
        }
    }

    ListView {
        id: listView
        height: 300

        anchors {
            top: searchField.bottom
        }
        model: resultsModel
        delegate: ResultDelegate {}
    }
}

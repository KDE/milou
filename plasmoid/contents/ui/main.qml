import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra
import org.kde.nepomuk 0.1 as Nepomuk

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

        Timer {
            id: timer
            interval: 200
            onTriggered: resultModel.setQueryString( searchField.text )
        }

        onTextChanged: timer.restart()
    }

    Component {
        id: sectionDelegate
        Item {
            Text {
                id: sectionText
                text: section
                font.bold: true

                x: 5
                y: 7
            }
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

            model: Nepomuk.ResultListModel {
                id: resultModel
                queryString: "Coldplay"
                queryLimit: 5

                onListingStarted: {
                    listView.focus = true
                    listView.currentIndex = 0
                }
            }
            delegate: ResultDelegate {
                width: listView.width
            }
            boundsBehavior: Flickable.StopAtBounds

            section.property: "type"
            section.delegate: sectionDelegate
        }
    }

    Component.onCompleted: {
        plasmoid.aspectRatioMode = IgnoreAspectRatio;
        plasmoid.popupIcon = "nepomuk";

        plasmoid.popupEvent.connect(setTextFieldFocus)
    }

    function setTextFieldFocus(shown) {
        searchField.focus = shown
        if (shown) {
            resultModel.clear()
            searchField.text = ""
        }
    }
}

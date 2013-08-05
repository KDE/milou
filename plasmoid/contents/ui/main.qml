import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra
import org.kde.nepomuk 0.1 as Nepomuk

Item {
    id: mainWidget
    property int minimumWidth: 300
    property int minimumHeight: listView.count ? listView.contentHeight + searchField.height + 5: searchField.height + 20
    property int maximumHeight: listView.count ? listView.contentHeight + searchField.height + 5: searchField.height + 20

    PlasmaComponents.TextField {
        id: searchField
        clearButtonShown: true
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        focus: true
        placeholderText: i18n("Search..")

        Keys.forwardTo: listView

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

    ListView {
        id: listView
        clip: true

        anchors {
            top: searchField.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom

            // vHanda: Random number - Is there some way to use consisten margins?
            //         PlasmaCore.FrameSvg does have margins, but one needs to construct
            //          a PlasmaCore.FrameSvg for that
            topMargin: 5
        }

        model: Nepomuk.SourcesModel {
            id: resultModel
            queryLimit: 20

            //onListingStarted: {
            //    listView.currentIndex = 0
            //}
        }
        delegate: ResultDelegate {
            id: resultDelegate
            width: listView.width

            // See Keys.onReturnPressed
            property variant theModel: model
        }
        //
        // vHanda: Ideally this should have gotten handled in the delagte's onReturnPressed
        // code, but the ListView doesn't seem forward keyboard events to the delgate when
        // it is not in activeFocus. Even manually adds Keys.forwardTo: resultDelegate
        // doesn't make any difference!
        Keys.onReturnPressed: {
            // This is absolutely mental. But it does not seem to work in any other way
            // Ideally something as simple as currentItem.url should work!
            Qt.openUrlExternally(currentItem.theModel.url)
        }

        boundsBehavior: Flickable.StopAtBounds

        section.property: "type"
        section.delegate: sectionDelegate
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

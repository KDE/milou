import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra
import org.kde.milou 0.1 as Milou

Item {
    id: mainWidget
    property int minimumWidth: 450
    property int minimumHeight: wrapper.minimumHeight + wrapper.anchors.topMargin
    property int maximumHeight: wrapper.minimumHeight + wrapper.anchors.topMargin

    // The wrapper just exists for giving an appropriate top margin
    // when it is placed on the top edge of the screen
    Item {
        id: wrapper

        property int minimumHeight: listView.count ? listView.contentHeight + searchField.height + 5: searchField.height + 20
        property int maximumHeight: listView.count ? listView.contentHeight + searchField.height + 5: searchField.height + 20

        anchors {
            fill: parent
            topMargin: plasmoid.isTopEdge() ? 7 : 0
        }

        SearchField {
            id: searchField
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            onTextChanged: {
                listView.setQueryString(text)
            }
        }

        ResultsView {
            id: listView

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
        }

        Component.onCompleted: {
            plasmoid.popupEventSignal.connect(setTextFieldFocus)
            plasmoid.settingsChanged.connect(loadSettings)
        }

        function setTextFieldFocus(shown) {
            searchField.setFocus();
            searchField.selectAll();
        }

        function loadSettings() {
            listView.loadSettings()
        }
    }
}

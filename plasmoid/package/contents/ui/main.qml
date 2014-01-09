import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra
import org.kde.milou 0.1 as Milou

Item {
    id: mainWidget
    property int minimumWidth: 450
    property int minimumHeight: wrapper.minimumHeight + wrapper.anchors.topMargin + wrapper.anchors.bottomMargin
    property int maximumHeight: wrapper.minimumHeight + wrapper.anchors.topMargin + wrapper.anchors.bottomMargin

    // The wrapper just exists for giving an appropriate top/bottom margin
    // when it is placed on the top/bottom edge of the screen
    Item {
        id: wrapper

        property int minimumHeight: listView.count ? listView.contentHeight + searchField.height + 5: searchField.height + 20
        property int maximumHeight: listView.count ? listView.contentHeight + searchField.height + 5: searchField.height + 20

        anchors {
            fill: parent
            topMargin: plasmoid.isTopEdge() ? 7 : 0
            bottomMargin: plasmoid.isBottomEdge() ? 7 : 0
        }

        SearchField {
            id: searchField

            anchors {
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
                left: parent.left
                right: parent.right

                // vHanda: Random number - Is there some way to use consisten margins?
                //         PlasmaCore.FrameSvg does have margins, but one needs to construct
                //          a PlasmaCore.FrameSvg for that
                topMargin: plasmoid.isBottomEdge() ? 0 : 5
                bottomMargin: plasmoid.isBottomEdge() ? 5 : 0
            }
        }

        Component.onCompleted: {
            plasmoid.popupEventSignal.connect(setTextFieldFocus)
            plasmoid.settingsChanged.connect(loadSettings)

            if (!plasmoid.isBottomEdge()) {
                // Normal view
                searchField.anchors.top = wrapper.top
                listView.anchors.top = searchField.bottom
                listView.anchors.bottom = wrapper.bottom
            }
            else {
                // When on the bottom
                listView.anchors.top = wrapper.top
                listView.anchors.bottom = searchField.top
                searchField.anchors.bottom = wrapper.bottom
            }
        }

        function setTextFieldFocus(shown) {
            searchField.setFocus();
            searchField.selectAll();

            if (!shown)
                listView.clearPreview();
        }

        function loadSettings() {
            listView.loadSettings()
        }
    }
}

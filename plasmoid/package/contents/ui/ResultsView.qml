import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra
import org.kde.milou 0.1 as Milou

ListView {
    id: listView
    property alias queryString: resultModel.queryString

    clip: true

    model: Milou.SourcesModel {
        id: resultModel
        queryLimit: 20


        onRowsInserted: {
            listView.currentIndex = 0
        }
    }

    delegate: ResultDelegate {
        id: resultDelegate
        width: listView.width
    }

    //
    // vHanda: Ideally this should have gotten handled in the delagte's onReturnPressed
    // code, but the ListView doesn't seem forward keyboard events to the delgate when
    // it is not in activeFocus. Even manually adding Keys.forwardTo: resultDelegate
    // doesn't make any difference!
    Keys.onReturnPressed: {
        resultModel.run(currentIndex);
        plasmoid.hidePopup()
    }

    Keys.onTabPressed: {
        currentIndex = (currentIndex + 1) % count
    }

    Keys.onBacktabPressed: {
        if (currentIndex > 0)
            currentIndex = currentIndex - 1
        else
            currentIndex = count - 1;
    }

    boundsBehavior: Flickable.StopAtBounds

    PlasmaCore.Theme {
        id: theme
    }

    Component {
        id: sectionDelegate
        Item {
            Text {
                id: sectionText
                text: section
                color: theme.textColor
                opacity: 0.5

                x: 110 - width
                y: 7
            }
        }
    }

    section.property: "type"
    section.delegate: sectionDelegate

    function loadSettings() {
        resultModel.loadSettings()
    }

    function setQueryString(string) {
        resultModel.queryString = string
    }
}

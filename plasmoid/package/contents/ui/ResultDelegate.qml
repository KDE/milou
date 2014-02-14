import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1 as QtExtra
import org.kde.milou 0.1 as Milou
import "../code/globals.js" as Globals

QtExtra.MouseEventListener {
    id: resultDelegate
    property variant theModel: model

    width: listItem.implicitWidth
    height: listItem.implicitHeight

    hoverEnabled: true

    onContainsMouseChanged: {
        if (containsMouse) {
            preview.mimetype = model.previewType;
            preview.url = model.previewUrl;
            urlLabel.text = model.previewLabel

            dialog.delegate = resultDelegate;
        } else {
            dialog.prevDelegate = dialog.delegate
            dialog.delegate = null;
        }
    }

    // FIXME
    // This has been done so that the current item looks selected
    // We need a proper fix in PlasmaComponents.ListItem to show the currentItem
    // as selected
    opacity: ListView.isCurrentItem ? 0.5 : 1

    PlasmaComponents.ListItem {
        id: listItem
        enabled: true

        // Try to find some signal called activated? and use that?
        onClicked: {
            listView.currentIndex = model.index
            listView.model.run(model.index);
            view.clearPreview();
        }

        QtExtra.QIconItem {
            id: typePixmap
            width: Globals.IconSize
            height: Globals.IconSize

            icon: model.decoration
            smooth: true

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: Globals.CategoryWidth
            }
        }

        PlasmaComponents.Label {
            id: displayLabel
            text: model.display
            wrapMode: Text.Wrap

            width: parent.width
            height: typePixmap.height

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
}

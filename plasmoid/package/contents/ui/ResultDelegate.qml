import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1 as QtExtra
import org.kde.milou 0.1 as Milou

PlasmaComponents.ListItem {
    id: resultDelegate
    enabled: true

    // FIXME
    // This has been done so that the current item looks selected
    // We need a proper fix in PlasmaComponents.ListItem to show the currentItem
    // as selected
    opacity: ListView.isCurrentItem ? 0.5 : 1

    // Try to find some signal called activated? and use that?
    onClicked: {
        listView.currentIndex = model.index
        listView.model.run(model.index);
        plasmoid.hidePopup()
    }

    QtExtra.QIconItem {
        id: typePixmap
        width: 16
        height: 16

        icon: model.decoration
        smooth: true

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: 120
        }
    }

    PlasmaComponents.Label {
        id: displayLabel
        text: model.display
        wrapMode: Text.Wrap

        width: parent.width
        height: 16

        elide: Text.ElideRight
        maximumLineCount: 1

        anchors {
            left: typePixmap.right
            leftMargin: 5
            verticalCenter: parent.verticalCenter
            right: parent.right
        }
    }

    // Tooltip
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            dialog.delegate = resultDelegate
            // vHanda: Maybe this should be moved to the preview class?
            var mime = preview.mimetype
            var url = preview.url

            preview.mimetype = model.previewType
            preview.url = model.previewUrl
            if (mime != preview.mimetype || url != preview.url)
                preview.refresh()

            if (preview.loaded)
                dialog.visible = true
        }

        onExited: {
            dialog.visible = false
        }
    }
}

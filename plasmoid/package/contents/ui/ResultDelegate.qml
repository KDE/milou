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
    PlasmaCore.Dialog {
        id: dialog
        mainItem: Milou.Preview {
            id: preview
            width: 100
            height: 100

            onLoadingFinished: {
                dialog.visible = true
            }
            onLoadingFailed: {
                dialog.visible = false
            }
        }

        Component.onCompleted: {
            dialog.setAttribute(Qt.WA_X11NetWmWindowTypeToolTip, true)
            dialog.windowFlags = Qt.Window|Qt.WindowStaysOnTopHint|Qt.X11BypassWindowManagerHint
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled: true

        hoverEnabled: true
        onEntered: {
            //FIXME: We'll need better positioning in the future
            var point = dialog.popupPosition(resultDelegate, Qt.AlignCenter)
            //var point = plasmoid.tooltipPosition(resultDelegate, 100, 100);
            dialog.x = point.x + 50
            dialog.y = point.y

            preview.mimetype = model.previewType
            preview.url = model.previewUrl

            if (preview.loaded)
                dialog.visible = true
        }

        onExited: {
            dialog.visible = false
        }
    }
}

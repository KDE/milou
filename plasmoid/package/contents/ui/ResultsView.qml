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

    model: Milou.ReverseModel {
        sourceModel: Milou.SourcesModel {
            id: resultModel
            queryLimit: 20
        }
        // Is this still required?
        onRowsInserted: {
            if (reversed) {
                // The extra -1 is because the SourcesModel is slightly strange and cannot
                // have an overlapping removeRows when insertingRows
                listView.currentIndex = listView.count - 1 - 1
            }
            else {
                listView.currentIndex = 0
            }
        }
        onModelReset: {
            if (reversed) {
                listView.currentIndex = listView.count - 1
            }
            else {
                listView.currentIndex = 0
            }
        }

        reversed: plasmoid.isBottomEdge()
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
        listView.model.run(currentIndex);
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
        preview.highlight = string
    }

    onCurrentItemChanged: {
        showPreview();
    }

    // Tooltip
    PlasmaCore.Dialog {
        id: dialog
        property Item delegate
        property Item prevDelegate

        mainItem: QtExtra.MouseEventListener {
            hoverEnabled: true

            width: childrenRect.width
            height: childrenRect.height

            onContainsMouseChanged: {
                if (containsMouse) {
                    if (dialog.visible)
                        hideTimer.stop()
                }
                else {
                    hideTimer.start()
                }
            }

            Milou.Preview {
                id: preview

                onLoadingFinished: {
                    if (!dialog.delegate)
                        return

                    var height = preview.height + urlLabel.height + urlLabel.anchors.topMargin
                    var point = plasmoid.tooltipPosition(dialog.delegate, preview.width, height)
                    dialog.x = point.x
                    dialog.y = point.y

                    // dialog.visible = true
                    // We cannot do this because PlasmaCore Dialog is strange. If we just set visible
                    // to true, the width and height are never updated
                    // Therefore we give it time to update its width and height
                    plasmaDialogIsSlowTimer.start();
                }

                Timer {
                    id: plasmaDialogIsSlowTimer
                    // Plasma::Dialog has a timer of 150 internally
                    interval: 155
                    repeat: false

                    onTriggered: {
                        dialog.visible = true
                    }
                }
            }

            PlasmaComponents.Label {
                id: urlLabel
                anchors {
                    top: preview.bottom
                    topMargin: 5
                }
                width: preview.width
                height: 16
                elide: Text.ElideLeft
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Component.onCompleted: {
            dialog.setAttribute(Qt.WA_X11NetWmWindowTypeToolTip, true)
            dialog.windowFlags = Qt.Window|Qt.WindowStaysOnTopHint|Qt.X11BypassWindowManagerHint
        }

        // The delegate changes when the mouse hover starts on an item
        onDelegateChanged: {
            if (delegate) {
                showTimer.start()
                hideTimer.stop()

                if (prevDelegate != delegate) {
                    dialog.visible = false
                }
            }
            else {
                showTimer.stop()
                hideTimer.start()
            }
        }
    }

    Timer {
        id: showTimer
        interval: 340
        repeat: false

        onTriggered: {
            preview.refresh();
        }
    }

    Timer {
        id: hideTimer
        interval: 500
        repeat: false

        onTriggered: {
            clearPreview()
        }
    }

    function clearPreview() {
        dialog.visible = false
        preview.clear()
    }

    function showPreview() {
        preview.mimetype = currentItem.theModel.previewType;
        preview.url = currentItem.theModel.previewUrl;
        urlLabel.text = currentItem.theModel.previewLabel

        dialog.delegate = null
        dialog.delegate = currentItem
    }
}

import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra

Item {
    property string title
    property variant keys
    property variant values
    property int length
    property string iconName

    width: childrenRect.width
    height: childrenRect.height

    Row {

        QtExtra.QIconItem {
            id: iconItem
            width: height
            height: rightSide.height

            icon: iconName
            smooth: true
        }

        Column {
            id: rightSide
            PlasmaComponents.Label {
                text: title
                height: 32
                color: theme.textColor
                font.pointSize: theme.defaultFont.pointSize * 1.5
            }

            Repeater {
                model: length

                Row {
                    PlasmaComponents.Label {
                        text: keys[index] + " "
                        height: 16
                        color: theme.textColor
                        opacity: 0.5
                    }

                    PlasmaComponents.Label {
                        text: values[index]
                        height: 16
                        color: theme.textColor
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }
}

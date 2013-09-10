import QtQuick 1.1

import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1 as QtExtra

Item {
    property string title
    property string artist
    property string album
    property string duration

    Column {
        PlasmaComponents.Label {
            text: title
            height: 32
            color: theme.textColor
            font.pointSize: theme.defaultFont.pointSize * 1.5
        }

        Row {
            PlasmaComponents.Label {
                text: i18n("Artist: ")
                height: 16
                color: theme.textColor
                opacity: 0.5
            }

            PlasmaComponents.Label {
                text: artist
                height: 16
                color: theme.textColor
                elide: Text.ElideRight
            }
        }

        Row {
            PlasmaComponents.Label {
                text: i18n("Album: ")
                height: 16
                color: theme.textColor
                opacity: 0.5
            }

            PlasmaComponents.Label {
                text: album
                height: 16
                color: theme.textColor
                elide: Text.ElideRight
            }
        }

        Row {
            PlasmaComponents.Label {
                text: i18n("Duration: ")
                height: 16
                color: theme.textColor
                opacity: 0.5
            }

            PlasmaComponents.Label {
                text: duration
                height: 16
                color: theme.textColor
                elide: Text.ElideRight
            }
        }
    }
}

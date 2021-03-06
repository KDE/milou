import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.milou 0.1 as Milou
import org.kde.plasma.components 2.0 as PlasmaComponents

ColumnLayout {
    width: 1000
    height: 1100

    Milou.Preview {
        focus: true
        Layout.fillWidth: true
        Layout.fillHeight: true

        mimetype: "text/plain"
        url: "/home/vishesh/t"

        Component.onCompleted: {
            refresh();
        }
    }
}

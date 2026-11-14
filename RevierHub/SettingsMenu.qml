import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: settingsMenu

    signal closeRequested()
    signal mapSettingsRequested()

    Rectangle {
        x: 30
        y: 30
        width: settingsColumn.implicitWidth + 32
        height: settingsColumn.implicitHeight + 32
        radius: 12
        color: "#E6F3EDE0"
        border.color: "#8066584A"
        border.width: 1

        ColumnLayout {
            id: settingsColumn
            anchors.fill: parent
            anchors.margins: 16
            spacing: 5

            Label {
                text: "Settings"
                font.bold: true
            }

            Button {
                text: "Map settings"
                onClicked: settingsMenu.mapSettingsRequested()
            }

            Button {
                text: "Close"
                onClicked: settingsMenu.closeRequested()
            }
        }
    }
}

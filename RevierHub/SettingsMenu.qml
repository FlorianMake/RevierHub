import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: settingsMenu

    signal closeRequested()
    signal mapSettingsRequested()

    x: 30
    y: 30

    ColumnLayout {
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

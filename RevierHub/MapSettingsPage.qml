import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: mapSettingsPage

    required property var mainWindow
    required property var viewSettings

    signal closeRequested()

    x: 30
    y: 30

    ColumnLayout {
        spacing: 5

        Text {
            text: "Map settings"
        }

        RowLayout {
            Text {
                text: "Map styles"
            }

            ComboBox {
                id: mapStyleComboBox

                Layout.leftMargin: 20
                Layout.preferredWidth: 120
                Layout.preferredHeight: 80

                model: ["Satellite", "Hunting", "Sepia"]
                currentIndex: mainWindow.mapStyle
                onActivated: mainWindow.updateStyle(currentIndex)
            }
        }

        Switch {
            text: "Show distance rings"
            checked: mainWindow.showDistanceRings
            onToggled: viewSettings.showDistanceRings = checked
        }

        Switch {
            text: "Show weather overlay"
            checked: mainWindow.showWeatherOverlay
            onToggled: viewSettings.showWeatherOverlay = checked
        }

        Label {
            text: "Hunting areas"
            font.bold: true
        }

        Button {
            text: mainWindow.drawingArea ? "Drawing area" : "Draw area"
            enabled: !mainWindow.drawingArea && !mainWindow.editingArea
            onClicked: mainWindow.beginDrawing()
        }

        Repeater {
            model: mainWindow.huntingAreas

            delegate: RowLayout {
                required property var modelData
                Layout.maximumWidth: 330

                CheckBox {
                    text: modelData.name
                    checked: mainWindow.isAreaVisible(modelData.id)
                    onToggled: mainWindow.setAreaVisible(modelData.id, checked)
                }

                RadioButton {
                    text: "Primary"
                    checked: mainWindow.primaryAreaId === modelData.id
                    enabled: mainWindow.isAreaVisible(modelData.id)
                    onClicked: mainWindow.setPrimaryArea(modelData.id)
                }

                Button {
                    text: "Edit"
                    onClicked: mainWindow.beginEditing(modelData)
                }

                Button {
                    text: "Delete"
                    onClicked: mainWindow.removeArea(modelData.id)
                }
            }
        }

        Button {
            text: "Back"
            onClicked: mapSettingsPage.closeRequested()
        }
    }
}

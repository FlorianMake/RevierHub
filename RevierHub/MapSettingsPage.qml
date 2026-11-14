import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: mapSettingsPage

    required property var mainWindow
    required property var viewSettings

    signal closeRequested()

    Rectangle {
        id: settingsPanel
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

            Label {
                visible: mainWindow.mapStyle === 1
                text: "Hunting map details"
                font.bold: true
            }

            Switch {
                visible: mainWindow.mapStyle === 1
                text: "Show street names"
                checked: mainWindow.showStreetNames
                onToggled: viewSettings.showStreetNames = checked
            }

            Switch {
                visible: mainWindow.mapStyle === 1
                text: "Show contour lines"
                checked: mainWindow.showContourLines
                onToggled: viewSettings.showContourLines = checked
            }

            Label {
                text: "Distance rings"
                font.bold: true
            }

            Switch {
                text: "Show distance rings"
                checked: mainWindow.showDistanceRings
                onToggled: viewSettings.showDistanceRings = checked
            }

            GridLayout {
                visible: mainWindow.showDistanceRings
                columns: 3

                CheckBox {
                    text: "40 m"
                    checked: mainWindow.showDistanceRing40
                    onToggled: viewSettings.showDistanceRing40 = checked
                }

                CheckBox {
                    text: "50 m"
                    checked: mainWindow.showDistanceRing50
                    onToggled: viewSettings.showDistanceRing50 = checked
                }

                CheckBox {
                    text: "75 m"
                    checked: mainWindow.showDistanceRing75
                    onToggled: viewSettings.showDistanceRing75 = checked
                }

                CheckBox {
                    text: "100 m"
                    checked: mainWindow.showDistanceRing100
                    onToggled: viewSettings.showDistanceRing100 = checked
                }

                CheckBox {
                    text: "150 m"
                    checked: mainWindow.showDistanceRing150
                    onToggled: viewSettings.showDistanceRing150 = checked
                }

                CheckBox {
                    text: "200 m"
                    checked: mainWindow.showDistanceRing200
                    onToggled: viewSettings.showDistanceRing200 = checked
                }

                CheckBox {
                    text: "300 m"
                    checked: mainWindow.showDistanceRing300
                    onToggled: viewSettings.showDistanceRing300 = checked
                }

                CheckBox {
                    text: "500 m"
                    checked: mainWindow.showDistanceRing500
                    onToggled: viewSettings.showDistanceRing500 = checked
                }
            }

            Switch {
                text: "Show information overlay"
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
}

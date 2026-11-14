import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: huntingAreasSettingsPage

    required property var mainWindow

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
                onClicked: huntingAreasSettingsPage.closeRequested()
            }
        }
    }
}

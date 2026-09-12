// MainMenuPage.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: "Main Menu"

    background: Rectangle {
        color: "#233126"
    }

    header: ToolBar {
        height: 56

        background: Rectangle {
            color: "#2C3B2F"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 12

            ToolButton {
                text: "\u25C0"     // ◀
                font.pixelSize: 20

                background: Rectangle {
                    color: "transparent"
                }

                onClicked: stackView.pop()
            }

            Label {
                Layout.fillWidth: true
                text: "Main Menu"
                color: "white"
                font.pixelSize: 20
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }

            // Spacer to keep the title centered
            Item {
                width: 40
                height: 1
            }
        }
    }

    ScrollView {
        anchors.fill: parent

        Column {
            width: parent.width
            spacing: 14
            padding: 16

            MenuCard {
                title: "Map Style"
                subtitle: "Choose map appearance"
                onClicked: stackView.push("MapStylePage.qml")
            }

            MenuCard {
                title: "Hunting District"
                subtitle: "Manage hunting areas"
                onClicked: stackView.push("HuntingDistrictPage.qml")
            }

            MenuCard {
                title: "Border Regulations"
                subtitle: "Temporary restricted areas"
                onClicked: stackView.push("BorderRegulationsPage.qml")
            }
        }
    }
}

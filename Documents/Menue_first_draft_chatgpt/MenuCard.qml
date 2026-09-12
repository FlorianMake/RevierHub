import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Frame {
    id: root

    property alias title: titleLabel.text
    property alias subtitle: subtitleLabel.text

    signal clicked()

    width: parent.width
    height: 82

    background: Rectangle {
        radius: 14
        color: "#3A4A3D"
        border.color: "#516352"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16

        Rectangle {
            width: 38
            height: 38
            radius: 8
            color: "#526556"
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Label {
                id: titleLabel
                color: "white"
                font.pixelSize: 18
                font.bold: true
            }

            Label {
                id: subtitleLabel
                color: "#D0D0D0"
                font.pixelSize: 13
            }
        }

        Label {
            text: "❯"
            color: "white"
            font.pixelSize: 20
        }
    }
}

import QtQuick
import QtQuick.Controls

Image {
    id: testImage
    x: 50
    y: 15

    width: 20
    height: 40
    source: "qrc:/qt/qml/RevierHub/Resources/Menue_draft.png"

    MouseArea {
        anchors.fill: parent
        onClicked: {
            menueStack.push("SettingsMenue.qml")
        }
    }
}
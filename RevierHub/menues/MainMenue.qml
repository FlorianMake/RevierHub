import QtQuick
import QtQuick.Controls

Image {
    id: testImage
    width: 30
    height: 50
    source: "qrc:/qt/qml/RevierHub/Resources/Menue_draft.png"

    MouseArea {
        anchors.fill: parent
        onClicked: {
            menueStack.push("SettingsMenue.qml")
        }
    }
}
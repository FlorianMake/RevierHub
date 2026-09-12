import QtQuick
import QtQuick.Controls

Item {
    id: page

    width: 800
    height: 600

    Button {
        x:30
        y:50
        text: "Closing"
        onClicked: {
            console.log("Close clicked, StackView.view is:", page.StackView.view)
            page.StackView.view.pop(null)
        }
    }


    Image {
        id: testImage
        x: 150
        y: 115

        source: "qrc:/qt/qml/RevierHub/Resources/Close_draft.png"
    }
}
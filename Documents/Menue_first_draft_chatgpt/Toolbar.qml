header: ToolBar {
    background: Rectangle { color: "#2C3B2F" }

    RowLayout {
        anchors.fill: parent

        ToolButton {
            icon.name: "go-previous"    // or text: "←"
            onClicked: stackView.pop()
        }

        Label {
            Layout.fillWidth: true
            text: qsTr("Main Menu")
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            font.pixelSize: 20
            font.bold: true
        }

        Item { width: 40 }
    }
}

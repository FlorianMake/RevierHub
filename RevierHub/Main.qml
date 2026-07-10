import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

ApplicationWindow {
    visible: true
    width: Screen.width
    height: Screen.height

    Plugin {
        id: mapPlugin
        name: "maplibre"
        PluginParameter {
            name: "maplibre.map.styles"
            value: "https://tiles.openfreemap.org/styles/liberty"
        }
    }

    Item{
        id:myItem
        property double newScale
        property double lastScale
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: liveLocation.newCenter
        zoomLevel: 18

        MapQuickItem {
            coordinate: liveLocation.coordinate
            anchorPoint.x: 5
            anchorPoint.y: 5

            sourceItem: Rectangle {
                width: 10
                height: 10
                color: "red"
                radius: 5
            }
        }
        MapPolyline {
            line.width: 3
            line.color: "#009E75"
            path: trails.currentTrail
        }

        DragHandler {
            id: drag
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
         }

        PinchHandler {
            target: null
            maximumScale: 22
            minimumScale: 3

            onScaleChanged: {
                console.log("scale: " + scale)

                if(scale < (11 + 8)) {
                    map.zoomLevel = (scale + 8)
                }
                else if( scale > (30 + 8) ) {
                    map.zoomLevel = (30 + 8)
                }
                else {
                    map.zoomLevel = (scale + 8)
                }
            }
        }
    }

    Image {
        id: testImage
        width: 60
        height: 100
        source: "qrc:/qt/qml/RevierHub/Resources/Menue_draft.png"

        onStatusChanged: {
            console.log(source, status, Image.Error)
        }
    }

    Button {
        id: mainMenueBtn
        x: 50
        y: 5

        icon.source: "qrc:/qt/qml/RevierHub/Resources/Menue_draft.png"
    }

    // debug overlay — sits on top
    /* Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        width: debugText.width + 20
        height: debugText.height + 16
        color: "#AA000000"  // semi-transparent black
        radius: 8

        Text {
            id: debugText
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 12
            text: "Pts: " + trails.trailLength + " coord: " + liveLocation.coordinate +" cnt: " + pointCount
        }
    }*/
}
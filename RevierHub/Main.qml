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
            target: null    // handle it manually

            onScaleChanged: {
                console.log("scale original: " + scale)

                if(scale > 1) {
                    myItem.newScale = scale - ((scale-1) / 10)
                }
                else {
                    myItem.newScale = scale + scale - ((0.9) / 10)
                }

                console.log("scale recalculated: " + myItem.newScale)

                map.zoomLevel = myItem.newScale
            }
        }

        /* PinchHandler {
            target: null    // handle it manually

            onScaleChanged: {
                console.log("scale original: " + scale)

                if(scale > 1) {
                    myItem.newScale = scale - ((scale-1) / 10)
                }
                else {
                    myItem.newScale = scale + scale - ((0.9) / 10)
                }

                console.log("scale recalculated: " + myItem.newScale)

                map.zoomLevel *= myItem.newScale
            }
        }*/

        /*PinchHandler {
            onActiveChanged: {
                if (active)
                    startZoom = map.zoomLevel
            }

            onScaleChanged: {
                map.zoomLevel = startZoom * scale
            }
        }*/
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
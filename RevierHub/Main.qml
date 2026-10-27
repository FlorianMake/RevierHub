import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning
import QtQuick.Layouts
import QtCore

// import "menues"


ApplicationWindow {
    id:mainWindow
    visible: true
    width: Screen.width
    height: Screen.height



    //SettingsMenue {
    //    id: settingsMenue
    //}
    Item{
        id:settingsMenue

        x:30
        y:30


        ColumnLayout {
            spacing: 5

            Text{
                text: "Map settings"
            }

            RowLayout {
                Text{
                    text: "Map styles"
                }

                ComboBox {
                    id: mapStyleComboBox

                    Layout.leftMargin: 20
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 80

                    model: ["Sepia", "Satellite"]
                    currentIndex: mainWindow.mapStyle
                    onActivated: mainWindow.updateStyle(currentIndex)
                }
            }

            Switch {
                text: "Show distance rings"
                checked: mainWindow.showDistanceRings
                onToggled: viewSettings.showDistanceRings = checked
            }

            Button {
                id: closingButton

                text: "Closing"
                onClicked: {
                    menueStack.pop()
                }
            }
        }
    }

    //MainMenue {
    //    id: mainMenue
    //}
    Image {
        id: mainMenue

        x:30
        y:30

        width: 40
        height: 60

        source: "qrc:/qt/qml/RevierHub/Resources/Menue_draft.png"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                menueStack.push(settingsMenue)
            }
        }
    }



    Settings {
        id: viewSettings
        category: "view"
        property int mapStyle: 0
        property bool showDistanceRings: true
    }

    property int mapStyle: viewSettings.mapStyle
    property bool showDistanceRings: viewSettings.showDistanceRings
    onMapStyleChanged: applyMapStyle()

    Plugin {
        id: mapPlugin
        name: "maplibre"
        PluginParameter {
            name: "maplibre.map.styles"
            value: "https://tiles.openfreemap.org/styles/liberty,"
                 + "https://api.maptiler.com/maps/satellite/style.json?key=BCUiLcvKmomeRGpKOIea"
        }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: liveLocation.newCenter
        zoomLevel: 18
        property real pinchStartZoom: zoomLevel

        onSupportedMapTypesChanged: mainWindow.applyMapStyle()

        MapCircle {
            center: liveLocation.coordinate
            radius: 200
            color: "transparent"
            border.color: "#4A4A4A"
            border.width: 2
            visible: mainWindow.showDistanceRings
        }

        MapCircle {
            center: liveLocation.coordinate
            radius: 100
            color: "transparent"
            border.color: "#4A4A4A"
            border.width: 2
            visible: mainWindow.showDistanceRings
        }

        MapCircle {
            center: liveLocation.coordinate
            radius: 50
            color: "transparent"
            border.color: "#4A4A4A"
            border.width: 2
            visible: mainWindow.showDistanceRings
        }

        MapQuickItem {
            coordinate: liveLocation.coordinate.atDistanceAndAzimuth(100, 0)
            anchorPoint.x: distanceLabel.width / 2
            anchorPoint.y: distanceLabel.height
            visible: mainWindow.showDistanceRings

            sourceItem: Text {
                id: distanceLabel
                text: "100 m"
                color: "#4A4A4A"
                font.bold: true
                font.pixelSize: 10
            }
        }

        MapQuickItem {
            coordinate: liveLocation.coordinate.atDistanceAndAzimuth(200, 0)
            anchorPoint.x: outerDistanceLabel.width / 2
            anchorPoint.y: outerDistanceLabel.height
            visible: mainWindow.showDistanceRings

            sourceItem: Text {
                id: outerDistanceLabel
                text: "200 m"
                color: "#4A4A4A"
                font.bold: true
                font.pixelSize: 10
            }
        }

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
            onActiveChanged: {
                if (active)
                    map.pinchStartZoom = map.zoomLevel
            }
            onScaleChanged: map.zoomLevel = Math.max(
                                map.minimumZoomLevel,
                                Math.min(map.maximumZoomLevel,
                                         map.pinchStartZoom + Math.log2(scale)))
        }
    }

    function updateStyle(styleIndex) {
        viewSettings.mapStyle = styleIndex
    }

    function applyMapStyle() {
        if (mapStyle >= 0 && mapStyle < map.supportedMapTypes.length)
            map.activeMapType = map.supportedMapTypes[mapStyle]
    }

    StackView {
        id: menueStack
        anchors.fill: parent
        initialItem: mainMenue        // <-- object reference now, not "menues/MainMenue.qml"
    }
}
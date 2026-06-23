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
    // liberty for openfreemap...
    // "https://tiles.openfreemap.org/styles/liberty"


    // https://cloud.maptiler.com/maps/
    // https://cloud.maptiler.com/account/keys/

    // satelite picture but quite old...
    // "https://api.maptiler.com/maps/satellite/style.json?key=BCUiLcvKmomeRGpKOIea"



    // "https://basemaps.arcgis.com/arcgis/rest/services/World_Imagery/VectorTileServer"


    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(47.8639, 10.2662)
        zoomLevel: 15
    }
}
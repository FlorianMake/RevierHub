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

                    model: ["Satellite", "Hunting", "Sepia"]
                    currentIndex: mainWindow.mapStyle
                    onActivated: mainWindow.updateStyle(currentIndex)
                }
            }

            Switch {
                text: "Show distance rings"
                checked: mainWindow.showDistanceRings
                onToggled: viewSettings.showDistanceRings = checked
            }

            Label {
                text: "Hunting areas"
                font.bold: true
            }

            Button {
                text: mainWindow.drawingArea ? "Drawing area" : "Draw area"
                enabled: !mainWindow.drawingArea && !mainWindow.editingArea
                onClicked: mainWindow.beginDrawing()
            }

            Repeater {
                model: mainWindow.huntingAreas

                delegate: RowLayout {
                    required property var modelData
                    Layout.maximumWidth: 330

                    CheckBox {
                        text: modelData.name
                        checked: mainWindow.isAreaVisible(modelData.id)
                        onToggled: mainWindow.setAreaVisible(modelData.id, checked)
                    }

                    RadioButton {
                        text: "Primary"
                        checked: mainWindow.primaryAreaId === modelData.id
                        enabled: mainWindow.isAreaVisible(modelData.id)
                        onClicked: mainWindow.setPrimaryArea(modelData.id)
                    }

                    Button {
                        text: "Edit"
                        onClicked: mainWindow.beginEditing(modelData)
                    }

                    Button {
                        text: "Delete"
                        onClicked: mainWindow.removeArea(modelData.id)
                    }
                }
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
        property real mapZoomLevel: 18
        property bool hasLastMapPosition: false
        property real lastMapLatitude: 0
        property real lastMapLongitude: 0
        property bool repositioningEnabled: true
        property var visibleHuntingAreaIds: []
        property int primaryHuntingAreaId: -1
    }

    property int mapStyle: viewSettings.mapStyle
    property bool showDistanceRings: viewSettings.showDistanceRings
    property var huntingAreas: []
    property int primaryAreaId: viewSettings.primaryHuntingAreaId
    property bool drawingArea: false
    property bool drawingMapNavigation: false
    property bool editingArea: false
    property bool areaInteractionActive: drawingArea || editingArea
    property int selectedAreaId: -1
    property int draftAreaId: -1
    property var draftAreaPoints: []
    property var editedAreaPoints: []
    property int areaSettingsRevision: 0
    property int minimumDraftPointDistance: 50
    property int vertexCollapseDistance: 10
    onMapStyleChanged: applyMapStyle()
    onPrimaryAreaIdChanged: viewSettings.primaryHuntingAreaId = primaryAreaId

    function reloadHuntingAreas() {
        var storedAreas = database.huntingAreas()
        var completedAreas = []
        var savedDraft = null
        for (var areaIndex = 0; areaIndex < storedAreas.length; ++areaIndex) {
            if (storedAreas[areaIndex].isDraft)
                savedDraft = storedAreas[areaIndex]
            else
                completedAreas.push(storedAreas[areaIndex])
        }
        huntingAreas = completedAreas
        var knownIds = []
        for (var i = 0; i < huntingAreas.length; ++i)
            knownIds.push(huntingAreas[i].id)
        var visible = []
        for (var j = 0; j < viewSettings.visibleHuntingAreaIds.length; ++j) {
            var id = Number(viewSettings.visibleHuntingAreaIds[j])
            if (knownIds.indexOf(id) !== -1)
                visible.push(id)
        }
        viewSettings.visibleHuntingAreaIds = visible
        if (primaryAreaId !== -1 && knownIds.indexOf(primaryAreaId) === -1)
            primaryAreaId = -1
        if (!drawingArea && savedDraft) {
            draftAreaId = savedDraft.id
            draftAreaPoints = savedDraft.points.slice()
            areaNameInput.text = savedDraft.name
            drawingArea = true
            drawingMapNavigation = false
            viewSettings.repositioningEnabled = false
        }
        areaSettingsRevision++
    }

    function isAreaVisible(areaId) {
        return viewSettings.visibleHuntingAreaIds.indexOf(areaId) !== -1
    }

    function setAreaVisible(areaId, visible) {
        var ids = viewSettings.visibleHuntingAreaIds.slice()
        var index = ids.indexOf(areaId)
        if (visible && index === -1)
            ids.push(areaId)
        else if (!visible && index !== -1)
            ids.splice(index, 1)
        viewSettings.visibleHuntingAreaIds = ids
        if (!visible && primaryAreaId === areaId)
            primaryAreaId = -1
        areaSettingsRevision++
    }

    function setPrimaryArea(areaId) {
        if (!isAreaVisible(areaId))
            setAreaVisible(areaId, true)
        primaryAreaId = areaId
        areaSettingsRevision++
    }

    function beginDrawing() {
        draftAreaId = database.createHuntingAreaDraft("Unfinished hunting area")
        if (draftAreaId < 0)
            return
        drawingArea = true
        editingArea = false
        selectedAreaId = -1
        draftAreaPoints = []
        drawingMapNavigation = false
        viewSettings.repositioningEnabled = false
    }

    function addDrawingPoint(coordinate) {
        if (!drawingArea || drawingMapNavigation)
            return

        if (draftAreaPoints.length > 0) {
            var previousPosition = map.fromCoordinate(
                        draftAreaPoints[draftAreaPoints.length - 1])
            var currentPosition = map.fromCoordinate(coordinate)
            var distance = Math.hypot(
                        currentPosition.x - previousPosition.x,
                        currentPosition.y - previousPosition.y)
            if (distance < minimumDraftPointDistance)
                return
        }

        var points = draftAreaPoints.slice()
        points.push(coordinate)
        if (database.updateHuntingAreaDraftVertices(draftAreaId, points))
            draftAreaPoints = points
    }

    function undoDrawingPoint() {
        if (!drawingArea || draftAreaPoints.length === 0)
            return
        var points = draftAreaPoints.slice()
        points.pop()
        if (database.updateHuntingAreaDraftVertices(draftAreaId, points))
            draftAreaPoints = points
    }

    function moveDraftPoint(index, coordinate) {
        if (!drawingArea || index < 0 || index >= draftAreaPoints.length)
            return
        var points = draftAreaPoints.slice()
        points[index] = coordinate
        if (database.updateHuntingAreaDraftVertices(draftAreaId, points))
            draftAreaPoints = points
    }

    function insertPoint(points, afterIndex, coordinate) {
        var nextIndex = (afterIndex + 1) % points.length
        var currentPosition = map.fromCoordinate(coordinate)
        var beforePosition = map.fromCoordinate(points[afterIndex])
        var nextPosition = map.fromCoordinate(points[nextIndex])
        if (Math.hypot(currentPosition.x - beforePosition.x,
                       currentPosition.y - beforePosition.y) < minimumDraftPointDistance
                || Math.hypot(currentPosition.x - nextPosition.x,
                              currentPosition.y - nextPosition.y) < minimumDraftPointDistance)
            return points
        var updated = points.slice()
        updated.splice(afterIndex + 1, 0, coordinate)
        return updated
    }

    function edgeHandles(points) {
        var handles = []
        for (var index = 0; index < points.length; ++index) {
            var next = points[(index + 1) % points.length]
            handles.push({
                afterIndex: index,
                coordinate: QtPositioning.coordinate(
                    (points[index].latitude + next.latitude) / 2,
                    (points[index].longitude + next.longitude) / 2)
            })
        }
        return handles
    }

    function insertDraftPoint(afterIndex, coordinate) {
        var points = insertPoint(draftAreaPoints, afterIndex, coordinate)
        if (database.updateHuntingAreaDraftVertices(draftAreaId, points))
            draftAreaPoints = points
    }

    function finishDrawing() {
        if (draftAreaPoints.length < 3)
            return
        if (!database.finishHuntingAreaDraft(
                    draftAreaId, areaNameInput.text, draftAreaPoints))
            return
        setAreaVisible(draftAreaId, true)
        if (primaryAreaId === -1)
            primaryAreaId = draftAreaId
        areaNameInput.text = ""
        draftAreaPoints = []
        draftAreaId = -1
        drawingArea = false
        drawingMapNavigation = false
        reloadHuntingAreas()
    }

    function cancelDrawing() {
        if (draftAreaId >= 0)
            database.deleteHuntingArea(draftAreaId)
        areaNameInput.text = ""
        draftAreaPoints = []
        draftAreaId = -1
        drawingArea = false
        drawingMapNavigation = false
    }

    function beginEditing(area) {
        selectedAreaId = area.id
        editedAreaPoints = area.points.slice()
        drawingArea = false
        editingArea = true
        viewSettings.repositioningEnabled = false
    }

    function moveEditedPoint(index, coordinate) {
        if (!editingArea || index < 0 || index >= editedAreaPoints.length)
            return false
        var points = editedAreaPoints.slice()
        var candidatePosition = map.fromCoordinate(coordinate)
        for (var pointIndex = 0; pointIndex < points.length; ++pointIndex) {
            if (pointIndex === index)
                continue
            var existingPosition = map.fromCoordinate(points[pointIndex])
            if (Math.hypot(candidatePosition.x - existingPosition.x,
                           candidatePosition.y - existingPosition.y)
                    <= vertexCollapseDistance) {
                if (points.length <= 3)
                    return false
                points.splice(index, 1)
                editedAreaPoints = points
                return true
            }
        }
        points[index] = coordinate
        editedAreaPoints = points
        return false
    }

    function insertEditedPoint(afterIndex, coordinate) {
        editedAreaPoints = insertPoint(editedAreaPoints, afterIndex, coordinate)
    }

    function saveEdit() {
        if (database.updateHuntingAreaVertices(selectedAreaId, editedAreaPoints)) {
            editingArea = false
            selectedAreaId = -1
            editedAreaPoints = []
        }
    }

    function cancelEdit() {
        editingArea = false
        selectedAreaId = -1
        editedAreaPoints = []
    }

    function removeArea(areaId) {
        if (!database.deleteHuntingArea(areaId))
            return
        setAreaVisible(areaId, false)
        if (selectedAreaId === areaId)
            cancelEdit()
    }

    Plugin {
        id: mapPlugin
        name: "maplibre"
        PluginParameter {
            name: "maplibre.map.styles"
            value: "https://api.maptiler.com/maps/satellite/style.json?key=BCUiLcvKmomeRGpKOIea,"
                 + huntingStyleUrl + ","
                 + "https://tiles.openfreemap.org/styles/liberty"
        }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        zoomLevel: Math.max(11, Math.min(19, viewSettings.mapZoomLevel))

        onSupportedMapTypesChanged: mainWindow.applyMapStyle()
        Component.onCompleted: mainWindow.recenterToLastAcceptedPosition()

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

        MapItemView {
            model: mainWindow.huntingAreas
            delegate: MapPolygon {
                required property var modelData
                path: modelData.points
                color: "transparent"
                border.color: "#F57C00"
                border.width: modelData.id === mainWindow.primaryAreaId ? 3 : 2
                visible: {
                    mainWindow.areaSettingsRevision
                    return mainWindow.isAreaVisible(modelData.id)
                           && !(mainWindow.editingArea
                                && modelData.id === mainWindow.selectedAreaId)
                }
            }
        }

        MapPolyline {
            path: mainWindow.draftAreaPoints
            line.color: "#F57C00"
            line.width: 3
            visible: mainWindow.drawingArea && mainWindow.draftAreaPoints.length >= 2
        }

        MapPolygon {
            path: mainWindow.editedAreaPoints
            color: "transparent"
            border.color: "#F57C00"
            border.width: 3
            visible: mainWindow.editingArea
        }

        MapItemView {
            model: mainWindow.drawingArea ? mainWindow.draftAreaPoints
                                          : mainWindow.editingArea
                                            ? mainWindow.editedAreaPoints : []

            delegate: MapQuickItem {
                id: vertexHandle
                required property int index
                required property var modelData
                coordinate: modelData
                anchorPoint.x: handle.width / 2
                anchorPoint.y: handle.height / 2

                sourceItem: Rectangle {
                    id: handle
                    width: 32
                    height: 32
                    radius: width / 2
                    color: "#FF9800"
                    border.color: "white"
                    border.width: 2

                    DragHandler {
                        id: vertexDrag
                        target: null
                        grabPermissions: PointerHandler.CanTakeOverFromAnything
                        property point startPosition
                        property bool vertexRemoved: false
                        onActiveChanged: {
                            if (active) {
                                startPosition = map.fromCoordinate(vertexHandle.coordinate)
                                vertexRemoved = false
                            }
                        }

                        onTranslationChanged: {
                            if (!active || vertexRemoved)
                                return
                            var position = Qt.point(startPosition.x + translation.x,
                                                    startPosition.y + translation.y)
                            var coordinate = map.toCoordinate(position)
                            if (mainWindow.drawingArea)
                                mainWindow.moveDraftPoint(vertexHandle.index, coordinate)
                            else if (mainWindow.moveEditedPoint(
                                         vertexHandle.index, coordinate))
                                vertexRemoved = true
                        }
                    }
                }
            }
        }

        MapItemView {
            model: mainWindow.drawingArea ? mainWindow.edgeHandles(mainWindow.draftAreaPoints)
                                          : mainWindow.editingArea
                                            ? mainWindow.edgeHandles(mainWindow.editedAreaPoints) : []

            delegate: MapQuickItem {
                required property var modelData
                coordinate: modelData.coordinate
                anchorPoint.x: addHandle.width / 2
                anchorPoint.y: addHandle.height / 2

                sourceItem: Rectangle {
                    id: addHandle
                    width: 20
                    height: 20
                    radius: width / 2
                    color: "white"
                    border.color: "#F57C00"
                    border.width: 2

                    Text {
                        anchors.centerIn: parent
                        text: "+"
                        color: "#F57C00"
                        font.bold: true
                    }

                    TapHandler {
                        onTapped: {
                            if (mainWindow.drawingArea)
                                mainWindow.insertDraftPoint(
                                    modelData.afterIndex, modelData.coordinate)
                            else
                                mainWindow.insertEditedPoint(
                                    modelData.afterIndex, modelData.coordinate)
                        }
                    }
                }
            }
        }

        TapHandler {
            enabled: mainWindow.drawingArea && !mainWindow.drawingMapNavigation
            onTapped: function(eventPoint) {
                mainWindow.addDrawingPoint(map.toCoordinate(eventPoint.position))
            }
        }

        DragHandler {
            id: drag
            target: null
            enabled: !mainWindow.editingArea
                     && (!mainWindow.drawingArea || mainWindow.drawingMapNavigation)
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
            onActiveChanged: {
                if (active)
                    viewSettings.repositioningEnabled = false
            }
        }

    }

    Rectangle {
        id: areaWorkflow
        z: 10
        visible: mainWindow.drawingArea || mainWindow.editingArea
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 12
        width: Math.min(parent.width - 24, 420)
        height: workflowColumn.implicitHeight + 20
        radius: 8
        color: "#EE202020"

        ColumnLayout {
            id: workflowColumn
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8

            Label {
                text: mainWindow.drawingArea
                      ? mainWindow.drawingMapNavigation
                        ? "Move the map, then continue drawing"
                        : "Draw area: tap the map to add vertices"
                      : "Edit area: drag vertices to adjust"
                color: "white"
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }

            TextField {
                id: areaNameInput
                visible: mainWindow.drawingArea
                placeholderText: "Area name"
                color: "#202020"
                placeholderTextColor: "#666666"
                Layout.fillWidth: true
                Layout.preferredHeight: 44

                background: Rectangle {
                    radius: 4
                    color: "white"
                }
            }

            Label {
                visible: mainWindow.drawingArea
                text: mainWindow.draftAreaPoints.length
                      + " vertices (minimum 3) - enter a name, then finish"
                color: "white"
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }

            GridLayout {
                columns: 2
                columnSpacing: 8
                rowSpacing: 8
                Layout.fillWidth: true

                Button {
                    visible: mainWindow.drawingArea
                    text: mainWindow.drawingMapNavigation ? "Continue drawing" : "Move map"
                    Layout.fillWidth: true
                    onClicked: {
                        mainWindow.drawingMapNavigation
                                = !mainWindow.drawingMapNavigation
                    }
                }

                Button {
                    visible: mainWindow.drawingArea
                    text: "Undo point"
                    Layout.fillWidth: true
                    enabled: mainWindow.draftAreaPoints.length > 0
                    onClicked: mainWindow.undoDrawingPoint()
                }

                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    onClicked: mainWindow.drawingArea
                               ? mainWindow.cancelDrawing()
                               : mainWindow.cancelEdit()
                }

                Button {
                    text: mainWindow.drawingArea ? "Finish" : "Save"
                    Layout.fillWidth: true
                    enabled: mainWindow.drawingArea
                             ? mainWindow.draftAreaPoints.length >= 3
                               && areaNameInput.text.trim().length > 0
                             : mainWindow.editedAreaPoints.length >= 3
                    onClicked: mainWindow.drawingArea
                               ? mainWindow.finishDrawing()
                               : mainWindow.saveEdit()
                }
            }
        }
    }

    function updateStyle(styleIndex) {
        viewSettings.mapStyle = styleIndex
    }

    function applyMapStyle() {
        if (mapStyle >= 0 && mapStyle < map.supportedMapTypes.length)
            map.activeMapType = map.supportedMapTypes[mapStyle]
    }

    function recenterToLastAcceptedPosition() {
        if (viewSettings.hasLastMapPosition) {
            map.center = QtPositioning.coordinate(
                        viewSettings.lastMapLatitude,
                        viewSettings.lastMapLongitude)
        } else if (liveLocation.newCenter.isValid) {
            map.center = liveLocation.newCenter
        }
    }

    Connections {
        target: liveLocation

        function onAcceptedPositionChanged(coordinate) {
            viewSettings.lastMapLatitude = coordinate.latitude
            viewSettings.lastMapLongitude = coordinate.longitude
            viewSettings.hasLastMapPosition = true
            if (viewSettings.repositioningEnabled)
                map.center = coordinate
        }
    }

    StackView {
        id: menueStack
        anchors.fill: parent
        initialItem: mainMenue        // <-- object reference now, not "menues/MainMenue.qml"
    }

    Connections {
        target: database
        function onHuntingAreasChanged() {
            mainWindow.reloadHuntingAreas()
        }
    }

    Component.onCompleted: reloadHuntingAreas()

    Button {
        id: repositionButton
        width: 56
        height: 56
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 82
        z: 1

        onClicked: {
            if (mainWindow.areaInteractionActive)
                return
            viewSettings.repositioningEnabled = !viewSettings.repositioningEnabled
            if (viewSettings.repositioningEnabled)
                recenterToLastAcceptedPosition()
        }

        background: Item {}

        contentItem: Item {
            opacity: viewSettings.repositioningEnabled ? 1 : 0.45

            Rectangle {
                width: 3
                height: 42
                anchors.centerIn: parent
                color: "white"
            }

            Rectangle {
                width: 42
                height: 3
                anchors.centerIn: parent
                color: "white"
            }

            Rectangle {
                width: 11
                height: 11
                radius: 6
                anchors.centerIn: parent
                color: viewSettings.repositioningEnabled ? "#E85A5A" : "white"
            }
        }
    }

    Popup {
        id: zoomSliderOverlay
        parent: Overlay.overlay
        visible: true
        modal: false
        focus: false
        closePolicy: Popup.NoAutoClose
        padding: 0
        property bool isLandscape: parent.width > parent.height
        width: zoomSlider.implicitWidth
        height: parent.height * (isLandscape ? 0.55 : 0.33)
        x: parent.width - width - (isLandscape ? 102 : 15)
        y: parent.height - height - (isLandscape ? 40 : 82)

        background: Item {}

        contentItem: Slider {
            id: zoomSlider
            anchors.fill: parent
            orientation: Qt.Vertical
            from: 11
            to: 19
            value: map.zoomLevel
            onMoved: viewSettings.mapZoomLevel = value

            background: Rectangle {
                x: zoomSlider.leftPadding
                   + (zoomSlider.availableWidth - width) / 2
                y: zoomSlider.topPadding
                width: 10
                height: zoomSlider.availableHeight
                radius: width / 2
                color: "#B33A3A3A"
            }

            handle: Rectangle {
                x: zoomSlider.leftPadding
                   + (zoomSlider.availableWidth - width) / 2
                y: zoomSlider.topPadding
                   + zoomSlider.visualPosition * (zoomSlider.availableHeight - height)
                width: 28
                height: 28
                radius: width / 2
                color: "#CC4A4A4A"
                border.color: "#B32D2D2D"
                border.width: 1
            }
        }
    }

}
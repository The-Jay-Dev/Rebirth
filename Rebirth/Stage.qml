import QtQuick 2.15
import Rebirth
import HitObjectEnums 1.0
import QtQuick.Layouts
import QtQuick.Particles
import Qt5Compat.GraphicalEffects

Item {
    id: root

    required property var mainParticleSystem
    required property var mainParticleImg
    property alias trackVerticalStretch: trackVerticalStretchAnim
    property alias trackVerticalCompress: trackVerticalCompressAnim
    property alias laneBorderVerticalCompress: laneBorderVerticalCompressAnim
    property bool gameStart: false
    property real trackWidth: track.width
    property real trackXPosition: track.x
    property real trackHeight: track.height
    property var laneLightColor: Qt.rgba(1.0, 0.85, 0.4, 0.5)
    property string hitObjEighthNoteColor:  "#fff987"
    property string hitObjTripletNoteColor: "#ff84f4"
    property string hitObjParadoxNoteColor: "#ff5b5b"
    property string hitObjQuarterNoteColor:  "#9afbff"
    property string hitObjHoldNoteColor: "white"
    property string doubleTapNoteColor: "#8eff73"

    anchors.fill: parent
    anchors.centerIn: parent

    onLaneLightColorChanged: {
        laneALightColor.color = laneLightColor
        laneBLightColor.color = laneLightColor
        laneCLightColor.color = laneLightColor
        laneDLightColor.color = laneLightColor
    }

    Connections{
      target: gameManager

      function onLevelComplete(){
        controlPanel.anchors.bottom = undefined
        controlPanelDepartureAnim.start()
      }
    }

    Timer{
        id:controlPanelRevealDelay
        interval: 500
        running: false
        onTriggered: controlPanelRevealAnim.start()
    }

    GridLayout{


        anchors.fill: parent
        anchors.centerIn: parent
        columns: 3

        Emitter{
            id: leftEmitter
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            system: mainParticleSystem
            emitRate: 2
            lifeSpan: 5000
            lifeSpanVariation: 500
            size: 16
            endSize: 18
            Layout.column: 0

            velocity: AngleDirection {
                angle: -90
                magnitude: 30
                magnitudeVariation: 20 //If this num is equal to magnitude, some particles will remain static
            }
        }

        Emitter{
            id: rightEmitter
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            system: mainParticleSystem
            emitRate: 2
            lifeSpan: 5000
            lifeSpanVariation: 500
            size: 16
            endSize: 32
            Layout.column: 2
            velocity: AngleDirection {
                angle: -90
                magnitude: 30
                magnitudeVariation: 20
            }

        }

        Rectangle {
            id: track

            color: "black"
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: windowObject.width* .6
            Layout.preferredHeight: 0
            Layout.column: 1

            opacity: trackOpacity

            Component.onCompleted: {
                // Just because the object is created doesn't mean it's positioned yet; call after everything is positioned
                Qt.callLater(() => {
                                 judgementLineHandler.noteTrackWidth = width;
                             });
            }

            onWidthChanged: {
                hitObjectModel.noteTrackWidth = width;
         }

            PropertyAnimation{
                id: trackVerticalStretchAnim
                target: track
                to: windowObject.height
                property:"Layout.preferredHeight"
                easing.type: Easing.InOutQuad
                onStopped: {

                    track.Layout.preferredHeight = Qt.binding(() => windowObject.height)
                    controlPanelRevealDelay.start()
                }
            }

            PropertyAnimation{
                id: trackVerticalCompressAnim
                target: track
                to: 0
                property:"Layout.preferredHeight"
                easing.type: Easing.InOutQuad
            }

            PropertyAnimation{
                id: laneBorderVerticalCompressAnim
                targets: [laneABorder, laneBBorder, laneCBorder]
                to: 0
                property:"height"
                easing.type: Easing.InOutQuad
            }


            Repeater{
                id: repeater
                model: hitObjectModel

                signal trailReady(var trailObj)

                delegate: HitObject{

                    property var trail
                    property var holdNoteController
                    property var secondTapRing
                    property bool active: noteActive
                    property var noteColorType: colorType

                    y: yPosition
                    x: xPosition
                    noteWidth: (track.width/4) * 0.8
                    noteHeight: width/6
                    noteOpacity: noteVisible
                  //  debugTextValue: debugText //Excessive bindings are expensive. Only use when debugging

                    onXChanged: {
                        if(holdNoteController && slideEventReady){
                            if(holdNoteController.x > (holdNoteController.rightBounds + holdNoteController.boundsOffset) || holdNoteController.x < (holdNoteController.leftBounds - holdNoteController.boundsOffset) || holdFailed){
                                hitObjectModel.holdFailed(model.index);
                                holdNoteController.destroy()
                            }
                        }
                    }

                    onNoteColorTypeChanged: {
                        determineNoteColor()
                    }

                    onActiveChanged: {
                        if (!active){
                            visible = false

                            if(holdNoteController){
                               holdNoteController.destroy()
                            }

                            if(secondTapRing){
                                secondTapRing.destroy()
                            }

                            if(trail){
                                trail.destroy()
                                trail = null
                            }
                        }
                        else
                        {
                            visible = true
                        }
                    }



                    Connections{
                        target: hitObjectModel

                        function onSpawnTrail(objectPoolIndex){
                            if (model.index === objectPoolIndex){
                                var component = Qt.createComponent("Trail.qml");

                                if (type === HitObjectEnums.Hold){ //Hold Note
                                    trail = component.createObject(track, {spawnX: xPosition, spawnY: Qt.binding(() => y), trailWidth: noteWidth});
                                }
                                else
                                {
                                    trail = component.createObject(track, {spawnX: xPosition, spawnY: Qt.binding(() => y), trailWidth: noteWidth });
                                    trail.trailBottomLeft = Qt.binding(() => xPosition + noteWidth)
                                    trail.trailBottomRight = Qt.binding(() => xPosition)
                                }

                                trail.noteXPosB = Qt.binding(() => tailNoteXPosition + noteWidth)
                                trail.noteYPosB = Qt.binding(() => tailNoteYPosition)
                                trail.noteXPosC = Qt.binding(() => tailNoteXPosition)
                                trail.noteYPosC = Qt.binding(() => tailNoteYPosition)
                            }
                        }

                        function onDespawnTrail( objectPoolIndex){
                            if (model.index === objectPoolIndex){
                                if(trail){
                                    trail.destroy()
                                    trail = null
                                }
                            }

                        }

                        function onSpawnedGimmickObjChanged(index){
                            //Root of the problem L is logged twice so that means this was also called twice
                            if(index === model.index){
                                if(type === HitObjectEnums.Hold|| type === HitObjectEnums.Slide){
                                    spawnHoldEmitter()
                                }
                                else if (type === HitObjectEnums.DoubleTap){
                                    spawnSecondTapJudgementRing(judgementRingMaxScale)
                                }
                            }
                        }
                    }


                    function spawnHoldEmitter(){
                        var component;
                        var collisionSize = laneALight.width/6;
                        var noteCenter = (xPosition + (noteWidth/2)) - (collisionSize/2);

                        if (type === HitObjectEnums.Slide){
                            component = Qt.createComponent("HoldNoteController.qml");
                            holdNoteController = component.createObject(root, {x: track.x + noteCenter, bottomMargin: root.height - judgementLineHandler.yPosition,  controllerWidth: collisionSize, emitterWidth: laneALight.width, isSlideController: true, startX: track.x + noteCenter, parentNoteIndex: model.index});
                            holdNoteController.leftBounds =  Qt.binding(()=> track.x + x )
                            holdNoteController.rightBounds =  Qt.binding(()=> (track.x + x +  noteWidth))
                            holdNoteController.x = Qt.binding(()=> holdNoteController.startX + hitObjectModel.relativeMouseX)

                        }
                        else if (type === HitObjectEnums.Hold){
                            component = Qt.createComponent("HoldNoteController.qml");
                            holdNoteController = component.createObject(root, {x: track.x + noteCenter, bottomMargin: root.height - judgementLineHandler.yPosition,  controllerWidth: collisionSize,emitterWidth:  laneALight.width, isSlideController: false, parentNoteIndex: model.index});
                        }
                    }

                    function spawnSecondTapJudgementRing(ringScale){
                        var component = Qt.createComponent("DoubleTapJudgementRing.qml");
                        secondTapRing = component.createObject(track, {width: noteWidth, height: noteHeight, x: x, y: Qt.binding(() => yPosition), ringScale: ringScale});
                    }
                    Component.onDestruction: {
                        if(holdNoteController){
                           holdNoteController.destroy()
                        }

                        if(secondTapRing){
                            secondTapRing.destroy()
                        }

                        if(trail){
                            trail.destroy()
                            trail = null
                        }
                    }

                    Component.onCompleted: {
                        hitObjectModel.noteHeight = height
                        hitObjectModel.noteWidth = width
                    }

                    function determineNoteColor(){

                        if (colorType === HitObjectEnums.HoldNote || colorType === HitObjectEnums.SlideNote){ //Hold or Slide
                            hitObjectColor = holdNoteColor
                        }
                        else if (colorType === HitObjectEnums.ForgottenNote){
                            hitObjectColor = "red"
                        }
                        else if (colorType === HitObjectEnums.DoubleTapNote){
                            hitObjectColor = doubleTapNoteColor
                        }
                        else if (colorType === HitObjectEnums.QuarterNote){

                            hitObjectColor = quarterNoteColor
                        }
                        else if (colorType === HitObjectEnums.EighthNote)
                        {
                            hitObjectColor= eighthNoteColor
                        }
                        else if (colorType === HitObjectEnums.TripletNote){
                            hitObjectColor = tripletNoteColor
                        }
                        else{
                            hitObjectColor = quarterNoteColor
                        }
                    }
                }
            }


            RowLayout{
                spacing: 0
                anchors.fill: parent
                Connections{
                    target: gameManager

                    function onLaneHeld(lane){

                        if (gameStart && !root.parent.gameEnd){
                            if (lane === 0){
                                laneALight.opacity = 1
                            }
                            else if (lane === 1){
                                laneBLight.opacity = 1
                            }
                            if (lane === 2){
                                laneCLight.opacity = 1
                            }
                            else if (lane === 3){
                                laneDLight.opacity = 1
                            }

                        }
                    }
                    function onGameEnd(){
                        root.parent.gameEnd = true;
                        laneALight.opacity = 0
                        laneBLight.opacity = 0
                        laneCLight.opacity = 0
                        laneDLight.opacity = 0
                    }

                    function onLaneReleased(lane){
                        if (lane === 0){
                            laneALight.opacity = 0
                        }
                        else if (lane === 1){
                            laneBLight.opacity = 0
                        }
                        if (lane === 2){
                            laneCLight.opacity = 0
                        }
                        else if (lane === 3){
                            laneDLight.opacity = 0
                        }
                    }
                }

                Item{
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item{
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottomMargin: controlPanel.height
                        height: windowObject.height - controlPanel.height

                        Rectangle{
                            id: laneALight
                            opacity: 0
                            anchors.fill: parent

                            Behavior on opacity{
                                NumberAnimation{
                                    duration: laneOpacityDuration
                                    easing.type: Easing.InOutQuad
                                }
                            }

                            gradient: Gradient{
                                GradientStop{position: 0.3; color: "transparent"}
                                GradientStop{id:laneALightColor; position: 1.0; color: laneLightColor; }
                            }
                        }
                    }


                    Rectangle{
                        id: laneABorder
                        width: 1
                        anchors.margins: 0
                        height: parent.height
                        color: "white"
                        anchors.right: parent.right
                        opacity: laneBorderOpacity
                    }
                }

                Item{
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item{

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottomMargin: controlPanel.height
                        height: windowObject.height - controlPanel.height

                        Rectangle{
                            id: laneBLight
                            opacity: 0
                            anchors.fill: parent

                            Behavior on opacity{
                                NumberAnimation{
                                    duration: laneOpacityDuration
                                    easing.type: Easing.InOutQuad
                                }
                            }

                            gradient: Gradient{
                                GradientStop{position: 0.3; color: "transparent"}
                                GradientStop{id:laneBLightColor; position: 1.0; color: laneLightColor}
                            }
                        }
                    }



                    Rectangle{
                        id: laneBBorder
                        width: 1
                        anchors.margins: 0
                        height: parent.height
                        color: "white"
                        anchors.right: parent.right
                        opacity: laneBorderOpacity
                    }
                }

                Item{
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item{

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottomMargin: controlPanel.height
                        height: windowObject.height - controlPanel.height

                        Rectangle{
                            id: laneCLight
                            opacity: 0
                            anchors.fill: parent

                            Behavior on opacity{
                                NumberAnimation{
                                    duration: laneOpacityDuration
                                    easing.type: Easing.InOutQuad
                                }
                            }

                            gradient: Gradient{
                                GradientStop{position: 0.3; color: "transparent"}
                                GradientStop{id:laneCLightColor; position: 1.0; color: laneLightColor}
                            }
                        }
                    }
                    Rectangle{
                        id: laneCBorder
                        width: 1
                        anchors.margins: 0
                        height: parent.height
                        color: "white"
                        anchors.right: parent.right
                        opacity: laneBorderOpacity
                    }
                }

                Item{
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    Layout.bottomMargin: controlPanel.height
                    Layout.fillWidth: true
                    Layout.preferredHeight: windowObject.height - controlPanel.height

                    Rectangle{
                        id: laneDLight
                        opacity: 0
                        anchors.fill: parent

                        Behavior on opacity{
                            NumberAnimation{
                                duration: laneOpacityDuration
                                easing.type: Easing.InOutQuad
                            }
                        }

                        gradient: Gradient{
                            GradientStop{position: 0.3; color: "transparent"}
                            GradientStop{id:laneDLightColor; position: 1.0; color: laneLightColor;}
                        }
                    }
                }
            }
        }

    }

    JudgementLine{
        id: judgementLine
        z:1

        anchors.horizontalCenter: controlPanel.horizontalCenter
        anchors.bottom: controlPanel.top
        width: controlPanel.width
        height: controlPanel.height * .03
        onHeightChanged: {
            judgementLineHandler.yPosition = y + (height/2);
        }

        onYChanged: {
            judgementLineHandler.yPosition = y + (height/2);
        }
    }

    Rectangle{
        id: controlPanel
        z:1
        anchors.horizontalCenter: parent.horizontalCenter
        width: window.width
        height: parent.height * .18
        y: windowObject.height

        color: Qt.rgba(0, 0, 0, 0.55) //Opacity is inherited amongst all childen, so we use rgba

        PropertyAnimation{
            id: controlPanelRevealAnim
            target: controlPanel
            to: windowObject.height - controlPanel.height
            property:"y"
            easing.type: Easing.InOutQuad

            onStopped:{
                gameStart = true
                controlPanel.anchors.bottom = root.bottom
            }
        }

        PropertyAnimation{
            id: controlPanelDepartureAnim
            target: controlPanel
            from: windowObject.height - controlPanel.height
            to: windowObject.height
            property:"y"
            easing.type: Easing.InOutQuad

            onStopped:{
                gameManager.calculateResult()
                displayResultDelayTimer.start()
            }
        }
        RowLayout{

            id: panelLayout
            width: stage.trackWidth
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            Item{
                id: keyAContainer
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true

                Layout.fillHeight: true

                Text{
                    id:keyA
                    font.family:customFont.barlowLight
                    font.weight: Font.Light
                    text: "A"
                    color: "white"
                    anchors.centerIn: parent
                    font.pixelSize: controlPanel.width * controlsTextSizeMultiplier
                }
            }

            Item{
                id: keyBContainer
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true

                Layout.fillHeight: true

                Text{
                    id:keyB
                    font.family:customFont.barlowLight
                    font.weight: Font.Light

                    text: "S"
                    color: "white"
                    anchors.centerIn: parent
                    font.pixelSize: controlPanel.width * controlsTextSizeMultiplier
                }
            }

            Item{
                id: keyCContainer
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true

                Layout.fillHeight: true

                Text{
                    id:keyC

                    font.family:customFont.barlowLight
                    font.weight: Font.Light
                    text: "D"
                    color: "white"
                    anchors.centerIn: parent
                    font.pixelSize: controlPanel.width * controlsTextSizeMultiplier
                }
            }


            Item{
                id: keyDContainer
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true

                Layout.fillHeight: true

                Text{
                    id:keyD
                    font.family:customFont.barlowLight
                    font.weight: Font.Light
                    text: "F"
                    color: "white"
                    anchors.centerIn: parent
                    font.pixelSize: controlPanel.width * controlsTextSizeMultiplier
                }
            }
        }
    }
}

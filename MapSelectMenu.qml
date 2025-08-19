import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Shapes
import Qt5Compat.GraphicalEffects
import QtMultimedia

Item {
    id: root
    property bool mapSelected: false
    property int fadeOutDuration: 100
    required property var windowObject
    required property var backgroundObject
    property real controlsTextSizeMultiplier: .05
    property int mapIndex: 0
    anchors.fill: parent

    Component.onCompleted:{
        dataHandlerModel.titleScreen = false
        dataHandlerModel.gameStart = false
        dataHandlerModel.prepMapSelect()
    }


    QmlSoundEffects{
        id: soundEffects
    }

    Connections{
        target: dataHandlerModel

        function onDataRefreshed(){
            backgroundObject.gameStart = false
            blankScreenOpacity.start()
        }
    }

    Rectangle{
        id: blankScreen

        color: "black"
        anchors.fill: parent
        opacity:1
        z:15

        NumberAnimation {
            id: blankScreenOpacity
            target: blankScreen
            property: "opacity"
            duration: 200
            to: 0
        }


        NumberAnimation {
            id: blankScreenOpaque
            target: blankScreen
            property: "opacity"
            duration: 200
            to: 1

            onStopped: {
                backToTitleDelay.start()
            }
        }
    }


    Timer{
      id:backToTitleDelay
      interval: 1000
      running: false
      onTriggered: windowObject.switchToTitle(true)
    }

    MapSelectTransition{
        id:mapSelectTransition
        windowHeight: windowObject.height
    }

    Item{

        id:refreshedNotifContainer
        height: 0
        width: parent.width
        anchors.centerIn: parent
        z:12
        SequentialAnimation on height {
            id:refreshedAnim
            running: false

            NumberAnimation {
                from: 0
                to: windowObject.height *.2
                duration: 200

                easing.type: Easing.InOutQuad
            }
            PauseAnimation {duration: 500}
            NumberAnimation {
                from: windowObject.height *.2
                to: 0
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }

        Rectangle{
            opacity: 0.6
            anchors.fill: parent
            color: "black"
        }
        Text {
            anchors.centerIn: parent
            text: "Refreshed."

            font.family:customFont.barlowLight
            font.weight: Font.Light
            color: "white"
            font.pixelSize: parent.height * 0.3
            visible: parent.height > 0
        }
    }



    Item{
        width: parent.height * .1
        height: width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        x: sideMenu.open ? parent.width * .85 : parent.width * .9
        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
        z:7
        opacity: mapSelected ? 0: 1


        Behavior on opacity{

            NumberAnimation {

                duration: fadeOutDuration
                easing.type: Easing.InOutQuad
            }
        }
        Image {
            id: arrow
            source: "UI/chevron-back-sharp.svg"
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            smooth: true

            opacity:  sideMenu.open ? 0  : 1
            layer{
                enabled: true
                effect: ColorOverlay{
                    color: "white"
                }
            }

            Behavior on opacity {


                NumberAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }


            SequentialAnimation on scale {
                loops: Animation.Infinite
                NumberAnimation {
                    from: 1
                    to: 1.2
                    duration: 500

                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    from: 1.2
                    to: 1
                    duration: 500
                    easing.type: Easing.InOutQuad

                }
            }


        }
    }


    MapSelectSideMenu{
        id: sideMenu
        menuWidth: root.width * 0.2
        menuHeight: root.height - bottomBorder.height
        anchors.top: parent.top
        opacity: mapSelected ? 0: 1
        Behavior on opacity{

            NumberAnimation {

                duration: fadeOutDuration
                easing.type: Easing.InOutQuad
            }
        }

    }
    Item{
        height: root.height - bottomBorder.height

        width: parent.width - sideMenu.menuWidth
        anchors.left: parent.left
        z: 9
        opacity: mapSelected ? 0: 1
        Behavior on opacity{

            NumberAnimation {

                duration: fadeOutDuration
                easing.type: Easing.InOutQuad
            }
        }
        Rectangle
        {
            anchors.fill: parent
            color: "black"

            opacity: sideMenu.open ? 0.6 : 0.0
            Behavior on opacity {
                NumberAnimation {

                    duration: 300
                    easing.type: Easing.InOutQuad
                }

            }
            HoverHandler{
                id: sideMenuExitZone
                enabled: false
                onHoveredChanged: {
                    if (hovered){

                        sideMenuHoverHandler.enabled = true
                        sideMenu.open = false
                        sideMenuExitZone.enabled = false
                    }
                }
            }
        }
    }

    Rectangle{
        color: "black"
        opacity: .4
        anchors.fill: parent
    }

    CustomFonts{
        id:customFont
    }

    ColumnLayout{
        anchors.fill: parent
        spacing: 0
        opacity: mapSelected ? 0: 1
        Behavior on opacity{

            NumberAnimation {

                duration: fadeOutDuration
                easing.type: Easing.InOutQuad
            }
        }

        RowLayout{
            spacing: 0
            Layout.fillHeight: true
            Layout.fillWidth: true

            opacity: mapSelected ? 0: 1
            Behavior on opacity{

                NumberAnimation {

                    duration: fadeOutDuration
                    easing.type: Easing.InOutQuad
                }
            }

            Item{
                id: mapInfoContainer
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item{
                    width: parent.width *0.8
                    height: parent.height
                    anchors.centerIn: parent
                    Rectangle{
                        property real bgWidth: parent.width
                        id: mapInfoBG
                        color: "black"
                        width: parent.width
                        height: parent.height
                        anchors.centerIn: parent
                        gradient: Gradient{
                            GradientStop{position: 0; color: Qt.rgba(0,0,0,0)}
                            GradientStop{position: 0.2; color: Qt.rgba(0,0,0,0.4)}
                            GradientStop{position: 0.8; color: Qt.rgba(0,0,0,0.4)}
                            GradientStop{position: 1; color: Qt.rgba(0,0,0,0)}
                        }
                    }


                    ColumnLayout{
                        id:mapInfo
                        anchors.centerIn: parent
                        spacing: 15
                        width: mapInfoBG.width
                        z: 10



                        RowLayout{
                            id: mapArtist
                            spacing: 5
                            property bool hovered: false
                            Layout.alignment: Qt.AlignHCenter
                            HoverHandler{
                                id: mapArtistHoverHandler
                                onHoveredChanged: {
                                    if(hovered){
                                        mapArtist.hovered = true
                                         soundEffects.menuHover.play()
                                        mapArtistText.elide = Text.ElideNone
                                        mapArtistText.text = dataHandlerModel.currentMapArtist

                                    }
                                    else
                                    {
                                        mapArtist.hovered = false
                                        mapArtistText.elide = Text.ElideRight
                                        mapArtistText.text =  "Artist: " +  dataHandlerModel.currentMapArtist
                                    }
                                }
                            }

                            Text{
                                id:mapArtistText
                                property real textSize: mapInfoBG.height * controlsTextSizeMultiplier
                                text: "Artist: " +  dataHandlerModel.currentMapArtist
                                font.family:customFont.barlowLight
                                font.weight: Font.Light
                                color: "white"

                                  Layout.alignment: Qt.AlignHCenter
                                elide: Text.ElideRight
                                Layout.preferredWidth:  mapInfoBG.width
                                font.pixelSize: mapArtist.hovered ? textSize * 1.1 : textSize


                                Behavior on font.pixelSize {


                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                            }

                        }
                        RowLayout{
                            id: mapIllustration
                            spacing: 5
                            property bool hovered: false
                            Layout.alignment: Qt.AlignHCenter
                            HoverHandler{
                                id: mapIllustrationHoverHandler
                                onHoveredChanged: {
                                    if(hovered){
                                        mapIllustration.hovered = true
                                         soundEffects.menuHover.play()
                                        mapIllustrationText.elide = Text.ElideNone
                                        mapIllustrationText.text = dataHandlerModel.currentMapIllustration
                                    }
                                    else
                                    {
                                        mapIllustration.hovered = false

                                        mapIllustrationText.elide = Text.ElideRight
                                        mapIllustrationText.text = "Illustration: " +  dataHandlerModel.currentMapIllustration
                                    }
                                }
                            }

                            Text{
                                id: mapIllustrationText
                                property real textSize: mapInfoBG.height * controlsTextSizeMultiplier
                                text: "Illustration: " +  dataHandlerModel.currentMapIllustration
                                font.family:customFont.barlowLight
                                font.weight: Font.Light
                                color: "white"

                                  Layout.alignment: Qt.AlignHCenter
                                elide: Text.ElideRight
                                Layout.preferredWidth:  mapInfoBG.width
                                font.pixelSize: mapIllustration.hovered ? textSize * 1.1 : textSize


                                Behavior on font.pixelSize {


                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                            }

                        }

                        RowLayout{
                            id: mapBPM
                            spacing: 5
                            property bool hovered: false
                              Layout.preferredWidth:  mapInfoBG.width
                            Layout.alignment: Qt.AlignHCenter
                            HoverHandler{
                                id: mapBPMHoverHandler
                                onHoveredChanged: {
                                    if(hovered){
                                        mapBPM.hovered = true
                                         soundEffects.menuHover.play()
                                    }
                                    else
                                    {
                                        mapBPM.hovered = false
                                    }
                                }
                            }

                            Text{
                                property real textSize: mapInfoBG.height * controlsTextSizeMultiplier
                                text: "BPM: " + dataHandlerModel.currentMapBPM
                                font.family:customFont.barlowLight
                                font.weight: Font.Light
                                elide: Text.ElideRight

                                  Layout.alignment: Qt.AlignHCenter
                                color: "white"
                                font.pixelSize: mapBPM.hovered ? textSize * 1.1 : textSize


                                Behavior on font.pixelSize {


                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                            }

                        }

                        RowLayout{
                            id: mapDifficulty
                            spacing: 5
                            property bool hovered: false
                            Layout.alignment: Qt.AlignHCenter
                              Layout.preferredWidth:  mapInfoBG.width
                            HoverHandler{
                                id: mapDifficultyHoverHandler
                                onHoveredChanged: {
                                    if(hovered){
                                        mapDifficulty.hovered = true
                                         soundEffects.menuHover.play()
                                    }
                                    else
                                    {
                                        mapDifficulty.hovered = false
                                    }
                                }
                            }

                            Text{
                                property real textSize: mapInfoBG.height * controlsTextSizeMultiplier
                                text: "Difficulty: " +   dataHandlerModel.currentMapDifficulty
                                font.family:customFont.barlowLight
                                font.weight: Font.Light
                                color: "white"
                                font.pixelSize: mapDifficulty.hovered ? textSize * 1.1 : textSize
                                Layout.alignment: Qt.AlignHCenter
                                elide: Text.ElideRight




                                Behavior on font.pixelSize {


                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                            }

                        }

                        RowLayout{
                            id: mapStyle
                            spacing: 5
                            property bool hovered: false
                            Layout.alignment: Qt.AlignHCenter
                            HoverHandler{
                                id: mapStyleHoverHandler
                                onHoveredChanged: {
                                    if(hovered){
                                        mapStyle.hovered = true
                                         soundEffects.menuHover.play()
                                    }
                                    else
                                    {
                                        mapStyle.hovered = false
                                    }
                                }
                            }

                            Text{
                                property real textSize: mapInfoBG.height * controlsTextSizeMultiplier
                                text: "Style:"
                                font.family:customFont.barlowLight
                                font.weight: Font.Light
                                color: "white"
                                font.pixelSize: mapStyle.hovered ? textSize * 1.1 : textSize
                                elide: Text.ElideRight


                                Behavior on font.pixelSize {


                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                            }
                            Text{
                                property real textSize: mapInfoBG.height * controlsTextSizeMultiplier
                                text: dataHandlerModel.currentMapStyle
                                font.family:customFont.barlowLight
                                font.weight: Font.Light
                                color: dataHandlerModel.currentMapStyle === "Invoke" ? "orange" : "#00b5ff"
                                font.pixelSize: mapStyle.hovered ? textSize * 1.1 : textSize

                                Behavior on font.pixelSize {


                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                            }
                        }

                    }

                }

            }
            Item{
                id: mapIDContainer
                Layout.preferredWidth: windowObject.width * .8
                Layout.fillHeight: true

                RowLayout{
                    anchors.fill: parent
                    spacing:0



                    ColumnLayout{
                        Layout.alignment: Qt.AlignCenter
                        spacing: 20


                        Text{

                            Layout.alignment: Qt.AlignCenter
                            text: dataHandlerModel.currentMapTitle
                            font.family:customFont.barlowLight
                            font.weight: Font.Light
                            color: "white"
                            font.pixelSize: windowObject.height * .05

                        }

                        Item{
                            id: mapImgContainer
                            Layout.alignment: Qt.AlignCenter
                            Layout.preferredWidth: windowObject.width * .5
                            Layout.preferredHeight: windowObject.height * .5



                            Image{
                                id: mapImg
                                source:  dataHandlerModel.currentMapImgPath
                                anchors.centerIn: parent

                                height: parent.height
                                width: parent.width;

                                visible: false
                            }

                            Shape{
                                anchors.fill: parent
                                anchors.centerIn: parent
                                id: maskBounds
                                visible: true
                                property real localX
                                property real localY

                                ShapePath{
                                    fillColor: "black"
                                    strokeColor: "black"
                                    strokeWidth: 0
                                    capStyle: ShapePath.FlatCap
                                    startX: (root.width -(root.width - mapImgContainer.width) - mapImgContainer.width)+50
                                    startY:   (root.height -(root.height - mapImgContainer.height) - mapImgContainer.height)
                                    PathLine{ relativeX:  mapImgContainer.width - 50; relativeY:0}
                                    PathLine{ relativeX: -50; relativeY: mapImgContainer.height}
                                    PathLine{ relativeX:-mapImgContainer.width +50; relativeY:0}

                                }
                            }

                            OpacityMask {
                                anchors.fill: parent
                                anchors.centerIn: parent
                                source: mapImg
                                maskSource: maskBounds
                            }
                        }


                    }
                    Item{

                        Layout.preferredWidth: windowObject.width * .1
                        Layout.alignment: Qt.AlignRight
                        Layout.fillHeight: true
                        z: 11

                        Item{
                            id: guide
                            height: parent.height
                            width: parent.width
                            anchors.right: parent.right
                        }

                        HoverHandler{

                            id: sideMenuHoverHandler
                            acceptedDevices: PointerDevice.Mouse
                            onHoveredChanged: {

                                if (hovered){
                                     soundEffects.menuSlide.play()
                                    sideMenu.open = true
                                    enabled = false
                                    sideMenuExitZone.enabled = true
                                }
                            }

                        }
                    }
                }
            }
        }

        Item{
            id: bottomBorder

            opacity: mapSelected ? 0: 1
            Behavior on opacity{

                NumberAnimation {

                    duration: fadeOutDuration
                    easing.type: Easing.InOutQuad
                }
            }
            Layout.preferredWidth: windowObject.width
            Layout.preferredHeight: windowObject.height * .2
            Rectangle{
                color: "black"
                anchors.fill: parent
                opacity: .6
            }

            RowLayout{
                spacing: 0
                anchors.fill: parent
                Item{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    RowLayout{
                        id: mapOptionsContainer
                        spacing: 0

                        anchors.fill: parent

                        Item{
                            width: height
                            height:root.height  * .1
                            Layout.alignment: Qt.AlignCenter

                            Image{
                                 property bool hovered
                                id:backToTitleText
                                source: "UI/chevron-back-sharp.svg"
                                anchors.centerIn: parent
                                width: height
                                height: backToTitleText.hovered ? root.height  * .1 : root.height  * .08

                                fillMode: Image.PreserveAspectFit

                                layer{
                                    enabled: true
                                    effect: ColorOverlay{
                                        color: "white"
                                    }
                                }

                                Behavior on height {


                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.InOutQuad
                                    }
                                }

                                MouseArea{

                                    id: backToTitleHoverArea
                                    anchors.fill: parent

                                    hoverEnabled: true
                                    onEntered: {

                                        backToTitleText.hovered = true
                                         soundEffects.menuHover.play()

                                    }
                                    onExited:{
                                        backToTitleText.hovered = false

                                    }

                                    onClicked: {
                                       blankScreenOpaque.start()
                                        playMouseArea.enabled = false
                                        sideMenuHoverHandler.enabled = false
                                        mapDifficultyHoverHandler.enabled = false
                                        mapArtistHoverHandler.enabled = false
                                        mapBPMHoverHandler.enabled = false
                                        mapStyleHoverHandler.enabled = false
                                        refreshHoverArea.enabled = false
                                        soundEffects.optionConfirm.play()
                                        sideMenuExitZone.enabled = false
                                        sideMenuHoverHandler.enabled = false
                                        enabled = false

                                    }
                                }
                            }
                        }

                        Item{
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            width: height
                            height: root.height  * .1
                            Image{
                                 property bool hovered
                                anchors.centerIn: parent
                                id:refreshImg
                                source: "UI/refresh-outline.svg"
                                fillMode: Image.PreserveAspectFit

                                width: height
                                height: refreshImg.hovered ? root.height  * .1 : root.height  * .08
                                layer{
                                    enabled: true
                                    effect: ColorOverlay{
                                        color: "white"
                                    }
                                }

                                Behavior on height {
                                    NumberAnimation {
                                        duration: 100
                                        easing.type: Easing.InOutQuad
                                    }
                                }
                            }
                            MouseArea{

                                id: refreshHoverArea
                                anchors.fill: parent

                                hoverEnabled: true
                                onEntered: {
                                    soundEffects.menuHover.play()
                                    refreshImg.hovered = true
                                }
                                onExited:{
                                    refreshImg.hovered = false
                                }

                                onClicked: {
                                     soundEffects.optionConfirm.play()
                                    refreshedAnim.start()
                                    dataHandlerModel.currentMapIndex = 0
                                    dataHandlerModel.mapRefresh()
                                }
                            }
                        }
                    }
                }


                Item{
                    id: mapPlayContainer
                    property bool hovered
                    Layout.preferredWidth: windowObject.width * .4
                    Layout.fillHeight: true

                    RowLayout{
                        anchors.centerIn: parent
                        spacing: 10
                        Text{
                            property real textHeight: mapPlayContainer.height * .3
                            Layout.alignment: Qt.AlignCenter
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            text: "PLAY"
                            font.family:customFont.barlowLight
                            font.weight: Font.Light
                            color: "white"
                            font.pixelSize: mapPlayContainer.hovered ? textHeight * 1.2: mapPlayContainer.height * .3

                            Behavior on font.pixelSize {


                                NumberAnimation {
                                    duration: 50
                                    easing.type: Easing.InOutQuad
                                }
                            }
                            MouseArea{

                                id: playMouseArea
                                anchors.fill: parent
                                hoverEnabled: true

                                onEntered: {
                                     soundEffects.menuHover.play()
                                    mapPlayContainer.hovered = true

                                }
                                onExited: {
                                    mapPlayContainer.hovered = false

                                }
                                onClicked:{
                                    enabled = false
                                    soundEffects.optionConfirm.play()
                                    mapSelected = true

                                    backToTitleHoverArea.enabled = false
                                    sideMenuHoverHandler.enabled = false
                                    mapDifficultyHoverHandler.enabled = false
                                    mapArtistHoverHandler.enabled = false
                                    mapBPMHoverHandler.enabled = false
                                    mapStyleHoverHandler.enabled = false
                                    refreshHoverArea.enabled = false
                                    sideMenuExitZone.enabled = false
                                    sideMenuHoverHandler.enabled = false

                                    mapSelectTransition.transitionActive = true
                                    dataHandlerModel.uninitializeMusic()
                                }
                            }
                        }
                    }
                }
                Item{
                    id: mapScoreContainer

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    opacity: 0

                    RowLayout{
                        id: mapScore
                        anchors.centerIn: parent
                        spacing: 5

                        Text{
                            Layout.alignment: Qt.AlignCenter
                            text: "Score:"
                            font.family:customFont.barlowLight
                            font.weight: Font.Light
                            color: "white"
                            font.pixelSize: mapScoreContainer.height * .2

                        }
                        Text{
                            Layout.alignment: Qt.AlignCenter
                            text: "400000"
                            font.family:customFont.barlowLight
                            font.weight: Font.Light
                            color: "white"
                            font.pixelSize: mapScoreContainer.height * .2

                        }
                    }

                }
            }
        }
    }



}


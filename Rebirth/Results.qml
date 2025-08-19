import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtMultimedia

Item {
    id: root
    property bool mapSelected: false
    property bool fadeOutTransitionActive: false
    required property var windowObject
    required property var backgroundObject
    property real smallTextSizeMultiplier: 0.06
    property string capacityStatus
    anchors.fill: parent

    CustomFonts{
        id:customFont
    }

    MapSelectTransition{
        id:mapSelectTransition
        windowHeight: windowObject.height
    }

    QmlSoundEffects{
        id: soundEffects
    }

    Component.onCompleted:{
        gameInitializer.restoreCursor();
        dataHandlerModel.gameStart = false
        getRank()
        blankScreenTransparentAnim.start()
        if (backgroundObject)
            backgroundObject.gameStart = false

        if(gameManager.memoryStatus === 0){
            capacityStatus= "Preserved"
        }
        if(gameManager.memoryStatus === 1){
            capacityStatus = "Recovered"
        }
        if(gameManager.memoryStatus === 2){
            capacityStatus = "Forgotten"
        }
    }

    function getRank(){
        if (gameManager.rank === 0)
        {
            rankImg.source = "UI/S.png"
        }
        if (gameManager.rank === 1)
        {
            rankImg.source = "UI/A.png"
        }
        if (gameManager.rank === 2)
        {
            rankImg.source = "UI/B.png"
        }
        if (gameManager.rank === 3)
        {
            rankImg.source = "UI/C.png"
        }
        if (gameManager.rank === 4)
        {
            rankImg.source = "UI/D.png"
        }
    }

    Timer {
        id: sceneTransitionDelay
        interval: 1000
        running: false
        onTriggered: switchToMapSelect()

    }
    function switchToMapSelect(){
        windowObject.switchToMapSelect()
    }

    Rectangle{
        id: blankScreen
        color: "black"
        z:15
        anchors.fill: parent
        opacity:1

        NumberAnimation {
            id: blankScreenTransparentAnim
            target: blankScreen
            property: "opacity"
            duration: 200
            to: 0

        }

        NumberAnimation {
            id: blankScreenOpaqueAnim
            target: blankScreen
            property: "opacity"
            duration: 200
            to: 1

            onStopped: {
                sceneTransitionDelay.start()

            }
        }

    }



    ColumnLayout{
        anchors.fill: parent
        spacing: 0
        opacity:  mapSelectTransition.transitionActive ? 0:1

        Behavior on opacity{
            NumberAnimation{
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }

        Rectangle{
            Layout.preferredWidth: windowObject.width * 0.5
            Layout.preferredHeight: windowObject.height * .05
            Layout.alignment: Qt.AlignCenter

            gradient: Gradient{
                orientation: Gradient.Horizontal
                GradientStop{
                    position: 0
                    color: "transparent"
                }
                GradientStop{
                    position: 0.4
                    color: "black"
                }
                GradientStop{
                    position: 0.6
                    color: "black"
                }
                GradientStop{
                    position: 1
                    color: "transparent"
                }
            }

            Text{
                anchors.centerIn: parent
                text: "Results"
                font.family: customFont.barlowLight
                color: "white"
                font.weight: Font.Light
                font.pixelSize: parent.height * .4
            }
        }

        Item{
            Layout.preferredHeight: windowObject.height * .1
            Layout.fillWidth: true

            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                text: dataHandlerModel.currentMapTitle + " - " + dataHandlerModel.currentMapArtist
                font.family: customFont.barlowLight
                color: "white"
                font.weight: Font.Light
                font.pixelSize: resultsInfoBG.height * .1
            }
        }

        Item{
            Layout.preferredHeight: windowObject.height * .6
            Layout.fillWidth: true
            Item{
                id: resultsInfoBG




                anchors.centerIn: parent
                width: parent.width * .4
                height: parent.height * .8
                Rectangle{
                    anchors.fill: parent
                    opacity: 0.6
                    color: "black"
                }

                ColumnLayout{
                    anchors.fill: parent
                    spacing: 0
                    Item{
                        Layout.fillWidth: true
                        Layout.preferredHeight: resultsInfoBG.height * .4
                        Layout.alignment: Qt.AlignHCenter
                        Image{
                            id: rankImg
                            source: "UI/D.png"
                            fillMode: Image.PreserveAspectFit
                            height: parent.height * .6
                            width: height
                            anchors.centerIn: parent

                        }
                    }

                    Text{
                        Layout.alignment: Qt.AlignCenter
                        text: gameManager.score
                        font.family: customFont.barlowLight
                        color: "white"
                        font.weight: Font.Light
                        font.pixelSize: resultsInfoBG.height * .15

                    }

                    Item{
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        RowLayout{

                            anchors.fill: parent
                            Item{
                                Layout.alignment: Qt.AlignCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true


                                Text{
                                    anchors.centerIn: parent
                                    text: "Perfect: " + gameManager.perfectCount
                                    font.family: customFont.barlowLight
                                    color: "white"
                                    font.weight: Font.Light
                                    font.pixelSize: resultsInfoBG.height * smallTextSizeMultiplier
                                }

                            }

                            Item{
                                Layout.alignment: Qt.AlignCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true


                                Text{
                                    anchors.centerIn: parent
                                    text: "Good: " + gameManager.goodCount
                                    font.family: customFont.barlowLight
                                    color: "white"
                                    font.weight: Font.Light
                                    font.pixelSize: resultsInfoBG.height * smallTextSizeMultiplier
                                }

                            }


                            Item{
                                Layout.alignment: Qt.AlignCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true


                                Text{
                                    anchors.centerIn: parent
                                    text: "Miss: " + gameManager.missCount
                                    font.family: customFont.barlowLight
                                    color: "white"
                                    font.weight: Font.Light
                                    font.pixelSize: resultsInfoBG.height * smallTextSizeMultiplier
                                }

                            }
                            Item{
                                Layout.alignment: Qt.AlignCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true


                                Text{
                                    anchors.centerIn: parent
                                    text: "Combo: " + gameManager.combo
                                    anchors.rightMargin: 10
                                    font.family: customFont.barlowLight
                                    color: "white"
                                    font.weight: Font.Light
                                    font.pixelSize: resultsInfoBG.height * smallTextSizeMultiplier
                                }

                            }



                        }
                    }

                    Item{

                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredHeight: resultsInfoBG.height * .2
                        // visible: gameManager.highScoreAchieved ? true: false
                        //visible: false

                        RowLayout{
                            anchors.centerIn: parent

                            Text{
                                Layout.alignment: Qt.AlignCenter
                                text: "Capacity Status: "
                                font.family: customFont.barlowLight
                                color: "white"
                                font.weight: Font.Light
                                font.pixelSize: resultsInfoBG.height * smallTextSizeMultiplier

                            }
                            Text{
                                Layout.alignment: Qt.AlignCenter
                                text: capacityStatus
                                font.family: customFont.barlowRegular
                                color: capacityStatus === "Forgotten" ? Qt.darker("#f83665", 1.1): Qt.darker("cyan", 1.1)
                                font.pixelSize: resultsInfoBG.height * smallTextSizeMultiplier

                            }
                        }


                    }


                }
            }
        }



        RowLayout{
            spacing: 0
            Layout.preferredHeight: windowObject.height * .2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            opacity:  mapSelectTransition.transitionActive ? 0:1
            Behavior on opacity{
                NumberAnimation{
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }

            Item{

                Layout.fillHeight: true
                Layout.fillWidth: true
                Button{
                    id: backButton
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left

                    width: parent.width * .5
                    height:width * 0.5

                    background: Rectangle{

                        anchors.fill: parent

                        color: "black"
                        opacity: 0.6
                    }
                    Text{
                        id: backText
                        anchors.centerIn: parent
                        text: "Back"
                        font.family: customFont.barlowLight
                        color: "white"
                        font.weight: Font.Light
                        font.pixelSize: parent.height * 0.3
                    }

                    onHoveredChanged: {
                        enabled:  mapSelectTransition.transitionActive ? false:true
                        if(hovered){

                            backText.color = "orange"
                            soundEffects.menuHover.play()
                        }
                        else
                        {
                            backText.color = "white"
                        }
                    }

                    onClicked:{
                        enabled = false
                        backButton.enabled = false
                        blankScreenOpaqueAnim.start()
                        soundEffects.optionConfirm.play()
                    }
                }


            }
            Item{

                Layout.fillWidth: true
                height: windowObject.height * .2
                // Image{
                //     anchors.centerIn: parent
                //     source: "UI/EmotionalCapacity.png"
                //     fillMode: Image.PreserveAspectFit
                //     width: windowObject.width/4
                //     height: windowObject.height * .2
                // }
            }

            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true

                Button{
                    id: retryButton
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right

                    width: parent.width * .5
                    height:width * 0.5

                    background: Rectangle{

                        anchors.fill: parent

                        color: "black"
                        opacity: 0.6
                    }
                    Text{
                        id: retryText
                        anchors.centerIn: parent
                        text: "Retry"
                        font.family: customFont.barlowLight
                        color: "white"
                        font.weight: Font.Light
                        font.pixelSize: parent.height * 0.3
                    }

                    onHoveredChanged: {
                        enabled:  mapSelectTransition.transitionActive ? false:true
                        if(hovered){

                            retryText.color = "orange"
                              soundEffects.menuHover.play()
                        }
                        else
                        {
                            retryText.color = "white"
                        }
                    }


                    onClicked:{
                        retryButton.enabled = false
                        enabled = false
                        dataHandlerModel.uninitializeMusic();
                        soundEffects.optionConfirm.play()
                        mapSelectTransition.transitionActive = true

                    }
                }


            }

        }
    }
}

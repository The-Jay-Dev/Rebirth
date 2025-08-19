import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Particles
import QtMultimedia
Image {
      id: root
      required property var windowObject
      required property bool transition

      property bool displayMenu: false
      property bool mapSelect: false
      property bool latencyConfig: false

      source: "UI/TitleCardBG.png"
      fillMode: Image.Stretch
      anchors.fill: parent

     QmlSoundEffects{
       id: soundEffects
     }

      Component.onCompleted: {
            if (windowObject.firstRun){
                  windowObject.firstRun = false;
                  dataHandlerModel.titleScreen = true;
                  dataHandlerModel.prepMapSelect();
            }


            if(transition){
                  blankScreen.opacity = 1
                  blankScreenOpaque.start()
            }else{
                  blankScreen.opacity = 0
            }
            randTrackAttemptTimer.start()
      }

      Connections{
            target: dataHandlerModel;

            function onTitleTrackStarted(title){
                  trackTitleText.text =  title
                  trackTitleBG.width =  trackTitleBG.parent.width * 0.45
            }
      }

      Rectangle{
            id: trackTitleBG
            width: 0
            height: parent.height * 0.05
            anchors.top: parent.top
            anchors.right:parent.right
            Text{
                  id: trackTitleText
                  anchors.centerIn: parent
                  font.pixelSize: parent.height * 0.4
                  text: "Hofkgo"
                  color: "white"
                  font.family: customFonts.barlowLight
                  font.weight: Font.Light
            }
            Behavior on width{

                  NumberAnimation{
                        duration: 200
                        easing.type: Easing.InOutQuad
                  }
            }

            gradient: Gradient{
                  orientation: Gradient.Horizontal
                  GradientStop{ position:0; color: "transparent"}
                  GradientStop{position:1; color: "black"}
            }
      }

      Timer{
            id: randTrackAttemptTimer
            interval: 2000
            running: false
            repeat: true
            onTriggered: dataHandlerModel.attemptRandomTrackStart(Math.random() * (dataHandlerModel.mapCount - 0) + 0);
      }

      MouseArea{
            id: startMouseArea
            anchors.fill: parent

            onClicked:{
                  displayMenu = true
                  enabled = false
                   soundEffects.optionConfirm.play()
                  clickToStartTextOpacity.start()
            }
      }

      ImageParticle{
            id: particleImg
            source: "Particles/MemoryParticle.png"
            system: particleSystem
            z: 2
      }
      Timer {
            id: blankScreenDelayTimer
            interval: 1000
            running: false
            onTriggered: blankScreenOpacity.start()

      }

      Timer {
            id: sceneTransitionDelay
            interval: 5000
            repeat: true
            onTriggered: switchToMapSelect();
      }

      function switchToMapSelect(){
            windowObject.switchToMapSelect()
      }

      Rectangle{
            id: blankScreen
            color: "black"
            z:3
            anchors.fill: parent
            opacity:0

            NumberAnimation {
                  id: blankScreenOpacity
                  target: blankScreen
                  property: "opacity"
                  duration: 200
                  to: 1

                  onStopped: {
                        dataHandlerModel.titleScreen = false;
                        sceneTransitionDelay.start()
                  }
            }

            NumberAnimation {
                  id: blankScreenOpaque
                  target: blankScreen
                  property: "opacity"
                  duration: 200
                  to: 0
            }
      }

      ParticleSystem{
            id: particleSystem
      }

      Emitter{
            id: emitter
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: windowObject.width
            system: particleSystem
            maximumEmitted: 15
            emitRate: 1
            lifeSpan: 10000
            lifeSpanVariation: 500
            size: 32
            sizeVariation: 10
            Layout.column: 0
            z: 0
            velocity: AngleDirection {
                  angle: -90
                  magnitude: 40
                  magnitudeVariation: 20 //If this num is equal to magnitude, some particles will remain static
            }
      }

      CustomFonts{
            id: customFonts
      }


      Timer {
            id: menuDisplayTimer
            interval: 1000
            running: false
            onTriggered: mapSelectContainerOpacity.start()
      }

      Item{
            id: menuContainer
            width: windowObject.width * .4
            height: width
            anchors.centerIn: parent
            z: 2

            Image{
                  source: "UI/RebirthDiamond.png"
                  fillMode: Image.PreserveAspectFit
                  anchors.fill: parent

                  ColumnLayout{
                        spacing: 0
                        anchors.fill: parent
                        RowLayout{
                              Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                              Layout.fillWidth: true
                              Layout.preferredHeight: menuContainer.height * .65
                              spacing: 0
                              Image {
                                    property real rebirthTextWidth: menuContainer.width
                                    id: rebirthText
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                                    source: "UI/Rebirth.png"
                                    fillMode: Image.PreserveAspectFit
                                    Layout.preferredWidth: rebirthTextWidth
                                    Layout.preferredHeight: width * .3
                                    Layout.margins: 0
                              }


                        }
                        Item{
                              Layout.alignment: Qt.AlignTop |Qt.AlignHCenter
                              Layout.fillWidth: true
                              Layout.fillHeight: true
                              ColumnLayout{
                                    anchors.top: parent.top
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    spacing: 10
                                    Text{
                                          id:clickToStartText
                                          text: "Click to Start"
                                          font.family: customFonts.barlowLight
                                          font.weight: Font.Light
                                          Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                                          font.pixelSize: menuContainer.height * .05



                                          NumberAnimation {
                                                id: clickToStartTextOpacity
                                                target: clickToStartText
                                                property: "opacity"
                                                duration: 200
                                                to: displayMenu ? 0 : 1

                                                onStopped: {
                                                      clickToStartText.visible = false
                                                      menuDisplayTimer.start()
                                                }
                                          }
                                    }
                                    Item{
                                          property bool hovered: false
                                          id: mapSelectContainer
                                          Layout.preferredWidth: mapSelectText.contentWidth
                                          Layout.preferredHeight: mapSelectText.contentHeight
                                          Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                                          opacity:0
                                          Layout.topMargin: 10

                                          NumberAnimation {
                                                id: mapSelectContainerOpacity
                                                targets: [mapSelectContainer, exitGameContainer]
                                                property: "opacity"
                                                duration: 300
                                                to: displayMenu ? 1 : 0
                                                onStopped: {
                                                      mapSelectMouseArea.enabled=true
                                                      exitGameMouseArea.enabled=true
                                                     // latencyConfigMouseArea.enabled =true
                                                }
                                          }

                                          MouseArea{
                                                id: mapSelectMouseArea
                                                enabled: false
                                                anchors.fill: parent
                                                hoverEnabled: true

                                                onEntered:{
                                                      mapSelectContainer.hovered = true
                                                       soundEffects.menuHover.play()

                                                }
                                                onExited:{
                                                      mapSelectContainer.hovered = false
                                                }

                                                onClicked:{
                                                      mapSelect = true
                                                      enabled = false
                                                      //latencyConfigMouseArea.enabled = false
                                                      exitGameMouseArea.enabled = false
                                                       soundEffects.optionConfirm.play()
                                                      blankScreenDelayTimer.start()
                                                }
                                          }

                                          Text{
                                                id: mapSelectText
                                                text: "Map Select"
                                                font.family: customFonts.barlowLight
                                                font.weight: Font.Light
                                                font.pixelSize: mapSelectContainer.hovered ? menuContainer.height * .06: menuContainer.height * .05
                                                opacity: mapSelect ? 0 : 1
                                                anchors.horizontalCenter: parent.horizontalCenter

                                                Behavior on font.pixelSize {
                                                      NumberAnimation {
                                                            duration: 100
                                                            easing.type: Easing.InOutQuad
                                                      }

                                                }

                                                Behavior on opacity{
                                                      NumberAnimation
                                                      {
                                                            duration: 200
                                                            easing.type: Easing.InOutQuad
                                                      }
                                                }
                                          }
                                    }


                                    /* Configure button press delay (unused) */
                                    // Item{
                                    //       property bool hovered: false
                                    //       id: latencyConfigContainer
                                    //       opacity: 0
                                    //       Layout.preferredWidth: mapSelectText.contentWidth
                                    //       Layout.preferredHeight: mapSelectText.contentHeight
                                    //       Layout.alignment: Qt.AlignTop | Qt.AlignHCenter


                                    //       MouseArea{
                                    //             id: latencyConfigMouseArea
                                    //             enabled: false
                                    //             anchors.fill: parent
                                    //             hoverEnabled: true

                                    //             onEntered:{
                                    //                   latencyConfigContainer.hovered = true
                                    //                    soundEffects.menuHover.play()

                                    //             }
                                    //             onExited:{
                                    //                   latencyConfigContainer.hovered = false
                                    //             }

                                    //             onClicked:{

                                    //             }
                                    //       }

                                    //       Text{
                                    //             id: latencyConfigText
                                    //             text: "Latency Config"
                                    //             color: "grey"
                                    //             font.family: customFonts.barlowLight
                                    //             font.weight: Font.Light
                                    //             font.pixelSize: latencyConfigContainer.hovered ? menuContainer.height * .06: menuContainer.height * .05
                                    //             opacity: mapSelect ? 0 : 1
                                    //             anchors.horizontalCenter: parent.horizontalCenter

                                    //             Behavior on font.pixelSize {
                                    //                   NumberAnimation {
                                    //                         duration: 100
                                    //                         easing.type: Easing.InOutQuad
                                    //                   }
                                    //             }

                                    //             Behavior on opacity{
                                    //                   NumberAnimation
                                    //                   {
                                    //                         duration: 200
                                    //                         easing.type: Easing.InOutQuad
                                    //                   }
                                    //             }
                                    //       }
                                    // }

                                    Item{
                                          property bool hovered: false
                                          id: exitGameContainer
                                          opacity: 0
                                          Layout.preferredWidth: exitGameText.contentWidth
                                          Layout.preferredHeight: exitGameText.contentHeight
                                          Layout.alignment: Qt.AlignTop | Qt.AlignHCenter


                                          MouseArea{
                                                id: exitGameMouseArea
                                                enabled: false
                                                anchors.fill: parent
                                                hoverEnabled: true

                                                onEntered:{
                                                      exitGameContainer.hovered = true
                                                       soundEffects.menuHover.play()

                                                }
                                                onExited:{
                                                      exitGameContainer.hovered = false
                                                }

                                                onClicked:{
                                                       soundEffects.optionConfirm.play()
                                                      Qt.quit()
                                                }
                                          }

                                          Text{
                                                id: exitGameText
                                                text: "Exit Game"
                                                color: "black"
                                                font.family: customFonts.barlowLight
                                                font.weight: Font.Light
                                                font.pixelSize: exitGameContainer.hovered ? menuContainer.height * .06: menuContainer.height * .05
                                                opacity: mapSelect ? 0 : 1
                                                anchors.horizontalCenter: parent.horizontalCenter

                                                Behavior on font.pixelSize {
                                                      NumberAnimation {
                                                            duration: 100
                                                            easing.type: Easing.InOutQuad
                                                      }
                                                }

                                                Behavior on opacity{
                                                      NumberAnimation
                                                      {
                                                            duration: 200
                                                            easing.type: Easing.InOutQuad
                                                      }
                                                }
                                          }
                                    }

                              }
                        }
                  }
            }
      }
}

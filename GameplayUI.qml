import QtQuick
import Rebirth
import HitObjectEnums 1.0
import QtQuick.Layouts
import QtQuick.Particles
import Qt5Compat.GraphicalEffects


Item {

  id:root

  anchors.fill: parent
  width: windowObject.width
      objectName: "root"
  height: windowObject.height

  z: 0

  required property var windowObject
  property int topMargin: 20
  property real controlsTextSizeMultiplier: .02
  property real laneBorderOpacity: .3
  property real trackOpacity: .65
  property bool showProgressInfo: false
  property real laneLightAlpha: 0.6
  property real laneOpacityDuration: 100
  property bool gameEnd: false



  Component.onCompleted: {
    stage.trackVerticalStretch.start()
    showProgressInfo = true
    gameStartTimer.start()
  }

  Connections{
    target: gameManager


    function onCapacityProgressChanged(capacityProgress, stateName, capacityNumber){
      currentStateText.stateTitle = stateName
      currentStateChangeAnim.start()
      emotionalCapacity.tokenDiscovered(capacityProgress, capacityNumber)
      emotionalCapacity.setTokenSource(capacityProgress, stateName, capacityNumber)
    }
  }

  Connections{
    target: hitObjectModel

    function onNoteHit(judgement, colorType, xPosition, noteWidth){
      var perfectShockwavePath = "Particles/ParticleShockwavePerfect.png"
      var defaultShockwavePath = "Particles/ParticleShockwave.png"
      var component = Qt.createComponent("NoteShatterEffect.qml")
      var noteShatter = component.createObject(root, {windowObject: windowObject, containterSize: noteWidth, x: stage.trackXPosition + xPosition,  bottomMargin: root.height - judgementLineHandler.yPosition, effectRGBList: setParticleRGB(judgement, colorType ),colorVariationValue: judgement === "Perfect" ? 1 : 0,  core: judgement === "Perfect" ? perfectShockwavePath : defaultShockwavePath, type: colorType, judgementVal: judgement})
    }
  }

  CustomFonts{
    id:customFont
  }

  QmlSoundEffects{
    id:soundEffects
  }


  Timer{
    id: gameStartTimer
    interval: 3000
    running: false
    onTriggered: begin()
  }

  Timer{
    id: displayResultDelayTimer
    interval: 1000
    running: false
    onTriggered:showResult()
  }

  Timer{
    id: sceneTransitionDelayTimer
    interval: 3000
    running: false
    onTriggered: windowObject.switchToResults()
  }

  Timer {
    id: blankScreenelayTimer
    interval: 1000
    running: false
    onTriggered: blankScreenOpacity.start()

  }

  Rectangle{
    id: blankScreen
    color: "black"
    z:10
    anchors.fill: parent
    opacity:0

    NumberAnimation {
      id: blankScreenOpacity
      target: blankScreen
      property: "opacity"
      duration: 200
      to: 1

      onStopped: {
        sceneTransitionDelayTimer.start()
      }
    }
  }

  Image{
    id: resultsStatusImg
    source: "UI/Forgotten.png"
    width: parent.width * .8
    height: 0
    anchors.centerIn:parent
    fillMode: Image.PreserveAspectFit
    z: 10

    SequentialAnimation{
      id:resultsStatusDisplayAnim
      running: false


      NumberAnimation{
        target: resultsStatusImg
        property: "height"
        from: 0
        to: width/2
        duration: 1000
        onStarted: {
          resultDisplaySound.play()
        }
      }

      PauseAnimation {
        duration: 1000
      }

      NumberAnimation{
        target: resultsStatusImg
        property: "height"
        from: width/2
        to: 0
        duration: 1000
      }
      onStopped:{
        endGame()
      }
    }
  }

  Item{
    id: comboColumnContainer
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: parent.height * 0.05
    z: 5
    width: stage.trackWidth * .1
    height: stage.trackHeight *.1

    opacity: gameManager.combo > 1 ? 1 : 0
    Behavior on opacity{
      NumberAnimation{
        duration: 150
      }
    }

    ColumnLayout{
      id: comboInfoColumn
      spacing: 0
      anchors.fill: parent
      Item{
        width:comboColumnContainer.width
        height: width * .15
        Layout.alignment: Qt.AlignCenter

        Text{
          anchors.centerIn: parent
          id:comboText
          font.family:customFont.barlowLight
          font.weight: Font.Light
          text: "Combo"
          color: "white"
          font.pixelSize: stage.trackHeight * .03
        }
      }

      Item{

        Layout.alignment: Qt.AlignCenter
        width:comboColumnContainer.width
        height:width * .2

        Text{
          id:combo
          anchors.centerIn: parent
          font.family:customFont.barlowLight
          font.weight: Font.Light
          font.italic: true

          text: "0"
          color: "white"
          font.pixelSize:  stage.trackHeight * .05
          z: 1
          visible: false

          Connections{
            target: gameManager
            function onComboChanged(){
              combo.text = gameManager.combo
            }
            function onResultCalculated(memoryStatus){
              if(gameManager.memoryStatus === 0){
                resultsStatusImg.source = "UI/Preserved.png"
              }
              if(gameManager.memoryStatus === 1){
                resultsStatusImg.source = "UI/Recovered.png"
              }
              if(gameManager.memoryStatus === 2){
                resultsStatusImg.source = "UI/Forgotten.png"
              }
            }
          }

        }
        Rectangle{
          id:comboGradient
          anchors.fill: combo
          layer.enabled: true
          layer.smooth: true

          gradient: Gradient {
            GradientStop {id: cGStopA; position: 0.3; color: dataHandlerModel.highlightColor ?  Qt.lighter(dataHandlerModel.highlightColor, 3.0) : "white" }
            GradientStop {id: cGStopB; position: 1.0; color: dataHandlerModel.primaryColor ?  Qt.lighter(dataHandlerModel.primaryColor, 1.5):  "white" }
          }
          visible: false

        }

        OpacityMask{
          anchors.fill: combo
          source:comboGradient //Rectangle gradient
          maskSource: combo //Determines the bounds of the effect
        }


      }
    }
  }

  Text{
    id:score

    anchors.rightMargin:  20
    anchors.top: parent.top
    anchors.topMargin:  topMargin
    anchors.right: parent.right
    z: 1

    text: "0"
    color: "white"
    font.weight: Font.Light
    font.family:customFont.barlowLight
    font.pixelSize: parent.width * .025

    opacity: showProgressInfo ? 1 : 0

    Behavior on opacity{

      NumberAnimation{
        duration: 300
        easing.type: Easing.InOutQuad
      }
    }

    Connections{
      target: gameManager

      function onScoreChanged(){
        score.text = gameManager.score
      }
    }

  }

  ColumnLayout{
    id: emotionalCapacityDisplay
    spacing: 5
    anchors.topMargin: topMargin
    anchors.left: parent.left
    anchors.top: parent.top
    width: windowObject.width * .2
    z: 1
    opacity: showProgressInfo ? 1 : 0
    Behavior on opacity{

      NumberAnimation{
        duration: 300
        easing.type: Easing.InOutQuad
      }
    }

    EmotionalCapacity{
      id: emotionalCapacity
      Layout.alignment: Qt.AlignHCenter
      Layout.preferredWidth: parent.width
      Layout.preferredHeight: width/4
    }

    Text{
      property var stateTitle

      id:currentStateText
      font.family:customFont.barlowLight
      font.weight: Font.Light
      text: ""
      color: "white"
      font.pixelSize: windowObject.width * .02
      Layout.alignment: Qt.AlignHCenter

      NumberAnimation{
        id: currentStateChangeAnim
        target:currentStateText
        property: "opacity"
        from: 1
        to: 0
        duration: 200
        onStopped: {
          currentStateText.text = currentStateText.stateTitle
          currentStateDisplayAnim.start()
        }
      }

      SequentialAnimation{
        id: currentStateDisplayAnim
        running: false

        PauseAnimation {
          duration: 1000
        }

        NumberAnimation{
          target:currentStateText
          property: "opacity"
          from: 0
          to: 1
          duration: 200
        }

      }
    }

  }


  ImageParticle{
    id: stageParticleImg
    source: dataHandlerModel.particleFilePath === "" ?  dataHandlerModel.particleFilePath : "Particles/ParticleBase.png"
    system: stageParticleSystem
    color: dataHandlerModel.particleColor
    z: 1
  }

  ParticleSystem{
    id: stageParticleSystem
  }

  Stage{
    id: stage
    mainParticleSystem: stageParticleSystem
    mainParticleImg: stageParticleImg
    z: 2
  }


  function begin() {
    gameInitializer.begin()

    if(dataHandlerModel.laneTheme === "rebirth"){
      stage.laneLightColor = Qt.rgba(1.0, 0.85, 0.4,laneLightAlpha)
    }
    else if (dataHandlerModel.laneTheme === "pure"){
      stage.laneLightColor=  Qt.rgba(0.8, 0.5, 1.0, laneLightAlpha)
    }
    else if (dataHandlerModel.laneTheme === "life"){
      stage.laneLightColor = Qt.rgba(0.5, 1.0, 1.0,laneLightAlpha)
    }
  }
  function endGame(){
    soundEffects.resultHide.play()
    stage.trackVerticalCompress.start()
    stage.laneBorderVerticalCompress.start()
    blankScreenelayTimer.start()
  }

  function showResult(){
    soundEffects.resultDisplay.play()
    resultsStatusDisplayAnim.start()
  }

  function setParticleRGB( judgement, colorType){

    /*This function serves as a workaround to the burst particle issue:
    Burst ignores default particle color attributes and uses the RBG value of 1,1,1.
    However, if we grab the RGB values we intend to use, we can color the individual particles after the burst.
    */

    if(colorType === HitObjectEnums.QuarterNote)
      return [0.6039,0.9843,1.0]
    else if(colorType === HitObjectEnums.TripletNote)
      return [1.0,0.5176,0.9569]
    else if(colorType === HitObjectEnums.EighthNote)
      return[1.0,0.976,0.529]
    else if (colorType === HitObjectEnums.DoubleTapNote)
      return [0.556,1.0,0.451]
    else if (colorType === HitObjectEnums.ForgottenNote)
      return [1.0,0,0]
    else
      return [1,1,1]
  }
}


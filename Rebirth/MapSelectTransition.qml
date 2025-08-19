import QtQuick 2.15
import QtQuick.Layouts
Item{
       id:root
       required property var windowHeight
       property bool transitionActive: false
       anchors.fill: parent
       opacity: transitionActive ? 1 : 0
       z: 10

       CustomFonts{
              id:customFont
       }

       Timer {
              id: gameStartTimer
              interval: 2000
              running: false
              onTriggered: prepGameStart()
       }

       function prepGameStart(){
              opacityAnim.start()
              dataHandlerModel.gameStart = true
              backgroundObject.gameStart = true
       }

       transform: Scale{
              id: scaleObj
              origin.x: width/2
              origin.y: height/2
              xScale: 5
              yScale: 5
       }

       onTransitionActiveChanged: {
              scaleAnim.start()
              gameStartTimer.start()
       }

       PropertyAnimation {
              id: scaleAnim
              target: scaleObj
              properties: "xScale,yScale"
              duration: 300
              to:  1
              easing.type: Easing.InOutQuad
       }

       PropertyAnimation {
              id: opacityAnim
              target: root
              properties: "opacity"
              duration: 300
              to:  0
              easing.type: Easing.InOutQuad
              onStopped: {
                     transitionActive = false
                     windowObject.switchToGame();
              }
       }


       Behavior on opacity{
              NumberAnimation {
                     duration: 200
                     easing.type: Easing.InOutQuad
              }
       }

       ColumnLayout {
              anchors.centerIn: parent
              Text{
                     Layout.alignment: Qt.AlignCenter
                     text: dataHandlerModel.currentMapTitle
                     font.family:customFont.barlowLight
                     font.weight: Font.Light
                     font.pixelSize: windowHeight * .08
                     color: "white"
              }
              Text{
                     text: "Difficulty"
                     Layout.alignment: Qt.AlignCenter
                     font.family:customFont.barlowLight
                     font.weight: Font.Light
                     font.pixelSize:  windowHeight * .1
                     color: "white"
              }

              Text{
                     Layout.alignment: Qt.AlignCenter
                     text: dataHandlerModel.currentMapDifficulty
                     font.family:customFont.barlowRegular
                     font.pixelSize:  windowHeight * .2
                     color: dataHandlerModel.currentMapDifficulty < 6 ? "white" :"red"
              }
       }

}

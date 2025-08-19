import QtQuick 2.15
import QtMultimedia

import MessageHandler 1.0


Item{
    id: root
    anchors.fill: parent
    required property var windowObject
    required property bool titleScreen
    property bool errorMsg: false
    property int msgIndex: 0
    z: 30

    QmlSoundEffects{
        id:soundEffects
    }

    Column{
        spacing: 5
        Repeater{
            id:repeater
            model: 4
            delegate: Rectangle {

                property bool msgActive: false

                width: windowObject.width * 0.3
                height: width * 0.5
                radius: width * 0.02

                x: windowObject.width + 10
                border.width: width * 0.01
                border.color: Qt.darker(color, 1.5)

                Connections{
                    target: messageHandler

                    function onLogMessage(text, status){
                        if(!titleScreen){
                            if (msgIndex === model.index){
                                msgActive = true
                            }
                            else{
                                msgActive = false
                            }

                            activateNotification(text, status)
                        }
                    }
                }

                Connections{
                    target: dataHandlerModel
                    function onErrorOccurred(text){
                        notifText.text = text + "\n\nClick to Hide"
                        errorMsg = true
                        activateNotification()
                    }
                }

                CustomFonts{
                    id: customFonts
                }

                z: 20

                color: "white"
                MouseArea{
                    anchors.fill: parent
                    onClicked:{
                        notificationFinished()
                    }
                }


                function notificationFinished(){
                    soundEffects.menuSlide.play()
                    notifHideAnim.start()
                }

                function activateNotification(text, status){

                    if (msgActive){

                        if(status === MessageHandlerEnums.FAILED){

                            notifText.text = text + "\n\nClick to Hide"
                            errorMsg = true
                        }
                        else if (status === MessageHandlerEnums.SUCCESS){
                            notifText.text = text + "\n\nClick to Hide"
                            errorMsg = false                            }
                        else
                        {
                            notifText.text = text + "\n\nClick to Hide"
                            errorMsg = false
                        }
                        soundEffects.menuSlide.play()
                        notifDisplayAnim.start()

                    }
                }


                NumberAnimation{
                    id: notifDisplayAnim
                    target: repeater.itemAt(msgIndex)
                    property: "x"
                    from: windowObject.width + 10
                    to:  windowObject.width - width
                    duration: 200
                    running: false
                    easing.type: Easing.InOutQuad
                    onStarted: {
                       running = msgActive ? true : false
                    }

                    onStopped:{
                        msgIndex++;
                        if(msgIndex > 3){
                            msgIndex = 0;
                        }
                    }
                }

                NumberAnimation{
                    id: notifHideAnim
                    target: repeater.itemAt(model.index)
                    property: "x"
                    from: windowObject.width - width
                    to: windowObject.width + 10
                    duration: 200
                    easing.type: Easing.InOutQuad
                    running: false
                    onStarted: {
                        running = msgActive ? true : false
                    }
                    onStopped: {
                        if(msgActive)
                            msgActive = false
                         msgIndex = 0;
                    }

                }


                Text{

                    id: notifText
                    anchors.centerIn: parent
                    font.family: customFonts.barlowRegular
                    width: parent.width * 0.8
                    height: parent.height * 0.8
                    font.pixelSize: height * 0.1
                    wrapMode: Text.Wrap
                    color: errorMsg ?  Qt.darker("red", 1.5):Qt.darker("green", 1.5)
                    text: ""
                }
            }
        }
    }


}



import QtQuick
import Rebirth
//import HitObjectCpp


Window {
    id: window
    width: 640
    height: 480


    visible: true
    title: qsTr("Rebirth")
    color: "white"
    property int offset: 20
    property int boundaryOffset: 100
    property bool restrictionA: false
    property bool restrictionB: false
    property bool firstRun: true
    property bool atTitleScreen: true
    property var hitObject
    property var currentScene
    property var mapBackground

    Component.onCompleted: {
        switchToTitle(true)
        /*Debug*/
        //switchToGame(true)
    }

    NotificationMessage{
        id: notif
        windowObject: window
        titleScreen: atTitleScreen
    }


    function switchToTitle(activateTransition){
        atTitleScreen = true
        if (currentScene){
            currentScene.destroy()
        }

        var component = Qt.createComponent("TitleScreen.qml")
        if(component.status === Component.Ready){
            currentScene = component.createObject(window, {windowObject: window, transition: activateTransition})
        }
        else{

        }
    }

    function switchToMapSelect(){
        atTitleScreen = false
        if (currentScene){
            currentScene.destroy()
        }

        if (!mapBackground){
            var backgroundComponent = Qt.createComponent("MapBackground.qml")
            if(backgroundComponent.status === Component.Ready){
                mapBackground = backgroundComponent.createObject(window,{ windowObject: window, width: window.width,
                                                      height: window.height})
            }
        }

        var component = Qt.createComponent("MapSelectMenu.qml")
        if(component.status === Component.Ready){
            currentScene = component.createObject(window,{ windowObject: window, backgroundObject: mapBackground, width: window.width,
                                                  height: window.height})
        }
        else{

        }
    }
    function switchToGame(){
        atTitleScreen = false
        if (currentScene){
            currentScene.destroy()
        }

        var component = Qt.createComponent("GameplayUI.qml")
        if(component.status === Component.Ready){
            currentScene = component.createObject(window,{ windowObject: window, width: window.width,
                                                      height: window.height})
        }
        else{

        }
    }
    function switchToResults(){
        atTitleScreen = false
        if (currentScene){
            currentScene.destroy()
        }

        var component = Qt.createComponent("Results.qml")
        if(component.status === Component.Ready){
            currentScene = component.createObject(window,{ windowObject: window, backgroundObject: mapBackground, width: window.width,
                                                      height: window.height})
        }
        else{

        }
    }
}

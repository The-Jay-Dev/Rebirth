import QtQuick 2.15
import Qt5Compat.GraphicalEffects
Item{
    required property var windowObject
    property real gameplayDim: 0.6
    property real mapSelectDim: 0.4
    property bool gameStart: false

    width: windowObject.width
    height: windowObject.height
    anchors.fill: parent
    Component.onCompleted: {
        img.source = dataHandlerModel.currentMapImgPath
    }


    Image {

        id:img
        anchors.fill: parent
        z: 0

        Connections {
            target: dataHandlerModel
            function onDataRefreshed(){
                img.source = dataHandlerModel.currentMapImgPath
            }

            function onCurrentMapImgPathChanged(){
                img.source = dataHandlerModel.currentMapImgPath
            }

        }


    }

    Rectangle{
        id: imgDim
        anchors.fill: parent
        opacity: gameStart ? gameplayDim : mapSelectDim
        z: 1
        gradient: Gradient{
            GradientStop{
                id: imgDimColorA
                position: gameStart ? 0.1 : 1.0
                color: "black"
                Behavior on color{
                    NumberAnimation{
                        duration: 300
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
        GradientStop{
            id: imgDimColorB
            position: 1.0
            color: "black"
            Behavior on color{
                NumberAnimation{
                    duration: 300
                    easing.type: Easing.InOutQuad
                }
            }
        }

        Behavior on opacity{
            NumberAnimation{
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    }


    FastBlur {
        source:img
        anchors.fill: parent
        radius: gameStart ? 20 : 48
        Behavior on radius{
            NumberAnimation{
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    }
}



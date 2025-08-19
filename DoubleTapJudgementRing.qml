import QtQuick 2.15

Item {
    id: root
    property real ringScale

    Rectangle{
        id:outline
        height: parent.height
        width: parent.width
        color: "transparent"
        border.color: "white"
        border.width: width * 0.02
        anchors.centerIn: parent
        radius: height * .05
    }

    Connections{
        target: hitObjectModel
        function onUpdateSecondTapRingScale(updatedScale, opacityProgress){
            ringScale = updatedScale
           // outline.opacity = opacityProgress
        }
    }

    transform: Scale{
        origin.x: width/2
        origin.y: height/2
        xScale: ringScale
        yScale: ringScale
    }
}

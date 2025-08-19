import QtQuick
import Qt5Compat.GraphicalEffects

Item{

    property int noteOpacity: 1;
    property  string eighthNoteColor:  "#fffbac"
    property  string tripletNoteColor: "#ff84f4"
    property  string paradoxNoteColor: "#ff5b5b"
    property  string quarterNoteColor:  "#9afbff"
    property  string holdNoteColor: "white"

    property var debugTextValue;
    property real noteWidth;
    property real noteHeight;

    property string hitObjectColor: quarterNoteColor;

    width: noteWidth
    height: noteHeight
    z:2

    Rectangle{
        id:outline
        height: parent.height
        width: parent.width
        color: "black"
        border.color: hitObjectColor
        border.width: width * 0.02
        anchors.centerIn: parent
        radius: height * .05
        opacity: noteOpacity

        Image {
            id: base
            source: "UI/NoteBaseNeutral.png"
            fillMode: Image.PreserveAspectFit
            height: parent.height * .4
            width: height
            anchors.centerIn: parent
        }

        // Glow{
        //     source: base
        //     anchors.fill: base
        //     radius: 8
        //     spread: 0.4
        //     samples: 25
        //     color: hitObjectColor
        // }

        // Text{
        //     id: debugText
        //     text: debugTextValue
        //     anchors.centerIn: parent
        //     color: "white"
        //     font.pixelSize: parent.height * .8
        //     visible: true //comment out base and replace with debugText for value debugging
        // }
    }

    Glow{
        source: outline
        anchors.fill: outline
        radius: 8
        spread: 0.4
        samples: 17
        color: hitObjectColor
        opacity: noteOpacity
    }


}





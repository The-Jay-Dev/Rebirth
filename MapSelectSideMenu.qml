import QtQuick 2.15
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects




Rectangle {
    id: buttonContainer

    Behavior on x {
        NumberAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
    required property var menuWidth
    required property var menuHeight
    property bool open: false

    width: menuWidth
    height: menuHeight
    z:10
    x: open ? parent.width - width : parent.width + width
    y: 0
    color: Qt.rgba(1,1,1,0.4)

    ColumnLayout{
        spacing: 5
        width: parent.width
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        Repeater{
            model: dataHandlerModel
            delegate:
                MapSelectSideMenuOptionBtn{
            }
        }
    }

}



import QtQuick 2.15
import QtQuick.Layouts
import QtMultimedia

Item {
    id:root
    property bool hovered
    Layout.fillWidth: true
    Layout.preferredHeight: menuHeight * .1

    CustomFonts{
        id:customFont
    }

    QmlSoundEffects{
        id: soundEffects
    }

    MouseArea{
        anchors.fill: parent
        id: mouseArea
        hoverEnabled: true
        onEntered: {
            hovered = true
             soundEffects.menuHover.play()
        }
        onExited: {
            hovered = false
        }
        onClicked: {
             soundEffects.optionConfirm.play()
            dataHandlerModel.currentMapIndex = model.index
            dataHandlerModel.setCurrentMapDetails()
        }
    }

    Text{
        id: trackName
        anchors.centerIn: parent
        text: title

        font.family:customFont.barlowLight
        font.weight: Font.Light
        color: hovered ? "orange" : "white"
        width: parent.width
        fontSizeMode: Text.Fit
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pixelSize: hovered ? parent.height * 0.45 : parent.height * 0.4
        elide: Text.ElideRight

         Behavior on font.pixelSize {


            NumberAnimation {
                duration: 100
                easing.type: Easing.InOutQuad
            }
        }
    }
}

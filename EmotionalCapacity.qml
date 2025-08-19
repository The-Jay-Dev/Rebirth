import QtQuick 2.15
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Item {
    id: root

    property bool tokenOneActive: false
    property bool tokenTwoActive: false
    property bool tokenThreeActive: false
    property bool tokenFourActive: false
    property bool tokenAnomalyActive: false
    property real maxScale: 1.1
    property real minScale: 0.9
    property int glowSamples: 17
    QmlSoundEffects{
        id:sfx
    }

    function setTokenSource(num, stateName, capacityNumber){

        sfx.emotionalCapacityStateChange.play()
        scaleUp.start()
        if(num === 0){
            firstToken.stateName = stateName
            firstTokenOpacityAnim.start()
            firstTokenGlow.samples = glowSamples
            secondTokenGlow.samples = 0
            thirdTokenGlow.samples = 0
            fourthTokenGlow.samples = 0
            anomalyTokenGlow.samples = 0

        }
        else if(num === 1){
            secondToken.stateName = stateName
            secondTokenOpacityAnim.start()
            firstTokenGlow.samples = 0
            secondTokenGlow.samples = glowSamples
            thirdTokenGlow.samples = 0
            fourthTokenGlow.samples = 0
            anomalyTokenGlow.samples = 0
        }
        else if(num === 2){
            thirdToken.stateName = stateName
            thirdTokenOpacityAnim.start()
            firstTokenGlow.samples = 0
            secondTokenGlow.samples = 0
            thirdTokenGlow.samples = glowSamples
            fourthTokenGlow.samples = 0
            anomalyTokenGlow.samples = 0
        }
        else if(num === 3  && capacityNumber < 5){
            fourthToken.stateName = stateName
            fourthTokenOpacityAnim.start()
            firstTokenGlow.samples = 0
            secondTokenGlow.samples = 0
            thirdTokenGlow.samples = 0
            fourthTokenGlow.samples = glowSamples
            anomalyTokenGlow.samples = 0
        }
        else if (num === 3 && capacityNumber >= 5){
            anomalyToken.stateName = stateName
            anomalyTokenOpacityAnim.start()
            firstTokenGlow.samples = 0
            secondTokenGlow.samples = 0
            thirdTokenGlow.samples = 0
            fourthTokenGlow.samples = 0
            anomalyTokenGlow.samples = glowSamples
        }

        else if(num === 4 && capacityNumber >= 5){
            fourthToken.stateName = stateName
            fourthTokenOpacityAnim.start()
            firstTokenGlow.samples = 0
            secondTokenGlow.samples = 0
            thirdTokenGlow.samples = 0
            fourthTokenGlow.samples = glowSamples
            anomalyTokenGlow.samples = 0
        }
    }

    function tokenDiscovered(num, capacityNumber){
        if (num === 0){
            tokenOneActive = true
            tokenTwoActive = false
            tokenThreeActive = false
            tokenFourActive = false
            tokenAnomalyActive = false
        }
        else if (num === 1){
            tokenOneActive = false
            tokenTwoActive = true
            tokenThreeActive = false
            tokenFourActive = false
            tokenAnomalyActive = false
        }
        else if (num === 2){
            tokenOneActive = false
            tokenTwoActive = false
            tokenThreeActive = true
            tokenFourActive = false
            tokenAnomalyActive = false
        }
        else if (num === 3  && capacityNumber < 5){
            tokenOneActive = false
            tokenTwoActive = false
            tokenThreeActive = false
            tokenFourActive = true
            tokenAnomalyActive = false
        }
        else if (num === 3 && capacityNumber >= 5){
            tokenOneActive = false
            tokenTwoActive = false
            tokenThreeActive = false
            tokenFourActive = false
            tokenAnomalyActive = true
        }
        else if (num === 4 && capacityNumber >= 5){

            tokenOneActive = false
            tokenTwoActive = false
            tokenThreeActive = false
            tokenFourActive = true
            tokenAnomalyActive = false
        }
    }

    Image{
        id: emotionalCapacityImg
        source: "UI/EmotionalCapacity.png"
        fillMode: Image.PreserveAspectFit
        anchors.fill: parent

        transform: Scale{
            id: ecScale
            yScale: minScale
            xScale: minScale
            origin.x: width/2
            origin.y: height/2
        }

        PropertyAnimation{
            id: scaleUp
            target: ecScale
            running: false
            properties: "xScale,yScale"
            from: minScale
            to: maxScale
        }

        PropertyAnimation{
            id: scaleDown
            target: ecScale
            running: false
            properties: "xScale,yScale"
            duration: 100
            from: maxScale
            to: minScale
            easing.type: Easing.InOutQuad
        }


        GridLayout{
            id: columnLayout
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.centerIn:parent
            anchors.topMargin: 10
            columnSpacing: 0
            rowSpacing: 0
            width: root.width
            height: root.height

            rows: 2
            columns: 4

            Item{
                Layout.row: 0
                Layout.column: 0
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter


                Image{
                    property string stateName
                    id: firstToken

                    source: "UI/Unknown.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    opacity: 0
                    width:root.width * .12
                    height: width

                    NumberAnimation{
                        id: firstTokenOpacityAnim
                        targets: [firstToken,firstTokenGlow]
                        duration: 200
                        property: "opacity"

                        from: firstToken.opacity
                        to: 0
                        running: false
                        onStopped:{
                            firstToken.source = "UI/" + firstToken.stateName + ".png"
                            firstTokenOpaqueAnim.start()
                        }
                    }

                    SequentialAnimation{
                        id: firstTokenOpaqueAnim
                        running: false

                        PauseAnimation {
                            duration: 1000
                        }
                        NumberAnimation{
                            targets: [firstToken,firstTokenGlow]
                            duration: 200
                            property: "opacity"
                            from: 0
                            to: 1
                        }
                        onStopped: {
                             sfx.emotionalCapacityStateDiscovered.play()
                            scaleDown.start()
                        }
                    }
                }
                Glow {
                    id: firstTokenGlow
                    anchors.fill: firstToken
                    source: firstToken
                    color: "white"
                    radius: 16
                    spread: 0.4
                    opacity: 0
                    samples: glowSamples
                    Behavior on samples{
                        NumberAnimation{
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }

            Item{

                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                Image{
                    property string stateName
                    id: secondToken
                    source: "UI/Unknown.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    width:root.width * .12
                    height: width
                    opacity: 0

                    NumberAnimation{
                        id: secondTokenOpacityAnim
                        targets: [secondToken,secondTokenGlow]
                        duration: 200
                        property: "opacity"
                        from: secondToken.opacity
                        to: 0
                        running: false
                        onStopped:{
                            secondToken.source = "UI/" + secondToken.stateName + ".png"
                            secondTokenOpaqueAnim.start()
                        }
                    }

                    SequentialAnimation{
                        id: secondTokenOpaqueAnim
                        running: false

                        PauseAnimation {
                            duration: 1000
                        }
                        NumberAnimation{
                            targets: [secondToken,secondTokenGlow]
                            duration: 200
                            property: "opacity"
                            from: 0
                            to: 1
                        }
                        onStopped: {
                             scaleDown.start()
                             sfx.emotionalCapacityStateDiscovered.play()
                        }
                    }

                }
                Glow {
                    id: secondTokenGlow
                    anchors.fill: secondToken
                    source: secondToken
                    color: "white"
                    radius: 16
                    spread: 0.4
                    samples: glowSamples
                    opacity: 0
                    Behavior on samples{
                        NumberAnimation{
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }

            Item{

                Layout.row: 0
                Layout.column: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                Image{
                    property string stateName
                    id: thirdToken
                    source: "UI/Unknown.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    opacity: 0
                    width:root.width * .12
                    height: width

                    NumberAnimation{
                        id: thirdTokenOpacityAnim
                        targets: [thirdToken,thirdTokenGlow]
                        duration: 200
                        property: "opacity"
                        from: thirdToken.opacity
                        to: 0
                        running: false
                        onStopped:{
                            thirdToken.source = "UI/" + thirdToken.stateName + ".png"
                            thirdTokenOpaqueAnim.start()
                        }
                    }

                    SequentialAnimation{
                        id: thirdTokenOpaqueAnim
                        running: false

                        PauseAnimation {
                            duration: 1000
                        }
                        NumberAnimation{
                            targets: [thirdToken,thirdTokenGlow]
                            duration: 200
                            property: "opacity"
                            from: 0
                            to: 1
                        }
                        onStopped: {
                            scaleDown.start()
                             sfx.emotionalCapacityStateDiscovered.play()
                        }
                    }
                }
                Glow {
                    id: thirdTokenGlow
                    anchors.fill: thirdToken
                    source: thirdToken
                    color: "white"
                    radius: 16
                    opacity: 0
                    spread: 0.4
                    samples: glowSamples
                    Behavior on samples{
                        NumberAnimation{
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }
            Item{

                Layout.row: 0
                Layout.column: 3
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                Glow {
                    id: fourthTokenGlow
                    anchors.fill: fourthToken
                    source: fourthToken
                    color: "white"
                    radius: 16
                    spread: 0.4
                    opacity: 0
                    samples: glowSamples
                    Behavior on samples{
                        NumberAnimation{
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
                Image{
                    property string stateName
                    id: fourthToken
                    source: "UI/Unknown.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    width:root.width * .12
                    height: width
                    opacity: 0

                    NumberAnimation{
                        id: fourthTokenOpacityAnim
                        targets: [fourthToken,fourthTokenGlow]
                        duration: 200
                        property: "opacity"
                        from: fourthToken.opacity
                        to: 0
                        running: false
                        onStopped:{
                            fourthToken.source = "UI/" + fourthToken.stateName + ".png"
                            fourthTokenOpaqueAnim.start()
                        }
                    }

                    SequentialAnimation{
                        id: fourthTokenOpaqueAnim
                        running: false

                        PauseAnimation {
                            duration: 1000
                        }
                        NumberAnimation{
                            targets: [fourthToken,fourthTokenGlow]
                            duration: 200
                            property: "opacity"
                            from: 0
                            to: 1
                        }
                        onStopped: {
                            scaleDown.start()
                            sfx.emotionalCapacityStateDiscovered.play()

                        }
                    }
                }
            }

            Item{
                Layout.fillWidth: true
                Layout.preferredHeight: columnLayout.height * .5
                Layout.alignment: Qt.AlignCenter
                Layout.column: 0

                Layout.row: 1
                Layout.columnSpan: 2

            }

            Item{

                Layout.row: 1
                Layout.columnSpan: 2
                Layout.column: 2
                Layout.fillWidth: true
                Layout.preferredHeight: columnLayout.height * .5
                Glow {
                    id: anomalyTokenGlow
                    anchors.fill: anomalyToken
                    source: anomalyToken
                    color: "white"
                    radius: 16
                    spread: 0.4
                    samples: glowSamples
                    opacity: 0
                    Behavior on samples{
                        NumberAnimation{
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
                Image{
                    id: anomalyToken
                    property string stateName
                    source: "UI/Unknown.png"
                    fillMode: Image.PreserveAspectFit

                    opacity: 0
                    anchors.centerIn: parent
                    width:root.width * .12
                    height: width

                    NumberAnimation{
                        id: anomalyTokenOpacityAnim
                        targets: [anomalyToken,firstTokenGlow]
                        duration: 200
                        property: "opacity"
                        from: anomalyToken.opacity
                        to: 0
                        running: false
                        onStopped:{
                            anomalyToken.source = "UI/" + anomalyToken.stateName + ".png"
                            anomalyTokenOpaqueAnim.start()
                        }
                    }

                    SequentialAnimation{
                        id: anomalyTokenOpaqueAnim
                        running: false

                        PauseAnimation {
                            duration: 1000
                        }
                        NumberAnimation{
                            targets: [anomalyToken,anomalyTokenGlow]
                            duration: 200
                            property: "opacity"
                            from: 0
                            to: 1
                        }
                    }
                }
            }
        }
    }
}

import QtQuick 2.15
import QtQuick.Particles

Item {
    required property real controllerWidth
    required property real emitterWidth
    required property bool isSlideController
    required property real bottomMargin
    required property var parentNoteIndex
    property real leftBounds
    property real rightBounds
    property real startX
    property bool controllerReady: false
    property int boundsOffset: 10

    id: root
    width: controllerWidth
    height: width
    anchors.bottom: parent.bottom
    anchors.bottomMargin: bottomMargin - height/2

    z:3
    Component.onCompleted: {
        waveImg.source = "Particles/ParticleShockwavePerfect.png"
    }

    ParticleSystem{
        id: coreSystem
        anchors.fill: parent

    }

    ParticleSystem{
        id: waveSystem
        anchors.fill: parent
    }

    Emitter{
        id: coreEmitter
        system: coreSystem
        anchors.centerIn: parent

        lifeSpan: 150
        emitRate: 8
        size: emitterWidth  * .1
        endSize: emitterWidth * .15

        velocity: PointDirection {
            x: 0
            y: 0
        }

        ImageParticle {
            system: coreSystem
            anchors.centerIn: parent
            source: "UI/NoteBaseNeutral.png"   // use a glowing dot or spark
            entryEffect: ImageParticle.Fade

        }
    }

    Emitter{
        id: waveEmitter
        anchors.centerIn: parent
        system: waveSystem
        lifeSpan: 150
        emitRate: 8
        size: emitterWidth * 0.8
        endSize: emitterWidth * 2


        velocity: PointDirection {
            x: 0
            y: 0
        }

        ImageParticle {
            id: waveImg
            system: waveSystem
            anchors.centerIn: parent
            source: "Particles/ParticleShockwave.png"   // use a glowing dot or spark
            entryEffect: ImageParticle.Fade
            opacity: 0.7

        }
    }
}


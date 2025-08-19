import QtQuick 2.15
import QtMultimedia

Item {
    id: root
    property alias resultDisplay: resultDisplaySound
    property alias resultHide: resultHideSound
    property alias emotionalCapacityStateDiscovered: emotionalCapacityStateDiscoveredSound
    property alias emotionalCapacityStateChange: emotionalCapacityStateChangeSound
    property alias menuHover: menuHoverSound
    property alias optionConfirm: optionConfirmSound
    property alias menuSlide: menuSlideSound

    Component.onCompleted: {
        menuSlideSound.volume = 0
        menuSlideSound.play()
        menuSlideSound.stop()
        menuSlideSound.volume = .3

        emotionalCapacityStateChangeSound.volume = 0
        emotionalCapacityStateChangeSound.play()
        emotionalCapacityStateChangeSound.stop()
        emotionalCapacityStateChangeSound.volume = 1

        emotionalCapacityStateDiscoveredSound.volume = 0
        emotionalCapacityStateDiscoveredSound.play()
        emotionalCapacityStateDiscoveredSound.stop()
        emotionalCapacityStateDiscoveredSound.volume = 1
    }

    SoundEffect {
         id: menuHoverSound
         source:  "file:///"  + gameInitializer.appDirectory + "/Sounds/RebirthUIsoundASelect.wav"
         volume: .8
    }

    SoundEffect {
          id: optionConfirmSound
          source:  "file:///"  + gameInitializer.appDirectory + "/Sounds/RebirthUIsoundAConfirmed.wav"
          volume: .7
    }
    SoundEffect {
        id: menuSlideSound
        source:  "file:///"  + gameInitializer.appDirectory + "/Sounds/RebirthUIsoundAPopupOpened.wav"
        volume: .3
    }

    SoundEffect {
        id: emotionalCapacityStateDiscoveredSound
        source:  "file:///"  + gameInitializer.appDirectory + "/Sounds/RebirthUIEmotionalCapacityStateDiscovered.wav"
        volume: 1
    }
    SoundEffect {
        id: emotionalCapacityStateChangeSound
        source:  "file:///"  + gameInitializer.appDirectory + "/Sounds/RebirthUIEmotionalCapacityStateChange.wav"
        volume: 1
    }
    SoundEffect {
      id: resultDisplaySound
      source:  "file:///" + gameInitializer.appDirectory + "/Sounds/RebirthUIsoundPowerOn.wav"
        volume: .2
    }
    SoundEffect {
      id: resultHideSound
      source:  "file:///"  + gameInitializer.appDirectory + "/Sounds/RebirthUIsoundShutOff.wav"
      volume: .2
    }

}

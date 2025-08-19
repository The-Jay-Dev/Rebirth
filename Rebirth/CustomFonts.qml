import QtQuick 2.15

Item {

    id: root

    readonly property string barlowBlack: barlowBlackFont.name
    readonly property string barlowLightItalic: barlowLightItalicFont.name
    readonly property string barlowLight: barlowLightFont.name
    readonly property string barlowRegular: barlowRegularFont.name
    FontLoader{id: barlowBlackFont
      source: "Fonts/Barlow-Black.ttf"
    }

    FontLoader{
        id: barlowRegularFont
        source: "Fonts/Barlow-Regular.ttf"
    }

    FontLoader{
        id: barlowLightItalicFont
        source: "Fonts/Barlow-LightItalic.ttf"
    }
    FontLoader{
        id: barlowLightFont
        source: "Fonts/Barlow-Light.ttf"
    }
}


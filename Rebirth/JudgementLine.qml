import QtQuick
import Qt5Compat.GraphicalEffects

Item{
               Rectangle {
                              id:line
                              anchors.fill: parent
                              color: "white"
               }
               
               Glow {
                              anchors.fill: line
                              source: line
                              color: "white"
                              radius: 12
                              spread: 0.0
                              opacity: 1
                              samples: 25
               }
}


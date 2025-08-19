import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Shapes


Shape{

    id: root
    property real trailWidth
    required property real spawnY
    required property real spawnX
    property alias noteYPosB: pathlineB.y
    property alias noteXPosB: pathlineB.x
    property alias noteYPosC: pathlineC.y
    property alias noteXPosC: pathlineC.x
    property alias trailBottomLeft: pathlineA.x
    property alias trailBottomRight:pathlineD.x

    anchors.fill: parent
    z: 1

    ShapePath{
        id: trailPath
        fillRule:  ShapePath.WindingFill
        capStyle: ShapePath.FlatCap
        pathHints: ShapePath.PathNonIntersecting

        fillGradient: LinearGradient{
            x1: spawnX
            y1: spawnY
            x2:noteXPosC
            y2:noteYPosC
            GradientStop{
                position:0.4
                color: "#597679"
            }
            GradientStop{
                position:1
                color: "#f9ffdc"
            }
        }

        startX: spawnX; startY: spawnY
        PathLine {id: pathlineA; x: trailPath.startX + trailWidth; y: trailPath.startY}
        PathLine { id: pathlineB; }
        PathLine {id: pathlineC;}
        PathLine {id: pathlineD; x:trailPath.startX; y:trailPath.startY}
    }
}



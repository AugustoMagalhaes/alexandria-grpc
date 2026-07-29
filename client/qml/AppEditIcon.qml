import QtQuick
import QtQuick.Shapes
import Alexandria

Item {
    id: icon
    property color color: Theme.textPrimary
    implicitWidth: 20
    implicitHeight: 20

    Rectangle {
        id: shaft
        width: icon.width * 0.62
        height: icon.height * 0.16
        radius: height / 2
        color: icon.color
        anchors.centerIn: parent
        rotation: -45
        transformOrigin: Item.Center
    }

    Shape {
        anchors.fill: parent
        ShapePath {
            fillColor: icon.color
            strokeColor: "transparent"
            startX: icon.width * 0.06
            startY: icon.height * 0.94
            PathLine { x: icon.width * 0.22; y: icon.height * 0.78 }
            PathLine { x: icon.width * 0.14; y: icon.height * 0.86 }
            PathLine { x: icon.width * 0.06; y: icon.height * 0.94 }
        }
    }
}
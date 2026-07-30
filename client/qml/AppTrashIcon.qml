import QtQuick
import Alexandria

Item {
    id: icon
    property color color: Theme.errorColor
    implicitWidth: 20
    implicitHeight: 20

    Rectangle {
        id: handle
        width: icon.width * 0.32
        height: icon.height * 0.12
        radius: height / 2
        color: icon.color
        anchors.horizontalCenter: parent.horizontalCenter
        y: 0
    }

    Rectangle {
        id: lid
        width: icon.width * 0.85
        height: icon.height * 0.12
        radius: height / 2
        color: icon.color
        anchors.horizontalCenter: parent.horizontalCenter
        y: handle.height * 0.7
    }

    Rectangle {
        id: body
        width: icon.width * 0.65
        height: icon.height * 0.68
        radius: 3
        color: "transparent"
        border.color: icon.color
        border.width: 2
        anchors.horizontalCenter: parent.horizontalCenter
        y: lid.y + lid.height + 1

        Row {
            anchors.centerIn: parent
            spacing: body.width * 0.2

            Repeater {
                model: 3
                Rectangle {
                    width: 2
                    height: body.height * 0.55
                    radius: 1
                    color: icon.color
                }
            }
        }
    }
}
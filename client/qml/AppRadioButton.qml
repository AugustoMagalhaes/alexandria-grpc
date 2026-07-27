import QtQuick
import QtQuick.Controls.Basic
import Alexandria

RadioButton {
    id: control

    contentItem: Label {
        text: control.text
        color: Theme.textPrimary
        font.pixelSize: Theme.fontSizeBody
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }

    indicator: Rectangle {
        implicitWidth: 20
        implicitHeight: 20
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 10
        color: Theme.surfaceColor
        border.color: control.checked ? Theme.primaryColor : Theme.borderColor
        border.width: 1

        Rectangle {
            visible: control.checked
            anchors.centerIn: parent
            width: 10
            height: 10
            radius: 5
            color: Theme.primaryColor
        }
    }
}
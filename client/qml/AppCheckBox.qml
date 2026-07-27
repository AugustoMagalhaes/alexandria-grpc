import QtQuick
import QtQuick.Controls.Basic
import Alexandria

CheckBox {
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
        radius: 4
        color: Theme.surfaceColor
        border.color: control.checked ? Theme.primaryColor : Theme.borderColor
        border.width: 1

        Rectangle {
            visible: control.checked
            anchors.centerIn: parent
            width: 12
            height: 12
            radius: 2
            color: Theme.primaryColor
        }
    }
}
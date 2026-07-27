import QtQuick
import QtQuick.Controls.Basic
import Alexandria

Button {
    id: control
    property bool primary: false

    padding: 10
    leftPadding: 16
    rightPadding: 16

    background: Rectangle {
        radius: Theme.radiusSmall
        color: control.primary
            ? (control.pressed ? Theme.primaryColorDark : Theme.primaryColor)
            : (control.pressed ? "#E5E5E2" : Theme.surfaceColor)
        border.color: control.primary ? Theme.primaryColor : Theme.borderColor
        border.width: 1
        opacity: control.enabled ? 1.0 : 0.5
    }

    contentItem: Text {
        text: control.text
        font.pixelSize: Theme.fontSizeBody
        color: control.primary ? "white" : Theme.textPrimary
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
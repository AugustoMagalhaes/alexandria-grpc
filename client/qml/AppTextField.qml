import QtQuick
import QtQuick.Controls.Basic
import Alexandria

TextField {
    id: control
    color: Theme.textPrimary
    placeholderTextColor: Theme.textSecondary
    font.pixelSize: Theme.fontSizeBody
    selectionColor: Theme.primaryColor

    padding: 10

    background: Rectangle {
        radius: Theme.radiusSmall
        color: Theme.surfaceColor
        border.color: control.activeFocus ? Theme.primaryColor : Theme.borderColor
        border.width: control.activeFocus ? 2 : 1
    }
}
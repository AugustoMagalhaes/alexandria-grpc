import QtQuick
import QtQuick.Controls.Basic
import Alexandria

SpinBox {
    id: control

    contentItem: TextInput {
        text: control.textFromValue(control.value, control.locale)
        color: Theme.textPrimary
        font.pixelSize: Theme.fontSizeBody
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: Qt.ImhDigitsOnly
    }

    background: Rectangle {
        radius: Theme.radiusSmall
        color: Theme.surfaceColor
        border.color: control.activeFocus ? Theme.primaryColor : Theme.borderColor
        border.width: control.activeFocus ? 2 : 1
    }
}
import QtQuick
import QtQuick.Controls.Basic
import Alexandria

Button {
    id: control
    property bool primary: false
    property bool success: false
    property bool neutral: false

    padding: 10
    leftPadding: 16
    rightPadding: 16

    background: Rectangle {
        radius: Theme.radiusSmall
        color: {
            if (control.success) {
                return control.pressed ? Qt.darker(Theme.successColor, 1.15) : Theme.successColor
            }
            if (control.primary) {
                return control.pressed ? Theme.primaryColorDark : Theme.primaryColor
            }
            if (control.neutral) {
                return control.pressed ? "#3A3A3A" : "#4A4A4A"
            }
            return control.pressed ? "#E5E5E2" : Theme.surfaceColor
        }
        border.color: (control.primary || control.success || control.neutral) ? color : Theme.borderColor
        border.width: 1
        opacity: control.enabled ? 1.0 : 0.5
    }

    contentItem: Text {
        text: control.text
        font.pixelSize: Theme.fontSizeBody
        color: (control.primary || control.success || control.neutral) ? "white" : Theme.textPrimary
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
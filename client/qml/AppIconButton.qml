import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Button {
    id: control
    property string iconType: "edit"
    property bool destructive: iconType === "trash"

    implicitWidth: control.text.length > 0 ? (contentRow.implicitWidth + 24) : 36
    implicitHeight: 36

    background: Rectangle {
        radius: Theme.radiusSmall
        color: control.pressed ? "#E5E5E2" : Theme.surfaceColor
        border.color: Theme.borderColor
        border.width: 1
        opacity: control.enabled ? 1.0 : 0.5
    }

    contentItem: RowLayout {
        id: contentRow
        spacing: 6
        anchors.centerIn: parent

        Loader {
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            sourceComponent: control.iconType === "trash" ? trashComp : editComp
        }

        Label {
            text: control.text
            visible: control.text.length > 0
            color: control.destructive ? Theme.errorColor : Theme.textPrimary
            font.pixelSize: Theme.fontSizeBody
        }
    }

    Component {
        id: trashComp
        AppTrashIcon { color: Theme.errorColor }
    }

    Component {
        id: editComp
        AppEditIcon { color: Theme.textSecondary }
    }
}
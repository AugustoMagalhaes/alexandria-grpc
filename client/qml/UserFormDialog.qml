import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Dialog {
    id: dialog
    modal: true
    anchors.centerIn: parent
    width: 340
    padding: 24

    background: Rectangle {
        color: Theme.surfaceColor
        radius: Theme.radiusMedium
        border.color: Theme.borderColor
        border.width: 1
    }

    Connections {
        target: Session
        function onServerConfiguredChanged() {
            if (!Session.serverConfigured && dialog.opened) {
                dialog.close()
            }
        }
    }

    property var userSavedCallback: null
    property bool showSuccess: false

    Timer {
        id: closeTimer
        interval: 700
        onTriggered: {
            dialog.close()
            if (dialog.userSavedCallback) {
                dialog.userSavedCallback()
            }
        }
    }

    UserFormViewModel {
        id: formModel

        onSaved: {
            dialog.showSuccess = true
            closeTimer.start()
        }
    }

    function openForCreate() {
        formModel.reset()
        dialog.showSuccess = false
        dialog.open()
    }

    function openForEdit(user) {
        formModel.userId = user.id
        formModel.username = user.username
        formModel.password = ""
        formModel.isAdmin = user.isAdmin
        dialog.showSuccess = false
        dialog.open()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        Label {
            text: formModel.isEditing ? qsTr("Edit User") : qsTr("Add User")
            font.pixelSize: Theme.fontSizeTitle
            font.bold: true
            color: Theme.textPrimary
        }

        AppTextField {
            placeholderText: qsTr("Username")
            text: formModel.username
            enabled: !formModel.isEditing
            onTextChanged: formModel.username = text
            Layout.fillWidth: true
        }

        AppTextField {
            placeholderText: formModel.isEditing ? qsTr("New password (leave blank to keep current)") : qsTr("Password (min. 6 characters)")
            text: formModel.password
            echoMode: TextInput.Password
            onTextChanged: formModel.password = text
            Layout.fillWidth: true
            Keys.onReturnPressed: if (!dialog.showSuccess && formModel.username.length > 0 && !formModel.busy) formModel.save()
            Keys.onEnterPressed: if (!dialog.showSuccess && formModel.username.length > 0 && !formModel.busy) formModel.save()
        }

        AppCheckBox {
            text: qsTr("Administrator")
            checked: formModel.isAdmin
            onCheckedChanged: formModel.isAdmin = checked
        }

        Label {
            text: dialog.showSuccess ? qsTr("Saved successfully") : formModel.errorMessage
            color: dialog.showSuccess ? Theme.successColor : Theme.errorColor
            wrapMode: Text.WordWrap
            visible: dialog.showSuccess || formModel.errorMessage.length > 0
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 8

            AppButton {
                text: qsTr("Cancel")
                onClicked: dialog.close()
            }

            AppButton {
                text: qsTr("Save")
                primary: true
                enabled: {
                    if (formModel.busy) return false
                    if (formModel.username.length === 0) return false
                    if (!formModel.isEditing && formModel.password.length < 6) return false
                    if (formModel.isEditing && formModel.password.length > 0 && formModel.password.length < 6) return false
                    return true
                }
                onClicked: formModel.save()
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 32

            BusyIndicator {
                anchors.centerIn: parent
                running: formModel.busy
                visible: formModel.busy
            }
        }
    }
}
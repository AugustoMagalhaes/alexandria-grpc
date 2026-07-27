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

    property var userSavedCallback: null

    UserFormViewModel {
        id: formModel

        onSaved: {
            dialog.close()
            if (dialog.userSavedCallback) {
                dialog.userSavedCallback()
            }
        }
    }

    function openForCreate() {
        formModel.reset()
        dialog.open()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        Label {
            text: qsTr("Add User")
            font.pixelSize: Theme.fontSizeTitle
            font.bold: true
            color: Theme.textPrimary
        }

        AppTextField {
            placeholderText: qsTr("Username")
            text: formModel.username
            onTextChanged: formModel.username = text
            Layout.fillWidth: true
        }

        AppTextField {
            placeholderText: qsTr("Password (min. 6 characters)")
            text: formModel.password
            echoMode: TextInput.Password
            onTextChanged: formModel.password = text
            Layout.fillWidth: true
        }

        AppCheckBox {
            text: qsTr("Administrator")
            checked: formModel.isAdmin
            onCheckedChanged: formModel.isAdmin = checked
        }

        Label {
            text: formModel.errorMessage
            color: Theme.errorColor
            wrapMode: Text.WordWrap
            visible: formModel.errorMessage.length > 0
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
                enabled: formModel.username.length > 0 && formModel.password.length >= 6 && !formModel.busy
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
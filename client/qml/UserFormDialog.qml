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
    title: qsTr("Add User")

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
        spacing: 12

        TextField {
            placeholderText: qsTr("Username")
            text: formModel.username
            onTextChanged: formModel.username = text
            Layout.fillWidth: true
        }

        TextField {
            placeholderText: qsTr("Password (min. 6 characters)")
            text: formModel.password
            echoMode: TextInput.Password
            onTextChanged: formModel.password = text
            Layout.fillWidth: true
        }

        CheckBox {
            text: qsTr("Administrator")
            checked: formModel.isAdmin
            onCheckedChanged: formModel.isAdmin = checked
        }

        Label {
            text: formModel.errorMessage
            color: "red"
            wrapMode: Text.WordWrap
            visible: formModel.errorMessage.length > 0
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight

            Button {
                text: qsTr("Cancel")
                onClicked: dialog.close()
            }

            Button {
                text: qsTr("Save")
                enabled: formModel.username.length > 0 && formModel.password.length >= 6 && !formModel.busy
                onClicked: formModel.save()
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 20

            BusyIndicator {
                anchors.centerIn: parent
                running: formModel.busy
                visible: formModel.busy
            }
        }
    }
}
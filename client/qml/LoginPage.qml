import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Item {
    Rectangle {
        anchors.fill: parent
        color: Theme.backgroundColor
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Theme.spacingMedium
        width: 300

        Label {
            text: qsTr("Alexandria")
            font.pixelSize: Theme.fontSizeHeading
            font.bold: true
            color: Theme.textPrimary
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: qsTr("School Library")
            font.pixelSize: Theme.fontSizeBody
            color: Theme.textSecondary
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: Theme.spacingMedium
        }

        AppTextField {
            id: usernameField
            placeholderText: qsTr("Username")
            Layout.fillWidth: true
        }

        AppTextField {
            id: passwordField
            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
            Layout.fillWidth: true
        }

        AppCheckBox {
            text: qsTr("Remember me")
            checked: Session.rememberMe
            onCheckedChanged: Session.rememberMe = checked
        }

        AppButton {
            text: qsTr("Log in")
            primary: true
            Layout.fillWidth: true
            enabled: !Session.busy
            onClicked: Session.login(usernameField.text, passwordField.text)
        }

        Label {
            text: Session.errorMessage
            color: Theme.errorColor
            visible: Session.errorMessage.length > 0
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 32

            BusyIndicator {
                anchors.centerIn: parent
                running: Session.busy
                visible: Session.busy
            }
        }
    }
}
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Item {
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16
        width: 280

        Label {
            text: qsTr("Alexandria")
            font.pixelSize: 28
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: usernameField
            placeholderText: qsTr("Username")
            Layout.fillWidth: true
        }

        TextField {
            id: passwordField
            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Log in")
            Layout.fillWidth: true
            enabled: !Session.busy
            onClicked: Session.login(usernameField.text, passwordField.text)
        }

        Label {
            text: Session.errorMessage
            color: "red"
            visible: Session.errorMessage.length > 0
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        BusyIndicator {
            running: Session.busy
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
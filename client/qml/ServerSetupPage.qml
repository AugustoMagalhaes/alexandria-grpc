import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Item {
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16
        width: 340

        Label {
            text: qsTr("Connect to Library Server")
            font.pixelSize: 22
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: qsTr("Enter the server address (e.g. 192.168.1.10:50051)")
            wrapMode: Text.WordWrap
            visible: !Session.connecting
            Layout.fillWidth: true
        }

        TextField {
            id: addressField
            text: Session.serverAddress.length > 0 ? Session.serverAddress : "127.0.0.1:50051"
            visible: !Session.connecting
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Connect")
            visible: !Session.connecting
            enabled: addressField.text.length > 0
            Layout.fillWidth: true
            onClicked: Session.connectToServer(addressField.text)
        }

        BusyIndicator {
            running: Session.connecting
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: qsTr("Connecting to server…")
            visible: Session.connecting
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: Session.connectionError
            color: "red"
            wrapMode: Text.WordWrap
            visible: !Session.connecting && Session.connectionError.length > 0
            Layout.fillWidth: true
        }
    }
}
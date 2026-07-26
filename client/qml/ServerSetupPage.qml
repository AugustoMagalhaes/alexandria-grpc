import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Item {
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16
        width: 320

        Label {
            text: qsTr("Connect to Library Server")
            font.pixelSize: 22
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: qsTr("Enter the server address (e.g. 192.168.1.10:50051)")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        TextField {
            id: addressField
            text: Session.serverAddress
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Connect")
            Layout.fillWidth: true
            enabled: addressField.text.length > 0
            onClicked: Session.connectToServer(addressField.text)
        }
    }
}
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
        width: 340

        Label {
            text: qsTr("Connect to Library Server")
            font.pixelSize: Theme.fontSizeTitle
            font.bold: true
            color: Theme.textPrimary
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: qsTr("Enter the server address (e.g. 192.168.1.10:50051)")
            color: Theme.textSecondary
            wrapMode: Text.WordWrap
            visible: !Session.connecting
            Layout.fillWidth: true
        }

        AppTextField {
            id: addressField
            text: Session.serverAddress.length > 0 ? Session.serverAddress : "127.0.0.1:50051"
            visible: !Session.connecting
            Layout.fillWidth: true
        }

        AppButton {
            text: qsTr("Connect")
            primary: true
            visible: !Session.connecting
            enabled: addressField.text.length > 0
            Layout.fillWidth: true
            onClicked: Session.connectToServer(addressField.text)
        }

        Label {
            text: qsTr("Connecting to server…")
            color: Theme.textSecondary
            visible: Session.connecting
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: Session.connectionError
            color: Theme.errorColor
            wrapMode: Text.WordWrap
            visible: !Session.connecting && Session.connectionError.length > 0
            Layout.fillWidth: true
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 32

            BusyIndicator {
                anchors.centerIn: parent
                running: Session.connecting
                visible: Session.connecting
            }
        }
    }
}
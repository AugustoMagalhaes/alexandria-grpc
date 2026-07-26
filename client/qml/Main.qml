import QtQuick
import QtQuick.Controls.Basic
import Alexandria

ApplicationWindow {
    id: window
    width: 800
    height: 600
    minimumWidth: 400
    minimumHeight: 300
    visible: true
    title: qsTr("Alexandria")

    property bool serverConfirmed: false

    ServerSetupPage {
        anchors.fill: parent
        visible: !window.serverConfirmed
    }

    LoginPage {
        anchors.fill: parent
        visible: window.serverConfirmed && !Session.authenticated
    }

    BookListPage {
        anchors.fill: parent
        visible: window.serverConfirmed && Session.authenticated
    }

    Connections {
        target: Session
        function onServerAddressChanged() {
            window.serverConfirmed = true
        }
    }
}
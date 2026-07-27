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

    property bool showAdminPanel: false

    ServerSetupPage {
        anchors.fill: parent
        visible: !Session.serverConfigured
    }

    LoginPage {
        anchors.fill: parent
        visible: Session.serverConfigured && !Session.authenticated
    }

    BookListPage {
        anchors.fill: parent
        visible: Session.serverConfigured && Session.authenticated && !window.showAdminPanel
        onAdminPanelRequested: window.showAdminPanel = true
    }

    AdminPanelPage {
        anchors.fill: parent
        visible: Session.serverConfigured && Session.authenticated && window.showAdminPanel
        onBackRequested: window.showAdminPanel = false
    }

    Connections {
        target: Session
        function onAuthenticationChanged() {
            if (!Session.authenticated) {
                window.showAdminPanel = false
            }
        }
    }
}
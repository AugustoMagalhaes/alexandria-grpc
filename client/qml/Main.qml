import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

ApplicationWindow {
    id: window
    width: 800
    height: 720
    minimumWidth: 400
    minimumHeight: 400
    visible: true
    visibility: Window.Maximized
    title: qsTr("Alexandria")

    property bool showAdminPanel: false
    property bool showServerSetup: false

    ServerSetupPage {
        anchors.fill: parent
        visible: !Session.serverConfigured
    }

    LoginPage {
        anchors.fill: parent
        visible: Session.serverConfigured && !Session.authenticated
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        visible: Session.serverConfigured && Session.authenticated

        AppHeader {
            Layout.fillWidth: true
            currentTab: window.showServerSetup ? "server" : (window.showAdminPanel ? "users" : "books")
            onTabSelected: (tab) => {
                window.showServerSetup = false
                window.showAdminPanel = (tab === "users")
            }
            onChangeServerRequested: window.showServerSetup = true
            onLogoutRequested: Session.logout()
        }

        BookListPage {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !window.showAdminPanel && !window.showServerSetup
        }

        AdminPanelPage {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: window.showAdminPanel && !window.showServerSetup
        }

        ServerSetupPage {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: window.showServerSetup
        }
    }

    Connections {
        target: Session
        function onAuthenticationChanged() {
            if (!Session.authenticated) {
                window.showAdminPanel = false
                window.showServerSetup = false
            }
        }
        function onServerConfiguredChanged() {
            if (Session.serverConfigured) {
                window.showServerSetup = false
            }
        }
    }
}
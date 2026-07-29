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
            currentTab: window.showAdminPanel ? "users" : "books"
            onTabSelected: (tab) => window.showAdminPanel = (tab === "users")
            onChangeServerRequested: Session.requestServerChange()
            onLogoutRequested: Session.logout()
        }

        BookListPage {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !window.showAdminPanel
        }

        AdminPanelPage {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: window.showAdminPanel
        }
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
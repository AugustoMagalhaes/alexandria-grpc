import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Rectangle {
    id: header
    height: 64
    color: Theme.primaryColorDark

    property string currentTab: "books"
    signal tabSelected(string tab)
    signal changeServerRequested()
    signal logoutRequested()

    component HeaderTab: Item {
        id: tab
        property string text: ""
        property bool active: false
        signal clicked()

        implicitWidth: label.implicitWidth + 4
        implicitHeight: 40

        Label {
            id: label
            anchors.centerIn: parent
            text: tab.text
            color: tab.active ? "white" : "#B9D2D8"
            font.pixelSize: Theme.fontSizeBody
            font.bold: true
        }

        Rectangle {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: label.implicitWidth
            height: 3
            radius: 1.5
            color: "white"
            visible: tab.active
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: tab.clicked()
        }
    }

    component HeaderActionButton: Rectangle {
        id: btn
        property string text: ""
        signal clicked()

        implicitWidth: label.implicitWidth + 24
        implicitHeight: 36
        radius: Theme.radiusSmall
        color: mouseArea.pressed ? Qt.rgba(1, 1, 1, 0.25) : Qt.rgba(1, 1, 1, 0.12)
        border.color: Qt.rgba(1, 1, 1, 0.4)
        border.width: 1

        Label {
            id: label
            anchors.centerIn: parent
            text: btn.text
            color: "white"
            font.pixelSize: Theme.fontSizeBody
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: btn.clicked()
        }
    }

    Label {
        anchors.left: parent.left
        anchors.leftMargin: Theme.spacingMedium
        anchors.verticalCenter: parent.verticalCenter
        text: qsTr("Alexandria")
        font.pixelSize: Theme.fontSizeTitle
        font.bold: true
        color: "white"
    }

    Row {
        anchors.centerIn: parent
        spacing: Theme.spacingLarge * 2

        HeaderTab {
            text: qsTr("Books")
            active: header.currentTab === "books"
            onClicked: header.tabSelected("books")
        }

        HeaderTab {
            text: qsTr("Manage Users")
            visible: Session.isAdmin
            active: header.currentTab === "users"
            onClicked: header.tabSelected("users")
        }

        HeaderTab {
            text: qsTr("Change Server")
            active: header.currentTab === "server"
            onClicked: header.changeServerRequested()
        }
    }

    HeaderActionButton {
        anchors.right: parent.right
        anchors.rightMargin: Theme.spacingMedium
        anchors.verticalCenter: parent.verticalCenter
        text: qsTr("Log Out")
        onClicked: header.logoutRequested()
    }
}
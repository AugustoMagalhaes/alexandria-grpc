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

    LoginPage {
        anchors.fill: parent
        visible: !Session.authenticated
    }

    BookListPage {
        anchors.fill: parent
        visible: Session.authenticated
    }
}
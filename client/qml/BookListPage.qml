import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Item {
    BookListViewModel {
        id: viewModel
    }

    onVisibleChanged: {
        if (visible) {
            viewModel.refresh()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 12

            Label {
                text: Session.isAdmin ? qsTr("Books (Administrator)") : qsTr("Books")
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Change server")
                onClicked: Session.requestServerChange()
            }

            Button {
                text: qsTr("Log out")
                onClicked: Session.logout()
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: viewModel.books

            delegate: ItemDelegate {
                width: ListView.view.width
                text: modelData.title + " — " + modelData.author
            }
        }
    }

    Label {
        anchors.centerIn: parent
        text: viewModel.errorMessage
        color: "red"
        visible: viewModel.errorMessage.length > 0
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: viewModel.busy
    }
}
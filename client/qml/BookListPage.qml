import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Item {
    id: page

    BookListViewModel {
        id: viewModel
    }

    onVisibleChanged: {
        if (visible) {
            viewModel.refresh(searchField.text)
        }
    }

    BookFormDialog {
        id: formDialog
        bookSavedCallback: function() { viewModel.refresh(searchField.text) }
    }

    DeleteConfirmDialog {
        id: deleteDialog
        onConfirmed: (id) => viewModel.deleteBook(id)
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
            }

            Item {
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

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 12
            spacing: 8

            TextField {
                id: searchField
                placeholderText: qsTr("Search by title or author")
                Layout.fillWidth: true
                onTextChanged: viewModel.refresh(text)
            }

            Button {
                text: qsTr("Add Book")
                visible: Session.isAdmin
                onClicked: formDialog.openForCreate()
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: viewModel.books
            clip: true

            delegate: ItemDelegate {
                width: ListView.view.width
                height: 56

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: modelData.title
                            font.bold: true
                        }

                        Label {
                            text: modelData.author + " · " + modelData.availableCopies + "/" + modelData.totalCopies + qsTr(" available")
                            font.pixelSize: 12
                        }
                    }

                    Button {
                        text: qsTr("Edit")
                        visible: Session.isAdmin
                        onClicked: formDialog.openForEdit(modelData)
                    }

                    Button {
                        text: qsTr("Delete")
                        visible: Session.isAdmin
                        onClicked: deleteDialog.openFor(modelData)
                    }
                }
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
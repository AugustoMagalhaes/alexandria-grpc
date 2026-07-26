import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Dialog {
    id: dialog
    modal: true
    anchors.centerIn: parent
    width: 360
    title: formModel.isEditing ? qsTr("Edit Book") : qsTr("Add Book")

    property var bookSavedCallback: null

    BookFormViewModel {
        id: formModel

        onSaved: {
            dialog.close()
            if (dialog.bookSavedCallback) {
                dialog.bookSavedCallback()
            }
        }
    }

    function openForCreate() {
        formModel.reset()
        dialog.open()
    }

    function openForEdit(book) {
        formModel.bookId = book.id
        formModel.title = book.title
        formModel.author = book.author
        formModel.isbn = book.isbn
        formModel.totalCopies = book.totalCopies
        formModel.availableCopies = book.availableCopies
        dialog.open()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        TextField {
            placeholderText: qsTr("Title")
            text: formModel.title
            onTextChanged: formModel.title = text
            Layout.fillWidth: true
        }

        TextField {
            placeholderText: qsTr("Author")
            text: formModel.author
            onTextChanged: formModel.author = text
            Layout.fillWidth: true
        }

        TextField {
            placeholderText: qsTr("ISBN")
            text: formModel.isbn
            onTextChanged: formModel.isbn = text
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Total copies")
            }

            SpinBox {
                from: 1
                to: 9999
                value: formModel.totalCopies
                onValueChanged: {
                    formModel.totalCopies = value
                    if (!formModel.isEditing) {
                        formModel.availableCopies = value
                    }
                }
                Layout.fillWidth: true
            }
        }

        RowLayout {
            Layout.fillWidth: true
            visible: formModel.isEditing

            Label {
                text: qsTr("Available copies")
            }

            SpinBox {
                from: 0
                to: formModel.totalCopies
                value: formModel.availableCopies
                onValueChanged: formModel.availableCopies = value
                Layout.fillWidth: true
            }
        }

        Label {
            text: formModel.errorMessage
            color: "red"
            wrapMode: Text.WordWrap
            visible: formModel.errorMessage.length > 0
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight

            Button {
                text: qsTr("Cancel")
                onClicked: dialog.close()
            }

            Button {
                text: qsTr("Save")
                enabled: formModel.title.length > 0 && formModel.author.length > 0 && formModel.isbn.length > 0 && !formModel.busy
                onClicked: formModel.save()
            }
        }

        BusyIndicator {
            running: formModel.busy
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
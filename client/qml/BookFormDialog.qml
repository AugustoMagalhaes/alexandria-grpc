import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Dialog {
    id: dialog
    modal: true
    anchors.centerIn: parent
    width: 360
    padding: 24

    background: Rectangle {
        color: Theme.surfaceColor
        radius: Theme.radiusMedium
        border.color: Theme.borderColor
        border.width: 1
    }

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
        spacing: 16

        Label {
            text: formModel.isEditing ? qsTr("Edit Book") : qsTr("Add Book")
            font.pixelSize: Theme.fontSizeTitle
            font.bold: true
            color: Theme.textPrimary
        }

        AppTextField {
            placeholderText: qsTr("Title")
            text: formModel.title
            onTextChanged: formModel.title = text
            Layout.fillWidth: true
        }

        AppTextField {
            placeholderText: qsTr("Author")
            text: formModel.author
            onTextChanged: formModel.author = text
            Layout.fillWidth: true
        }

        AppTextField {
            placeholderText: qsTr("ISBN")
            text: formModel.isbn
            onTextChanged: formModel.isbn = text
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: qsTr("Total copies")
                color: Theme.textPrimary
            }

            AppSpinBox {
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
            spacing: 8
            visible: formModel.isEditing

            Label {
                text: qsTr("Available copies")
                color: Theme.textPrimary
            }

            AppSpinBox {
                from: 0
                to: formModel.totalCopies
                value: formModel.availableCopies
                onValueChanged: formModel.availableCopies = value
                Layout.fillWidth: true
            }
        }

        Label {
            text: formModel.errorMessage
            color: Theme.errorColor
            wrapMode: Text.WordWrap
            visible: formModel.errorMessage.length > 0
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 8

            AppButton {
                text: qsTr("Cancel")
                onClicked: dialog.close()
            }

            AppButton {
                text: qsTr("Save")
                primary: true
                enabled: formModel.title.length > 0 && formModel.author.length > 0 && formModel.isbn.length > 0 && !formModel.busy
                onClicked: formModel.save()
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 32

            BusyIndicator {
                anchors.centerIn: parent
                running: formModel.busy
                visible: formModel.busy
            }
        }
    }
}
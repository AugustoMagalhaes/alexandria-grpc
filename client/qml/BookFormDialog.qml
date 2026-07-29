import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Dialog {
    id: dialog
    modal: true
    anchors.centerIn: parent
    width: 380
    padding: 24

    background: Rectangle {
        color: Theme.surfaceColor
        radius: Theme.radiusMedium
        border.color: Theme.borderColor
        border.width: 1
    }

    Connections {
        target: Session
        function onServerConfiguredChanged() {
            if (!Session.serverConfigured && dialog.opened) {
                dialog.close()
            }
        }
    }

    property var bookSavedCallback: null
    property bool showSuccess: false

    Timer {
        id: closeTimer
        interval: 700
        onTriggered: {
            dialog.close()
            if (dialog.bookSavedCallback) {
                dialog.bookSavedCallback()
            }
        }
    }

    BookFormViewModel {
        id: formModel

        onSaved: {
            dialog.showSuccess = true
            closeTimer.start()
        }
    }

    function openForCreate() {
        formModel.reset()
        dialog.showSuccess = false
        dialog.open()
    }

    function openForEdit(book) {
        formModel.bookId = book.id
        formModel.title = book.title
        formModel.author = book.author
        formModel.isbn = book.isbn
        formModel.schoolCode = book.schoolCode
        formModel.category = book.category
        formModel.keywords = book.keywords
        formModel.borrowable = book.borrowable
        formModel.totalCopies = book.totalCopies
        formModel.availableCopies = book.availableCopies
        dialog.showSuccess = false
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

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 16

            AppTextField {
                placeholderText: qsTr("Title")
                text: formModel.title
                onTextChanged: formModel.title = text
                Layout.fillWidth: true
                Keys.onReturnPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
                Keys.onEnterPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
            }

            AppTextField {
                placeholderText: qsTr("Author")
                text: formModel.author
                onTextChanged: formModel.author = text
                Layout.fillWidth: true
                Keys.onReturnPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
                Keys.onEnterPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
            }

            AppTextField {
                placeholderText: qsTr("ISBN (optional)")
                text: formModel.isbn
                onTextChanged: formModel.isbn = text
                Layout.fillWidth: true
                Keys.onReturnPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
                Keys.onEnterPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
            }

            AppTextField {
                placeholderText: qsTr("School code (optional)")
                text: formModel.schoolCode
                onTextChanged: formModel.schoolCode = text
                Layout.fillWidth: true
                Keys.onReturnPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
                Keys.onEnterPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
            }

            AppTextField {
                placeholderText: qsTr("Category (e.g. children, adult, education theory)")
                text: formModel.category
                onTextChanged: formModel.category = text
                Layout.fillWidth: true
                Keys.onReturnPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
                Keys.onEnterPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
            }

            AppTextField {
                placeholderText: qsTr("Keywords (comma separated)")
                text: formModel.keywords
                onTextChanged: formModel.keywords = text
                Layout.fillWidth: true
                Keys.onReturnPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
                Keys.onEnterPressed: if (formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy) formModel.save()
            }

            AppCheckBox {
                text: qsTr("Can be borrowed")
                checked: formModel.borrowable
                onCheckedChanged: formModel.borrowable = checked
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                columnSpacing: 12
                rowSpacing: 12

                Label {
                    text: qsTr("Total copies")
                    color: Theme.textPrimary
                    Layout.preferredWidth: 120
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

                Label {
                    text: qsTr("Available copies")
                    color: Theme.textPrimary
                    Layout.preferredWidth: 120
                    visible: formModel.isEditing
                }

                AppSpinBox {
                    from: 0
                    to: formModel.totalCopies
                    value: formModel.availableCopies
                    onValueChanged: formModel.availableCopies = value
                    Layout.fillWidth: true
                    visible: formModel.isEditing
                }
            }
        }

        Label {
            text: dialog.showSuccess ? qsTr("Saved successfully") : formModel.errorMessage
            color: dialog.showSuccess ? Theme.successColor : Theme.errorColor
            wrapMode: Text.WordWrap
            visible: dialog.showSuccess || formModel.errorMessage.length > 0
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
                enabled: formModel.title.length > 0 && formModel.author.length > 0 && !formModel.busy
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
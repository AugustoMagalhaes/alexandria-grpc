import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Item {
    id: page

    signal adminPanelRequested()

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
        color: Theme.backgroundColor
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            color: Theme.surfaceColor
            border.color: Theme.borderColor
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingMedium
                anchors.rightMargin: Theme.spacingMedium
                spacing: Theme.spacingSmall

                Label {
                    text: Session.isAdmin ? qsTr("Books (Administrator)") : qsTr("Books")
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.textPrimary
                }

                Item {
                    Layout.fillWidth: true
                }

                AppButton {
                    text: qsTr("Manage Users")
                    visible: Session.isAdmin
                    onClicked: page.adminPanelRequested()
                }

                AppButton {
                    text: qsTr("Change server")
                    onClicked: Session.requestServerChange()
                }

                AppButton {
                    text: qsTr("Log out")
                    onClicked: Session.logout()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingMedium
            spacing: Theme.spacingSmall

            AppTextField {
                id: searchField
                placeholderText: qsTr("Search by title or author")
                Layout.fillWidth: true
                onTextChanged: viewModel.refresh(text)
            }

            AppButton {
                text: qsTr("Add Book")
                primary: true
                visible: Session.isAdmin
                onClicked: formDialog.openForCreate()
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: Theme.spacingMedium
            Layout.rightMargin: Theme.spacingMedium
            model: viewModel.books
            spacing: Theme.spacingSmall
            clip: true

            delegate: Rectangle {
                width: ListView.view.width
                height: 64
                radius: Theme.radiusSmall
                color: Theme.surfaceColor
                border.color: Theme.borderColor
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingSmall
                    spacing: Theme.spacingSmall

                    ColumnLayout {
                        spacing: 2

                        RowLayout {
                            spacing: 8

                            Label {
                                text: modelData.title
                                font.bold: true
                                font.pixelSize: Theme.fontSizeBody
                                color: Theme.textPrimary
                            }

                            Label {
                                text: qsTr("Not borrowable")
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.errorColor
                                visible: !modelData.borrowable
                            }
                        }

                        Label {
                            text: modelData.author + " · " + modelData.availableCopies + "/" + modelData.totalCopies + qsTr(" available")
                                + (modelData.category.length > 0 ? " · " + modelData.category : "")
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.textSecondary
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    AppButton {
                        text: qsTr("Edit")
                        visible: Session.isAdmin
                        onClicked: formDialog.openForEdit(modelData)
                    }

                    AppButton {
                        text: qsTr("Delete")
                        visible: Session.isAdmin
                        onClicked: deleteDialog.openFor(modelData, qsTr("Delete Book"))
                    }
                }
            }
        }
    }

    Label {
        anchors.centerIn: parent
        text: viewModel.errorMessage
        color: Theme.errorColor
        visible: viewModel.errorMessage.length > 0
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: viewModel.busy
        visible: viewModel.busy
    }
}
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Dialogs
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

    Dialog {
        id: bulkDeleteDialog
        modal: true
        anchors.centerIn: parent
        width: 320
        padding: 24

        background: Rectangle {
            color: Theme.surfaceColor
            radius: Theme.radiusMedium
            border.color: Theme.borderColor
            border.width: 1
        }

        ColumnLayout {
            id: bulkDeleteContent
            anchors.fill: parent
            spacing: 16
            focus: true

            Keys.onReturnPressed: confirmAction()
            Keys.onEnterPressed: confirmAction()

            function confirmAction() {
                bulkDeleteDialog.close()
                viewModel.deleteSelected()
            }

            Label {
                text: qsTr("Delete Selected Books")
                font.pixelSize: Theme.fontSizeTitle
                font.bold: true
                color: Theme.textPrimary
            }

            Label {
                text: qsTr("Are you sure you want to delete %1 book(s)? This cannot be undone.").arg(viewModel.selectedCount)
                color: Theme.textPrimary
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight
                spacing: 8

                AppButton {
                    text: qsTr("Cancel")
                    onClicked: bulkDeleteDialog.close()
                }

                AppButton {
                    text: qsTr("Delete")
                    primary: true
                    onClicked: bulkDeleteContent.confirmAction()
                }
            }
        }

        onOpened: bulkDeleteContent.forceActiveFocus()
    }

    CsvImportDialog {
        id: csvImportDialog
        importFinishedCallback: function() { viewModel.refresh(searchField.text) }
    }

    FileDialog {
        id: exportDialog
        title: qsTr("Save CSV file")
        fileMode: FileDialog.SaveFile
        nameFilters: [qsTr("CSV files (*.csv)")]
        defaultSuffix: "csv"
        onAccepted: csvExportModel.exportToFile(selectedFile.toString().replace("file://", ""))
    }

    CsvViewModel {
        id: csvExportModel
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.backgroundColor
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacingMedium
            Layout.leftMargin: Theme.spacingMedium
            Layout.rightMargin: Theme.spacingMedium
            spacing: Theme.spacingSmall

            Label {
                text: qsTr("Books")
                font.pixelSize: Theme.fontSizeTitle
                font.bold: true
                color: Theme.textPrimary
            }

            AppButton {
                text: qsTr("Export CSV")
                neutral: true
                visible: Session.isAdmin
                onClicked: exportDialog.open()
            }

            AppButton {
                text: qsTr("Import CSV")
                neutral: true
                visible: Session.isAdmin
                onClicked: {
                    csvImportDialog.resetForm()
                    csvImportDialog.open()
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingMedium
            spacing: Theme.spacingSmall

            AppTextField {
                id: searchField
                placeholderText: qsTr("Search by title, author, category, school code or keywords")
                Layout.fillWidth: true
                onTextChanged: viewModel.refresh(text)
            }

            AppButton {
                text: qsTr("Add Book")
                success: true
                visible: Session.isAdmin
                onClicked: formDialog.openForCreate()
            }
        }

        Label {
            text: csvExportModel.statusMessage
            color: csvExportModel.statusIsError ? Theme.errorColor : Theme.successColor
            visible: csvExportModel.statusMessage.length > 0
            Layout.leftMargin: Theme.spacingMedium
            Layout.bottomMargin: Theme.spacingSmall
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 44
            Layout.leftMargin: Theme.spacingMedium
            Layout.rightMargin: Theme.spacingMedium
            visible: Session.isAdmin
            color: Theme.primaryColor
            radius: Theme.radiusSmall

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingMedium
                anchors.rightMargin: Theme.spacingMedium
                spacing: Theme.spacingSmall

                Label {
                    text: viewModel.selectedCount > 0
                        ? qsTr("%1 selected").arg(viewModel.selectedCount)
                        : qsTr("No books selected")
                    color: "white"
                    Layout.fillWidth: true
                }

                AppButton {
                    text: qsTr("Delete Selected")
                    enabled: viewModel.selectedCount > 0
                    onClicked: bulkDeleteDialog.open()
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: Theme.spacingMedium
            Layout.rightMargin: Theme.spacingMedium
            Layout.topMargin: Theme.spacingSmall
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

                    AppCheckBox {
                        visible: Session.isAdmin
                        checked: viewModel.isSelected(modelData.id)
                        onCheckedChanged: viewModel.toggleSelection(modelData.id)
                    }

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
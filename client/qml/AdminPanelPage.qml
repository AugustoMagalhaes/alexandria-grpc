import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Dialogs
import Alexandria

Item {
    id: page

    signal backRequested()

    UserListViewModel {
        id: viewModel
    }

    onVisibleChanged: {
        if (visible) {
            viewModel.refresh()
        }
    }

    UserFormDialog {
        id: userFormDialog
        userSavedCallback: function() { viewModel.refresh() }
    }

    DeleteConfirmDialog {
        id: deleteDialog
        onConfirmed: (id) => viewModel.deleteUser(id)
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
                text: qsTr("Delete Selected Users")
                font.pixelSize: Theme.fontSizeTitle
                font.bold: true
                color: Theme.textPrimary
            }

            Label {
                text: qsTr("Are you sure you want to delete %1 user(s)? This cannot be undone.").arg(viewModel.selectedCount)
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
                    text: qsTr("User Management")
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.textPrimary
                }

                Item {
                    Layout.fillWidth: true
                }

                AppButton {
                    text: qsTr("Export CSV")
                    onClicked: exportDialog.open()
                }

                AppButton {
                    text: qsTr("Import CSV")
                    onClicked: {
                        csvImportDialog.resetForm()
                        csvImportDialog.open()
                    }
                }

                AppButton {
                    text: qsTr("Add User")
                    primary: true
                    onClicked: userFormDialog.openForCreate()
                }

                AppButton {
                    text: qsTr("Back to Books")
                    onClicked: page.backRequested()
                }
            }
        }

        Label {
            text: csvExportModel.statusMessage
            color: csvExportModel.statusIsError ? Theme.errorColor : Theme.successColor
            visible: csvExportModel.statusMessage.length > 0
            Layout.leftMargin: Theme.spacingMedium
            Layout.topMargin: Theme.spacingSmall
            Layout.bottomMargin: Theme.spacingSmall
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 44
            Layout.leftMargin: Theme.spacingMedium
            Layout.rightMargin: Theme.spacingMedium
            Layout.topMargin: Theme.spacingMedium
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
                        : qsTr("No users selected")
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
            model: viewModel.users
            spacing: Theme.spacingSmall
            clip: true

            delegate: Rectangle {
                width: ListView.view.width
                height: 56
                radius: Theme.radiusSmall
                color: Theme.surfaceColor
                border.color: Theme.borderColor
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingSmall
                    spacing: Theme.spacingSmall

                    AppCheckBox {
                        checked: viewModel.isSelected(modelData.id)
                        onCheckedChanged: viewModel.toggleSelection(modelData.id)
                    }

                    Label {
                        text: modelData.username
                        font.bold: true
                        font.pixelSize: Theme.fontSizeBody
                        color: Theme.textPrimary
                    }

                    Label {
                        text: modelData.roleLabel
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.textSecondary
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    AppButton {
                        text: qsTr("Delete")
                        onClicked: deleteDialog.openFor({ id: modelData.id, title: modelData.username }, qsTr("Delete User"))
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
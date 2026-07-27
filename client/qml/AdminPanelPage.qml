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

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: Theme.spacingMedium
            Layout.rightMargin: Theme.spacingMedium
            Layout.topMargin: Theme.spacingMedium
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
                        onClicked: deleteDialog.openFor({ id: modelData.id, title: modelData.username })
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
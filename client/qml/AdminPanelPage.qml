import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt.labs.platform as Platform
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

    Platform.FileDialog {
        id: exportDialog
        title: qsTr("Save CSV file")
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: [qsTr("CSV files (*.csv)")]
        defaultSuffix: "csv"
        onAccepted: csvExportModel.exportToFile(file.toString().replace("file://", ""))
    }

    CsvViewModel {
        id: csvExportModel
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
                text: qsTr("User Management")
                font.pixelSize: 18
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Export CSV")
                onClicked: exportDialog.open()
            }

            Button {
                text: qsTr("Import CSV")
                onClicked: {
                    csvImportDialog.resetForm()
                    csvImportDialog.open()
                }
            }

            Button {
                text: qsTr("Add User")
                onClicked: userFormDialog.openForCreate()
            }

            Button {
                text: qsTr("Back to Books")
                onClicked: page.backRequested()
            }
        }

        Label {
            text: csvExportModel.statusMessage
            color: csvExportModel.statusIsError ? "red" : "green"
            visible: csvExportModel.statusMessage.length > 0
            Layout.leftMargin: 12
            Layout.bottomMargin: 8
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: viewModel.users
            clip: true

            delegate: ItemDelegate {
                width: ListView.view.width
                height: 48

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8

                    Label {
                        text: modelData.username
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    Label {
                        text: modelData.roleLabel
                    }

                    Button {
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
        color: "red"
        visible: viewModel.errorMessage.length > 0
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: viewModel.busy
    }
}
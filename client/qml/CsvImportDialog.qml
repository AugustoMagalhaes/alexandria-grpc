import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Dialogs
import Alexandria

Dialog {
    id: dialog
    modal: true
    anchors.centerIn: parent
    width: 360
    title: qsTr("Import Books from CSV")

    property var importFinishedCallback: null

    CsvViewModel {
        id: csvModel
        onImportFinished: {
            if (dialog.importFinishedCallback) {
                dialog.importFinishedCallback()
            }
        }
    }

    property string selectedFilePath: ""
    property bool replaceExisting: false

    function resetForm() {
        selectedFilePath = ""
        replaceExisting = false
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Select CSV file")
        nameFilters: [qsTr("CSV files (*.csv)")]
        onAccepted: dialog.selectedFilePath = selectedFile.toString().replace("file://", "")
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: dialog.selectedFilePath.length > 0 ? dialog.selectedFilePath : qsTr("No file selected")
            wrapMode: Text.WrapAnywhere
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Choose File…")
            onClicked: fileDialog.open()
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true

            RadioButton {
                text: qsTr("Append to existing books")
                checked: !dialog.replaceExisting
                onCheckedChanged: if (checked) dialog.replaceExisting = false
            }
        }

        RowLayout {
            Layout.fillWidth: true

            RadioButton {
                text: qsTr("Replace all books")
                checked: dialog.replaceExisting
                onCheckedChanged: if (checked) dialog.replaceExisting = true
            }
        }

        Label {
            text: csvModel.statusMessage
            color: csvModel.statusIsError ? "red" : "green"
            wrapMode: Text.WordWrap
            visible: csvModel.statusMessage.length > 0
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight

            Button {
                text: qsTr("Close")
                onClicked: dialog.close()
            }

            Button {
                text: qsTr("Import")
                enabled: dialog.selectedFilePath.length > 0 && !csvModel.busy
                onClicked: csvModel.importFromFile(dialog.selectedFilePath, dialog.replaceExisting)
            }
        }

        BusyIndicator {
            running: csvModel.busy
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
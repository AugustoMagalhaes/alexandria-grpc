import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Dialogs
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
        spacing: 16

        Label {
            text: qsTr("Import Books from CSV")
            font.pixelSize: Theme.fontSizeTitle
            font.bold: true
            color: Theme.textPrimary
        }

        Label {
            text: dialog.selectedFilePath.length > 0 ? dialog.selectedFilePath : qsTr("No file selected")
            color: Theme.textSecondary
            wrapMode: Text.WrapAnywhere
            Layout.fillWidth: true
        }

        AppButton {
            text: qsTr("Choose File…")
            onClicked: fileDialog.open()
            Layout.fillWidth: true
        }

        ColumnLayout {
            spacing: 4

            AppRadioButton {
                text: qsTr("Append to existing books")
                checked: !dialog.replaceExisting
                onCheckedChanged: if (checked) dialog.replaceExisting = false
            }

            AppRadioButton {
                text: qsTr("Replace all books")
                checked: dialog.replaceExisting
                onCheckedChanged: if (checked) dialog.replaceExisting = true
            }
        }

        Label {
            text: csvModel.statusMessage
            color: csvModel.statusIsError ? Theme.errorColor : Theme.successColor
            wrapMode: Text.WordWrap
            visible: csvModel.statusMessage.length > 0
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 8

            AppButton {
                text: qsTr("Close")
                onClicked: dialog.close()
            }

            AppButton {
                text: qsTr("Import")
                primary: true
                enabled: dialog.selectedFilePath.length > 0 && !csvModel.busy
                onClicked: csvModel.importFromFile(dialog.selectedFilePath, dialog.replaceExisting)
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 32

            BusyIndicator {
                anchors.centerIn: parent
                running: csvModel.busy
                visible: csvModel.busy
            }
        }
    }
}
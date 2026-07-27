import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Dialog {
    id: dialog
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

    property int targetBookId: 0
    property string targetTitle: ""
    property var onConfirmed: null

    function openFor(book) {
        targetBookId = book.id
        targetTitle = book.title
        dialog.open()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        Label {
            text: qsTr("Delete Book")
            font.pixelSize: Theme.fontSizeTitle
            font.bold: true
            color: Theme.textPrimary
        }

        Label {
            text: qsTr("Are you sure you want to delete \"%1\"?").arg(dialog.targetTitle)
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
                onClicked: dialog.close()
            }

            AppButton {
                text: qsTr("Delete")
                primary: true
                onClicked: {
                    dialog.close()
                    if (dialog.onConfirmed) {
                        dialog.onConfirmed(dialog.targetBookId)
                    }
                }
            }
        }
    }
}
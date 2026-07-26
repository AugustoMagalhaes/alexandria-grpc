import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Dialog {
    id: dialog
    modal: true
    anchors.centerIn: parent
    width: 320
    title: qsTr("Delete Book")

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
            text: qsTr("Are you sure you want to delete \"%1\"?").arg(dialog.targetTitle)
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight

            Button {
                text: qsTr("Cancel")
                onClicked: dialog.close()
            }

            Button {
                text: qsTr("Delete")
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
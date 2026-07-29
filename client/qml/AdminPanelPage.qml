import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Alexandria

Item {
    id: page

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

    Rectangle {
        anchors.fill: parent
        color: Theme.backgroundColor
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.spacingMedium
            spacing: Theme.spacingSmall

            Label {
                text: qsTr("User Management")
                font.pixelSize: Theme.fontSizeTitle
                font.bold: true
                color: Theme.textPrimary
                Layout.fillWidth: true
            }

            AppButton {
                text: qsTr("Add User")
                success: true
                onClicked: userFormDialog.openForCreate()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 44
            Layout.leftMargin: Theme.spacingMedium
            Layout.rightMargin: Theme.spacingMedium
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
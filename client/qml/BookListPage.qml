import QtQuick
import QtQuick.Controls.Basic
import Alexandria

Item {
    BookListViewModel {
        id: viewModel
    }

    onVisibleChanged: {
        if (visible) {
            viewModel.refresh()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"
    }

    ListView {
        anchors.fill: parent
        model: viewModel.books

        delegate: ItemDelegate {
            width: ListView.view.width
            text: modelData.title + " — " + modelData.author
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
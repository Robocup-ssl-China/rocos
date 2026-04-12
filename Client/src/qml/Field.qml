import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ZSS 1.0 as ZSS

Item {
    id: root
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#303030"
    }

    Column {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: fieldBar
            width: parent.width
            height: 48
            currentIndex: 1

            TabButton { text: "Origin" }
            TabButton { text: "Filtered B" }
            TabButton { text: "Filtered Y" }
        }

        Item {
            width: parent.width
            height: parent.height - fieldBar.height

            Rectangle {
                anchors.fill: parent
                border.color: "#555"
                border.width: 1
                color: "transparent"

                // Keep only one Field instance and switch its type by tab.
                ZSS.Field {
                    id: field
                    anchors.fill: parent
                    type: fieldBar.currentIndex + 1
                    draw: true
                }
            }
        }
    }
}
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ZSS 1.0 as ZSS

Item {
    id: root
    anchors.fill: parent

    ZSS.Interaction4Field {
        id: interaction
    }

    Timer {
        id: fpsTimer
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            fps.text = interaction.getFPS().toString()
                       + "\n" + interaction.getMedusaFPS(0).toString()
                       + "\n" + interaction.getMedusaFPS(1).toString()
        }
    }

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
            id: fieldArea
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
                    onWidthChanged: interaction.setSize(width, height)
                    onHeightChanged: interaction.setSize(width, height)
                    Component.onCompleted: interaction.setSize(width, height)
                }
            }

            Text {
                id: fpsWord
                text: qsTr("FPS")
                x: parent.width - 70
                y: 5
                color: "white"
                font.pointSize: 11
                font.weight: Font.Bold
            }

            Text {
                id: fps
                text: "0\n0\n0"
                x: parent.width - 30
                y: 5
                color: "#0077ff"
                font.pointSize: 12
                font.weight: Font.Bold
            }

            Text {
                id: positionDisplay
                color: "white"
                x: 10
                y: 5
                property string strX: "0"
                property string strY: "0"
                text: qsTr("( " + strX + " , " + strY + " )")
                font.pointSize: 12
                font.weight: Font.Bold
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.NoButton
                onPositionChanged: {
                    positionDisplay.strX = interaction.getRealX(mouseX).toString()
                    positionDisplay.strY = interaction.getRealY(mouseY).toString()
                }
            }
        }
    }
}
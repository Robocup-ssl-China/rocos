import QtQuick 2.12
import QtQuick.Controls 2.12
import Theme 1.0
Control{
    id:root
    property alias running: timer.running
    property int size : 6
    implicitWidth: Math.max(20,size)
    implicitHeight: Math.max(20,size)
    Rectangle{
        id:light
        property bool lightSwitch : true
        width:size
        height:size
        radius:size/2
        anchors.centerIn: parent
        color:!root.running ? T.c_red : lightSwitch ? T.c_green : "transparent"
        Timer{
            property int count : 0
            id:timer
            interval: 250
            repeat: true
            onTriggered: {
                count = (count + 1)%3

                light.lightSwitch = (count != 2)
            }
        }
    }
}

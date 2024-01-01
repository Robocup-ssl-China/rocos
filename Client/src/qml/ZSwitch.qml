import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.12
Grid{
    id:root;
    width:parent.itemWidth || 100;
    height:26;
    verticalItemAlignment: Grid.AlignVCenter;
    horizontalItemAlignment: Grid.AlignHCenter;
    spacing: 0;
    columns:3;
    property int radius : height/2;
    property string leftText : "No Left";
    property string rightText : "No Right";
    signal stateChanged(bool state);
    property bool checked : false;
    Text{
        color:"white"
        font.pixelSize: parent.height/2;
        width:parent.width*0.25;
        height:parent.height;
        text: parent.leftText;
        horizontalAlignment:Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    Switch{
        id:switcher
        width:parent.width*0.5;
        height:parent.height;
        indicator: Rectangle {
            x: parent.visualPosition * (parent.width - width)
            y: (parent.height - height) / 2
            width: parent.width/2;
            height: parent.height;
            radius: parent.parent.radius;
            color: parent.down ? "white" : "white"
            Behavior on x {
                enabled: !parent.pressed
                SmoothedAnimation { velocity: parent.width/2; }
            }
        }
        background: Rectangle {
            width:parent.width;
            height:parent.height;
            radius: parent.parent.radius;
            color: parent.checked ? Material.accent : Material.accent
        }
        checked : root.checked
        onCheckedChanged: {
            root.checked = switcher.checked
        }
    }
    Text{
        color:"white"
        font.pixelSize: parent.height/2;
        width:parent.width*0.25;
        height:parent.height;
        text: parent.rightText;
        horizontalAlignment:Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}

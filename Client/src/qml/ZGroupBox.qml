import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
GroupBox{
    title: qsTr("No Title")
    width:parent.itemWidth;
    label: Rectangle {
        x:parent.leftPadding;
        y:-12;
        color: "#303030"
        Text {
            text: parent.parent.title;
            color:"white"
            font.pixelSize: 16;
        }
        width: childrenRect.width
        height: childrenRect.height
    }
}

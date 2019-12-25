import QtQuick 2.10

Grid{
    id:root;
    width:parent.itemWidth || 100;
    height:parent.itemHeight || 60;
    verticalItemAlignment: Grid.AlignVCenter;
    horizontalItemAlignment: Grid.AlignHCenter;
    topPadding: 5;
    spacing: 3;
    columns: 3;
    property alias battery : battery.value;
    property alias capacity : capacity.value;
    property bool infrared: false;
    property bool team : true;
    property int number : 0;
    Rectangle{
        id:icon;
        width:parent.height;
        height:parent.height - parent.topPadding;
        radius: height/4;
        color : root.team ? "#f0ad4e" : "#337ab7";
//        source:number == -1 ? "/robot/z.png" : ("/robot/" + root.color + root.number + ".png");
        Text{
            anchors.verticalCenter: parent.verticalCenter;
            anchors.horizontalCenter: parent.horizontalCenter;
            text :root.number;
            font.weight: Font.ExtraBold;
            font.pixelSize: parent.width*0.8;
            opacity: 0.5;
        }
        Rectangle{
            x:0;
            y:0;
            width:parent.width
            height:parent.height
            radius: height/4;
            color: "#d9534f"
            opacity: root.infrared ? 1 : 0
        }
    }
    Grid{
        verticalItemAlignment: Grid.AlignVCenter;
        horizontalItemAlignment: Grid.AlignHCenter;
        spacing: 1;
        columns: 1;
        rows : 2;
        width : parent.width - parent.height - spacing;
        height:parent.height - parent.topPadding;
        property int itemHeight : (height - (rows-1) * spacing - 2*padding)/rows;
        property int itemWidth : width;

        ZValue{
            id : battery;
        }
        ZValue{
            id : capacity
        }
    }
}

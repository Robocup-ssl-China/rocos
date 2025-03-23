import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Theme 1.0
Control {
    id:root
    property alias container:grid
    property var title:""
    property alias bg:bg
    default property alias content: grid.children
    property double contentWidth:parent.contentWidth - leftPadding - rightPadding
    property bool fillWidth:true
    padding: 9
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding
    implicitWidth: fillWidth ? parent.contentWidth : col.implicitWidth+leftPadding+rightPadding
    implicitHeight: grid.implicitHeight+topPadding+bottomPadding
    background:Rectangle{
        id:bg
        color: "transparent"
        radius: 10
        border{
            color:T.__c["darker"][T.i_main]
            width: 3
        }
    }
    Rectangle{
        x:20;
        y:-(title.font.pixelSize)/2;
        implicitHeight: title.implicitHeight
        implicitWidth: title.implicitWidth
        color:T.__c["main"][T.i_main]
        Text{
            id:title
            text:root.title
            color:root.enabled ? T.findColorInvert(bg.border.color) : T.findDarkColor(T.findColorInvert(bg.border.color),2.0)
            font.pixelSize: 12
        }
    }


    contentItem:GridLayout{
        id: grid
        columns: 1
        property double contentWidth:(width - columnSpacing * (columns-1))/(columns)
        rowSpacing: 6
        columnSpacing: 6
        width:parent.contentWidth
        height:childrenRect.height
    }
}

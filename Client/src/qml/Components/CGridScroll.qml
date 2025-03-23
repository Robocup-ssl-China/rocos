import QtQuick 2.12
import QtQuick.Controls 2.12
import Theme 1.0
Control{
    property alias container:grid
    default property alias content: grid.children
    property alias bg:bg
    background: Rectangle{
        id:bg
        color:T.__c["main"][T.i_main]
    }
    ScrollView {
        id:scroll
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        anchors.fill: parent
        contentWidth: availableWidth
        contentHeight: grid.height
        clip: true
        focusPolicy: Qt.WheelFocus
        MouseArea{
            width:scroll.width
            height:scroll.height
            onClicked: {
                parent.forceActiveFocus()
            }
        }
        Grid{
            id: grid
            columns: 1
            property double contentWidth:(width - leftPadding - rightPadding - spacing * (columns-1))/(columns)
            padding: 5
            spacing: 5
            width:parent.width
        }
    }
}

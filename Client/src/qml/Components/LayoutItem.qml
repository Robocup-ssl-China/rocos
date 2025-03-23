import QtQuick 2.0

Rectangle {
    id:root
    property int padding:0
    property int topPadding: padding
    property int leftPadding: padding
    property int rightPadding: padding
    property int bottomPadding: padding
    default property alias content: core.children
    Row{
        spacing: 0
        Item{
            width:root.leftPadding
            height:root.height
        }
        Item{
            id:secRoot
            width:root.width - root.leftPadding - root.rightPadding
            Column{
                spacing: 0
                Item{
                    width:secRoot.width
                    height:root.topPadding
                }
                Item{
                    id:core
                }
                Item{
                    width:secRoot.width
                    height:root.bottomPadding
                }
            }
        }
        Item{
            width:root.rightPadding
            height:root.height
        }
    }
    Component.onCompleted: {
        console.log("debug : ",topPadding,bottomPadding,leftPadding,rightPadding)
    }
}

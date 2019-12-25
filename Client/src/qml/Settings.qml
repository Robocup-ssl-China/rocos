import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import ZSS 1.0 as ZSS
Item{
    ZSS.ParamModel{
        id:paramModel;
    }
    Shortcut{
        sequence: "r"
        onActivated: {
            paramModel.reload();
        }
    }
    TreeView{
        id:paramTree
        anchors.fill: parent
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        TableViewColumn {
            title: "Name"
            role: "settingName"
            width:parent.width*0.4;
            delegate:normalTextDelegate;
        }
        TableViewColumn {
            title: "Type"
            role: "settingType"
            width:parent.width*0.2;
            delegate:normalTextDelegate;
        }
        TableViewColumn {
            title: "Value"
            role: "settingValue"
            width:parent.width*0.4;
            delegate:stringDelegate;
        }

        style: TreeViewStyle {
            backgroundColor: "#484848";
            alternateBackgroundColor:"#404040";
            textColor:"#ccc";
//            branchDelegate: Rectangle {
//                width: 12; height: 12
//                color: styleData.isExpanded ? "#ccc" : "#aaa"
//                radius: width/2
//            }
//            frame: Rectangle {border {color: "blue"}}
            headerDelegate: Rectangle {
                height: headerItem.implicitHeight*1.4
                width: headerItem.implicitWidth
                color: "#333"
                Text {
                    id: headerItem
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: styleData.textAlignment
                    anchors.leftMargin: 12
                    text: styleData.value
                    elide: Text.ElideRight
                    color: "#fff";
                    renderType: Text.NativeRendering
                    font.pixelSize: 16;
                    font.family: "Arial";
                }
                Rectangle {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 1
                    anchors.topMargin: 1
                    width: 1
                    color: "#777"
                }
                Rectangle {
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 1
                    anchors.rightMargin: 1
                    height:1
                    color: "#777"
                }
            }
            rowDelegate:Rectangle{
                height: 30;
                width:parent.width;
                property color selectedColor: control.activeFocus ? "#07c" : "#999"
                color: !styleData.alternate ? alternateBackgroundColor : backgroundColor
            }
        }
        model:paramModel;
    }
    Component {
        id: stringDelegate
        Loader{
            property var styleData : parent.styleData;
            property string type : paramModel.getType(styleData.index);
            sourceComponent: styleData.hasChildren ? normalTextDelegate : (type == "Bool" ? boolDelegate : notBoolDelegate)
        }
    }
    Component{
        id:notBoolDelegate
        Rectangle{
            property var styleData : parent.styleData;
            color: input.activeFocus  ? "#ccc" : "transparent";
            height:30;
            TextInput {
                id:input;
                anchors.fill: parent;
                text: styleData.value
                color:activeFocus?"#222":styleData.textColor;
                horizontalAlignment: styleData.textAlignment
                font.pixelSize: 16;
                font.family:"Arial";
                leftPadding: 12;
                verticalAlignment: Text.AlignVCenter
                //validator:RegExpValidator { regExp: /^(-?)(0|([1-9][0-9]*))(\.[0-9]+)?$/ }
                onAccepted:{
                    if(paramModel.setData(styleData.index,text))
                        focus = false;
                }
            }
        }
    }
    Component{
        id:boolDelegate;
        Rectangle{
            property var styleData : parent.styleData;
            color: "transparent";
            height:30;
            width:parent.width;
            CheckBox{
                id:input;
                height:30;
                x:12;
                checked: styleData.value === "true"
                onClicked: paramModel.setData(styleData.index,checked ? "true" : "false")
            }
        }
    }
    Component{
        id: normalTextDelegate
//        Rectangle{
//            height:30;
//            color: "transparent"
            Text {
                lineHeight:30;
                leftPadding:12;
                anchors.verticalCenter: parent.verticalCenter
                color: styleData.textColor
                elide: styleData.elideMode
                text: styleData.value
                font.pixelSize: 15
            }
//        }
    }
}

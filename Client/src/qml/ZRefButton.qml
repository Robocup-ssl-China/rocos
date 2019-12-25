import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
Button{
    id:button;
    implicitWidth:parent.buttonWidth || parent.itemWidth || 20;
    implicitHeight:parent.buttonHeight || parent.itemHeight || 40;
    text:qsTr("BUTTON")
    property alias color : text.color;
    contentItem: Text {
        id: text;
        text: button.text
        font: button.font
        opacity: enabled ? 1.0 : 0.3
        color: "#ddd";
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}

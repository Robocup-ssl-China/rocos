import QtQuick 2.10
import QtQuick.Controls 2.3
Button{
    id:button;
    implicitWidth:parent.buttonWidth || parent.itemWidth || 20;
    implicitHeight:parent.buttonHeight || 46;
}

import QtQuick 2.10
import QtQuick.Controls 2.2
ComboBox {
    implicitWidth:parent.buttonWidth || parent.itemWidth || 20;
    implicitHeight:parent.buttonHeight || parent.itemHeight || 40;
}

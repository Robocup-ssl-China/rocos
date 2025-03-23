import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Theme 1.0
Item{
    id:root
    property alias btn : btn;
    property bool fillWidth : true
    property alias model:btn.model
    property string _s: "dark"
    implicitWidth: fillWidth ? parent.contentWidth : btn.implicitWidth
    implicitHeight: btn.implicitHeight
    onWidthChanged: {
        layout.width = width
    }
    onHeightChanged: {
        layout.height = height
    }
    RowLayout{
        id:layout
        ComboBox {
            id:btn
            Layout.preferredWidth: 300
            Layout.fillWidth: true
        }
    }

}


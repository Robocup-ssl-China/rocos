import QtQuick 2.12
import QtQuick.Controls 2.12
import Theme 1.0
Item{
    id:root
    property bool fillWidth : false
    property alias btn : btn
    property alias from : btn.from
    property alias to : btn.to
    property alias value : btn.value
    property string _s:"main2"

    implicitWidth: fillWidth ? parent.contentWidth : btn.implicitWidth
    implicitHeight: btn.implicitHeight
    onWidthChanged: {
        btn.width = width
    }
    onHeightChanged: {
        btn.height = height
    }
    SpinBox{
        editable: true
        id:btn
        contentItem: TextInput {
            z: 2
            text: btn.displayText

            font: btn.font
            color:btn.enabled ? btn.down ? T.__c[_s][T.i_active_font] : btn.hovered ? T.__c[_s][T.i_hover_font] : T.__c[_s][T.i_main_font] : T.__c[_s][T.i_disable_font]
            selectionColor: btn.palette.highlight
            selectedTextColor: btn.palette.highlightedText
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter

            readOnly: !btn.editable
            validator: btn.validator
            inputMethodHints: btn.inputMethodHints
        }
    }
}

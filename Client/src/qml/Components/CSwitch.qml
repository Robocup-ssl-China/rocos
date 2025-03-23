import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Universal 2.12
import Theme 1.0
Item{
    id:root
    property bool fillWidth : true
    property variant text: ["left","right"]
    property alias checked : btn.checked
    property alias font : btn.font
    property variant style : ["main2","main2"]
    property variant icon : ["",""]
    property alias display : btn.display
    property string _s: style[btn.checked?1:0]
    readonly property variant _t:T.__c

    implicitWidth: fillWidth ? parent.contentWidth : btn.implicitWidth
    implicitHeight: btn.implicitHeight
    onWidthChanged: {
        btn.width = width
    }
    onHeightChanged: {
        btn.height = height
    }
    Button {
        id: btn
        checkable: true
        text:root.text[checked?1:0]
        implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                                    implicitContentWidth + leftPadding + rightPadding)
        implicitHeight: Math.max(Math.max(implicitBackgroundHeight + topInset + bottomInset,
                                 implicitContentHeight + topPadding + bottomPadding),32)
        icon.source:root.icon[checked?1:0]
        icon.color: btn.down ? T.__c[_s][T.i_active_font] :
                               btn.hovered ? T.__c[_s][T.i_hover_font] : T.__c[_s][T.i_main_font]
        icon.width: display == AbstractButton.TextUnderIcon ? 30 : 20
        icon.height: display == AbstractButton.TextUnderIcon ? 30 : 20
        background: Rectangle {
            color: btn.down ? T.__c[_s][T.i_active] : btn.hovered ? T.__c[_s][T.i_hover] : T.__c[_s][T.i_main]
            border.color: "#26282a"
            border.width: 0
            radius: 4
        }
        contentItem:IconLabel {
            spacing: btn.spacing
            mirrored: btn.mirrored
            display: btn.display
            icon: btn.icon
            text: btn.text
            font: btn.font
            color : btn.down ? T.__c[_s][T.i_active_font] : btn.hovered ? T.__c[_s][T.i_hover_font] : T.__c[_s][T.i_main_font]
        }
    }
}

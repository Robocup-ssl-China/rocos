import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Universal 2.12
import Theme 1.0
Item{
    id:root
    property bool fillWidth : true
    property alias text : btn.text
    property alias font : btn.font
    property alias source : btn.icon.source
    property alias checkable : btn.checkable
    property alias checked : btn.checked
    property alias enabled: btn.enabled
    property alias btn : btn
    property string _s: "main2"
    property bool iconFull : false
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
        text:""
        implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                                    implicitContentWidth + leftPadding + rightPadding)
        implicitHeight: Math.max(Math.max(implicitBackgroundHeight + topInset + bottomInset,
                                 implicitContentHeight + topPadding + bottomPadding),32)
        padding: iconFull ? 0 : 6
        horizontalPadding: iconFull ? 0 : padding + 2
        spacing: iconFull ? 0 : 6
        icon.color: btn.enabled ? btn.down ? T.__c[_s][T.i_active_font] : btn.hovered ? T.__c[_s][T.i_hover_font] : T.__c[_s][T.i_main_font] : T.__c[_s][T.i_disable_font]
        background: Rectangle {
            color: btn.enabled ? btn.down ? T.__c[_s][T.i_active] : btn.hovered ? T.__c[_s][T.i_hover] : T.__c[_s][T.i_main] : T.__c[_s][T.i_disable]
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
            color : btn.enabled ? btn.down ? T.__c[_s][T.i_active_font] : btn.hovered ? T.__c[_s][T.i_hover_font] : T.__c[_s][T.i_main_font] : T.__c[_s][T.i_disable_font]
        }
    }
}

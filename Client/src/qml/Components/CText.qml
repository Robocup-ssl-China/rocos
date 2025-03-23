import QtQuick 2.12
import QtQuick.Controls 2.12
import Theme 1.0
Text{
    id:root
    property string _s: "main2"
    property bool fillWidth:false
    width:fillWidth ? parent.contentWidth : implicitWidth
    color: T.__c[_s][T.i_main_font]
    elide: Text.ElideRight
    verticalAlignment:Text.AlignVCenter
    font.pixelSize: 18
    leftPadding: 8
    rightPadding: 8
}

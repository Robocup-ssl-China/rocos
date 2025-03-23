import QtQuick 2.12
import "qrc:/kddockwidgets/private/quick/qml/" as KDDW
import Components 1.0 as Z
import Theme 1.0

KDDW.TitleBarBase{
    id: root
    color: T.__c["main2"][T.i_main]
    Text {
        color: T.__c["accent"][T.i_main]
        font.bold: true
        text: root.title
        anchors {
            left: parent.left
            leftMargin: 10
            verticalCenter: root.verticalCenter
        }
    }
    Z.CButton {
        id: floatingButton
        fillWidth: false
        visible: root.floatButtonVisible
        height: root.height - 15
        width: height
        iconFull: true
        source:"/resource/icon/window_maximize.png"
        anchors {
            right: root.right
            rightMargin: 30
            verticalCenter: root.verticalCenter
        }
        btn.onClicked: {
            root.floatButtonClicked();
        }
    }
    Z.CButton {
        id: closeButton
        visible: root.closeButtonEnabled
        fillWidth: false
        enabled: root.floatButtonVisible
        height: root.height - 15
        width: height
        iconFull: true
        source:"/resource/icon/window_close.png"
        anchors {
            right: root.right
            rightMargin: 10
            verticalCenter: root.verticalCenter
        }
        btn.onClicked: {
            root.closeButtonClicked();
        }
    }
}

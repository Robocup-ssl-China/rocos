import QtQuick
import QtQuick.Controls
import com.kdab.dockwidgets 2.0 as KDDW
import ZSS 1.0 as ZSS

// ZSS.Field{
//     type:0;
//     draw:true;
//     anchors.fill: parent;
// }

ApplicationWindow {
    visible: true
    width: 1000
    height: 800
    title: qsTr("@rocosCli")

    KDDW.DockingArea {
        id: root
        anchors.fill: parent

        persistentCentralItemFileName: ":/src/qml/Field.qml"
        options: KDDW.KDDockWidgets.MainWindowOption_HasCentralWidget

        uniqueName: "MainLayout-1"

        KDDW.DockWidget {
            id: dock5
            uniqueName: "dock5"
            Rectangle {
                id: guest
                color: "#2E8BC0"
                anchors.fill: parent
            }
        }
        Component.onCompleted: {
            addDockWidget(dock5, KDDW.KDDockWidgets.Location_OnBottom);
        }
    }

    KDDW.LayoutSaver {
        id: layoutSaver
    }
    function toggleDockWidget(dw) {
        if (dw.isOpen) {
            dw.close();
        } else {
            dw.show();
        }
    }
}
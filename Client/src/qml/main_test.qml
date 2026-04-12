import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.kdab.dockwidgets 2.0 as KDDW

ApplicationWindow {
    visible: true
    width: screen.width;
    height: screen.height - 100;
    title: qsTr("@rocosCli")

    KDDW.DockingArea {
        id: root
        anchors.fill: parent

        options: KDDW.KDDockWidgets.MainWindowOption_HasCentralFrame
        uniqueName: "MainLayout-1"

        KDDW.DockWidget {
            id: centralDock
            uniqueName: "centralField"
            title: "Field"
            Field {
                anchors.fill: parent
            }
        }

        KDDW.DockWidget {
            id: controlBoardDock
            uniqueName: "controlBoardDock"
            title: "ControlBoard"
            ControlBoard {
                anchors.fill: parent
            }
        }

        Component.onCompleted: {
            addDockWidgetAsTab(centralDock)
            addDockWidget(controlBoardDock,
                          KDDW.KDDockWidgets.Location_OnRight,
                          centralDock,
                          Qt.size(root.width * 0.2, root.height))
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

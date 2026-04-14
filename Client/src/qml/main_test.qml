import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.kdab.dockwidgets 2.0 as KDDW
import CustomDW 1.0

ApplicationWindow {
    visible: true
    width: screen.width;
    height: screen.height - 100;
    title: qsTr("@rocosCli")

    // qss-like style string for KDDockWidgets custom skin.
    Component.onCompleted: {
        DockStyle.styleSheet = "title-bg:#18202b; title-border:#2f3e52; title-fg:#eaf1ff;"
                              + "frame-bg:#0f151f; frame-border:#2f3e52; frame-radius:10;"
                              + "tab-bg:#141e2b; tab-border:#2c3a4f; tab-fg:#8ea3c0;"
                              + "tab-active-bg:#2c8cff; tab-active-fg:#ffffff;"
                              + "btn-hover-bg:#2f3f56; btn-pressed-bg:#213249; btn-fg:#eaf1ff;"
                              + "title-height:38; tab-height:34; btn-size:22"
    }

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

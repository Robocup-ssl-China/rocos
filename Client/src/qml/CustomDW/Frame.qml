/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.9
import QtQuick.Controls 2.9
import QtQuick.Layouts 1.9
import com.kdab.dockwidgets 2.0
import CustomDW 1.0

Rectangle {
    id: root

    property QtObject groupCpp
    readonly property QtObject titleBarCpp: groupCpp ? groupCpp.titleBar : null
    readonly property int nonContentsHeight: (titleBar.item ? titleBar.item.heightWhenVisible : 0) + tabbar.implicitHeight + (2 * contentsMargin) + titleBarContentsMargin
    property int contentsMargin: isMDI ? 2 : 2
    property int titleBarContentsMargin: 0
    property int mouseResizeMargin: 8
    readonly property bool isMDI: groupCpp && groupCpp.isMDI
    readonly property bool hasCustomMouseEventRedirector: false
    readonly property bool isFixedHeight: groupCpp && groupCpp.isFixedHeight
    readonly property bool isFixedWidth: groupCpp && groupCpp.isFixedWidth
    readonly property bool resizeAllowed: root.isMDI && !Singletons.helpers.isDragging && Singletons.dockRegistry && (!Singletons.helpers.groupViewInMDIResize || Singletons.helpers.groupViewInMDIResize === groupCpp)
    property alias tabBarHeight: tabbar.height

    anchors.fill: parent

    radius: DockStyle.frameRadius
    color: DockStyle.frameBackground
    border {
        color: DockStyle.frameBorderColor
        width: DockStyle.frameBorderWidth
    }

    onGroupCppChanged: {
        if (groupCpp) {
            groupCpp.setStackLayout(stackLayout);
        }
    }

    onNonContentsHeightChanged: {
        if (groupCpp)
            groupCpp.geometryUpdated();
    }

    // This KDDockWidgets build does not ship MDIResizeHandlerHelper.qml;
    // keep frame creation compatible by skipping those helper items.

    Loader {
        id: titleBar
        readonly property QtObject titleBarCpp: root.titleBarCpp
        source: groupCpp ? Singletons.widgetFactory.titleBarFilename()
                         : ""

        anchors {
            top:  parent ? parent.top : undefined
            left: parent ? parent.left : undefined
            right: parent ? parent.right : undefined
            topMargin: root.titleBarContentsMargin
            leftMargin: root.titleBarContentsMargin
            rightMargin: root.titleBarContentsMargin
        }
    }

    Connections {
        target: groupCpp
        function onCurrentIndexChanged() {
            tabbar.currentIndex = groupCpp.currentIndex;
        }
    }

//    MouseArea {
//        id: dragMouseArea
//        objectName: "kddwTabBarDragMouseArea"
//        hoverEnabled: true
//        anchors.fill: tabbar
//        enabled: tabbar.visible
//        z: 10
//    }

    TabBar {
        id: tabbar
        readonly property QtObject tabBarCpp: root.groupCpp ? root.groupCpp.tabWidget.tabBar
                                                            : null
        visible: count > 1
        height: visible ? DockStyle.tabBarHeight : 0

        background: Rectangle {
            color: DockStyle.tabBackground
            border.color: DockStyle.tabBorderColor
            border.width: 1
        }

        anchors {
            left: parent ? parent.left : undefined
            right: parent ? parent.right : undefined
            top: (titleBar && titleBar.visible) ? titleBar.bottom
                                                : (parent ? parent.top : undefined)
            topMargin: 0
            leftMargin: 1
            rightMargin: 1
        }

        width: parent ? parent.width : 0

        onCurrentIndexChanged: {
            if (root && root.groupCpp)
                root.groupCpp.tabWidget.setCurrentDockWidget(currentIndex);
        }

        onTabBarCppChanged: {
            if (tabBarCpp) {
//                if (!root.hasCustomMouseEventRedirector)
//                    tabBarCpp.redirectMouseEvents(dragMouseArea)

                // Setting just so the unit-tests can access the buttons
                tabBarCpp.tabBarQmlItem = this;
            }
        }

        Repeater {
            model: root.groupCpp ? root.groupCpp.tabWidget.dockWidgetModel : 0
            TabButton {
                property bool selected: tabbar.currentIndex == index
                background: Rectangle {
                    color: parent.selected ? DockStyle.tabActiveBackground : "transparent"
                    radius: 4
                    border.color: DockStyle.tabBorderColor
                    border.width: parent.selected ? 1 : 0
                }
                contentItem: Text {
                    text: title
                    font.weight: Font.Medium
                    color: selected ? DockStyle.tabActiveTextColor : DockStyle.tabTextColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }
        }
    }

    StackLayout {
        id: stackLayout
        anchors {
            left: parent ? parent.left : undefined
            right: parent ? parent.right : undefined
            top: (parent && tabbar.visible) ? tabbar.bottom : ((titleBar && titleBar.visible) ? titleBar.bottom
                                                                                              : parent ? parent.top : undefined)
            bottom: parent ? parent.bottom : undefined

            leftMargin: root.contentsMargin
            rightMargin: root.contentsMargin
            bottomMargin: root.contentsMargin
        }

        currentIndex: tabbar.currentIndex
    }
}

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CustomDW 1.0

Rectangle {
    id: root

    // Compatible with KDDockWidgets QtQuick title bar contract.
    readonly property QtObject titleBarCpp: parent ? parent.titleBarCpp : null
    readonly property string title: titleBarCpp ? titleBarCpp.title : ""
    readonly property bool floatButtonVisible: titleBarCpp ? titleBarCpp.floatButtonVisible : false
    readonly property bool closeButtonEnabled: titleBarCpp ? titleBarCpp.closeButtonEnabled : false
    readonly property bool maximizeButtonVisible: titleBarCpp ? titleBarCpp.maximizeButtonVisible : false
    readonly property bool minimizeButtonVisible: titleBarCpp ? titleBarCpp.minimizeButtonVisible : false
    readonly property bool maximizeUsesRestoreIcon: titleBarCpp ? titleBarCpp.maximizeUsesRestoreIcon : false
    readonly property bool isFocused: titleBarCpp ? titleBarCpp.isFocused : false
    readonly property bool showFloatWindowButton: floatButtonVisible
    readonly property bool showMaximizeWindowButton: !showFloatWindowButton && maximizeButtonVisible

    property int trafficButtonSize: 8
    property int trafficButtonHitSize: 16
    property int trafficSpacing: 4

    property int heightWhenVisible: Math.max(DockStyle.titleBarHeight, 28)

    signal closeButtonClicked()
    signal floatButtonClicked()
    signal maximizeButtonClicked()
    signal minimizeButtonClicked()

    visible: titleBarCpp && titleBarCpp.visible
    height: visible ? heightWhenVisible : 0
    implicitHeight: heightWhenVisible

    radius: DockStyle.titleBarRadius
    border.color: DockStyle.titleBarBorderColor
    border.width: 1
    color: "transparent"
    gradient: Gradient {
        GradientStop { position: 0.0; color: isFocused ? DockStyle.titleBarBackground : Qt.darker(DockStyle.titleBarBackground, 1.15) }
        GradientStop { position: 1.0; color: isFocused ? Qt.darker(DockStyle.titleBarBackground, 1.08) : Qt.darker(DockStyle.titleBarBackground, 1.25) }
    }

    MouseArea {
        id: titleBarDragMouseArea
        objectName: "titleBarMouseArea"
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onDoubleClicked: {
            if (root.titleBarCpp)
                root.titleBarCpp.onDoubleClicked()
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 10

        Item {
            id: leftButtons
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: root.trafficButtonHitSize * 3 + root.trafficSpacing * 2
            Layout.preferredHeight: root.trafficButtonHitSize

            Row {
                anchors.verticalCenter: parent.verticalCenter
                spacing: root.trafficSpacing

                ToolButton {
                    id: closeButton
                    objectName: "closeButton"
                    enabled: root.closeButtonEnabled
                    width: root.trafficButtonHitSize
                    height: root.trafficButtonHitSize
                    hoverEnabled: true
                    onClicked: root.closeButtonClicked()

                    contentItem: Item {
                        anchors.fill: parent
                        opacity: closeButton.hovered ? 0.82 : 0.0

                        Rectangle {
                            anchors.centerIn: parent
                            width: 6
                            height: 1
                            radius: 1
                            color: "#65131a"
                            rotation: 45
                        }
                        Rectangle {
                            anchors.centerIn: parent
                            width: 6
                            height: 1
                            radius: 1
                            color: "#65131a"
                            rotation: -45
                        }

                        Behavior on opacity { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
                    }

                    background: Rectangle {
                        implicitWidth: root.trafficButtonSize
                        implicitHeight: root.trafficButtonSize
                        anchors.centerIn: parent
                        antialiasing: true
                        radius: width / 2
                        border.width: 0.7
                        border.color: Qt.darker("#ff5f57", 1.12)
                        color: closeButton.down ? "#ea4e47" : (closeButton.hovered ? "#ff5f57" : "#ff6b63")
                        scale: closeButton.down ? 0.95 : 1.0

                        Rectangle {
                            anchors.fill: parent
                            antialiasing: true
                            radius: parent.radius
                            border.width: 0
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#2fffffff" }
                                GradientStop { position: 1.0; color: "#00000000" }
                            }
                        }

                        Behavior on color { ColorAnimation { duration: 140; easing.type: Easing.OutCubic } }
                        Behavior on scale { NumberAnimation { duration: 90; easing.type: Easing.OutQuad } }
                    }
                }

                ToolButton {
                    id: minimizeButton
                    objectName: "minimizeButton"
                    visible: true
                    enabled: root.minimizeButtonVisible
                    width: root.trafficButtonHitSize
                    height: root.trafficButtonHitSize
                    hoverEnabled: true
                    onClicked: root.minimizeButtonClicked()

                    contentItem: Item {
                        anchors.fill: parent
                        opacity: minimizeButton.hovered ? 0.82 : 0.0

                        Rectangle {
                            anchors.centerIn: parent
                            width: 6
                            height: 1
                            radius: 1
                            color: "#735516"
                        }

                        Behavior on opacity { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
                    }

                    background: Rectangle {
                        implicitWidth: root.trafficButtonSize
                        implicitHeight: root.trafficButtonSize
                        anchors.centerIn: parent
                        antialiasing: true
                        radius: width / 2
                        border.width: 0.7
                        border.color: Qt.darker("#ffbd2e", 1.14)
                        color: minimizeButton.down ? "#ebb02a" : (minimizeButton.hovered ? "#ffbd2e" : "#ffc84f")
                        scale: minimizeButton.down ? 0.95 : 1.0
                        opacity: minimizeButton.enabled ? 1.0 : 0.55

                        Rectangle {
                            anchors.fill: parent
                            antialiasing: true
                            radius: parent.radius
                            border.width: 0
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#30ffffff" }
                                GradientStop { position: 1.0; color: "#00000000" }
                            }
                        }

                        Behavior on color { ColorAnimation { duration: 140; easing.type: Easing.OutCubic } }
                        Behavior on scale { NumberAnimation { duration: 90; easing.type: Easing.OutQuad } }
                        Behavior on opacity { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }
                    }
                }

                ToolButton {
                    id: floatButton
                    objectName: "floatButton"
                    visible: root.showFloatWindowButton
                    enabled: root.floatButtonVisible
                    width: root.trafficButtonHitSize
                    height: root.trafficButtonHitSize
                    hoverEnabled: true
                    onClicked: root.floatButtonClicked()

                    contentItem: Item {
                        anchors.fill: parent
                        opacity: floatButton.hovered ? 0.82 : 0.0

                        Rectangle {
                            anchors.centerIn: parent
                            anchors.horizontalCenterOffset: -1
                            anchors.verticalCenterOffset: 1
                            width: 5
                            height: 4
                            color: "transparent"
                            border.width: 1
                            border.color: "#1c5f31"
                            radius: 1
                        }
                        Rectangle {
                            anchors.centerIn: parent
                            anchors.horizontalCenterOffset: 2
                            anchors.verticalCenterOffset: -2
                            width: 5
                            height: 4
                            color: "transparent"
                            border.width: 1
                            border.color: "#1c5f31"
                            radius: 1
                        }

                        Behavior on opacity { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
                    }

                    background: Rectangle {
                        implicitWidth: root.trafficButtonSize
                        implicitHeight: root.trafficButtonSize
                        anchors.centerIn: parent
                        antialiasing: true
                        radius: width / 2
                        border.width: 0.7
                        border.color: Qt.darker("#28c840", 1.14)
                        color: floatButton.down ? "#25b33d" : (floatButton.hovered ? "#28c840" : "#4fd764")
                        scale: floatButton.down ? 0.95 : 1.0

                        Rectangle {
                            anchors.fill: parent
                            antialiasing: true
                            radius: parent.radius
                            border.width: 0
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#2fffffff" }
                                GradientStop { position: 1.0; color: "#00000000" }
                            }
                        }

                        Behavior on color { ColorAnimation { duration: 140; easing.type: Easing.OutCubic } }
                        Behavior on scale { NumberAnimation { duration: 90; easing.type: Easing.OutQuad } }
                    }
                }

                ToolButton {
                    id: maximizeButton
                    objectName: "maximizeButton"
                    visible: root.showMaximizeWindowButton
                    enabled: root.maximizeButtonVisible
                    width: root.trafficButtonHitSize
                    height: root.trafficButtonHitSize
                    hoverEnabled: true
                    onClicked: root.maximizeButtonClicked()

                    contentItem: Item {
                        anchors.fill: parent
                        opacity: maximizeButton.hovered ? 0.82 : 0.0

                        Rectangle {
                            anchors.centerIn: parent
                            width: 6
                            height: 1
                            radius: 1
                            color: "#1c5f31"
                        }
                        Rectangle {
                            anchors.centerIn: parent
                            width: 1
                            height: 6
                            radius: 1
                            color: "#1c5f31"
                        }

                        Behavior on opacity { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
                    }

                    background: Rectangle {
                        implicitWidth: root.trafficButtonSize
                        implicitHeight: root.trafficButtonSize
                        anchors.centerIn: parent
                        antialiasing: true
                        radius: width / 2
                        border.width: 0.7
                        border.color: Qt.darker("#28c840", 1.14)
                        color: maximizeButton.down ? "#25b33d" : (maximizeButton.hovered ? "#28c840" : "#4fd764")
                        scale: maximizeButton.down ? 0.95 : 1.0

                        Rectangle {
                            anchors.fill: parent
                            antialiasing: true
                            radius: parent.radius
                            border.width: 0
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#2fffffff" }
                                GradientStop { position: 1.0; color: "#00000000" }
                            }
                        }

                        Behavior on color { ColorAnimation { duration: 140; easing.type: Easing.OutCubic } }
                        Behavior on scale { NumberAnimation { duration: 90; easing.type: Easing.OutQuad } }
                    }
                }
            }
        }

        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: root.title
            color: DockStyle.titleTextColor
            font.pixelSize: 13
            font.bold: true
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: leftButtons.Layout.preferredWidth
            Layout.preferredHeight: leftButtons.Layout.preferredHeight
        }
    }

    onTitleBarCppChanged: {
        if (titleBarCpp) {
            if (titleBarCpp.redirectMouseEvents)
                titleBarCpp.redirectMouseEvents(titleBarDragMouseArea)
            titleBarCpp.titleBarQmlItem = root
        }
    }

    onCloseButtonClicked: {
        if (titleBarCpp)
            titleBarCpp.onCloseClicked()
    }

    onFloatButtonClicked: {
        if (titleBarCpp)
            titleBarCpp.onFloatClicked()
    }

    onMaximizeButtonClicked: {
        if (titleBarCpp)
            titleBarCpp.onMaximizeClicked()
    }

    onMinimizeButtonClicked: {
        if (titleBarCpp)
            titleBarCpp.onMinimizeClicked()
    }
}

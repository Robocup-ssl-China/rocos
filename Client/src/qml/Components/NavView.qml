import QtQuick 2.15

Item {
    id: control
    clip: true

    property var items: ["Origin", "Filtered Blue", "Filtered Yellow"]
    property var itemWeights: []
    property int currentIndex: 0
    property int cellHeight: 38
    property int cellWidth: 800
    property int minCellWidth: 80
    property int tabPadding: 12
    property int tabSpacing: 4
    property int sidePadding: 6
    property int verticalPadding: 2
    property int indicatorWidth: 3
    property real indicatorRadius: 1.5
    property int panelRadius: 5
    property int itemRadius: 4
    property color panelColor: "#3d3d3d"
    property color hoverColor: "#4a4a4a"
    property color pressedColor: "#434343"
    property color activeColor: "#505050"
    property color normalColor: "transparent"
    property color textColor: "#d8d8d8"
    property color activeTextColor: "#ffffff"
    property color accentColor: "#00a6ff"
    property color borderColor: "#2d2d2d"

    signal itemClicked(int index, string title)

    implicitHeight: cellHeight + verticalPadding * 2
    implicitWidth: cellWidth

    readonly property int tabCount: Math.max(1, items ? items.length : 0)
    readonly property int availableTabWidth: Math.max(0,
        width - sidePadding * 2 - tabSpacing * (tabCount - 1))
    readonly property real totalWeight: {
        if (!items || items.length <= 0) {
            return 1
        }
        var sum = 0
        for (var i = 0; i < items.length; i++) {
            sum += weightAt(i)
        }
        return Math.max(1, sum)
    }
    readonly property bool canApplyMinWidth: availableTabWidth >= minCellWidth * tabCount

    function itemTitleAt(index) {
        if (!items || index < 0 || index >= items.length) {
            return ""
        }
        var item = items[index]
        if (typeof item === "string") {
            return item
        }
        if (item && item.title !== undefined) {
            return item.title
        }
        return String(item)
    }

    function weightAt(index) {
        var weight = 1
        if (itemWeights && index >= 0 && index < itemWeights.length) {
            weight = Number(itemWeights[index])
        } else if (items && index >= 0 && index < items.length) {
            var item = items[index]
            if (item && item.weight !== undefined) {
                weight = Number(item.weight)
            }
        }
        if (isNaN(weight) || weight <= 0) {
            return 1
        }
        return weight
    }

    function tabWidthAt(index) {
        var weightedWidth = Math.floor(availableTabWidth * weightAt(index) / totalWeight)
        if (canApplyMinWidth) {
            return Math.max(minCellWidth, weightedWidth)
        }
        return Math.max(0, weightedWidth)
    }

    function tabXAt(index) {
        if (!items || index < 0 || index >= items.length) {
            return sidePadding
        }
        var x = sidePadding
        for (var i = 0; i < index; i++) {
            x += tabWidthAt(i) + tabSpacing
        }
        return x
    }

    function safeIndex(index) {
        if (!items || items.length <= 0) {
            return -1
        }
        return Math.max(0, Math.min(index, items.length - 1))
    }

    onCurrentIndexChanged: {
        var idx = safeIndex(currentIndex)
        if (idx !== currentIndex) {
            currentIndex = idx
            return
        }
        switchPulse.restart()
    }

    onItemsChanged: {
        var idx = safeIndex(currentIndex)
        if (idx !== currentIndex) {
            currentIndex = idx
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: control.panelRadius
        color: control.panelColor
        border.color: control.borderColor
        border.width: 1
    }

    Rectangle {
        id: activeBackground
        visible: control.currentIndex >= 0 && control.items && control.items.length > 0
        x: control.tabXAt(control.currentIndex)
        y: control.verticalPadding
        width: control.tabWidthAt(control.currentIndex)
        height: control.cellHeight - control.verticalPadding * 2
        radius: control.itemRadius
        color: control.activeColor
        transformOrigin: Item.Center

        Behavior on x {
            NumberAnimation {
                duration: 180
                easing.type: Easing.OutCubic
            }
        }

        Behavior on width {
            NumberAnimation {
                duration: 180
                easing.type: Easing.OutCubic
            }
        }
    }

    Rectangle {
        id: activeIndicator
        visible: activeBackground.visible
        width: control.indicatorWidth
        height: 18
        radius: control.indicatorRadius
        color: control.accentColor
        x: activeBackground.x + 6
        y: activeBackground.y + (activeBackground.height - height) / 2

        Behavior on x {
            NumberAnimation {
                duration: 90
                easing.type: Easing.OutCubic
            }
        }
    }

    SequentialAnimation {
        id: switchPulse
        NumberAnimation {
            target: activeBackground
            property: "scale"
            to: 1.02
            duration: 60
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            target: activeBackground
            property: "scale"
            to: 1.0
            duration: 120
            easing.type: Easing.OutCubic
        }
    }

    Row {
        id: tabsRow
        anchors.left: parent.left
        anchors.leftMargin: control.sidePadding
        anchors.verticalCenter: parent.verticalCenter
        spacing: control.tabSpacing

        Repeater {
            model: control.items

            delegate: Item {
                width: control.tabWidthAt(index)
                height: control.cellHeight

                Rectangle {
                    id: itemFace
                    anchors.fill: parent
                    anchors.topMargin: control.verticalPadding
                    anchors.bottomMargin: control.verticalPadding
                    radius: control.itemRadius
                    color: {
                        if (index === control.currentIndex) {
                            return "transparent"
                        }
                        if (mouseArea.pressed) {
                            return control.pressedColor
                        }
                        if (mouseArea.containsMouse) {
                            return control.hoverColor
                        }
                        return control.normalColor
                    }
                    scale: mouseArea.pressed ? 0.98 : 1.0

                    Behavior on color {
                        ColorAnimation {
                            duration: 110
                        }
                    }

                    Behavior on scale {
                        NumberAnimation {
                            duration: 70
                            easing.type: Easing.OutCubic
                        }
                    }
                }

                Text {
                    id: titleText
                    anchors.verticalCenter: itemFace.verticalCenter
                    anchors.left: itemFace.left
                    anchors.leftMargin: index === control.currentIndex ? 16 : 12
                    anchors.right: itemFace.right
                    anchors.rightMargin: 12
                    text: control.itemTitleAt(index)
                    color: index === control.currentIndex ? control.activeTextColor : control.textColor
                    font.pixelSize: 13
                    font.bold: false
                    horizontalAlignment: Text.AlignLeft
                    elide: Text.ElideRight

                    Behavior on color {
                        ColorAnimation {
                            duration: 140
                        }
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        control.currentIndex = index
                        control.itemClicked(index, control.itemTitleAt(index))
                    }
                }

                Behavior on x {
                    NumberAnimation {
                        duration: 167
                        easing.type: Easing.OutCubic
                    }
                }
            }
        }
    }
}
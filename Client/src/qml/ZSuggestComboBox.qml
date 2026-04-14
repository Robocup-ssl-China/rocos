import QtQuick 2.10
import QtQuick.Controls 2.15

Item {
    id: root

    property var model: []
    property int currentIndex: -1
    readonly property string currentText: (currentIndex >= 0 && currentIndex < model.length) ? String(model[currentIndex]) : ""
    property string placeholderText: "Type to search"
    property int marqueePauseDuration: 1200
    property int marqueeEndPauseDuration: 800
    property int marqueeReturnDuration: 250
    property int marqueeSpeedFactor: 12
    property int highlightedFilteredIndex: -1

    signal activated(int index)

    implicitWidth: parent.buttonWidth || parent.itemWidth || 20
    implicitHeight: parent.buttonHeight || parent.itemHeight || 40

    function updateTextByCurrentIndex() {
        if (currentIndex >= 0 && currentIndex < model.length) {
            input.text = String(model[currentIndex])
        } else {
            input.text = ""
            currentIndex = -1
        }
    }

    function refreshFilteredModel() {
        filteredModel.clear()
        var keyword = input.text.toLowerCase()
        if (!input.activeFocus || keyword.length === 0) {
            highlightedFilteredIndex = -1
            popup.close()
            return
        }
        for (var i = 0; i < model.length; ++i) {
            var text = String(model[i])
            if (text.toLowerCase().indexOf(keyword) !== -1) {
                filteredModel.append({"text": text, "sourceIndex": i})
            }
        }
        if (filteredModel.count > 0) {
            highlightedFilteredIndex = 0
            popup.open()
        } else {
            highlightedFilteredIndex = -1
            popup.close()
        }
    }

    function moveHighlight(step) {
        if (filteredModel.count <= 0) return
        if (!popup.visible) popup.open()
        if (highlightedFilteredIndex < 0) {
            highlightedFilteredIndex = 0
            return
        }
        highlightedFilteredIndex = (highlightedFilteredIndex + step + filteredModel.count) % filteredModel.count
    }

    function confirmSelection() {
        if (filteredModel.count <= 0) return
        var idx = highlightedFilteredIndex >= 0 ? highlightedFilteredIndex : 0
        var item = filteredModel.get(idx)
        root.selectItem(item.sourceIndex, item.text)
    }

    function selectItem(sourceIndex, text) {
        currentIndex = sourceIndex
        input.text = text
        popup.close()
        input.focus = false
        activated(sourceIndex)
    }

    onModelChanged: {
        if (currentIndex >= model.length) {
            currentIndex = model.length > 0 ? 0 : -1
        }
        updateTextByCurrentIndex()
        refreshFilteredModel()
    }

    onCurrentIndexChanged: {
        updateTextByCurrentIndex()
    }

    TextField {
        id: input
        anchors.fill: parent
        placeholderText: root.placeholderText
        topPadding: 0
        bottomPadding: 0
        leftPadding: 8
        rightPadding: 8
        verticalAlignment: TextInput.AlignVCenter
        color: marqueeOverlay.visible ? "transparent" : "#ffffff"
        placeholderTextColor: "#ffffff"
        selectedTextColor: "#ffffff"
        selectionColor: "#279BE6"
        background: Rectangle {
            radius: 2
            color: input.enabled ? "#3a3a3a" : "#2f2f2f"
            border.width: 0
            border.color: "transparent"
        }

        Keys.onDownPressed: function(event) {
            root.moveHighlight(1)
            event.accepted = true
        }

        Keys.onUpPressed: function(event) {
            root.moveHighlight(-1)
            event.accepted = true
        }

        Keys.onReturnPressed: function(event) {
            root.confirmSelection()
            event.accepted = true
        }

        Keys.onEnterPressed: function(event) {
            root.confirmSelection()
            event.accepted = true
        }

        onTextEdited: {
            refreshFilteredModel()
        }

        onActiveFocusChanged: {
            if (!activeFocus) {
                popup.close()
            } else {
                refreshFilteredModel()
            }
        }
    }

    Rectangle {
        id: marqueeOverlay
        anchors {
            left: input.left
            right: input.right
            top: input.top
            bottom: input.bottom
            leftMargin: input.leftPadding
            rightMargin: input.rightPadding
            topMargin: 1
            bottomMargin: 1
        }
        visible: !input.activeFocus
                 && input.text.length > 0
                 && marqueeText.implicitWidth > (width - 4)
        clip: true
        color: "transparent"

        Text {
            id: marqueeText
            text: input.text
            color: "#ffffff"
            font: input.font
            y: (parent.height - implicitHeight) / 2
            x: 0

            SequentialAnimation on x {
                running: marqueeOverlay.visible
                loops: Animation.Infinite

                PauseAnimation { duration: root.marqueePauseDuration }

                NumberAnimation {
                    to: -(marqueeText.implicitWidth - marqueeOverlay.width + 8)
                    duration: Math.max(root.marqueePauseDuration,
                                       (marqueeText.implicitWidth - marqueeOverlay.width + 8) * root.marqueeSpeedFactor)
                    easing.type: Easing.Linear
                }

                PauseAnimation { duration: root.marqueeEndPauseDuration }

                NumberAnimation {
                    to: 0
                    duration: root.marqueeReturnDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.IBeamCursor
            onClicked: input.forceActiveFocus()
        }
    }

    ListModel {
        id: filteredModel
    }

    Popup {
        id: popup
        width: root.width
        y: root.height
        padding: 0
        focus: false

        contentItem: ListView {
            id: listView
            model: filteredModel
            clip: true
            implicitHeight: Math.min(contentHeight, 240)
            currentIndex: root.highlightedFilteredIndex

            delegate: ItemDelegate {
                width: listView.width
                highlighted: index === root.highlightedFilteredIndex
                contentItem: Item {
                    clip: true

                    Text {
                        id: delegateText
                        text: model.text
                        color: "#ffffff"
                        font: input.font
                        y: (parent.height - implicitHeight) / 2
                        x: 0

                        SequentialAnimation on x {
                            running: marqueeMask.visible
                            loops: Animation.Infinite

                            PauseAnimation { duration: root.marqueePauseDuration }

                            NumberAnimation {
                                to: -(delegateText.implicitWidth - marqueeMask.width + 8)
                                duration: Math.max(root.marqueePauseDuration,
                                                   (delegateText.implicitWidth - marqueeMask.width + 8) * root.marqueeSpeedFactor)
                                easing.type: Easing.Linear
                            }

                            PauseAnimation { duration: root.marqueeEndPauseDuration }

                            NumberAnimation {
                                to: 0
                                duration: root.marqueeReturnDuration
                                easing.type: Easing.InOutQuad
                            }
                        }
                    }

                    Item {
                        id: marqueeMask
                        anchors.fill: parent
                        visible: delegateText.implicitWidth > width
                    }
                }
                onClicked: {
                    root.highlightedFilteredIndex = index
                    root.selectItem(model.sourceIndex, model.text)
                }
            }
        }
    }
}

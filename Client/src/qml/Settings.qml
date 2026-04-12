import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ZSS 1.0 as ZSS

Item {
    id: root

    readonly property int roleSettingName: 257
    readonly property int roleSettingType: 258
    readonly property int roleSettingValue: 259

    ZSS.ParamModel {
        id: paramModel
    }

    function groupIndexAt(row) {
        return paramModel.index(row, 0)
    }

    function childIndexAt(row) {
        if (groupList.currentIndex < 0)
            return null
        return paramModel.index(row, 0, groupIndexAt(groupList.currentIndex))
    }

    function childDataAt(row, role) {
        const idx = childIndexAt(row)
        if (!idx)
            return ""
        return paramModel.data(idx, role)
    }

    function childCount() {
        if (groupList.currentIndex < 0)
            return 0
        return paramModel.rowCount(groupIndexAt(groupList.currentIndex))
    }

    Shortcut {
        sequence: "r"
        onActivated: {
            paramModel.reload()
            if (groupList.currentIndex < 0 && paramModel.rowCount() > 0)
                groupList.currentIndex = 0
        }
    }

    Component.onCompleted: {
        if (paramModel.rowCount() > 0)
            groupList.currentIndex = 0
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.28
            color: "#404040"

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "#333333"

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        text: "Group"
                        color: "#ffffff"
                        font.pixelSize: 15
                    }
                }

                ListView {
                    id: groupList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: paramModel

                    delegate: Rectangle {
                        width: groupList.width
                        height: 34
                        color: ListView.isCurrentItem ? "#2d6a9f" : "#404040"

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 12
                            text: settingName
                            color: "#e8e8e8"
                            font.pixelSize: 14
                            elide: Text.ElideRight
                            width: parent.width - 20
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: groupList.currentIndex = index
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#484848"

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "#333333"

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Text {
                            Layout.preferredWidth: parent.width * 0.4
                            Layout.fillHeight: true
                            leftPadding: 12
                            verticalAlignment: Text.AlignVCenter
                            text: "Name"
                            color: "#ffffff"
                            font.pixelSize: 15
                        }
                        Text {
                            Layout.preferredWidth: parent.width * 0.2
                            Layout.fillHeight: true
                            leftPadding: 12
                            verticalAlignment: Text.AlignVCenter
                            text: "Type"
                            color: "#ffffff"
                            font.pixelSize: 15
                        }
                        Text {
                            Layout.preferredWidth: parent.width * 0.4
                            Layout.fillHeight: true
                            leftPadding: 12
                            verticalAlignment: Text.AlignVCenter
                            text: "Value"
                            color: "#ffffff"
                            font.pixelSize: 15
                        }
                    }
                }

                ListView {
                    id: valueList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: childCount()

                    delegate: Rectangle {
                        required property int index

                        width: valueList.width
                        height: 34
                        color: index % 2 === 0 ? "#484848" : "#404040"

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

                            Text {
                                Layout.preferredWidth: parent.width * 0.4
                                Layout.fillHeight: true
                                leftPadding: 12
                                verticalAlignment: Text.AlignVCenter
                                text: root.childDataAt(index, root.roleSettingName)
                                color: "#d6d6d6"
                                elide: Text.ElideRight
                                font.pixelSize: 14
                            }

                            Text {
                                Layout.preferredWidth: parent.width * 0.2
                                Layout.fillHeight: true
                                leftPadding: 12
                                verticalAlignment: Text.AlignVCenter
                                text: root.childDataAt(index, root.roleSettingType)
                                color: "#d6d6d6"
                                elide: Text.ElideRight
                                font.pixelSize: 14
                            }

                            Loader {
                                id: editorLoader
                                property int rowIndex: index
                                Layout.preferredWidth: parent.width * 0.4
                                Layout.fillHeight: true
                                sourceComponent: root.childDataAt(index, root.roleSettingType) === "Bool" ? boolEditor : textEditor
                                onLoaded: {
                                    if (item)
                                        item.rowIndex = rowIndex
                                }
                                onRowIndexChanged: {
                                    if (item)
                                        item.rowIndex = rowIndex
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: textEditor
        TextField {
            property int rowIndex: -1

            text: root.childDataAt(rowIndex, root.roleSettingValue)
            color: "#e8e8e8"
            leftPadding: 12
            background: Rectangle {
                color: parent.activeFocus ? "#5a5a5a" : "transparent"
                border.color: parent.activeFocus ? "#8aaed1" : "transparent"
                border.width: 1
            }
            onAccepted: {
                const idx = root.childIndexAt(rowIndex)
                if (idx)
                    paramModel.setData(idx, text)
            }
            onEditingFinished: {
                const idx = root.childIndexAt(rowIndex)
                if (idx)
                    paramModel.setData(idx, text)
            }
        }
    }

    Component {
        id: boolEditor
        CheckBox {
            property int rowIndex: -1

            leftPadding: 12
            checked: root.childDataAt(rowIndex, root.roleSettingValue) === "true"
            onClicked: {
                const idx = root.childIndexAt(rowIndex)
                if (idx)
                    paramModel.setData(idx, checked ? "true" : "false")
            }
        }
    }
}

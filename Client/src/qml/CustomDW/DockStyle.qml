pragma Singleton
import QtQuick 2.15

QtObject {
    id: root

    // A lightweight qss-like input. Example:
    // "title-bg:#1f2733; title-fg:#eef3ff; frame-radius:8; tab-active-bg:#2f7dff"
    property string styleSheet: ""

    property color titleBarBackground: "#1f2733"
    property color titleBarBorderColor: "#2b3646"
    property color titleTextColor: "#eef3ff"
    property color buttonTextColor: "#d8e1f0"
    property color buttonHoverBackground: "#314054"
    property color buttonPressedBackground: "#223044"

    property color frameBackground: "#0f141c"
    property color frameBorderColor: "#2b3646"
    property int frameBorderWidth: 1
    property int frameRadius: 8

    property color dockWidgetBackground: frameBackground

    property color tabBackground: "#16202c"
    property color tabBorderColor: "#253141"
    property color tabTextColor: "#8ea0b8"
    property color tabActiveBackground: "#2f7dff"
    property color tabActiveTextColor: "#ffffff"

    property color floatingBorderColor: "#2b3646"
    property int floatingBorderWidth: 1
    property int floatingMargins: 4

    property int titleBarHeight: 36
    property int titleBarRadius: 8
    property int tabBarHeight: 34
    property int titleHorizontalPadding: 12
    property int buttonSize: 22

    onStyleSheetChanged: applyStyleSheet(styleSheet)

    function toNumberOrDefault(value, fallback) {
        var n = Number(value)
        return isNaN(n) ? fallback : n
    }

    function applyStyleSheet(sheet) {
        if (!sheet || sheet.trim().length === 0)
            return

        var entries = sheet.split(";")
        for (var i = 0; i < entries.length; ++i) {
            var line = entries[i].trim()
            if (!line)
                continue

            var sep = line.indexOf(":")
            if (sep <= 0)
                continue

            var key = line.slice(0, sep).trim().toLowerCase()
            var value = line.slice(sep + 1).trim()
            if (!value)
                continue

            switch (key) {
            case "title-bg":
                titleBarBackground = value
                break
            case "title-border":
                titleBarBorderColor = value
                break
            case "title-fg":
                titleTextColor = value
                break
            case "btn-fg":
                buttonTextColor = value
                break
            case "btn-hover-bg":
                buttonHoverBackground = value
                break
            case "btn-pressed-bg":
                buttonPressedBackground = value
                break
            case "frame-bg":
                frameBackground = value
                break
            case "frame-border":
                frameBorderColor = value
                break
            case "frame-border-width":
                frameBorderWidth = toNumberOrDefault(value, frameBorderWidth)
                break
            case "frame-radius":
                frameRadius = toNumberOrDefault(value, frameRadius)
                break
            case "dock-bg":
                dockWidgetBackground = value
                break
            case "tab-bg":
                tabBackground = value
                break
            case "tab-border":
                tabBorderColor = value
                break
            case "tab-fg":
                tabTextColor = value
                break
            case "tab-active-bg":
                tabActiveBackground = value
                break
            case "tab-active-fg":
                tabActiveTextColor = value
                break
            case "floating-border":
                floatingBorderColor = value
                break
            case "floating-border-width":
                floatingBorderWidth = toNumberOrDefault(value, floatingBorderWidth)
                break
            case "floating-margins":
                floatingMargins = toNumberOrDefault(value, floatingMargins)
                break
            case "title-height":
                titleBarHeight = toNumberOrDefault(value, titleBarHeight)
                break
            case "title-radius":
                titleBarRadius = toNumberOrDefault(value, titleBarRadius)
                break
            case "tab-height":
                tabBarHeight = toNumberOrDefault(value, tabBarHeight)
                break
            case "title-padding":
                titleHorizontalPadding = toNumberOrDefault(value, titleHorizontalPadding)
                break
            case "btn-size":
                buttonSize = toNumberOrDefault(value, buttonSize)
                break
            default:
                break
            }
        }
    }
}
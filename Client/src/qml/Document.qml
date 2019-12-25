import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import ZSSDocument 1.0

Item {
    visible: true
    width: parent.width
    height: 400
    Shortcut {
        id: copyAction
        sequence: "ctrl+shift+c"
        onActivated: {
            console.log("Copied!");
            textArea.copy()
        }
        enabled: textArea.focus;
    }
    TextArea {
        readOnly: true;
        id: textArea
        frameVisible: false
        anchors.fill: parent;
        text: "";
        style: TextAreaStyle {
            textColor: "#aaa"
            selectionColor: "steelblue"
            selectedTextColor: "#eee"
            backgroundColor: "#222"
            font.pixelSize: 16
        }
    }

    DocumentHandler {
        id: document
        onClearText: {
            textArea.text = "";
        }
        onAddText: {
            textArea.append(str);
        }
    }
}

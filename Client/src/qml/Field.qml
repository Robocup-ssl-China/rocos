import QtQuick
import ZSS 1.0 as ZSS

Item{
    property alias type: field.type;
    property alias draw: field.draw;
    anchors.fill: parent;
    ZSS.Field{
        id: field;
        type:0;
        draw:true;
        anchors.fill: parent;
    }
    onWidthChanged: {
        console.log("Field.qml width changed", width, height);
        // field.resize(width, height);
    }
    onHeightChanged: {
        console.log("Field.qml height changed", width, height);
        // field.resize(width, height);
    }
}
import QtQuick 2.15

Column{
    property double contentWidth:width - leftPadding - rightPadding
    id: col
    padding: 5
    spacing: 5
    width:parent.width
}

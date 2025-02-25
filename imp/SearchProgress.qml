import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQml.Models 2.2

Item
{
    id: itFindProgress
    height: 80
    width: 500

    ImpStyle
    {
        id: impStyle
    }

    ProgressBar
    {
        id: pbFind
        objectName: "pbFind"
        anchors.fill: parent
        value: 0 // from 0 to 1
        background: Rectangle
        {
            anchors.fill: parent
            color: "lightgray"
        }
        contentItem: Item
        {
            anchors.fill: parent
            anchors.leftMargin: height * 0.05
            anchors.rightMargin: anchors.leftMargin
            Rectangle
            {
                anchors.verticalCenter: parent.verticalCenter
                width: pbFind.visualPosition * parent.width
                height: parent.height * 0.9
                radius: height / 16
                color: impStyle.chekedColor
            }
            Text {
                id: textComment
                objectName: "textComment"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: parent.width / 20
                text: " "
                font.pixelSize: parent.height / 2
            }
            Text {
                id: textPercent
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: textComment.anchors.leftMargin
                text: String(Math.round(pbFind.value * 100)) + "%"
                font.pixelSize: textComment.font.pixelSize
                visible: ((pbFind.value === 1) | (pbFind.value === 0)) ? false : true
            }
        }
    }
}


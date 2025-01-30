import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQml.Models 2.2

Item {

    ImpStyle
    {
        id: impStyle
    }

    ProgressBar
    {
        id: pbFind
        objectName: "pbFind"
        height: 30
        width: 210
        value: 0 // from 0 to 1
        background: Rectangle
        {
            anchors.fill: parent
            color: "lightgray"
        }
        contentItem: Item
        {
            anchors.fill: parent
            Rectangle
            {
                anchors.verticalCenter: parent.verticalCenter
                width: pbFind.visualPosition * parent.width
                height: parent.height * 0.8

                radius: height/4
                color: impStyle.chekedColor
            }
            Text {
                id: textComment
                objectName: "textComment"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                text: " "
            }
            Text {
                id: textPercent
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                text: String(Math.round(pbFind.value * 100)) + "%  "
                visible: ((pbFind.value === 1) | (pbFind.value === 0)) ? false : true
            }
        }
    }
}


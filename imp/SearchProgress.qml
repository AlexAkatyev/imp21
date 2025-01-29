import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQml.Models 2.2

Item {

    ProgressBar
    {
        id: pbFind
        objectName: "pbFind"
        height: 30
        width: 210
        value: 0 // from 0 to 1
        background: Rectangle
        {
            width: pbFind.width
            height: pbFind.height
            color: "lightgray"
        }
        contentItem: Item
        {
            width: parent.width
            height: parent.height
            Rectangle
            {
                anchors.verticalCenter: parent.verticalCenter
                width: pbFind.visualPosition * parent.width
                height: parent.height * 0.8

                radius: height/4
                color: "green"
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


}

import QtQuick 2.5
import QtQuick.Controls 2.1


Item
{
    property double scaleX: 320
    onScaleXChanged: width = scaleX;
    property double scaleY: 480
    onScaleYChanged: height = scaleY;
    width: scaleX
    height: scaleY

    property int limitedMeasPoint: 0;
    property int currentPoint: 0;

    onLimitedMeasPointChanged:
    {
        tfRange.text = limitedMeasPoint;
    }

    Button // Кнопка УСТАНОВКИ
    {
        id: btSet
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width/2
        height: parent.height/15 - 1
        text: "Установки"
        background: Rectangle{
            color: itSet.visible ? "#b5e7e7" : "#e7e7e7"
        }
        onClicked:
        {
            // Выбор рабочего окна
            itSet.visible = true;
            itAb.visible = false;
        }
    }

    Button // Кнопка О ДАТЧИКЕ
    {
        id: btAb
        anchors.left: btSet.right
        anchors.right: parent.right
        anchors.top: parent.top
        height: btSet.height
        text: "О датчике"
        background: Rectangle{
            color: itAb.visible ? "#b5e7e7" : "#e7e7e7"
        }
        onClicked:
        {
            // Выбор рабочего окна
            itSet.visible = false;
            itAb.visible = true;
        }
    }

    Item { // Окно установок +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        id: itSet
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: btSet.bottom
        anchors.bottom: parent.bottom
        visible: true
        Grid {
            anchors.fill: parent
            anchors.margins: 10
            columns: 2
            rows: 4
            spacing: 10
            Text {
                text: "Единица измерения"
            }
            Text { // Вводится единица измерения
                id: tfUM
                objectName: "tfUM"
            }
            Text {
                text: "Диапазон измерения"
            }
            Text { // Вводится диапазон показаний
                id: tfRange
                objectName: "tfRange"
            }
            Text {
                text: "Диапазон обнуления"
            }
            Text { // Вводится диапазон показаний
                id: tfSetZero
                objectName: "tfSetZero"
            }
            Text {
                text: "Диапазон предустанова"
            }
            Text { // Вводится диапазон показаний
                id: tfPreSet
                objectName: "tfPreSet"
            }
        }

    }

    Item { // Окно информации о датчике ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        id: itAb
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: btSet.bottom
        anchors.bottom: parent.bottom
        visible: false
        Text {
            id: txLeft
            x: parent.width/20
            y: 2*parent.height/40
            text: "Серийный номер"
        }
        Text {
            id: txtSerialNum
            objectName: "txtSerialNum"
            x: parent.width/2
            y: txLeft.y
        }
        Text {
            x: txLeft.x
            y: 6*parent.height/40
            text: "Тип датчика"
        }
        Text {
            id: txtType
            objectName: "txtType"
            x: txLeft.x
            y: 8*parent.height/40
        }
        Text {
            x: txLeft.x
            y: 12*parent.height/40
            text: "Дата выпуска платы"
        }
        Text {
            id: txtData
            objectName: "txtData"
            x: parent.width/2
            y: 12*parent.height/40
        }
        Text {
            x: txLeft.x
            y: 16*parent.height/40
            text: "Подключение:"
        }
        Text {
            id: txtConnect
            objectName: "txtConnect"
            x: parent.width/2
            y: 16*parent.height/40
        }
        Image {
            id: imLogo
            objectName: "imLogo"
            source: "qrc:/logo.bmp"
            x: 3*parent.width/24
            y: parent.height/2
            width: 18*parent.width/24
            height: parent.height/2
        }
    }
}


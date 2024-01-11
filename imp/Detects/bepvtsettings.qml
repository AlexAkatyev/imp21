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

    signal saveUserName();

    property int limitedMeasPoint: 0;
    property int currentPoint: 0;

    onLimitedMeasPointChanged:
    {
        tfRange.text = limitedMeasPoint;
    }

    function getAcceptedInput(input)
    {
        var result = input;
        currentPoint = input;
        limitedMeasPoint = tfRange.text;
        if(currentPoint > limitedMeasPoint)
            result = limitedMeasPoint;
        if(currentPoint < -limitedMeasPoint)
            result = -limitedMeasPoint;
        return result;
    }

    Button // Кнопка УСТАНОВКИ
    {
        id: btSet
        x: 1
        y: 0
        width: parent.width/3 - 2
        height: parent.height/15 - 1
        text: "Установки"
        background: Rectangle{
            color: itSet.visible ? "#b5e7e7" : "#e7e7e7"
        }
        onClicked:
        {
            // Выбор рабочего окна
            itSet.visible = true;
            itCalib.visible = false;
            itAb.visible = false;
        }
    }

    Button // Кнопка КАЛИБРОВКА
    {
        id: btCalib
        x: parent.width/3 + 1
        y: 0
        width: parent.width/3 - 2
        height: parent.height/15 - 1
        text: "Калибровка"
        background: Rectangle{
            color: itCalib.visible ? "#b5e7e7" : "#e7e7e7"
        }
        onClicked:
        {
            // Выбор рабочего окна
            itSet.visible = false;
            itCalib.visible = true;
            itAb.visible = false;
        }
    }

    Button // Кнопка О ДАТЧИКЕ
    {
        id: btAb
        x: 2*parent.width/3 + 1
        y: 0
        width: parent.width/3 - 2
        height: parent.height/15 - 1
        text: "О датчике"
        background: Rectangle{
            color: itAb.visible ? "#b5e7e7" : "#e7e7e7"
        }
        onClicked:
        {
            // Выбор рабочего окна
            itSet.visible = false;
            itCalib.visible = false;
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
            rows: 8
            spacing: 10
            Text {
                text: "Наименование"
            }
            TextField { // Вводится имя датчика
                id: tfName
                objectName: "tfName"
                font.pixelSize: 11
            }
            Text {
                text: "        "
            }
            Button {
                text:  "Записать имя датчика"
                font.pixelSize: 11
                onClicked: saveUserName()
            }
            Text {
                text: "Единица измерения"
            }
            Text { // Вводится единица измерения
                id: tfUM
                objectName: "tfUM"
            }
            Text {
                text: "Число измерительных\nпериодов"
                visible: false
            }
            Text { // Вводится количество измерительных периодов
                id: tfCP
                objectName: "tfCP"
                visible: false
            }
            Text {
                text: "Диапазон измерения"
                visible: false
            }
            Text { // Вводится диапазон показаний
                id: tfRange
                objectName: "tfRange"
                visible: false
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
            Text {
                id: tTxtModbusAddress
                objectName: "tTxtModbusAddress"
                text: "Modbus адрес"
                visible: tModbusAddress.visible
            }
            Text { // Вводится адрес по протоколу MODBUS, если датчик имеет выход RS485
                id: tModbusAddress
                objectName: "tModbusAddress"
                visible: tModbusAddress.text != ""
            }
        }

    }
    Item { // Окно калибровки +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        id: itCalib
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: btSet.bottom
        anchors.bottom: parent.bottom
        visible: false
        Text {
            id: headerTochka
            x: 9*parent.width/100
            y: 0
            text: "Калибровочная\n      точка"
            font.pixelSize: 11
        }
        Text {
            x: parent.width/2
            y: parent.height/80
            text: "Отсчеты датчика"
        }
        Item { // калибровочная таблица
            id: calibrTable
            anchors.top: headerTochka.bottom
            anchors.left: parent.left
            anchors.leftMargin: parent.width/20
            anchors.right: parent.right
            anchors.rightMargin: parent.width/20
            anchors.bottom: txtCurrent.top
            Text {
                id: txN10
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: parent.height/20
                text: "10"
                font.pixelSize: 13
            }
            Text { // Отсчет замера 10
                id: tfMP10
                objectName: "tfMP10"
                anchors.left: parent.left
                anchors.leftMargin: 15*parent.width/100
                anchors.top: parent.top
                anchors.topMargin: txN10.anchors.topMargin
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 10
                id: tfP10
                objectName: "tfP10"
                anchors.left: parent.left
                anchors.leftMargin: parent.width/2
                anchors.top: parent.top
                anchors.topMargin: txN10.anchors.topMargin
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN9
                anchors.left: parent.left
                anchors.top: txN10.bottom
                text: " 9"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 10
                id: tfMP9
                objectName: "tfMP9"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN9.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 10
                id: tfP9
                objectName: "tfP9"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN9.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN8
                anchors.left: parent.left
                anchors.top: txN9.bottom
                text: " 8"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 8
                id: tfMP8
                objectName: "tfMP8"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN8.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 8
                id: tfP8
                objectName: "tfP8"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN8.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN7
                anchors.left: parent.left
                anchors.top: txN8.bottom
                text: " 7"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 7
                id: tfMP7
                objectName: "tfMP7"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN7.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 7
                id: tfP7
                objectName: "tfP7"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN7.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN6
                anchors.left: parent.left
                anchors.top: txN7.bottom
                text: " 6"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 6
                id: tfMP6
                objectName: "tfMP6"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN6.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 6
                id: tfP6
                objectName: "tfP6"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN6.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN5
                anchors.left: parent.left
                anchors.top: txN6.bottom
                text: " 5"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 5
                id: tfMP5
                objectName: "tfMP5"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN5.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 5
                id: tfP5
                objectName: "tfP5"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN5.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN4
                anchors.left: parent.left
                anchors.top: txN5.bottom
                text: " 4"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 4
                id: tfMP4
                objectName: "tfMP4"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN4.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 4
                id: tfP4
                objectName: "tfP4"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN4.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN3
                anchors.left: parent.left
                anchors.top: txN4.bottom
                text: " 3"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 3
                id: tfMP3
                objectName: "tfMP3"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN3.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 3
                id: tfP3
                objectName: "tfP3"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN3.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN2
                anchors.left: parent.left
                anchors.top: txN3.bottom
                text: " 2"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 2
                id: tfMP2
                objectName: "tfMP2"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN2.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 2
                id: tfP2
                objectName: "tfP2"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN2.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN1
                anchors.left: parent.left
                anchors.top: txN2.bottom
                text: " 1"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 1
                id: tfMP1
                objectName: "tfMP1"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN1.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 1
                id: tfP1
                objectName: "tfP1"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN1.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txN0
                anchors.left: parent.left
                anchors.top: txN1.bottom
                text: " 0"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера 1
                id: tfMZERO
                objectName: "tfMZERO"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txN0.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер 0
                id: tfZERO
                objectName: "tfZERO"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txN0.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM1
                anchors.left: parent.left
                anchors.top: txN0.bottom
                text: "-1"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -1
                id: tfMM1
                objectName: "tfMM1"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM1.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -1
                id: tfM1
                objectName: "tfM1"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM1.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM2
                anchors.left: parent.left
                anchors.top: txNM1.bottom
                text: "-2"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -2
                id: tfMM2
                objectName: "tfMM2"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM2.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -2
                id: tfM2
                objectName: "tfM2"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM2.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM3
                anchors.left: parent.left
                anchors.top: txNM2.bottom
                text: "-3"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -3
                id: tfMM3
                objectName: "tfMM3"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM3.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -3
                id: tfM3
                objectName: "tfM3"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM3.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM4
                anchors.left: parent.left
                anchors.top: txNM3.bottom
                text: "-4"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -4
                id: tfMM4
                objectName: "tfMM4"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM4.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -4
                id: tfM4
                objectName: "tfM4"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM4.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM5
                anchors.left: parent.left
                anchors.top: txNM4.bottom
                text: "-5"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -5
                id: tfMM5
                objectName: "tfMM5"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM5.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -5
                id: tfM5
                objectName: "tfM5"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM5.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM6
                anchors.left: parent.left
                anchors.top: txNM5.bottom
                text: "-6"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -6
                id: tfMM6
                objectName: "tfMM6"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM6.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -6
                id: tfM6
                objectName: "tfM6"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM6.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM7
                anchors.left: parent.left
                anchors.top: txNM6.bottom
                text: "-7"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -7
                id: tfMM7
                objectName: "tfMM7"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM7.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -7
                id: tfM7
                objectName: "tfM7"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM7.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM8
                anchors.left: parent.left
                anchors.top: txNM7.bottom
                text: "-8"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -8
                id: tfMM8
                objectName: "tfMM8"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM8.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -8
                id: tfM8
                objectName: "tfM8"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM8.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM9
                anchors.left: parent.left
                anchors.top: txNM8.bottom
                text: "-9"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -9
                id: tfMM9
                objectName: "tfMM9"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM9.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -9
                id: tfM9
                objectName: "tfM9"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM9.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text {
                id: txNM10
                anchors.left: parent.left
                anchors.top: txNM9.bottom
                text: "-10"
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Отсчет замера -10
                id: tfMM10
                objectName: "tfMM10"
                anchors.left: parent.left
                anchors.leftMargin: tfMP10.anchors.leftMargin
                anchors.top: txNM10.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
            Text { // Замер -9
                id: tfM10
                objectName: "tfM10"
                anchors.left: parent.left
                anchors.leftMargin: tfP10.anchors.leftMargin
                anchors.top: txNM10.anchors.top
                font.pixelSize: txN10.font.pixelSize
            }
        }
        Text {
            id: txHC
            anchors.left: parent.left
            anchors.leftMargin: parent.width/20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.height/20
            text: "Текущие отсчеты"
        }
        Text {
            id: txtCurrent
            objectName: "txtCurrent"
            anchors.left: txHC.right
            anchors.leftMargin: parent.width/20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: txHC.anchors.bottomMargin
            font.pixelSize: 14
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
            text: "Тип конвертора"
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
            text: "Дата изготовления"
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
            id: imTehnika
            source: "qrc:/logo.bmp"
            x: 3*parent.width/24
            y: parent.height/2
            width: 18*parent.width/24
            height: parent.height/2
        }
    }
}


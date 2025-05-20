import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3


Item
{
    id: itInfo
    height: 450
    width: 400

    property string idNumber: ""
    property string detectName: ""
    property string measUnit: ""
    property string measRange: ""
    property string zeroRange: ""
    property string preSet: ""

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

    ImpStyle
    {
        id: impStyle
    }

    Rectangle {
        anchors.fill: parent
        color: impStyle.windowColor

        TabBar {
            id: tbInfo
            width: parent.width
            Material.accent: impStyle.chekedColor

            TabButton {
                id: tbnSet
                text: "Установки"
                background: Rectangle {
                    width: parent.width
                    color: tbnSet.checked ? impStyle.windowColor : impStyle.unChekedTabButtonColor
                }
            }

            TabButton {
                id: tbnCalib
                text: "Калибровка"
                background: Rectangle {
                    width: parent.width
                    color: tbnCalib.checked ? impStyle.windowColor : impStyle.unChekedTabButtonColor
                }
            }

            TabButton {
                id: tbnAbout
                text: "О датчике"
                background: Rectangle {
                    width: parent.width
                    color: tbnAbout.checked ? impStyle.windowColor : impStyle.unChekedTabButtonColor
                }
            }
        }
    }

    StackLayout {
        width: parent.width
        currentIndex: tbInfo.currentIndex

        Item {
            id: itSetTab

            Column {
                spacing: 10
                topPadding: 50
                padding: 10

                GroupBox {
                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 10

                        Row {
                            spacing: 20


                            Text {
                                text: "Наименование"
                                font.pixelSize: tfUM.font.pixelSize
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField { // Вводится имя датчика
                                id: tfName
                                objectName: "tfName"
                                font.pixelSize: tfUM.font.pixelSize
                                width: 150
                                text: detectName
                            }
                        }

                        Button {
                            id: btSetName
                            text:  "Записать имя датчика"
                            font.pixelSize: tfUM.font.pixelSize
                            font.capitalization: Font.MixedCase
                            background: Rectangle {
                                color: btSetName.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                border.color: impStyle.borderColor
                            }
                            onClicked:
                            {
                                sigSetNewUserName(idNumber, tfName.text);
                            }
                        }
                    }


                }

                Grid {
                    columns: 2
                    rows: 5
                    spacing: 20
                    topPadding: 20

                    Text {
                        text: "Единица измерения"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text { // Вводится единица измерения
                        id: tfUM
                        font.pixelSize: 15
                        text: measUnit
                    }
                    Text {
                        text: "Диапазон измерения"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text { // Вводится диапазон показаний
                        id: tfRange
                        font.pixelSize: tfUM.font.pixelSize
                        text: measRange
                    }
                    Text {
                        text: "Диапазон обнуления"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text { // Вводится диапазон показаний
                        id: tfSetZero
                        font.pixelSize: tfUM.font.pixelSize
                        text: zeroRange
                    }
                    Text {
                        text: "Диапазон предустанова"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text { // Вводится диапазон показаний
                        id: tfPreSet
                        font.pixelSize: tfUM.font.pixelSize
                        text: preSet
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
        }
        Item { // Окно калибровки +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            id: itCalib
            visible: false
            Text {
                id: headerTochka
                anchors.top: parent.top
                anchors.topMargin: tbnSet.height * 1.2
                anchors.left: parent.left
                anchors.leftMargin: 18 * parent.width / 100
                text: "Калибровочная\n      точка"
                font.pixelSize: 11
            }
            Text {
                anchors.left: parent.left
                anchors.leftMargin: parent.width/2
                anchors.bottom: headerTochka.bottom
                text: "Отсчеты датчика"
            }
            Item { // калибровочная таблица
                id: calibrTable
                anchors.top: headerTochka.bottom
                anchors.topMargin: 10
                anchors.left: parent.left
                anchors.leftMargin: parent.width/20
                anchors.right: parent.right
                anchors.rightMargin: parent.width/20
                //anchors.bottom: txtCurrent.top
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
                anchors.top: parent.top
                anchors.topMargin: tbnSet.height * 1.2 + 380
                text: "Текущие отсчеты"
            }
            Text {
                id: txtCurrent
                objectName: "txtCurrent"
                anchors.left: txHC.right
                anchors.leftMargin: 30
                anchors.top: txHC.top
                text: "!!!!"
                font.pixelSize: 14
            }
        }

        Item {
            id: itAboutTab

            Column {
                spacing: 10
                topPadding: 50
                padding: 10

                Grid {
                    columns: 2
                    rows: 4
                    spacing: 20
                    topPadding: 20

                    Text {
                        id: txLeft
                        text: "Серийный номер"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        id: txtSerialNum
                        objectName: "txtSerialNum"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        text: "Тип конвертора"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        id: txtType
                        objectName: "txtType"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        text: "Дата изготовления"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        id: txtData
                        objectName: "txtData"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        text: "Подключение:"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        id: txtConnect
                        objectName: "txtConnect"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                }


//                Image {
//                    id: imLogo
//                    objectName: "imLogo"
//                    source: "qrc:/logo.bmp"
//                }
            }
        }
    }
}


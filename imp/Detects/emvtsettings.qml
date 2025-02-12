import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3


Item
{
    height: 500
    width: 355

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

                Grid {
                    columns: 2
                    rows: 4
                    spacing: 20
                    topPadding: 20
                    Text {
                        text: "Единица измерения"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text { // Вводится единица измерения
                        id: tfUM
                        objectName: "tfUM"
                        font.pixelSize: 15
                    }
                    Text {
                        text: "Диапазон измерения"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text { // Вводится диапазон показаний
                        id: tfRange
                        objectName: "tfRange"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        text: "Диапазон обнуления"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text { // Вводится диапазон показаний
                        id: tfSetZero
                        objectName: "tfSetZero"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text {
                        text: "Диапазон предустанова"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                    Text { // Вводится диапазон показаний
                        id: tfPreSet
                        objectName: "tfPreSet"
                        font.pixelSize: tfUM.font.pixelSize
                    }
                }
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
    }
}

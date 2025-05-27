import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3


Item
{
    height: 400
    width: 400

    property string measUnit: ""
    property string measRange: ""
    property string zeroRange: ""
    property string preSet: ""

    ImpStyle
    {
        id: impStyle
    }

    Rectangle
    {
        anchors.fill: parent

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
                }
            }
        }
    }


//                Image {
//                    id: imLogo
//                    objectName: "imLogo"
//                    source: "qrc:/logo.bmp"
//                    x: 3*parent.width/24
//                    y: parent.height/2
//                    width: 18*parent.width/24
//                    height: parent.height/2
//                }
}

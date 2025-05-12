import QtQuick 2.0
import QtQuick.Controls 2.12

Item
{
    height: 30
    onHeightChanged:
    {
        paintIndicate();
    }

    width: 100
    onWidthChanged:
    {
        paintIndicate();
    }

    // scale
    property color scaleColor: "black"
    property int scaleThickness: 1
    property int scaleWThickness: 3

    property real indication: 0
    onIndicationChanged:
    {
        paintIndicate();
    }

    property real hLimit: 0
    onHLimitChanged:
    {
        paintIndicate();
    }
    property real lLimit: 0
    onLLimitChanged:
    {
        paintIndicate();
    }
    property real priemka: 0
    onPriemkaChanged:
    {
        paintIndicate();
    }
    property color badColor: "red"
    property color priemkaColor: "orange"
    property color passedColor: "green"


    property int sumGauge: 51 // Размах индикатора в делениях в одну сторону
    property real unitPoint: 1
    onUnitPointChanged:
    {
        paintIndicate();
    }


    function paintIndicate()
    {
        var maxWidth = width / 2;
        var divisionPrice = maxWidth / sumGauge / unitPoint;
        var w = 0;
        if (indication > 0)
        {
            lh.width = 0;
            w = divisionPrice * indication;
            rh.width = w > maxWidth ? maxWidth : w;
        }
        else if (indication < 0)
        {
            rh.width = 0;
            w = - divisionPrice * indication;
            lh.width = w > maxWidth ? maxWidth : w;
        }
        else
        {
            lh.width = 0;
            rh.width = 0;
        }
        setColor();
    }


    function setColor()
    {
        var c = passedColor;
        if (indication >= hLimit || indication <= lLimit)
        {
            c = badColor;
        }
        else if (indication >= (hLimit - priemka) || indication <= (lLimit + priemka))
        {
            c = priemkaColor;
        }
        rh.color = c;
        lh.color = c;
        zero.color = c;
    }

    // Базовые элементы
    Rectangle
    {
        id: scaleXLine
        height: scaleWThickness
        width: parent.width
        color: scaleColor
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height / 5
    }
    Rectangle
    {
        id: zero
        height: parent.height - scaleXLine.anchors.bottomMargin
        width: scaleThickness
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
    }

    // Индикатор
    Rectangle
    {
        id: lh
        height: zero.height
        anchors.right: zero.left
        anchors.top: parent.top
        anchors.bottom: scaleXLine.top
    }
    Rectangle
    {
        id: rh
        height: zero.height
        anchors.top: parent.top
        anchors.bottom: scaleXLine.top
        anchors.left: zero.right
    }

    // Шкала
    Rectangle
    {
        height: scaleWThickness
        width: parent.width
        color: scaleColor
        anchors.top: parent.top
    }
    Item
    {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: scaleXLine.anchors.bottomMargin
        Repeater
        {
            model: 11
            Item
            {
                Rectangle
                {
                    id: marker
                    width: scaleWThickness
                    height: scaleXLine.anchors.bottomMargin / 2
                    x: scaleXLine.width / 2 + scaleXLine.width * (index - 5) * 10 / (2 * sumGauge) - 1
                    anchors.top: parent.top
                    color: scaleColor
                }
                Label
                {
                    text: (index - 5) * unitPoint * 10
                    font.pixelSize: scaleXLine.anchors.bottomMargin * 0.5
                    anchors.top: marker.bottom
                    anchors.horizontalCenter: marker.horizontalCenter
                }
            }
        }
        Repeater
        {
            model: sumGauge * 2 + 1
            Item
            {
                Rectangle
                {
                    width: scaleThickness
                    height: scaleXLine.anchors.bottomMargin / 4 * (index % 2 != 0 ? 1.5 : 1)
                    x: scaleXLine.width / 2 + scaleXLine.width * (index - sumGauge) / (2 * sumGauge)
                    anchors.top: parent.top
                    color: scaleColor
                }
            }
        }
    }

}

import QtQuick 2.0

Item
{
    height: 30
    width: 100

    property real indication: 0
    onIndicationChanged:
    {
        paintScale();
    }

    property int sumGauge: 51 // Размах индикатора в делениях в одну сторону
    property real unitPoint: 1
    onUnitPointChanged:
    {
        paintScale();
    }


    function paintScale()
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
    }

    Rectangle
    {
        id: zero
        height: parent.height
        width: 1
        color: "black"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle
    {
        id: lh
        height: zero.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: zero.left
        color: "red"
    }
    Rectangle
    {
        id: rh
        height: zero.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: zero.right
        color: "blue"
    }
}

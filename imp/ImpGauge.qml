import QtQuick 2.5
import QtQuick.Controls 2.1

Item
{
    id: impGauge 
    objectName: "impGauge" 
    property real impGaugevalue: inputIndicator.mess
    property int tickmarkInset: toPixels(0.04)
    property int minorTickmarkInset: tickmarkInset
    property int labelStepSize: 20
    property int labelInset: 32
    property int outerRadius: height/2
    property real xCenter: outerRadius
    property real yCenter: outerRadius
    property real needleLength: outerRadius - tickmarkInset * 1.25
    property real needleTipWidth: toPixels(0.01)
    property real needleBaseWidth: toPixels(0.06)
    property int degGauge: 150 // Размах индикатора в градусах в одну сторону
    property int sumGauge: 51 // Размах индикатора в делениях в одну сторону
    property int lowLimitg: inputIndicator.lowLimit
    property int highLimitg: inputIndicator.highLimit
    property real unitPointg: inputIndicator.unitPoint
    property bool blDetect1Enable: inputIndicator.blDetect1EnableInput
    property bool blDetect2Enable: inputIndicator.blDetect2EnableInput
    property bool blDetect1Work: inputIndicator.blDetect1WorkInput
    property bool blDetect2Work: inputIndicator.blDetect2WorkInput
    property bool blRect1Color: false
    property bool blRect2Color: false
    property string peekMeasText: " "
    property bool peekedData: false

    onImpGaugevalueChanged: canvasGauge.requestPaint(); // перерисовка сдвинутой стрелки
    onLowLimitgChanged: canvasGauge.requestPaint(); // Перерисовка шкалы
    onHighLimitgChanged: canvasGauge.requestPaint(); // Перерисовка шкалы
    onUnitPointgChanged: canvasGauge.requestPaint(); // Перерисовка шкалы
    onPeekedDataChanged:
    {
        if (peekedData)
        {
            peekMeas.text = peekMeasText;
            peekMeas.opacity = 1;
            oaPeekMeas.running = true;
        }
        peekedData = false;
    }

    function toPixels(percentage) {
        return percentage * outerRadius;
    }

    function degToRad(degrees) {
        return degrees * (Math.PI / 180);
    }

    function radToDeg(radians) {
        return radians * (180 / Math.PI);
    }

    function valueToDegForGauge(gaugvalue) // значение преобразует в угол поворота стрелки
    {
        var gaugdeg;
        var degGauge1 = degGauge + (degGauge / sumGauge); // Для отображения зашкаливания
        gaugdeg = (-1) * gaugvalue / inputIndicator.unitPoint * degGauge / sumGauge;
        if (gaugdeg < degGauge1 * (-1)) gaugdeg = degGauge1 * (-1);
        if (gaugdeg > degGauge1) gaugdeg = degGauge1;
        return gaugdeg;
    }

    function round10(realNum, accur) // преобразует число в строку с учетом количества цифр после запятой
    {
        var temp, index, len;
        var stroka;
        temp = 1
        for (index = 1; index < accur + 1; index = index + 1)
        {
            temp = temp * 10;
        }
        index = Math.round(realNum * temp) / temp;
        stroka = index.toString();
        if (accur !== 0)
        {
            temp = stroka.lastIndexOf(".");
            len = stroka.length;
            if ( temp !== -1)
            {
                for (index = 0; index < accur - (len - temp - 1); index++)
                    stroka = stroka.concat("0");
            }
            else
            {
                stroka = stroka.concat(".");
                for (index = 0; index<accur; index++)
                    stroka = stroka.concat("0");
            }
        }
        return stroka;
    }

    function shiftX(x, y, shiftdeg)
    {
        var newx;
        newx = Math.sqrt(x*x + y*y) * Math.cos(degToRad(shiftdeg)+Math.acos(x/(Math.sqrt(x*x + y*y))));
        return y>0 ? 0-newx : newx;
    }
    function shiftY(x, y, shiftdeg)
    {
        var newy;
        newy = Math.sqrt(x*x + y*y) * Math.sin(degToRad(shiftdeg)+Math.acos(x/(Math.sqrt(x*x + y*y))));
        return y>0 ? newy : 0-newy;
    }
    function drawNeedle(ctx)
    { // Рисование стрелки
        var valueDeg = valueToDegForGauge(impGaugevalue); // перевод измеренного значения в угол поворота стрелки
        ctx.beginPath();
        ctx.moveTo(xCenter + shiftX(0, needleBaseWidth / 2, valueDeg),
                   yCenter + shiftY(0, needleBaseWidth / 2, valueDeg));
        ctx.lineTo(xCenter + shiftX(0 - needleBaseWidth / 2, 0, valueDeg),
                   yCenter + shiftY(0 - needleBaseWidth / 2, 0, valueDeg));
        ctx.lineTo(xCenter + shiftX(0 - needleTipWidth / 2, 0 - needleLength, valueDeg),
                   yCenter + shiftY(0 - needleTipWidth / 2, 0 - needleLength, valueDeg));
        ctx.lineTo(xCenter + shiftX(needleTipWidth / 2, 0 - needleLength, valueDeg),
                   yCenter + shiftY(needleTipWidth / 2, 0 - needleLength, valueDeg));
        ctx.lineTo(xCenter + shiftX(needleBaseWidth / 2, 0, valueDeg),
                   yCenter + shiftY(needleBaseWidth / 2, 0, valueDeg));
        ctx.lineTo(xCenter + shiftX(0, needleBaseWidth / 2, valueDeg),
                   yCenter + shiftY(0, needleBaseWidth / 2, valueDeg));
        ctx.closePath();
        ctx.fillStyle = Qt.rgba(0, 0, 0, 0.9);
        ctx.fill();
    }

    function getLengthMark(index)
    {
        // соотношение 6 - 8 - 10
        // outerRadius * 0.07 эквивалентно 6
        var baseLength = outerRadius * 0.07;
        var k = 1;
        if ((index / 5) == Math.round(index/5))
            k = 1.333333;
        if ((index / 10) == Math.round(index/10))
            k = 1.666667;
        return baseLength * k;
    }

    function getColorStatus()
    {
        // 0 - normal
        // 1 - overload
        // 2 - limit
        // 3 - priemka
        var result = 0;
        var mess = inputIndicator.mess;
        if (inputIndicator.blOverRange1 | inputIndicator.blOverRange2)
            result = 1;
        else if (inputIndicator.dopusk)
            if ((mess >= inputIndicator.highLimit) |
                    (mess <= inputIndicator.lowLimit ))
                result = 2;
            else if ((mess >= inputIndicator.highLimit - inputIndicator.priemka) |
                     (mess <= inputIndicator.lowLimit + inputIndicator.priemka))
                result = 3;
        return result;
    }

    function getColorMessText()
    {
        var result = impStyle.baseTextColor;
        var status = getColorStatus();
        if (status === 1)
            result = "cyan";
        else if (status === 2)
            result = impStyle.warningColor;
        else if (status === 3)
            result = "orange";
        return result;
    }

    function exorbitantFilter(mess)
    {
        if (getColorStatus() === 1) // overload
        {
            if (mess > 0) mess = "+^^^";
            if (mess < 0) mess = "-vvv";
        }
        return mess;
    }

    function getColorMark(pos)
    {
        return ((pos > inputIndicator.highLimit) |
                (pos < inputIndicator.lowLimit))
                & inputIndicator.dopusk
               ? impStyle.warningColor
               : impStyle.baseTextColor
    }


    // Фон циферблата
    Rectangle
    {
        anchors.fill: parent
        color: getColorStatus() === 0 ? impStyle.actionbarColor : "lightcoral"
        radius: width/2
    }
    // Маркеры на циферблате
    Repeater
    {
        model: 103
        Marker
        {
            property int output: index - 51
            anchors.fill: impGauge
            colorMark: ((output > inputIndicator.highLimit/inputIndicator.unitPoint) |
                        (output < inputIndicator.lowLimit/inputIndicator.unitPoint))
                        & inputIndicator.dopusk
                       ? impStyle.warningColor
                       : impStyle.baseTextColor
            centerX: outerRadius
            centerY: outerRadius
            radiusDial: outerRadius
            thicknessMark: outerRadius * 0.01
            lengthMark: getLengthMark(output)
            angleMark: output*degGauge/sumGauge
            visibleMark: true
        }

    }

    // Вывод значения датчика в цифровом виде
    Text
    {
        id: messText
        font.pixelSize: toPixels(0.3)
        text: exorbitantFilter(round10(inputIndicator.mess, inputIndicator.accuracy))
        color: getColorMessText()
        horizontalAlignment: Text.AlignRight
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.verticalCenter
        anchors.topMargin: toPixels(0.25)
        ColorAnimation {
            id: caMessTextToBlack
            loops: 1
            to: impStyle.baseTextColor
            duration: 100
        }
        ColorAnimation {
            id: caMessTextToTomato
            loops: 1
            to: impStyle.warningColor
            duration: 100
        }
    }

    // Вывод захваченного значения
    Text
    {
        id: peekMeas
        text: " "
        font.pixelSize: toPixels(0.04)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: messText.top
        color: impStyle.baseTextColor
        opacity: 0
        OpacityAnimator
        {
            id: oaPeekMeas
            target: peekMeas
            loops: 1
            from: 1
            to: 0
            duration: 500
        }
    }

    // Вывод единиц измерения датчика
    Text
    {
        text: round10(inputIndicator.unitPoint, inputIndicator.accurDivision) + " " + inputIndicator.messUnit
        color: impStyle.baseTextColor
        font.pixelSize: toPixels(0.09)
        y: 3*parent.height/10
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // Надписи на шкале
    Text
    {
        property real output: (-50) * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.left: parent.left
        anchors.leftMargin: toPixels(0.6)
        anchors.top: parent.verticalCenter
        anchors.topMargin: toPixels(0.6)
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: 50 * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.right: parent.right
        anchors.rightMargin: toPixels(0.6)
        anchors.top: parent.verticalCenter
        anchors.topMargin: toPixels(0.6)
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: (-40) * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.left: parent.left
        anchors.leftMargin: toPixels(0.3)
        anchors.top: parent.verticalCenter
        anchors.topMargin: toPixels(0.3)
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: 40 * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.right: parent.right
        anchors.rightMargin: toPixels(0.3)
        anchors.top: parent.verticalCenter
        anchors.topMargin: toPixels(0.3)
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: (-30) * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.left: parent.left
        anchors.leftMargin: toPixels(0.2)
        anchors.verticalCenter: parent.verticalCenter
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: 30 * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.right: parent.right
        anchors.rightMargin: toPixels(0.2)
        anchors.verticalCenter: parent.verticalCenter
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: (-20) * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.left: parent.left
        anchors.leftMargin: toPixels(0.3)
        anchors.bottom: parent.verticalCenter
        anchors.bottomMargin: toPixels(0.3)
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: 20 * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.right: parent.right
        anchors.rightMargin: toPixels(0.3)
        anchors.bottom: parent.verticalCenter
        anchors.bottomMargin: toPixels(0.3)
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: (-10) * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.left: parent.left
        anchors.leftMargin: toPixels(0.6)
        anchors.bottom: parent.verticalCenter
        anchors.bottomMargin: toPixels(0.6)
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property real output: 10 * inputIndicator.unitPoint
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.right: parent.right
        anchors.rightMargin: toPixels(0.6)
        anchors.bottom: parent.verticalCenter
        anchors.bottomMargin: toPixels(0.6)
        text: round10(output, inputIndicator.accurDivision == 0 ? 0 : inputIndicator.accurDivision-1)
        color: getColorMark(output)
        antialiasing: true
    }
    Text
    {
        property int output: 0
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: toPixels(0.2)
        text: output
        color: getColorMark(output)
        antialiasing: true
    }
    // Рисуем стрелку
    Canvas
    {
        id: canvasGauge
        anchors.fill: parent
        antialiasing: true
        onPaint:
        {
            var ctx = getContext("2d");
            ctx.globalCompositeOperation = "source-over";
            ctx.reset();
            drawNeedle(ctx); // Рисование стрелки
        }
    }

    // Индикация работы датчиков
    Row
    {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.max(6, outerRadius * 0.1)
        anchors.horizontalCenter: parent.horizontalCenter
        Rectangle
        {
            id: indWork1
            height: Math.max(20, outerRadius * 0.1)
            width: Math.max(20, outerRadius * 0.1)
            radius: width/2
            border.color: impStyle.baseTextColor
            color: blRect1Color ? impStyle.actionbarColor : impStyle.baseTextColor
            visible: inputIndicator.blDetect1EnableInput
            Text
            {
                id: txtD1
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "1"
                color: blRect1Color ? impStyle.baseTextColor : impStyle.actionbarColor
                font.pixelSize: Math.max(6, outerRadius * 0.07)
            }
        }
        Text
        {
            text: indWork1.visible & indWork2.visible ? "  " : ""
        }
        Rectangle
        {
            id: indWork2
            height: Math.max(20, outerRadius * 0.1)
            width: Math.max(20, outerRadius * 0.1)
            radius: width/2
            border.color: impStyle.baseTextColor
            color: blRect2Color ? impStyle.actionbarColor : impStyle.baseTextColor
            visible: inputIndicator.blDetect2EnableInput
            Text
            {
                id: txtD2
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "2"
                color: blRect2Color ? impStyle.baseTextColor : impStyle.actionbarColor
                font.pixelSize: Math.max(6, outerRadius * 0.07)
            }
        }
    }

    Timer
    {
        interval: 500
        running: true
        repeat: true
        onTriggered:
        {
            if (inputIndicator.blDetect1WorkInput)
            {
                blRect1Color = !blRect1Color;
                inputIndicator.blDetect1WorkInput = false;
            }
            if (inputIndicator.blDetect2WorkInput)
            {
                blRect2Color = !blRect2Color;
                inputIndicator.blDetect2WorkInput = false;
            }
        }
    }
}

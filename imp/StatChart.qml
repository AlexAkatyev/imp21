import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQml.Models 2.2

Item
{
    id: statChart
    objectName: "statChart"
    property bool blRun: false // false - график не запущен
    property bool blMode: false // false - Ручной, true - Автоматический
    property int maxSizeMess: 100 // ширина видимого графика в секундах
    property int countMess: 0 // количество произведенных замеров
    property int period_ms: 1000
    property int maxLenMess: 100000 // длительность измерений, милисекунды
    property double dbHiLimit: 30
    property double dbLoLimit: -30
    property double iGaugeUnit: 1
    property string strMessUnit: "Не определен"
    property double inputDataChart : inputIndicator.mess + inputIndicator.zeroShift
    property int inputDataCommand: 0 // 0 - ничего делать не надо, 1 - изменено значение inputDataChart
    //property int indexMess: 0; // количество замеров в dataChart
    property string textcsv: ""
    property string txFormula: "Формула не задана"

    property double maxMess: -65353
    property double minMess: 65535
    property int countFlex: 10

    property int shiftSec: 0 // Сдвиг при прокрутке, сек

    signal sigClickedPrint();
    signal sigClickedSaveCSV();
    signal sigClickedSaveXLS();

    width: 480
    height: 320

    onBlRunChanged:
    {
        timerRunning.running = blRun & blMode;
        timerHand.running = blRun & !blMode;
        imPause.visible = blRun;
        imStart.visible = !blRun;
        shiftSec = 0;
    }

    onMaxSizeMessChanged:
    {
        idcanvas.requestPaint();
    }

    onPeriod_msChanged:
    {
        idcanvas.requestPaint();
        timerRunning.interval = period_ms;
    }

    onDbHiLimitChanged:
    {
        idcanvas.requestPaint();
    }

    onDbLoLimitChanged:
    {
        idcanvas.requestPaint();
    }

    onIGaugeUnitChanged:
    {
        idcanvas.requestPaint();
    }

    onStrMessUnitChanged:
    {
        txMessUnit.text = strMessUnit;
    }

    onInputDataCommandChanged:
    {
        if (inputDataCommand === 1)
        {
            if (blRun | !blMode)
            { // Отработка в режиме запуска и ижидается останов
                lmDataChart.append({"time":(new Date()), "mess":inputDataChart});
                countMess++;
                if (inputDataChart > maxMess)
                    maxMess = inputDataChart;
                if (inputDataChart < minMess)
                    minMess = inputDataChart;
                if (blMode && countMess >= Math.round(maxLenMess/period_ms))
                    blRun = false;
                idcanvas.requestPaint(); // Вызов сигнала Paint для графика
            }
            inputDataCommand = 0; // Команда отработана
        }
    }

    function clearModel()
    {
        lmDataChart.clear();
        countMess = 0;
        maxMess = -65535;
        minMess = 65535;
        idcanvas.requestPaint(); // Вызов сигнала Paint для графика
    }

    function fillTextCSV()
    {
        textcsv = "Формула;" + txFormula + "\n";
        textcsv += "Режим;" + (blMode ? "Автоматический" : "Ручной") + "\n";
        textcsv += "Единица измерения;" + strMessUnit + "\n";
        textcsv += "\n"; // пустая строка
        textcsv += "Произведено замеров;" + countMess.toString() + "\n";
        if (inputIndicator.dopusk)
        {
            textcsv += "Верхнее допустимое значение;" + dbHiLimit.toString() + "\n";
            textcsv += "Нижнее допустимое значение;" + dbLoLimit.toString() + "\n";
        }
        textcsv += "\n"; // пустая строка
        textcsv += "Время замера;Измеренное значение\n";
        for (var i = 0; i < countMess; i++)
            textcsv += Qt.formatTime(lmDataChart.get(i).time, "hh:mm:ss.zzz")
                       + ";"
                       + impGauge.round10(lmDataChart.get(i).mess, inputIndicator.accuracy)
                       + "\n";
    }

    function flexibleY()
    {
        return (lmDataChart.count > countFlex
                && (maxMess - minMess) > 10)
    }

    function getLim1()
    {
        if (flexibleY())
            return impGauge.round10(maxMess, inputIndicator.accuracy);
        else
            return 50 * iGaugeUnit;
    }

    function getLim2()
    {
        if (flexibleY())
            return impGauge.round10(minMess, inputIndicator.accuracy);
        else
            return -50 * iGaugeUnit;
    }


    ListModel { // Здесь хранятся данные для отображения графика
        id: lmDataChart
    }

    Timer
    {
        id: timerRunning
        interval: period_ms
        running: false
        repeat: true
        onTriggered: inputDataCommand = 1;
    }

    Timer
    {
        id: timerHand
        interval: 100
        running: false
        repeat: true
        onTriggered: idcanvas.requestPaint();
    }


    Button
    {
        id: btLong
        text: ">"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: 30
        height: 60
        onReleased: {
            var len = maxSizeMess *= 1.25;
            maxSizeMess = len > 250 ? 250 : len;
        }
        ToolTip
        {
            text: "Увеличить график"
        }
    }

    Button
    {
        id: btShort
        text: "<"
        anchors.right: btLong.left
        anchors.bottom: parent.bottom
        width: btLong.width
        height: btLong.height
        onReleased: {
            var len = maxSizeMess *= 0.75;
            maxSizeMess = len < 5 ? 5 : len;
        }
        ToolTip
        {
            text: "Уменьшить график"
        }
    }

    MouseArea
    {
        id: btSaveCSV
        anchors.right: btShort.left
        anchors.bottom: parent.bottom
        height: btLong.height
        width: height
        hoverEnabled: true
        onEntered: ttSaveCSV.visible = true
        onExited: ttSaveCSV.visible = false
        ToolTip
        {
            id: ttSaveCSV
            text: "Сохранить CSV"
            x: width / 2
            y: height / 2
        }
        Image {
            source: "qrc:/ext_csv_filetype_icon_176252.png"
            anchors.fill: parent
        }
        onClicked:
        { // Запись текста в файл
            fillTextCSV();
            sigClickedSaveCSV();
        }
    }

    MouseArea
    {
        id: btSaveXLS
        anchors.bottom: parent.bottom
        anchors.right: btSaveCSV.left
        height: btSaveCSV.height
        width: btSaveCSV.width
        hoverEnabled: true
        onEntered: ttSaveXLS.visible = true
        onExited: ttSaveXLS.visible = false
        ToolTip
        {
            id: ttSaveXLS
            text: "Сохранить XLSX"
            x: width / 2
            y: height / 2
        }
        Image {
            source: "qrc:/ext_xlsx_icon_176245.png"
            anchors.fill: parent
        }
        onClicked:
        { // Запись текста в файл
            fillTextCSV();
            sigClickedSaveXLS();
        }
    }

    Button {
        id: btStatSetNull
        anchors.bottom: parent.bottom
        anchors.right: btSaveXLS.left
        height: btSaveCSV.height
        width: 0.75 * btSaveCSV.width
        text: setZero ? "«Ø»" : "«0»"
        enabled: !blRun
        onReleased:
        {
            inputIndicator.zeroShift = setZero ? 0 : -inputIndicator.mess;
            setZero = !setZero;
        }
    }


//    MouseArea
//    {
//        id: btPrint
//        anchors.bottom: parent.bottom
//        anchors.right: btSaveXLS.left
//        height: btSaveCSV.height
//        width: btSaveCSV.width
//        hoverEnabled: true
//        onEntered: ttPrint.visible = true
//        onExited: ttPrint.visible = false
//        visible: false
//        ToolTip
//        {
//            id: ttPrint
//            text: "Печатать график"
//            x: width / 2
//            y: height / 2
//        }
//        Image {
//            source: "qrc:/Printer_Picture.png"
//            anchors.fill: parent
//        }
//        onClicked: sigClickedPrint();
//    }

    MouseArea
    {
        id: btClear
        anchors.bottom: parent.bottom
        anchors.right: btStatSetNull.left
        //anchors.topMargin: 15
        height: btSaveCSV.height
        width: btSaveCSV.width
        hoverEnabled: true
        onEntered: ttClrear.visible = true
        onExited: ttClrear.visible = false
        ToolTip
        {
            id: ttClrear
            text: "Очистить график"
            x: width / 2
            y: height / 2
        }
        Image {
            source: "qrc:/Gnome-Edit-Clear-64.png"
            anchors.fill: parent
        }
        onClicked: clearModel();
    }

    MouseArea
    {
        id: btCapture
        anchors.left: btStartStop.right
        anchors.bottom: parent.bottom
        height: btSaveCSV.height
        width: btSaveCSV.width
        hoverEnabled: true
        onEntered: ttCapture.visible = true
        onExited: ttCapture.visible = false
        visible: !blMode & blRun
        ToolTip
        {
            id: ttCapture
            text: "Внести текущее показание"
            x: width / 2
            y: height / 2
        }
        Image {
            id: imCapture
            source: "qrc:/gnome-dev-disc-dvdr-plus.png"
            anchors.fill: parent
        }
        onClicked:
        {
            inputDataCommand = 1;
        }
    }


    MouseArea
    {
        id: btStartStop
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        height: btSaveCSV.height
        width: btSaveCSV.width
        hoverEnabled: true
        onEntered: ttStartStop.visible = true
        onExited: ttStartStop.visible = false
        ToolTip
        {
            id: ttStartStop
            text: blRun ? "Остановить построение графика" : "Начать построение графика"
            x: width / 2
            y: height / 2
        }
        Image {
            id: imPause
            source: "qrc:/PauseNormal.png"
            anchors.fill: parent
            visible: blRun
        }
        Image {
            id: imStart
            source: "qrc:/Play-1-Pressed-icon.png"
            anchors.fill: parent
            visible: !blRun
        }
        onClicked:
        {
            blRun = !blRun;
        }
    }


    Item
    {
        id: itChart
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: btSaveCSV.top
        Rectangle
        {
            anchors.fill: parent
            color: "light yellow"
        }
        Canvas {
            id: idcanvas
            anchors.fill: parent

            property real xGridOffset: width * 3 / 40
            property real xGridStep: 0
            property int iVerticalLine: 0
            property int iHorizontalLine: 11  // Число должно быть нечетным
            property real yGridOffset: 0//height / (iHorizontalLine + 1)
            property real yGridStep: height / (iHorizontalLine + 1)
            property real koef: 1

            Text {
                id: txMessUnit
                text: strMessUnit
                x: 0
                y: idcanvas.yGridOffset
            }
            Text {
                id: txZero
                text: "0"
                x: idcanvas.xGridOffset/2 - txZero.width/2
                y: idcanvas.calcMessToY(0) //idcanvas.yGridOffset + (idcanvas.height - idcanvas.yGridOffset)/2 - txZero.height/2
            }
            Text {
                id: txHi
                text: dbHiLimit.toString()
                x: idcanvas.xGridOffset/2 - txHi.width/2
                y: idcanvas.calcMessToY(dbHiLimit) - txHi.height/2
                visible: inputIndicator.dopusk
            }
            Text {
                id: txLim1
                text: getLim1().toString()
                x: idcanvas.xGridOffset/2 - txLim1.width/2
                y: idcanvas.calcMessToY(getLim1()) - txLim1.height/2
                visible: flexibleY()
            }
            Text {
                id: txLo
                text: dbLoLimit.toString()
                x: idcanvas.xGridOffset/2 - txLo.width/2
                y: idcanvas.calcMessToY(dbLoLimit) - txLo.height/2
                visible: inputIndicator.dopusk
            }
            Text {
                id: txLim2
                text: getLim2().toString()
                x: idcanvas.xGridOffset/2 - txLim2.width/2
                y: idcanvas.calcMessToY(getLim2()) - txLim2.height/2
                visible: flexibleY()
            }

            MouseArea {
                anchors.fill: parent
                onWheel: {
                    if (countMess === 0 || blRun)
                        return;
                    var newShift = shiftSec - (maxSizeMess * wheel.angleDelta.y / 10 / 120);
                    var hrel = ((new Date()) - lmDataChart.get(0).time)/1000 - 0.95 * maxSizeMess;
                    if (newShift > hrel)
                        newShift = hrel;
                    if (newShift < 0)
                        newShift = 0;
                    if (shiftSec !== newShift)
                    {
                        shiftSec = newShift;
                        idcanvas.requestPaint();
                    }
                }
            }

            function drawBackground(ctx) {
                ctx.lineWidth = 1;
                ctx.save();
                ctx.fillStyle = "light yellow";
                ctx.fillRect(0, 0, idcanvas.width, idcanvas.height);
                // Horizontal grid lines
                if (flexibleY())
                {
                    var line = Math.floor(maxMess / 10) * 10;
                    if (line > maxMess)
                        line = line - 10;
                    ctx.strokeStyle = "#d7d7d7";
                    ctx.beginPath();
                    while (line > minMess)
                    {
                        ctx.moveTo(xGridOffset, idcanvas.calcMessToY(line));
                        ctx.lineTo(idcanvas.width, idcanvas.calcMessToY(line));
                        line = line - 10;
                    }
                    ctx.stroke();
                    ctx.save();
                    ctx.strokeStyle = "#000000";
                    ctx.beginPath();
                    ctx.moveTo(xGridOffset, idcanvas.calcMessToY(maxMess));
                    ctx.lineTo(idcanvas.width, idcanvas.calcMessToY(maxMess));
                    ctx.moveTo(xGridOffset, idcanvas.calcMessToY(minMess));
                    ctx.lineTo(idcanvas.width, idcanvas.calcMessToY(minMess));
                    ctx.moveTo(xGridOffset, idcanvas.calcMessToY(0));
                    ctx.lineTo(idcanvas.width, idcanvas.calcMessToY(0));
                    ctx.stroke();
                    ctx.save();
                }
                else
                {
                    ctx.strokeStyle = "#d7d7d7";
                    ctx.beginPath();
                    for (var i = 0; i < iHorizontalLine; i++) {
                        if (i === Math.floor(iHorizontalLine/2))
                        {
                            ctx.stroke();
                            ctx.save();
                            ctx.strokeStyle = "#000000";
                            ctx.beginPath();
                        }
                        ctx.moveTo(xGridOffset, idcanvas.yGridOffset + (i+1) * idcanvas.yGridStep);
                        ctx.lineTo(idcanvas.width, idcanvas.yGridOffset + (i+1) * idcanvas.yGridStep);
                        if (i === Math.floor(iHorizontalLine/2))
                        {
                            ctx.stroke();
                            ctx.restore();
                            ctx.beginPath();
                        }
                    }
                    ctx.stroke();
                    ctx.save();
                }

                // Vertical grid lines
                ctx.strokeStyle = "#d7d7d7";
                ctx.beginPath();
                iVerticalLine = maxSizeMess;
                koef = 1;
                while (iVerticalLine < 26)
                {
                    iVerticalLine = iVerticalLine * 2;
                    koef = koef / 2;
                }
                while (iVerticalLine > 50)
                {
                    iVerticalLine = iVerticalLine / 2;
                    koef = koef * 2;
                }
                xGridStep = (width - xGridOffset) / iVerticalLine;
                ctx.stroke();
                ctx.save();

                for (i = 0; i < iVerticalLine; i++)
                {
                    if (i/10 === Math.round(i/10))
                    {
                        ctx.stroke();
                        ctx.save();
                        ctx.strokeStyle = "#000000";
                        ctx.beginPath();
                        if (i > 0)
                            ctx.strokeText(i*koef, calcPointToX(i*koef) + 3, calcAbcissToY());
                    }
                    ctx.moveTo(xGridOffset + i * xGridStep, idcanvas.yGridOffset);
                    ctx.lineTo(xGridOffset + i * xGridStep, idcanvas.height);
                    if (i/10 === Math.round(i/10))
                    {
                        ctx.stroke();
                        ctx.restore();
                        ctx.beginPath();
                    }
                }
                ctx.stroke();
                ctx.save();

                ctx.strokeStyle = "#000000";
                ctx.beginPath();
                ctx.strokeText(maxSizeMess, calcPointToX(maxSizeMess) - 20, calcAbcissToY());
                ctx.stroke();
                ctx.save();

                ctx.restore();
            }

            function calcAbcissToY()
            {
                if (!flexibleY())
                    return calcMessToY(iGaugeUnit * (-5));
                else
                {
                    var yShift = (maxMess - minMess)/20;
                    if (maxMess > 0 && minMess < 0)
                        return calcMessToY(0 - yShift);
                    if (maxMess <= 0)
                        return calcMessToY(maxMess - yShift);
                    if (minMess >= 0)
                        return calcMessToY(minMess - yShift);
                }
            }

            function calcMessToY(mess)
            {
                var result;
                if (flexibleY())
                    result = yGridStep + (height - 2*yGridStep)*(maxMess - mess)/(maxMess - minMess);
                else
                    result = yGridOffset + ((iHorizontalLine + 1) * yGridStep * ((60 * iGaugeUnit - mess)/(120 * iGaugeUnit)));
                if (result < yGridOffset)
                    result = yGridOffset;
                if (result > yGridOffset + yGridStep * (iHorizontalLine+1))
                    result = yGridOffset + yGridStep * (iHorizontalLine+1);
                return result;
            }

            function drawLimit(ctx)
            {
                ctx.lineWidth = 1;
                ctx.save();
                ctx.strokeStyle = "#ff0000";
                ctx.beginPath();
                // Верхний предел
                ctx.moveTo(xGridOffset, calcMessToY(dbHiLimit));
                ctx.lineTo(idcanvas.width, calcMessToY(dbHiLimit));
                // Нижний предел
                ctx.moveTo(xGridOffset, calcMessToY(dbLoLimit));
                ctx.lineTo(idcanvas.width, calcMessToY(dbLoLimit));
                ctx.stroke();
                ctx.restore();
            }

           function calcPointToX(point)
           {
               var result;
               result = xGridOffset + point*(idcanvas.width - xGridOffset)/maxSizeMess;
               return result;
           }

           function calcMessToX(t)
           {
               var result = ((new Date()) - t) / 1000 - shiftSec;
               var lenData = (lmDataChart.get(lmDataChart.count-1).time - lmDataChart.get(0).time) / 1000;
               if (lenData < maxSizeMess)
                   result = xGridOffset + (t - lmDataChart.get(0).time)/1000*(idcanvas.width - xGridOffset)/maxSizeMess;
               else
                   result = idcanvas.width - result*(idcanvas.width - xGridOffset)/maxSizeMess;
               return result;
           }

           function checkDrawMessX(t)
           {
               var delta = ((new Date()) - t) / 1000 - shiftSec;
               return (delta <= maxSizeMess) && (delta >= 0);
           }

           function drawScroller(ctx)
           {
               ctx.save();
               ctx.strokeStyle = "#000000";
               ctx.lineWidth = 10;
               ctx.beginPath();
               var x0 = xGridOffset;
               var x1 = idcanvas.width
               if (countMess > 1)
               {
                   var begin = lmDataChart.get(0).time / 1000;
                   var end = (new Date()) / 1000;
                   var history = end - begin;
                   if (history > maxSizeMess)
                   {
                       var windowKoef = maxSizeMess / history;
                       var shiftKoef = shiftSec / history;
                       var xx0 = x0 + (x1-x0) * (1 - windowKoef - shiftKoef); // x0 + (x1-x0)*(1-windowKoef) - (x1-x0)*shiftKoef;
                       var xx1 = x1 - (x1-x0)*shiftKoef;
                       if (xx1 - xx0 < 10)
                           xx0 = xx1 - 10;
                       x0 = xx0;
                       x1 = xx1;
                   }
               }
               ctx.moveTo(x0, idcanvas.height - 5);
               ctx.lineTo(x1, idcanvas.height - 5);
               ctx.stroke();
               ctx.restore();
           }

           function drawResult(ctx)
           {
               if (countMess === 0)
                   return 0;
               if (blRun && checkDrawMessX(lmDataChart.get(countMess - 1).time))
               {
                   ctx.save();
                   var i = countMess - 1;
                   ctx.strokeStyle = "#0000ff";
                   ctx.lineWidth = 1;
                   ctx.beginPath();
                   ctx.arc(calcMessToX(lmDataChart.get(i).time), calcMessToY(lmDataChart.get(i).mess), 2, 0, Math.PI * 2, false);
                   for (; i > 0; --i)
                   {
                       ctx.moveTo(calcMessToX(lmDataChart.get(i).time), calcMessToY(lmDataChart.get(i).mess));
                       ctx.lineTo(calcMessToX(lmDataChart.get(i-1).time), calcMessToY(lmDataChart.get(i-1).mess));
                       if (!checkDrawMessX(lmDataChart.get(i-1).time))
                           break;
                       ctx.arc(calcMessToX(lmDataChart.get(i-1).time), calcMessToY(lmDataChart.get(i-1).mess), 2, 0, Math.PI * 2, false);
                   }
                   ctx.stroke();
                   ctx.restore();
               }
               else if (!blRun) // прокрутка
               {
                   ctx.save();
                   ctx.strokeStyle = "#0000ff";
                   ctx.lineWidth = 1;
                   ctx.beginPath();
                   for (var j = countMess - 1; j > 0; --j)
                   {
                       if (!checkDrawMessX(lmDataChart.get(j-1).time) && !checkDrawMessX(lmDataChart.get(j).time))
                       {
                           if (lmDataChart.get(j-1).time < ((new Date()) - 1000 * (maxSizeMess + shiftSec)))
                               break;
                           else
                               continue;
                       }
                       ctx.arc(calcMessToX(lmDataChart.get(j).time), calcMessToY(lmDataChart.get(j).mess), 2, 0, Math.PI * 2, false);
                       ctx.moveTo(calcMessToX(lmDataChart.get(j).time), calcMessToY(lmDataChart.get(j).mess));
                       ctx.lineTo(calcMessToX(lmDataChart.get(j-1).time), calcMessToY(lmDataChart.get(j-1).mess));
                       ctx.arc(calcMessToX(lmDataChart.get(j-1).time), calcMessToY(lmDataChart.get(j-1).mess), 2, 0, Math.PI * 2, false);
                   }
                   ctx.stroke();
                   ctx.restore();
               }
           }

            onPaint: {
                var ctx = idcanvas.getContext("2d");
                ctx.globalCompositeOperation = "source-over";
                drawBackground(ctx);
                if (inputIndicator.dopusk)
                    drawLimit(ctx);
                drawScroller(ctx);
                drawResult(ctx);
            }
        }
    }

}

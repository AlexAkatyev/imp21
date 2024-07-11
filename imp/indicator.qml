import QtQuick 2.5
import QtQuick.Controls 2.12
import QtMultimedia 5.5
import QtQuick.Layouts 1.12
import QtQml 2.12

Item
{
    property double scaleX: 1024
    onScaleXChanged: width = scaleX;
    property double scaleY: 768
    onScaleYChanged: height = scaleY;
    property int scaleBase: impGauge.width // for toPixels() only
    width: scaleX
    height: scaleY
    property int currentIndexMenuNumber: 1
    property bool audioEnable: false;
    onAudioEnableChanged: boolBuzzerPlay = getBuzzerPlay();
    property bool longPlay: false;
    onLongPlayChanged: timerBuzzer.interval = getLongPlay();
    property bool boolBuzzerPlay: getBuzzerPlay()
    onBoolBuzzerPlayChanged:
    {
        if (boolBuzzerPlay)
            timerBuzzer.start();
    }
    property bool blockRBChangeMode: false
    property bool deviationMode: false
    onDeviationModeChanged:
    {
        cbMode.checked = deviationMode
        lmMeasData.clear();
    }
    property real measMin: 0
    property real measMax: 0
    property real measDeviation: 0
    property int  prevTabIndex: 0
    property bool stateMeasure: false // true - идет замер
    property bool pauseMeasure: false // true - пауза в измерении
    property string textcsv: "";
    property bool setZero: false // нуль не установлен
    property real measValue: inputIndicator.mTranformFormulaReal + inputIndicator.zeroShift
    onMeasValueChanged:
    {
        if (stateMeasure && !pauseMeasure)
        {
            if (measMin > measValue)
            {
                measMin = measValue;
                measDeviation = measMax - measMin;
            }
            if (measMax < measValue)
            {
                measMax = measValue;
                measDeviation = measMax - measMin;
            }
        }
        boolBuzzerPlay = getBuzzerPlay();
    }

    property string highLevelFormula: "\nВерхнее предельное отклонение"
    property string lowLevelFormula: "\nНижнее предельное отклонение"

    property bool dopuskTest: inputIndicator.dopusk
    onDopuskTestChanged: dopusk.checked = inputIndicator.dopusk

    property bool clearMeasdata: false
    onClearMeasdataChanged:
    {
        if (clearMeasdata)
        {
            lmMeasData.clear();
            clearMeasdata = false;
        }
    }


    // сигнал изменения формулы
    signal sigChangeFormula();
    // Сигнал о необходимости обновления цены деления
    signal sigGetDivisionValue();
    // Сигнал об измениии допусков
    signal sigChangeLimit();
    // Сигнал об изменении способа показаний
    signal sigChangeIndication();
    // Сигнал нажатия кнопки открытия графика
    signal sigOpenChart();
    // Сигнал сохранения измерений
    signal sigClickedSave();
    signal sigAutoSave();
    // Сигнал ввода имени измерения
    signal sigNameEntered();
    // Сигнал изменения общего делителя формулы
    signal sigDividerEntered();
    // выбор файла для автосохранения
    signal sigPeekFile();
    // Send broadcast message with measurement
    signal sigSendMeasurementMessage(); // see measValue

    function toPixels(percentage) {
        return percentage * scaleBase / 100;
    }

    function fillTxtCsvForSave()
    {
        if (lmMeasData.count === 0)
            return 0;
        // Запись текста в файл
        textcsv = "Наименование индикатора;" + tfName.text + "\n";
        textcsv += "Формула";
        var strtemp = "";
        if (tfFactor1.text != "1")
            strtemp += tfFactor1.text + " * ";
        strtemp += cbListDetect1.currentText;
        if (tfIncert1.text != "0")
            strtemp += tfIncert1.text + " + ";
        if (cbListDetect2.currentIndex > 0)
        {
            if (tfFactor2.text != "1")
              strtemp += tfFactor2.text + " * ";
            strtemp += cbListDetect2.currentText;
            if (tfIncert2.text != "0")
              strtemp += tfIncert2.text + " + ";
        }
        textcsv += ";" + strtemp + "\n";
        if (deviationMode)
            strtemp = "Определение размаха";
        else
            strtemp = "Ручное измерение";
        textcsv += "Режим" + ";" + strtemp + "\n";
        textcsv += "Единица измерения;" + inputIndicator.messUnit + "\n";
        textcsv += "\n"; // пустая строка
        textcsv += "Произведено замеров;" + lmMeasData.count.toString() + "\n";
        textcsv += "Верхнее предельное значение;" + inputIndicator.highLimit.toString() + "\n";
        textcsv += "Нижнее предельное значение;" + inputIndicator.lowLimit.toString() + "\n";
        textcsv += "\n"; // пустая строка
        var i = 0;
        if (!deviationMode)
        {
            textcsv += "Номер замера;Время замера;Измеренное значение\n";
            for (; i < lmMeasData.count; i++)
                textcsv += (i + 1).toString()
                        + ";"
                        + Qt.formatTime(lmMeasData.get(i).time, "hh:mm:ss.zzz")
                        + ";"
                        + impGauge.round10(lmMeasData.get(i).meas, inputIndicator.accuracy)
                        + "\n";
        }
        else
        {
            textcsv += "Номер замера;MIN;MAX;MAX-MIN\n";
            for (; i < lmMeasData.count; i++)
            {
                textcsv += (i + 1).toString() +
                           ";" +
                           impGauge.round10(lmMeasData.get(i).min, inputIndicator.accuracy) +
                           ";" +
                           impGauge.round10(lmMeasData.get(i).max, inputIndicator.accuracy) +
                           ";" +
                           impGauge.round10(lmMeasData.get(i).deviation, inputIndicator.accuracy) +
                           "\n";
            }
        }
    }

    function saveMeasData()
    {
        if (lmMeasData.count === 0)
            return 0;
        fillTxtCsvForSave();
        sigClickedSave();
        return 1;
    }

    function autoSaveMeasData()
    {
        if (lmMeasData.count === 0)
            return 0;
        fillTxtCsvForSave();
        sigAutoSave();
        lmMeasData.clear();
        return 1;
    }


    function getFormula()
    {
        var formula = tfName.text + " : ";
        var factor1 = false;
        var factor2 = false;
        if (tfFactor1.text !== 0 && cbListDetect1.currentIndex !== 0)
        {
            formula = formula + tfFactor1.text + " * " + cbListDetect1.currentText;
            factor1 = true;
        }
        if (tfIncert1.text !== "0")
        {
            if (tfIncert1.text > 0)
                formula = formula + (factor1 ? " + " : "") + tfIncert1.text;
            else
                formula = formula + (factor1 ? " " : "") + tfIncert1.text;
            factor1 = true;
        }

        if (tfFactor2.text !== 0 && cbListDetect2.currentIndex !== 0)
        {
            formula = formula + (factor1 ? " + " : "") + tfFactor2.text + " * " + cbListDetect2.currentText;
            factor2 = true;
        }
        if (tfIncert2.text !== "0")
        {
            if (tfIncert2.text > 0)
                formula = formula + (factor1 | factor2 ? " + " : "") + tfIncert2.text;
            else
                formula = formula + (factor1 | factor2 ? " " : "") + tfIncert2.text;
        }
        return formula;
    }

    function getBuzzerPlay()
    {
//        return (((inputIndicator.mess + inputIndicator.zeroShift) > inputIndicator.highLimit) |
//                ((inputIndicator.mess + inputIndicator.zeroShift) < inputIndicator.lowLimit)) ? audioEnable : false;
        return (impGauge.getColorMessText() !== "black") ? (audioEnable & inputIndicator.dopusk) : false;
    }

    function getLongPlay()
    {
        return longPlay ? 1000 : 500;
    }

    function getInterval(h, l, i)
    {
        var intervals = i;
        if (intervals < 1)
            intervals = 1;
        return impGauge.round10((h-l)/intervals, inputIndicator.accuracy);
    }

    function setInterval()
    {
        intervalF.text = getInterval(tfHiLevelF.text, tfLoLevelF.text, countGroupsF.text);
    }

    function getSortGroup(m, hlevel, llevel, numbers)
    {
        var group = "";
        if (m > hlevel )
            group = "БРАК+";
        else if (m < llevel)
            group = "БРАК-";
        else
            group = numbers - Math.floor(numbers * (m - llevel) / (hlevel - llevel));
        return group;
    }

    function releaseMouseClicked()
    {
        if (deviationMode)
        {
            pauseMeasure = false;
            stateMeasure = !stateMeasure;
            if (stateMeasure) // начинается замер
            {
                measMin = measValue;
                measMax = measMin;
                measDeviation = 0;
            }
            else //замер окончен
                appendMeasDataToModel(false);
        }
    }

    function releaseData()
    {
        if (!deviationMode) // ручной замер
        {
            appendMeasDataToModel(true);
            sigSendMeasurementMessage();
        }
    }


    function appendMeasDataToModel(handle)
    {
        if (handle)
        {
            lmMeasData.append({"time":(new Date()), "meas":measValue});
            impGauge.peekMeasText = "Запись " + impGauge.round10(measValue, inputIndicator.accuracy);
        }
        else
        {
            lmMeasData.append({"min":measMin, "max":measMax, "deviation":measDeviation});
            impGauge.peekMeasText = "Запись min: "
                                    + impGauge.round10(measMin, inputIndicator.accuracy)
                                    + " max: "
                                    + impGauge.round10(measMax, inputIndicator.accuracy)
                                    + " dev: "
                                    + impGauge.round10(measDeviation, inputIndicator.accuracy);
        }
        impGauge.peekedData = true;
    }


    function setStatMode()
    {
        if (cbHand.checked)
        {
            statChart.blMode = false;
            statChart.enabled = true;
        }
        else if (cbAutomatic.checked)
        {
            statChart.blMode = true;
            statChart.enabled = true;
        }
        else
        {
            statChart.enabled = false;
            statChart.blRun = false;
        }
        statChart.clearModel();
        cbStatistic.checked = statChart.enabled;
    }

    property real mybeforeset: inputIndicator.beforeSet
    onMybeforesetChanged: {
        setTextBeforeSet();
    }

    function setTextBeforeSet()
    {
        var predustanov = impGauge.round10(inputIndicator.beforeSet, inputIndicator.accuracy);
        if (inputIndicator.beforeSet > 0)
            predustanov = "+" + predustanov;
        txBeforeSet.text = predustanov;
    }

    function receiptStatPeriod(stroka)
    {
        var mesSize = stroka;
        if (mesSize < 10)
        {
            statChart.period_ms = 10;
            tfStatPeriod.text = "10";
        }
        else if (mesSize > 10000)
        {
            statChart.period_ms = 10000;
            tfStatPeriod.text = "10000";
        }
        else
            statChart.period_ms = mesSize;
    }

    function receiptSumPoint(stroka)
    {
        statChart.clearModel();
        var mesSize = stroka;
        if (mesSize < 10)
        {
            statChart.maxSizeMess = 10;
            tfSumPoint.text = "10";
        }
        else if (mesSize > 1000)
        {
            statChart.maxSizeMess = 1000;
            tfSumPoint.text = "1000";
        }
        else
            statChart.maxSizeMess = mesSize;
    }

    function getNumberCharPointModel(theme)
    {
        if (theme === "mm")
            return ["0.001", "0.0001", "0.00001"];
        else if (theme === "inch")
            return ["0.000001", "0.0000001"];
        else if (theme === "angle seconds")
            return ["1", "0.1"];
        else // mkm
            return ["1", "0.1", "0.01", "0.001"];
    }

    function setIndicatorAccuracy()
    {
        var currentIndex = tfNumberCharPoint.currentIndex;
        if (currentIndex < 0)
            return;
        if (tgrb1.checked) // mm
            inputIndicator.accuracy = currentIndex + 3;
        else if (tgrb2.checked) // inch
            inputIndicator.accuracy = currentIndex + 6;
        else if (tgrb3.checked) // angle seconds
            inputIndicator.accuracy = currentIndex;
        else // mkm
            inputIndicator.accuracy = currentIndex;
    }

    function currentIndexFromAccuracy()
    {
        //inputIndicator.transGauge inputIndicator.accuracy
        if (inputIndicator.transGauge == 0) // mkm 0-3
            return inRange(0, 3, inputIndicator.accuracy) ? inputIndicator.accuracy : 1;
        else if (inputIndicator.transGauge == 1) // mm 3-5
            return inRange(3, 5, inputIndicator.accuracy) ? inputIndicator.accuracy - 3 : 0;
        else if (inputIndicator.transGauge == 2) // inch 6-7
            return inRange(6, 7, inputIndicator.accuracy) ? inputIndicator.accuracy - 6 : 0;
        else if (inputIndicator.transGauge == 3) // angle second 0-1
            return inRange(0, 1, inputIndicator.accuracy) ? inputIndicator.accuracy : 0;
        else
            return 0;
    }

    function inRange(l, h, d)
    {
        return d >= l && d <= h;
    }

    // Источник входных данных
    InputIndicator
    {
        id: inputIndicator
    }

    Timer
    {
        id: timerBuzzer
        repeat: false
        interval: getLongPlay()
        onRunningChanged:
        {
            if (running)
                auBuzzer.play();
            else
                auBuzzer.stop();
        }
    }

    Timer
    {
        id: timerSave
        objectName: "timerSave"
        repeat: true
        interval: tfAutoSave.text * 60 * 1000
        onTriggered: autoSaveMeasData();
    }

    Audio
    {
        id: auBuzzer
        source: "qrc:/1kHz_44100Hz_16bit_05sec.wav"
    }

    MouseArea
    {
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton
        onClicked: releaseMouseClicked()
    }

    ListModel { // Здесь хранятся данные измерений
        id: lmMeasData
    }

    Item {
        id: itMeasure
        anchors.fill: parent
        Button {
            id: btMenu
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            text: "Меню"
            width: parent.width / 5
            onReleased:
            {
                itMeasure.visible = false;
                itMenu.visible = true;
                tbMenu.currentIndex = currentIndexMenuNumber === 0 ? 1 : currentIndexMenuNumber;
            }
        }
        Button {
            id: btLeft
            anchors.left: btMenu.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible
            text: "<"
            width: btMenu.width
            onReleased: inputIndicator.beforeSet = inputIndicator.beforeSet - Math.pow(10, -inputIndicator.accuracy);
        }
        Button {
            id: btSetNull
            anchors.left: btLeft.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible
            text: setZero ? "«Ø»" : "«0»"
            width: parent.width / 5
            enabled: setZero | inputIndicator.enableSetZero;
            onReleased:
            {
                inputIndicator.zeroShift = setZero ? 0 : -inputIndicator.mess;
                setZero = !setZero;
            }
        }
        Button {
            id: btRight
            anchors.left: btSetNull.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible
            text: ">"
            width: btMenu.width
            onReleased: inputIndicator.beforeSet = inputIndicator.beforeSet + Math.pow(10, -inputIndicator.accuracy);
        }
        Button {
            id: btData
            anchors.left: btRight.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible && !itMin.visible
            text: "Data"
            width: btMenu.width
            onReleased: releaseData()
        }
        Button {
            id: btStartPause
            anchors.left: btRight.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible && itMin.visible
            icon.source: stateMeasure ? (pauseMeasure ? "qrc:/Play-1-Pressed-icon.png" : "qrc:/PauseNormal.png") : "qrc:/Play-1-Pressed-icon.png"
            width: btMenu.width / 2
            onReleased:
            {
                if (stateMeasure)
                    pauseMeasure = !pauseMeasure;
                else
                    releaseMouseClicked();
            }
        }
        Button {
            id: btStop
            anchors.left: btStartPause.right
            anchors.bottom: parent.bottom
            visible: btStartPause.visible
            icon.source: "qrc:/Ionic-Ionicons-Stop-512.png"
            width: btMenu.width / 2
            onReleased: releaseMouseClicked()
            enabled: stateMeasure
            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: 5
                anchors.topMargin: 5
                radius: 4
                color: "tomato"
                opacity: stateMeasure ? 0.5 : 0
            }
        }
        // Окно сортировки
        Item {
            id: itSortDisplay
            visible: cbSortFormula.checked
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: btMenu.top
            Grid {
                id: sortGrid
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                columns: 2
                rows: 8
                rowSpacing: 15
                columnSpacing: 100
                // ----------------------------------
                Text {
                    text: "\nКанал"
                }
                Text {
                    id: colHeadF
                    text: tfName.text
                    font.pixelSize: 25
                }
                // ----------------------------------
                Text {
                    text: "Количество групп"
                }
                Text {
                    text: countGroupsF.text
                }
                // ----------------------------------
                Text {
                    text: "Интервал"
                }
                Text {
                    text: getInterval(tfHiLevelF.text, tfLoLevelF.text, countGroupsF.text)
                }
                // ----------------------------------
                Text {
                    text: "ВПО"
                }
                Text {
                    text: tfHiLevelF.text
                }
                // ----------------------------------
                Text {
                    text: "НПО"
                }
                Text {
                    text: tfLoLevelF.text
                }
                // ----------------------------------
                Text {
                    id: headMess
                    text: " "
                    font.pixelSize: 22
                }
                Text {
                    text: impGauge.round10(inputIndicator.mTranformFormulaReal, inputIndicator.accuracy)
                    font.pixelSize: headMess.font.pixelSize
                    onTextChanged: tGroupF.text = getSortGroup(inputIndicator.mTranformFormulaReal,
                                                               tfHiLevelF.text,
                                                               tfLoLevelF.text,
                                                               countGroupsF.text);
                }
                // ----------------------------------
                Text {
                    text: "Единица измерения"
                }
                Text {
                    text: inputIndicator.messUnit === "" ? " " : inputIndicator.messUnit
                }
                // ----------------------------------
                Text {
                    id: headGroup
                    text: "ГРУППА"
                    font.pixelSize: 30
                }
                Text {
                    id: tGroupF
                    text: "0"
                    font.pixelSize: headGroup.font.pixelSize
                }
            }
        }
        // Стрелочный индикатор
        Item {
            id: itMain
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: btMenu.top
            Item {
                id: itMin
                visible: deviationMode & itMainGauge.visible
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: parent.width / 3
                height: visible ? parent.height / 20 : 0
                Text {
                    id: tMeasMin
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width/2
                    text: impGauge.round10(measMin, inputIndicator.accuracy)
                    font.pixelSize: 12
                }
                Text {
                    id: txtMin
                    anchors.right: tMeasMin.left
                    text: "min : "
                    font.pixelSize: tMeasMin.font.pixelSize
                }
            }
            Item {
                id: itMax
                visible: itMin.visible
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: itMin.width
                height: itMin.height
                Text {
                    id: tMeasMax
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width/2
                    text: impGauge.round10(measMax, inputIndicator.accuracy)
                    font.pixelSize: tMeasMin.font.pixelSize
                }
                Text {
                    id: txtMax
                    anchors.right: tMeasMax.left
                    text: "max : "
                    font.pixelSize: tMeasMin.font.pixelSize
                }
            }
            Item {
                id: itMinMax
                visible: itMin.visible
                anchors.bottom: parent.bottom
                anchors.left: itMin.right
                anchors.right: itMax.left
                height: itMin.height
                Text {
                    id: tMeasMaxMin
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width/2
                    text: impGauge.round10(measDeviation, inputIndicator.accuracy)
                    font.pixelSize: tMeasMin.font.pixelSize
                }
                Text {
                    id: txtMaxMin
                    anchors.right: tMeasMaxMin.left
                    text: "max-min : "
                    font.pixelSize: tMeasMin.font.pixelSize
                }
            }
            Item {
                id: itBeforeSet
                anchors.top: parent.top
                anchors.right: parent.right
                width: parent.width / 4
                visible: Math.abs(inputIndicator.beforeSet) > 0.5 * Math.pow(10,-inputIndicator.accuracy)
                Text {
                    id: txBeforeSet
                    anchors.top: parent.top
                    anchors.right: parent.right
                    text: setTextBeforeSet()
                    color: "red"
                }
                Text {
                    id: txNameBeforeSet
                    anchors.top: parent.top
                    anchors.right: txBeforeSet.left
                    text: "Предустанов : "
                    color: "red"
                }
            }
            Item { // непосредственно стрелка
                id: itMainGauge
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: itMin.top
                visible:  !itSortDisplay.visible & !statChart.visible
                Item {
                    id: itStrelka
                    anchors.fill: parent
                    visible: cbStrelka.checked
                    ImpGauge
                    {
                        id: impGauge
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width < parent.height ? parent.width : parent.height
                        height: parent.width < parent.height ? parent.width : parent.height
                    }
                    // Окраска циферблата при выходе за допуски
                    Rectangle
                    {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: impGauge.width
                        height: impGauge.height
                        radius: width/2
                        color: "red"
                        opacity: (((inputIndicator.mess + inputIndicator.zeroShift) > inputIndicator.highLimit) |
                                  ((inputIndicator.mess + inputIndicator.zeroShift) < inputIndicator.lowLimit)) ? 0.1 : 0
                        OpacityAnimator {
                            loops: 1
                            to: 0.1
                            duration: 10
                        }
                        OpacityAnimator {
                            loops: 1
                            to: 0
                            duration: 10
                        }
                        onOpacityChanged: boolBuzzerPlay = getBuzzerPlay();
                        visible: impGauge.visible & inputIndicator.dopusk
                    }
                }
                Item {
                    id: itZifra
                    anchors.fill: parent
                    visible: cbZifra.checked
                    Column {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 5
                        Text {
                            id: mUnit
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: inputIndicator.messUnit
                        }
                        Text {
                            id: measIndicate
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: impGauge.exorbitantFilter(impGauge.round10(inputIndicator.mess + inputIndicator.zeroShift, inputIndicator.accuracy))
                            color: impGauge.getColorMessText()
                            font.pixelSize: impGauge.toPixels(0.9)
                        }
                        Grid {
                            columns: 2
                            rows: 2
                            spacing: 10
                            anchors.horizontalCenter: parent.horizontalCenter
                            Text {
                                text: "ВПО      "
                            }
                            Text {
                                text: impGauge.round10(inputIndicator.highLimit, inputIndicator.accuracy)
                            }
                            Text {
                                text: "НПО      "
                            }
                            Text {
                                text: impGauge.round10(inputIndicator.lowLimit, inputIndicator.accuracy)
                            }
                        }
                    }
                }
            }
        }
        // График
        StatChart {
            id: statChart
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: btMenu.top
            visible: cbStatistic.checked
            dbHiLimit: Math.round(tfHiLimit.text);
            dbLoLimit: Math.round(tfLoLimit.text);
            iGaugeUnit: Math.round(tfUnitPoint.text);
            strMessUnit: inputIndicator.messUnit;
            txFormula: getFormula();
            enabled: false;
            opacity: 0.5;
            onEnabledChanged: opacity = enabled ? 1 : 0.5;
            maxSizeMess: tfSumPoint.text
            period_ms: tfStatPeriod.text
            maxLenMess: tfMeasLen.text * 1000
        }
        Text {
            anchors.horizontalCenter: statChart.horizontalCenter
            anchors.bottom: statChart.bottom
            anchors.bottomMargin: 80
            visible: statChart.visible
            opacity: 0.7
            font.pixelSize: 32
            text: impGauge.exorbitantFilter(impGauge.round10(inputIndicator.mess + inputIndicator.zeroShift, inputIndicator.accuracy))
            color: impGauge.getColorMessText()
        }
    }

    Item {
        id: itMenu
        anchors.fill: parent
        visible: false
        TabBar
        {
            id: tbMenu // здесь хранятся все настройки индикатора
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            Repeater
            {
                model: ["ВЫХОД",
                        "ИЗМЕРЕНИЕ",
                        "ДОПУСК",
                        "ДИСПЛЕЙ",
                        "СОРТИРОВКА",
                        "СТАТИСТИКА"]
                TabButton
                {
                    text: modelData
                    font.pixelSize: 10
                    ToolTip.visible: hovered
                    ToolTip.text: text
                }
            }
            onCurrentIndexChanged:
            {
                switch (prevTabIndex)
                {
                case 1: sigChangeFormula();
                        statChart.strMessUnit = inputIndicator.messUnit;
                        statChart.txFormula = getFormula();
                        break;
                case 2: sigChangeLimit();
                        statChart.dbHiLimit = tfHiLimit.text;
                        statChart.dbLoLimit = tfLoLimit.text;
                        sigChangeIndication();
                        break;
                case 3: sigGetDivisionValue();
                        statChart.iGaugeUnit = Math.round(tfUnitPoint.text);
                        setTextBeforeSet();
                        setIndicatorAccuracy();
                        break;
                case 4: sigChangeLimit();
                        statChart.dbHiLimit = tfHiLimit.text;
                        statChart.dbLoLimit = tfLoLimit.text;
                        break;
                case 5: receiptStatPeriod(tfStatPeriod.text);
                        receiptSumPoint(tfSumPoint.text);
                        statChart.blRun = false;
                        break;
                default:
                }

                currentIndexMenuNumber = prevTabIndex;
                prevTabIndex = currentIndex;

                if (currentIndex === 0)
                {
                    itMenu.visible = false;
                    itMeasure.visible = true;
                }
            }
        }

        StackLayout
        {
            id: slTabs
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: tbMenu.top
            currentIndex: tbMenu.currentIndex
            Item {
            }
            // Страница ИЗМЕРЕНИЕ
            Item {
                id: itFormula
                Column {
                    id: colLeft
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 5

                    Row {
                        spacing: colLeft.spacing
                        Text { // row 1
                            text: "\nНазвание:    "
                        }
                        TextField {
                            id: tfName
                            objectName: "tfName"
                            text: "Индикатор ?"
                            font.pixelSize: 12
                            onTextEdited: sigNameEntered()
                        }
                    }

                    Grid {
                        rows: 4
                        columns: 5
                        spacing: colLeft.spacing
                        horizontalItemAlignment: Grid.AlignHCenter
                        verticalItemAlignment: Grid.AlignVCenter
                        // row 1
                        Text {
                            text: " "
                        }
                        Text {
                            text: " "
                        }
                        Text {
                            text: "Формула индикатора"
                        }
                        Text {
                            text: " "
                        }
                        Text {
                            text: " "
                        }
                        // row 2
                        TextField {
                            id: tfFactor1
                            objectName: "tfFactor1"
                            text: "1"
                            width: itFormula.width / 28
                            font.pixelSize: tfName.font.pixelSize
                            ToolTip.text: "Множитель датчика 1"
                            ToolTip.visible: hovered
                        }
                        Text {
                            text: "*"
                        }
                        ComboBox{
                            id: cbListDetect1
                            objectName: "cbListDetect1"
                            indicator.scale: 0.6
                            width: itFormula.width / 2
                            font.pixelSize: tfName.font.pixelSize
                            ToolTip.text: "Датчик 1"
                            ToolTip.visible: hovered
                            popup.font.pixelSize: cbListDetect1.font.pixelSize
                            Rectangle // подсветка датчика при выходе за калибровочный диапазон
                            {
                                anchors.fill: parent
                                anchors.topMargin: parent.height/8
                                anchors.bottomMargin: parent.height/8
                                color: "cyan"
                                opacity: inputIndicator.blOverRange1 ? 0.3 : 0.0
                            }
                        }
                        Text {
                            text: "+"
                        }
                        TextField {
                            id: tfIncert1
                            objectName: "tfIncert1"
                            text: "0"
                            width: tfFactor1.width
                            font.pixelSize: tfName.font.pixelSize
                            ToolTip.text: "Предустанов датчика 1"
                            ToolTip.visible: hovered
                        }
                        // row 3
                        Text {
                            text: " "
                        }
                        Text {
                            text: " "
                        }
                        Text {
                            text: "+"
                        }
                        Text {
                            text: " "
                        }
                        Text {
                            text: " "
                        }
                        // row 4
                        TextField {
                            id: tfFactor2
                            objectName: "tfFactor2"
                            text: "1"
                            width: tfFactor1.width
                            font.pixelSize: tfName.font.pixelSize
                            ToolTip.text: "Множитель датчика 2"
                            ToolTip.visible: hovered
                        }
                        Text {
                            text: "*"
                        }
                        ComboBox{
                            id: cbListDetect2
                            objectName: "cbListDetect2"
                            indicator.scale: 0.6
                            font.pixelSize: tfName.font.pixelSize
                            width: cbListDetect1.width
                            ToolTip.text: "Датчик 2"
                            ToolTip.visible: hovered
                            popup.font.pixelSize: cbListDetect2.font.pixelSize
                            Rectangle // подсветка датчика при выходе за калибровочный диапазон
                            {
                                anchors.fill: parent
                                anchors.topMargin: parent.height/8
                                anchors.bottomMargin: parent.height/8
                                color: "cyan"
                                opacity: inputIndicator.blOverRange2 ? 0.3 : 0.0
                            }
                        }
                        Text {
                            text: "+"
                        }
                        TextField {
                            id: tfIncert2
                            objectName: "tfIncert2"
                            text: "0"
                            width: tfIncert1.width
                            font.pixelSize: tfName.font.pixelSize
                            ToolTip.text: "Предустанов датчика 2"
                            ToolTip.visible: hovered
                        }
                    }

                    Grid {
                        rows: 5
                        columns: 2
                        spacing: 0
                        Text { // row 1
                            text: "\nДелитель:"
                        }
                        TextField {
                            id: tfDivider
                            objectName: "tfDivider"
                            text: "1"
                            font.pixelSize: tfName.font.pixelSize
                            onTextEdited: sigDividerEntered()
                        }
                        Text { // row 2
                            text: "\nУсреднение, мс"
                        }
                        TextField {
                            id: tfPeriod
                            objectName: "tfPeriod"
                            font.pixelSize: tfName.font.pixelSize
                            validator: IntValidator{bottom: 0; top: 100000;}
                            text: "0"
                            inputMethodHints: Qt.ImhDigitsOnly
                            onTextChanged:
                            {
                                if (text > 100000)
                                    text = 100000;
                            }
                        }
                        Text { // row 3
                            text: "\nРежим max - min:"
                        }
                        CheckBox {
                            id: cbMode
                            checked: false
                            onCheckedChanged:
                            {
                                if (checked)
                                {
                                    blockRBChangeMode = true;
                                    deviationMode = true;
                                }
                                else
                                {
                                    blockRBChangeMode = true;
                                    deviationMode = false;
                                }
                            }
                        }
                        CheckBox { // row 4
                            id: automaticSave
                            objectName: "automaticSave"
                            text: "Автоматическое\nсохранение, мин"
                            checked: false
                            font.pixelSize: tfName.font.pixelSize
                            onReleased: {
                                if (checked) {
                                    timerSave.start();
                                    btSaveOption.text = "Выбрать файл выгрузки"
                                }
                                else {
                                    timerSave.stop();
                                    btSaveOption.text = "  Сохранить измерения  "
                                }
                            }
                        }
                        TextField {
                            id: tfAutoSave
                            objectName: "tfAutoSave"
                            font.pixelSize: tfName.font.pixelSize
                            validator: IntValidator{bottom: 1; top: 999;}
                            text: "10"
                            inputMethodHints: Qt.ImhDigitsOnly
                            onTextChanged:
                            {
                                if (text < 1)
                                    text = 1;
                                timerSave.interval = text * 60 * 1000;
                            }
                            enabled: automaticSave.checked
                        }
                        Button // row 5
                        {
                            id: btSaveOption
                            text: automaticSave.checked ? "Выбрать файл выгрузки" : "  Сохранить измерения  "
                            font.pixelSize: tfName.font.pixelSize
                            enabled: automaticSave.checked || lmMeasData.count > 0
                            onReleased: {
                                if (automaticSave.checked)
                                    sigPeekFile();
                                else
                                    saveMeasData();
                            }
                        }
                    }
                }

            }
            // Страница кнопки ДОПУСК
            Item {
                id: itDopusk
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    CheckBox {
                        id: dopusk
                        text: "Контроль допуска"
                        checked: inputIndicator.dopusk
                        onCheckedChanged: inputIndicator.dopusk = checked
                    }
                    Grid {
                        rows: 4
                        columns: 2
                        spacing: 2
                        enabled: inputIndicator.dopusk
                        Text {
                            text: highLevelFormula + "   "
                        }
                        TextField {
                            id: tfHiLimit
                            objectName: "tfHiLimit"
                            width: itDopusk.width/3
                            font.pixelSize: 12
                        }
                        Text {
                            text: lowLevelFormula + "   "
                        }
                        TextField {
                            id: tfLoLimit
                            objectName: "tfLoLimit"
                            width: tfHiLimit.width
                            font.pixelSize: tfHiLimit.font.pixelSize
                        }
                        Text {
                            text: "\nПриемочная граница    "
                        }
                        TextField {
                            id: tfPriemka
                            objectName: "tfPriemka"
                            text: inputIndicator.priemka
                            width: tfHiLimit.width
                            font.pixelSize: tfHiLimit.font.pixelSize
                        }
                        Text {
                            text: "\nЗвуковое оповещение   "
                        }
                        ComboBox {
                            model: ["Выключено", "Короткое", "Длинное"]
                            width: tfHiLimit.width
                            onCurrentIndexChanged:
                            {
                                switch (currentIndex)
                                {
                                case 0:
                                    audioEnable = false;
                                    break;
                                case 1:
                                    longPlay = false;
                                    audioEnable = true;
                                    break;
                                case 2:
                                    longPlay = true;
                                    audioEnable = true;
                                }
                            }

                        }
                    }
                }
            }
            // Страница кнопки ДИСПЛЕЙ
            Item {
                id: itGauge
                Column{
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    Grid {
                        columns: 2
                        rows: 3
                        spacing: 5
                        CheckBox {
                            id: cbStrelka
                            text: "Цифровое и стрелочное\nотображение показаний"
                            font.pixelSize: 12
                            checked: true
                            onCheckedChanged: cbZifra.checked = !checked
                        }
                        CheckBox {
                            id: cbZifra
                            text: "Цифровое\nотображение показаний"
                            font.pixelSize: cbStrelka.font.pixelSize
                            checked: false
                            onCheckedChanged: cbStrelka.checked = !checked
                        }
                        Text {
                            text: "\nЦена деления"
                        }
                        TextField{
                            id: tfUnitPoint
                            objectName: "tfUnitPoint"
                            width: itGauge.width/3
                            font.pixelSize: 14
                            inputMethodHints: Qt.ImhDigitsOnly
                        }
                        Text {
                            text: "\nДискретность"
                        }
                        ComboBox {
                            id: tfNumberCharPoint
                            objectName: "tfNumberCharPoint"
                            model: getNumberCharPointModel("mkm")
                            currentIndex: 1
                            width: itGauge.width/4
                            font.pixelSize: 14
                        }
                    }
                    GroupBox {
                        title: "Единицы измерения"
                        RowLayout {
                            anchors.fill: parent
                            spacing: 0;
                            onVisibleChanged:
                            {
                                if (!visible)
                                    return;
                                switch (inputIndicator.transGauge){
                                case 1: tgrb1.checked = true; // mm
                                    tfNumberCharPoint.model = getNumberCharPointModel("mm");
                                    break;
                                case 2: tgrb2.checked = true; // inch
                                    tfNumberCharPoint.model = getNumberCharPointModel("inch");
                                    break;
                                case 3: tgrb3.checked = true; // angle seconds
                                    tfNumberCharPoint.model = getNumberCharPointModel("angle seconds");
                                    break;
                                default: tgrb0.checked = true; //mkm
                                    tfNumberCharPoint.model = getNumberCharPointModel("mkm");
                                    break;
                                }
                            }

                            RadioButton {
                                id: tgrb0
                                text: "мкм";
                                onCheckedChanged:
                                {
                                    inputIndicator.transGauge = 0;
                                    tfNumberCharPoint.model = getNumberCharPointModel("mkm");
                                    tfNumberCharPoint.currentIndex = currentIndexFromAccuracy();
                                }
                            }
                            RadioButton {
                                id: tgrb1
                                text: "мм"
                                onCheckedChanged:
                                {
                                    inputIndicator.transGauge = 1;
                                    tfNumberCharPoint.model = getNumberCharPointModel("mm");
                                    tfNumberCharPoint.currentIndex = currentIndexFromAccuracy();
                                }
                            }
                            RadioButton {
                                id: tgrb2
                                text: "дюймы"
                                onCheckedChanged:
                                {
                                    inputIndicator.transGauge = 2;
                                    tfNumberCharPoint.model = getNumberCharPointModel("inch");
                                    tfNumberCharPoint.currentIndex = currentIndexFromAccuracy();
                                }
                            }
                            RadioButton {
                                id: tgrb3
                                text: "угл. секунды"
                                onCheckedChanged:
                                {
                                    inputIndicator.transGauge = 3;
                                    tfNumberCharPoint.model = getNumberCharPointModel("angle seconds");
                                    tfNumberCharPoint.currentIndex = currentIndexFromAccuracy();
                                }
                            }
                        }
                    }
                    Button {
                        id: btSetDefault
                        text: "Установить настройки по умолчанию"
                        onReleased: {
                            tfUnitPoint.text = 5;
                            tgrb0.checked = true; // mkm
                            tfNumberCharPoint.currentIndex = 1;
                            tfPriemka.text = 0;
                            inputIndicator.priemka = 0;
                            cbSortFormula.checked = false; // no sort
                            cbStatistic.checked = false; // no grafic
                            cbMode.checked = false; // no max-min
                            deviationMode = false;
                            statChart.blRun = false;
                            inputIndicator.beforeSet = 0;
                        }
                    }
                }

            }
            // Страница кнопки СОРТИРОВКА
            Item {
                id: itSort
                Grid {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    columns: 2
                    rows: 5
                    spacing: 15
                    // row 1
                    Text {
                        text: "\nРежим сортировки"
                    }
                    CheckBox {
                        id: cbSortFormula
                        objectName: "cbSortFormula"
                        checked: false
                        onCheckedChanged: {
                            if (checked)
                                cbStatistic.checked = false;
                        }
                    }
                    // row 2
                    Text {
                        text: highLevelFormula
                    }
                    TextField {
                        id: tfHiLevelF
                        objectName: "tfHiLevelF"
                        text: inputIndicator.highLimit
                        onTextChanged: {
                            setInterval();
                            tfHiLimit.text = text;
                        }
                    }
                    // row 3
                    Text {
                        text: lowLevelFormula
                    }
                    TextField {
                        id: tfLoLevelF
                        objectName: "tfLoLevelF"
                        text: inputIndicator.lowLimit
                        onTextChanged: {
                            setInterval();
                            tfLoLimit.text = text;
                        }
                    }
                    // row 4
                    Text {
                        text: "\nКоличество групп"
                    }
                    TextField {
                        id: countGroupsF
                        objectName: "countGroupsF"
                        inputMethodHints: Qt.ImhDigitsOnly
                        onTextChanged: setInterval()
                    }
                    // row 5
                    Text {
                        text: "\nИнтервал"
                    }
                    Row {
                        Button {
                            id: btLeftInterval
                            text: "<"
                            scale: 0.7
                            onReleased: {
                                var groups = countGroupsF.text;
                                if (groups > 1)
                                    groups = groups - 1;
                                countGroupsF.text = groups;
                            }
                        }
                        Text {
                            id: intervalF
                            objectName: "intervalF"
                            anchors.verticalCenter: btLeftInterval.verticalCenter
                            text: getInterval(tfHiLevelF.text, tfLoLevelF.text, countGroupsF.text)
                            font.pixelSize: 16
                            width: btLeftInterval.width
                        }
                        Button {
                            text: ">"
                            scale: btLeftInterval.scale
                            onReleased: {
                                var groups = countGroupsF.text - 1;
                                if (groups < 99)
                                    groups = groups + 2;
                                countGroupsF.text = groups;
                            }
                        }
                    }
                }
            }
            // Страница кнопки СТАТИСТИКА
            Item {
                id: itStatistic
                Column {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    CheckBox {
                        id: cbStatistic
                        text: "Режим отображения графика"
                        enabled: false
                        checked: false
                        font.pixelSize: 12
                        onCheckedChanged: {
                            if (checked)
                                cbSortFormula.checked = false;
                        }
                    }
                    CheckBox {
                        id: cbHand
                        text: "Ручная фиксация измерений"
                        font.pixelSize: cbStatistic.font.pixelSize
                        onCheckedChanged: {
                            if (checked)
                                cbAutomatic.checked = false;
                            setStatMode();
                        }
                    }
                    CheckBox {
                        id: cbAutomatic
                        text: "Автоматическая фиксация измерений"
                        font.pixelSize: cbStatistic.font.pixelSize
                        onCheckedChanged: {
                            if (checked)
                                cbHand.checked = false;
                            setStatMode();
                        }
                    }
                    Grid {
                        rows: 4
                        columns: 2
                        spacing: 2
                        Text
                        {
                            text: "\nШирина окна графика, с"
                        }
                        TextField
                        {
                            id: tfSumPoint
                            objectName: "tfSumPoint"
                            enabled: cbAutomatic.checked | cbHand.checked
                            width: itStatistic.width/4
                            font.pixelSize: cbStatistic.font.pixelSize
                            validator: IntValidator{bottom: 10; top: 1000;}
                            text: "100"
                            onTextChanged: statChart.maxSizeMess = text
                        }
                        Text
                        {
                            text: "\nКоличество точек"
                        }
                        TextField
                        {
                            id: tfPointCount
                            width: tfSumPoint.width
                            enabled: cbAutomatic.checked
                            font.pixelSize: cbStatistic.font.pixelSize
                            validator: IntValidator{bottom: 10; top: 1000000;}
                            text: "1000"
                            onTextEdited: {
                                if (cbAutomatic.checked)
                                {
                                    var data = tfPointCount.text * tfStatPeriod.text;
                                    tfMeasLen.text = (data / 1000).toString();
                                }
                            }
                        }
                        Text
                        {
                            text: "\nИнтервал измерений, мс"
                        }
                        TextField
                        {
                            id: tfStatPeriod
                            objectName: "tfStatPeriod"
                            width: tfSumPoint.width
                            enabled: cbAutomatic.checked
                            font.pixelSize: tfSumPoint.font.pixelSize
                            validator: IntValidator{bottom: 100; top: 10000;}
                            text: "100"
                            onTextEdited: {
                                if (cbAutomatic.checked)
                                {
                                    var data = tfMeasLen.text / tfStatPeriod.text;
                                    tfPointCount.text = Math.round(data * 1000).toString();
                                }
                            }
                        }
                        Text
                        {
                            text: "\nПродолжительность измерений, с  "
                        }
                        TextField
                        {
                            id: tfMeasLen
                            width: tfSumPoint.width
                            enabled: cbAutomatic.checked
                            font.pixelSize: tfSumPoint.font.pixelSize
                            validator: IntValidator{bottom: 10; top: 90000;}
                            text: "100"
                            onTextEdited: {
                                if (cbAutomatic.checked)
                                {
                                    var data = tfMeasLen.text / tfStatPeriod.text;
                                    tfPointCount.text = Math.round(data * 1000).toString();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

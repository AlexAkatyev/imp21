﻿import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtMultimedia 5.12
import QtQuick.Layouts 1.12
import QtQml 2.12
import QtQuick.Dialogs 1.2

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
    property real groupNumber: 0
    property int  prevTabIndex: 0
    property bool stateMeasure: false // true - идет замер
    property bool pauseMeasure: false // true - пауза в измерении
    property string textcsv: "";
    property bool setZero: false // нуль не установлен
    property real measValue: inputIndicator.mTranformFormulaReal
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
    property real sendData: 0.0

    property string highLevelFormula: "Верхнее предельное отклонение"
    property string lowLevelFormula: "Нижнее предельное отклонение"

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
    // Сигнал ввода имени измерения
    signal sigNameEntered();
    // Сигнал изменения общего делителя формулы
    signal sigDividerEntered();
    // выбор файла для автосохранения
    signal sigPeekFile();
    // Send broadcast message with measurement
    signal sigSendMeasurementMessage(); // see sendData
    // Установить/сбросить 0 позицию датчиков
    signal sigSetZeroShift();
    // нажата клавиша Data
    signal sigReleaseData();
    // передать текст формулы
    signal analyseComplexFormula(string inputText);
    signal sigEnableComplexFormula(bool en);

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
        return (impGauge.getColorMessText() !== impStyle.baseTextColor) ? (audioEnable & inputIndicator.dopusk) : false;
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
        intervalF.text = getInterval(tfHiLevelF.text, tfLoLevelF.text, countGroupsF.value);
    }


    function getSortGroup(m, hlevel, llevel, numbers)
    {
        var group = "";
        if (m > hlevel )
        {
            group = "БРАК+";
            groupNumber = numbers + 1;
        }
        else if (m < llevel)
        {
            group = "БРАК-";
            groupNumber = 0;
        }
        else
        {
            groupNumber = Math.floor(numbers * (m - llevel) / (hlevel - llevel)) + 1;
            group = groupNumber;
        }
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
            {
                appendMeasDataToModel(false);
                sigSendMeasurementMessage();
            }
        }
    }

    function releaseData()
    {
        if (!deviationMode) // ручной замер
        {
            appendMeasDataToModel(true);
            sendData = measValue;
        }
        else
        {
            sendData = measDeviation;
        }

        sigSendMeasurementMessage();
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
            tfStatPeriod.text = "10";
        }
        else if (mesSize > 10000)
        {
            tfStatPeriod.text = "10000";
        }
    }

    function receiptSumPoint(stroka)
    {
        var mesSize = stroka;
        if (mesSize < 10)
        {
            tfSumPoint.text = "10";
        }
        else if (mesSize > 1000)
        {
            tfSumPoint.text = "1000";
        }
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


    ImpStyle
    {
        id: impStyle
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
        interval: tfAutoSave.text * 1000
        onTriggered:
        {
            releaseData();
            sigReleaseData();
        }
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


    ColorDialog
    {
        id: colorDialog
        title: "Выберите цвет фона окна"
        modality: Qt.ApplicationModal
        visible: false
        onAccepted:
        {
            backRect.color = colorDialog.color;
        }
    }


    Item {
        id: itMeasure
        anchors.fill: parent

        Rectangle
        {
            id: backRect
            anchors.fill: parent
            color: impStyle.windowColor
        }

        Row {
            height: 50
            width: parent.width
            anchors.bottom: parent.bottom
        }

        Button {
            id: btPalette
            icon.name: "palette"
            icon.source: "icons/palette.png"
            background: Rectangle {
                color: btPalette.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                radius: 5
                height: 50
                width: 50
                anchors.centerIn: parent
            }
            onReleased:
            {
                colorDialog.color = impStyle.windowColor
                colorDialog.open();
            }
        }

        Button {
            id: btMenu
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            icon.name: "menu"
            icon.source: "icons/menu.png"
            width: parent.width / 5
            background: Rectangle {
                color: btMenu.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                radius: 5
                width: Math.min(btMenu.width)-5
                anchors.centerIn: parent
            }
            onReleased:
            {
                itMeasure.visible = false;
                itMenu.visible = true;
                tbMenu.currentIndex = currentIndexMenuNumber === 0 ? 1 : currentIndexMenuNumber;
            }
            ToolTip.text: "Меню"
            ToolTip.visible: hovered
        }
        Button {
            id: btLeft
            anchors.left: btMenu.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible
            icon.name: "left"
            icon.source: "icons/left.png"
            width: btMenu.width
            background: Rectangle {
                color: btLeft.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                radius: 5
                width: Math.min(btMenu.width)-5
                anchors.centerIn: parent
            }
            onReleased: inputIndicator.beforeSet = inputIndicator.beforeSet - Math.pow(10, -inputIndicator.accuracy);
        }
        Button {
            id: btSetNull
            anchors.left: btLeft.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible
            text: setZero ? "«Ø»" : "«0»"
            height: btMenu.height
            width: btMenu.width
            enabled: setZero | inputIndicator.enableSetZero;
            background: Rectangle {
                color: btSetNull.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                radius: 5
                width: Math.min(btMenu.width)-5
                anchors.centerIn: parent
            }
            onReleased:
            {
                setZero = !setZero;
                sigSetZeroShift();
            }
        }
        Button {
            id: btRight
            anchors.left: btSetNull.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible
            icon.name: "right"
            icon.source: "icons/right.png"
            width: btMenu.width
            background: Rectangle {
                color: btRight.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                radius: 5
                width: Math.min(btMenu.width)-5
                anchors.centerIn: parent
            }
            onReleased: inputIndicator.beforeSet = inputIndicator.beforeSet + Math.pow(10, -inputIndicator.accuracy);
        }
        Button {
            id: btData
            anchors.left: btRight.right
            anchors.bottom: parent.bottom
            visible: itMainGauge.visible && !itMin.visible
            icon.name: "save"
            icon.source: "icons/save.png"
            width: btMenu.width
            background: Rectangle {
                color: btData.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                radius: 5
                width: Math.min(btMenu.width)-5
                anchors.centerIn: parent
            }
            onReleased:
            {
                releaseData();
                sigReleaseData();
            }
            ToolTip.text: "Запись"
            ToolTip.visible: hovered
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
                color: impStyle.warningColor
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
                    text: countGroupsF.value
                }
                // ----------------------------------
                Text {
                    text: "Интервал"
                }
                Text {
                    text: intervalF.text
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
                                                               countGroupsF.value);
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
                height: visible ? (parent.height < parent.width ? parent.height : parent.width) / 20 : 0
                Text {
                    id: tMeasMin
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width/2
                    text: impGauge.round10(measMin, inputIndicator.accuracy)
                    font.pixelSize: parent.height - 2
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
                    text: "Δm : "
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
                    anchors.rightMargin: 4
                    text: setTextBeforeSet()
                    color: impStyle.warningColor
                }
                Text {
                    id: txNameBeforeSet
                    anchors.top: parent.top
                    anchors.right: txBeforeSet.left
                    text: "Предустанов : "
                    color: impStyle.warningColor
                }
            }
            Item { // непосредственно стрелка
                id: itMainGauge
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: itMin.top
                visible:  !itSortDisplay.visible
                Item {
                    id: itStrelka
                    anchors.fill: parent
                    visible: cbStrelka.checked
                    ImpGauge
                    {
                        id: impGauge
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width < parent.height ? parent.width - 10 : parent.height - 10
                        height: parent.width < parent.height ? parent.width - 10 : parent.height - 10
                    }
                    // Окраска циферблата при выходе за допуски
                    Rectangle
                    {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: impGauge.width
                        height: impGauge.height
                        radius: width/2
                        color: impStyle.warningColor
                        opacity: ((inputIndicator.mess > inputIndicator.highLimit) |
                                  (inputIndicator.mess < inputIndicator.lowLimit)) ? 0.1 : 0
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
                        LinearScale
                        {
                            id: linearScale
                            height: itZifra.height / 3
                            width: itZifra.width * 0.95
                            indication: inputIndicator.mess
                            unitPoint: inputIndicator.unitPoint
                            hLimit: inputIndicator.highLimit
                            lLimit: inputIndicator.lowLimit
                            priemka: inputIndicator.priemka
                        }

                        Text {
                            id: mUnit
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: inputIndicator.messUnit
                        }
                        Text {
                            id: measIndicate
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: impGauge.exorbitantFilter(impGauge.round10(inputIndicator.mess, inputIndicator.accuracy))
                            color: impGauge.getColorMessText()
                            font.pixelSize: toPixels(18)
                        }
                    }
                }
            }
        }
    }

    Item {
        id: itMenu
        anchors.fill: parent
        Material.accent: impStyle.chekedColor
        visible: false

        Rectangle {
            anchors.fill: parent
            color: impStyle.windowColor

            TabBar
            {
                id: tbMenu // здесь хранятся все настройки индикатора
                height: 50
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right

                Repeater
                {
                    model: [{text:"ИНДИКАТОР", image:"./icons/indicator.png"},
                            {text:"ОБЩИЕ НАСТРОЙКИ", image:"./icons/general_settings.png"},
                            {text:"ФОРМУЛА", image:"./icons/formula.png"},
                            {text:"ДОПУСК", image:"./icons/tolerance.png"},
                            {text:"ДИСПЛЕЙ", image:"./icons/display.png"},
                            {text:"СОРТИРОВКА", image:"./icons/sort.png"},
                           ]
                    TabButton
                    {
                        id: tbInd
                        height: 50
                        anchors.bottom: parent.bottom
                        Image
                        {
                            anchors.centerIn: parent
                            height: 30
                            width: 30
                            source: modelData.image
                        }
                        background: Rectangle {
                            height: parent.height
                            width: parent.width
                            color: tbInd.checked ? impStyle.windowColor : impStyle.unChekedTabButtonColor
                        }
                        ToolTip.visible: hovered
                        ToolTip.text: modelData.text

                    }
                }
                onCurrentIndexChanged:
                {
                    switch (prevTabIndex)
                    {
                    case 1:
                    case 2: sigChangeFormula();
                            break;
                    case 3: sigChangeLimit();
                            sigChangeIndication();
                            break;
                    case 4: sigGetDivisionValue();
                            setTextBeforeSet();
                            setIndicatorAccuracy();
                            break;
                    case 5: sigChangeLimit();
                            break;
                    case 6: receiptStatPeriod(tfStatPeriod.text);
                            receiptSumPoint(tfSumPoint.text);
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
                // Страница ОБЩИЕ НАСТРОЙКИ
                Item {
                    id: itFormula


                    Column {
                        id: colLeft
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 5

                        Grid {
                            rows: 6
                            columns: 2
                            spacing: 10
                            verticalItemAlignment: Grid.AlignVCenter

                            Text { // row 0
                                text: "Название:"
                                font.pixelSize: tfName.font.pixelSize
                            }
                            TextField {
                                id: tfName
                                objectName: "tfName"
                                text: "Индикатор ?"
                                font.pixelSize: 15
                                padding: 10
                                onTextEdited: sigNameEntered()
                                width: 120
                            }
                            Text { // row 1
                                text: "Делитель:"
                                font.pixelSize: tfName.font.pixelSize
                            }
                            TextField {
                                id: tfDivider
                                objectName: "tfDivider"
                                text: "1"
                                font.pixelSize: tfName.font.pixelSize
                                onTextEdited: sigDividerEntered()
                                padding: 10
                                width: tfName.width
                            }
                            Text { // row 2
                                text: "Усреднение, мс:"
                                font.pixelSize: tfName.font.pixelSize
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
                                padding: 10
                                width: tfName.width
                            }
                            Text { // row 3
                                text: "Режим max - min:"
                                font.pixelSize: tfName.font.pixelSize
                            }
                            CheckBox {
                                id: cbMode
                                padding: 0
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


                        }

                        Text {
                            text: "Автоматическое сохранение"
                            font.pixelSize: tfName.font.pixelSize
                        }

                        GroupBox {

                            Grid {
                                rows: 2
                                columns: 2
                                spacing: 10

                                verticalItemAlignment: Grid.AlignVCenter

                                Text { // row 4
                                    text: "Включить"
                                    font.pixelSize: tfName.font.pixelSize
                                }
                                CheckBox {
                                    id: automaticSave
                                    objectName: "automaticSave"
                                    padding: 0
                                    checked: false
                                    font.pixelSize: tfName.font.pixelSize
                                    onReleased: {
                                        if (checked) {
                                            timerSave.start();
                                        }
                                        else {
                                            timerSave.stop();
                                        }
                                    }
                                }

                                Text { // row 4
                                    text: "Частота сохранения, сек"
                                    font.pixelSize: tfName.font.pixelSize
                                }
                                TextField {
                                    id: tfAutoSave
                                    objectName: "tfAutoSave"
                                    text: "10"
                                    padding: 10
                                    font.pixelSize: tfName.font.pixelSize
                                    validator: IntValidator{bottom: 1; top: 999;}
                                    inputMethodHints: Qt.ImhDigitsOnly
                                    onTextChanged:
                                    {
                                        if (text < 1)
                                            text = 1;
                                        timerSave.interval = text * 1000;
                                    }
                                    enabled: automaticSave.checked
                                    width: tfName.width
                                }
                            }

                        }
                    }

                }
                // Страница ФОРМУЛА
                Item
                {
                    id: formula
                    Column
                    {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 30
                        CheckBox
                        {
                            id: cbComplexFormula
                            objectName: "cbComplexFormula"
                            text: "Использовать сложную формулу"
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pixelSize: tfName.font.pixelSize
                            onCheckedChanged:
                            {
                                sigEnableComplexFormula(checked);
                                impGauge.blComplexFormulaEnable = checked;
                            }
                        }

                        Grid {
                            id: easyFormula
                            rows: 4
                            columns: 5
                            spacing: colLeft.spacing
                            horizontalItemAlignment: Grid.AlignHCenter
                            verticalItemAlignment: Grid.AlignVCenter
                            visible: !cbComplexFormula.checked
                            height: 300
                            // row 1
                            Text {
                                text: " "
                            }
                            Text {
                                text: " "
                            }
                            Text {
                                text: "Формула индикатора"
                                font.pixelSize: tfName.font.pixelSize
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
                                width: 40
                                horizontalAlignment: "AlignHCenter"
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
                                height: 50
                                width: 200
                                background: Rectangle {
                                    color: cbListDetect1.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                    border.color: impStyle.borderColor
                                }
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
                                width: 100
                                horizontalAlignment: "AlignHCenter"
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
                                width: 40
                                horizontalAlignment: "AlignHCenter"
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
                                height: 50
                                width: cbListDetect1.width
                                background: Rectangle {
                                    color: cbListDetect2.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                    border.color: impStyle.borderColor
                                }
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
                                horizontalAlignment: "AlignHCenter"
                                font.pixelSize: tfName.font.pixelSize
                                ToolTip.text: "Предустанов датчика 2"
                                ToolTip.visible: hovered
                            }
                        }
                        Rectangle
                        {
                            id: complexFormulaItem
                            height: easyFormula.height * 4 / 5
                            width: formula.width * 8 / 10
                            visible: cbComplexFormula.checked
                            border.color: impStyle.borderColor
                            Flickable
                            {
                                id: flItem
                                anchors.fill: parent
                                anchors.bottomMargin: 3
                                anchors.leftMargin: anchors.bottomMargin
                                anchors.topMargin: anchors.bottomMargin
                                anchors.rightMargin: anchors.bottomMargin
                                visible: parent.visible
                                clip: true
                                flickableDirection: Flickable.VerticalFlick
                                ScrollBar.vertical: ScrollBar {}
                                ScrollBar.horizontal: null
                                TextArea.flickable: TextArea
                                {
                                    id: complexFormula
                                    objectName: "complexFormula"
                                    height: flItem.height
                                    width: flItem.width
                                    visible: parent.visible
                                    horizontalAlignment: Text.AlignLeft
                                    wrapMode: TextArea.WrapAtWordBoundaryOrAnywhere
                                    onTextChanged:
                                    {
                                        analyseComplexFormula(text);
                                    }
                                }
                            }
                        }
                        Text
                        {
                            id: formulaMessage
                            objectName: "formulaMessage"
                            visible: complexFormulaItem.visible
                            text: "Формула не задана"
                            font.pixelSize: tfName.font.pixelSize
                            height: easyFormula.height - complexFormulaItem.height
                            width: complexFormulaItem.width
                            color: impStyle.warningColor
                            property int status: 0
                            onStatusChanged:
                            {
                                if (status === 1)
                                {
                                    text = "Формула задана верно";
                                    color = impStyle.baseTextColor
                                }
                                else
                                {
                                    text = "Формула задана не верно";
                                    color = impStyle.warningColor
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
                            padding: 0
                            text: "Контроль допуска"
                            font.pixelSize: tfName.font.pixelSize
                            checked: inputIndicator.dopusk
                            onCheckedChanged: inputIndicator.dopusk = checked
                        }
                        Grid {
                            rows: 4
                            columns: 2
                            spacing: 2
                            enabled: inputIndicator.dopusk
                            verticalItemAlignment: Grid.AlignVCenter
                            Text {
                                text: highLevelFormula + "   "
                                font.pixelSize: tfName.font.pixelSize
                            }
                            TextField {
                                id: tfHiLimit
                                objectName: "tfHiLimit"
                                font.pixelSize: tfName.font.pixelSize
                                width: tfName.width
                                padding: 10
                                onTextChanged:
                                {
                                    tfHiLevelF.text = text;
                                    inputIndicator.highLimit = text;
                                    setInterval();
                                }
                            }
                            Text {
                                text: lowLevelFormula + "   "
                                font.pixelSize: tfName.font.pixelSize
                            }
                            TextField {
                                id: tfLoLimit
                                objectName: "tfLoLimit"
                                font.pixelSize: tfName.font.pixelSize
                                width: tfName.width
                                padding: 10
                                onTextChanged:
                                {
                                    tfLoLevelF.text = text;
                                    inputIndicator.lowLimit = text;
                                    setInterval();
                                }
                            }
                            Text {
                                text: "Приемочная граница    "
                                font.pixelSize: tfName.font.pixelSize
                            }
                            TextField {
                                id: tfPriemka
                                objectName: "tfPriemka"
                                text: inputIndicator.priemka
                                font.pixelSize: tfName.font.pixelSize
                                width: tfName.width
                                padding: 10
                            }
                            Text {
                                text: "Звуковое оповещение   "
                                font.pixelSize: tfName.font.pixelSize
                            }
                            ComboBox {
                                id: cbSound
                                model: ["Выкл.", "Короткое", "Длинное"]
                                font.pixelSize: tfName.font.pixelSize
                                height: 50
                                width: tfName.width
                                background: Rectangle {
                                    color: cbSound.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                    border.color: impStyle.borderColor
                                }
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
                                        timerBuzzer.start();
                                        break;
                                    case 2:
                                        longPlay = true;
                                        audioEnable = true;
                                        timerBuzzer.start();
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
                        spacing: 10

                        CheckBox {
                            id: cbStrelka
                            objectName: "cbStrelka"
                            text: "Цифровое и стрелочное отображение показаний"
                            font.pixelSize: tfName.font.pixelSize
                            padding: 0
                            checked: true
                            onCheckedChanged: cbZifra.checked = !checked
                        }
                        CheckBox {
                            id: cbZifra
                            objectName: "cbZifra"
                            text: "Цифровое и линейное отображение показаний"
                            font.pixelSize: tfName.font.pixelSize
                            padding: 0
                            checked: false
                            onCheckedChanged: cbStrelka.checked = !checked
                        }

                        Grid {
                            columns: 2
                            rows: 2
                            spacing: 10
                            verticalItemAlignment: Grid.AlignVCenter

                            Text {
                                text: "Цена деления"
                                font.pixelSize: tfName.font.pixelSize
                            }
                            TextField{
                                id: tfUnitPoint
                                objectName: "tfUnitPoint"
                                font.pixelSize: tfName.font.pixelSize
                                width: tfName.width
                                padding: 10
                                inputMethodHints: Qt.ImhDigitsOnly
                            }
                            Text {
                                text: "Дискретность"
                                font.pixelSize: tfName.font.pixelSize
                            }
                            ComboBox {
                                id: tfNumberCharPoint
                                objectName: "tfNumberCharPoint"
                                model: getNumberCharPointModel("mkm")
                                currentIndex: 1
                                font.pixelSize: tfName.font.pixelSize
                                height: 50
                                width: tfName.width
                                background: Rectangle {
                                    color: tfNumberCharPoint.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                    border.color: impStyle.borderColor
                                }
                            }
                        }

                        Text {
                            text: "Единицы измерения"
                            font.pixelSize: tfName.font.pixelSize
                        }

                        GroupBox {
                            font.pixelSize: tfName.font.pixelSize
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
                                    font.pixelSize: tfName.font.pixelSize
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
                                    font.pixelSize: tfName.font.pixelSize
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
                                    font.pixelSize: tfName.font.pixelSize
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
                                    font.pixelSize: tfName.font.pixelSize
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
                            text: "Сбросить настройки"
                            font.capitalization: Font.MixedCase
                            background: Rectangle {
                                color: btSetDefault.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                border.color: impStyle.borderColor
                            }
                            onReleased: {
                                tfUnitPoint.text = 5;
                                tgrb0.checked = true; // mkm
                                tfNumberCharPoint.currentIndex = 1;
                                tfPriemka.text = 0;
                                inputIndicator.priemka = 0;
                                cbSortFormula.checked = false; // no sort
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
                        verticalItemAlignment: Grid.AlignVCenter
                        // row 1
                        Text {
                            text: "Режим сортировки"
                            font.pixelSize: tfName.font.pixelSize
                        }
                        CheckBox {
                            id: cbSortFormula
                            objectName: "cbSortFormula"
                            padding: 5
                            checked: false
                        }
                        // row 2
                        Text {
                            text: highLevelFormula
                            font.pixelSize: tfName.font.pixelSize
                        }
                        TextField {
                            id: tfHiLevelF
                            objectName: "tfHiLevelF"
                            text: tfHiLimit.text
                            font.pixelSize: tfName.font.pixelSize
                            width: tfName.width
                            padding: 10
                            onTextChanged: {
                                tfHiLimit.text = text;
                                inputIndicator.highLimit = text;
                                setInterval();
                            }
                        }
                        // row 3
                        Text {
                            text: lowLevelFormula
                            font.pixelSize: tfName.font.pixelSize

                        }
                        TextField {
                            id: tfLoLevelF
                            objectName: "tfLoLevelF"
                            text: tfLoLimit.text
                            font.pixelSize: tfName.font.pixelSize
                            width: tfName.width
                            padding: 10
                            onTextChanged: {
                                tfLoLimit.text = text;
                                inputIndicator.lowLimit = text;
                                setInterval();
                            }
                        }
                        // row 4
                        Text {
                            text: "Количество групп"
                            font.pixelSize: tfName.font.pixelSize
                        }
                        SpinBox {
                            id: countGroupsF
                            objectName: "countGroupsF"
                            from: 1
                            to: 100
                            padding: 0
                            width: tfName.width
                            editable: true
                            background: Rectangle
                            {
                                border.color: impStyle.borderColor
                            }
                            onValueChanged:
                            {
                                setInterval();
                            }
                        }

                        // row 5
                        Text {
                            text: "Интервал"
                            font.pixelSize: tfName.font.pixelSize
                        }
                        TextField {
                            id: intervalF
                            objectName: "intervalF"
                            inputMethodHints: Qt.ImhDigitsOnly
                            font.pixelSize: tfName.font.pixelSize
                            width: tfName.width
                            padding: 10
                            enabled: false
                        }
                    }
                }


            }
        }
    }
}

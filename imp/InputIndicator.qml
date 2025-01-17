import QtQuick 2.5

Item
{
    id: inputIndicator
    objectName: "inputIndicator"
    property real mess: 0
    property int accuracy: 1
    property real lowLimit: -50
    property real highLimit: 50
    property real unitPoint: 5 // Менять цену деления для перерисовки шкалы
    property string messUnit: "_____"
    property int accurDivision: 0 //количество отображаемых знаков после запятой на метках циферблата

    // преобразование измерений из единиц измерния датчиков в единицы измерения пользователя
    // 0 - без преобразования
    // 1 - из мкм в мм
    // 2 - из мкм в inch
    // 3 - из мкм/м в угловые секунды
    property int transGauge: -1
    onTransGaugeChanged: {
        mess = getMeasForTransform(messReal);
        getMeasUnitForTransform();
    }
    // единица измерения датчика
    property string messUnitDetect: "_____"
    onMessUnitDetectChanged: {
        getMeasUnitForTransform();
    }

    // реальные измерения
    property real messReal: 0
    property real mTranformFormulaReal: 0
    onMessRealChanged: {
        mTranformFormulaReal = getMeasForTransform(messReal);
        mess = mTranformFormulaReal;
    }

    property real mDetect1: 0 // Показания датчика 1 с учетом коэффициента и сдвига
    property real mTranformD1: 0
    onMDetect1Changed: mTranformD1 = getMeasForTransform(mDetect1)
    property real mDetect2: 0 // Показания датчика 2 с учетом коэффициента и сдвига
    property real mTranformD2: 0
    onMDetect2Changed: mTranformD2 = getMeasForTransform(mDetect2)


    // контроль работоспособности датчика
    property bool blDetect1EnableInput: false
    property bool blDetect2EnableInput: false
    property bool blDetect1WorkInput: true
    property bool blDetect2WorkInput: true

    property bool blOverRange1: false  // выход датчика 1 за пределы таблицы калибровки
    property bool blOverRange2: false  // выход датчика 2 за пределы таблицы калибровки


    // флаг разрешения установки индикатора в 0. Зависит от параметров датчиков
    property bool enableSetZero: true


    // граница приемки
    property real priemka: 0

    // предустанов общий
    property real beforeSet: 0

    // контроль допуска
    property bool dopusk: true

    // Анимация запускается только при изменении зачения для показаний на шкале
    Behavior on mess {
        NumberAnimation {
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }

    function getMeasForTransform(mr) {
        var result = 0;
        switch (transGauge) {
        case 1:
            result = mr / 1000;
            break;
        case 2:
            result = mr / 25400;
            break;
        case 3:
            result = mr; // нужно узнать формулу
            break;
        default:
            result = mr;

            break;
        }
        return result;
    }

    function getMeasUnitForTransform() {
        switch (transGauge) {
        case 1:
            messUnit = "мм";
            break;
        case 2:
            messUnit = "inch";
            break;
        case 3:
            messUnit = "угл. сек.";
            break;
        default:
            messUnit = messUnitDetect;
            break;
        }
        return 0;
    }

}

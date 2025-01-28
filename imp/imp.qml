import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQml.Models 2.2


Item
{
    id: itWin
    objectName: "itWin"
    property double scaleX: 320
    onScaleXChanged: width = scaleX;
    property double scaleY: 480
    onScaleYChanged: height = scaleY;
    width: scaleX
    height: scaleY

    property string strSerialNumber: ""
    property string strActive: ""
    property string strNameDetect: ""
    property string strTypeDetect: ""
    property string strDataManuf: ""
    property string strPort: ""
    property string strModbusAddress: ""
    property int iCommand: 0 // 0 - нет команды
                             // 1 - стереть список
                             // 2 - добавить запись
                             // 3 - стереть запись
                             // 4 - коррекция записи
    property int iSumDetect: 0
    property bool blCalibrate: false // false - measure, true - calibrate

    onICommandChanged: // Получена команда работы со списком датчиков
    {
        if (iCommand === 1) // 1 - стереть список
        {
            lmDetect.clear(); // Очистка списка
        }
        if (iCommand === 2) // 2 - добавить запись
        {
            lmDetect.append({serialNumber:strSerialNumber,
                            activeState:strActive,
                            nameDetect:strNameDetect,
                            typeDetect:strTypeDetect,
                            dataManuf:strDataManuf,
                            port:strPort,
                            modbusAddress:strModbusAddress});
        }
        if (iCommand === 3) // 3 - стереть запись
        { // Номер датчика указан в strSerialNumber
            for (var index = iSumDetect-1; index > -1; index--)
            {
                if (strSerialNumber === lmDetect.get(index).serialNumber)
                {
                    lmDetect.remove(index);
                }
            }
        }
        if (iCommand === 4) // 4 - коррекция записи
        {
            for (var i = 0; i < lmDetect.count; i++)
            {
                if (lmDetect.get(i).serialNumber === strSerialNumber)
                {
                    lmDetect.get(i).activeState = strActive;
                    lvDetect.update();
                    break;
                }
            }
        }

        iCommand = 0; // 0 - нет команды
    }

    signal sigClickedbtAbout();
    signal sigClickedbtHelp();
    signal sigNewIndicator(string SerialNum); // Серийный номер индикатора, или "Нет"
    signal sigFindDetect();
    signal sigSelectDetectToInit(string SerialNum);

    Rectangle
    {
        id: actionbar
        height: 155
        color: "lightgray"
        anchors.left: itWin.left
        anchors.right: itWin.right


        Row {
            padding: 20
            spacing: 10

            Column {

                Row {
                    spacing: 10

                    Button
                    {
                        id: btFind
                        objectName: "btFind"
                        height: 100
                        width: 100
                        text: "Поиск
датчиков"

                        font.capitalization: Font.Capitalize
                        icon.name: "transducers_search"
                        icon.source: "icons/transducers_search.png"
                        display: Button.TextUnderIcon
                        onClicked: sigFindDetect();
                    }

                    Button
                    {
                        id: btIndicator
                        objectName: "btIndicator"
                        height: 100
                        width: 100
                        text: "Новый
индикатор"
                        font.capitalization: Font.Capitalize
                        icon.name: "indicator_add"
                        icon.source: "icons/indicator_add.png"
                        display: Button.TextUnderIcon
                        onClicked: sigNewIndicator("Нет"); // Датчик для нового индикатора не выбран
                    }
                }

                ProgressBar
                {
                    id: pbFind
                    objectName: "pbFind"
                    height: 30
                    width: 210
                    value: 0 // from 0 to 1
                    background: Rectangle
                    {
                        width: pbFind.width
                        height: pbFind.height
                        color: "lightgray"
                    }
                    contentItem: Item
                    {
                        width: parent.width
                        height: parent.height
                        Rectangle
                        {
                            anchors.verticalCenter: parent.verticalCenter
                            width: pbFind.visualPosition * parent.width
                            height: parent.height * 0.8

                            radius: height/4
                            color: "green"
                        }
                        Text {
                            id: textComment
                            objectName: "textComment"
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            text: " "
                        }
                        Text {
                            id: textPercent
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            text: String(Math.round(pbFind.value * 100)) + "%  "
                            visible: ((pbFind.value === 1) | (pbFind.value === 0)) ? false : true
                        }
                    }
                }
            }

            ToolSeparator {
                height: 120
            }

            Button
            {
                id: btCompose
                height: 100
                width: 100
                text: "Компоновать
существующие"

                font.capitalization: Font.Capitalize
                icon.name: "compose"
                icon.source: "icons/compose.png"
                display: Button.TextUnderIcon
                onClicked: sigFindDetect();
            }

            Button
            {
                id: btAddAndCompose
                height: 100
                width: 100
                text: "Добавить и
компоновать"
                font.capitalization: Font.Capitalize
                icon.name: "add_and_compose"
                icon.source: "icons/add_and_compose.png"
                display: Button.TextUnderIcon
                onClicked: sigNewIndicator("Нет"); // Датчик для нового индикатора не выбран
            }

            ToolSeparator {
                height: 120
            }

            Button
            {
                id: btAbout
                height: 100
                width: 100
                text: "Настройки
поиска"

                font.capitalization: Font.Capitalize
                background: Rectangle {
                    color: mouse.hovered ? "red" : "transparent"
                    border.color: "transparent"
                }

                icon.name: "transducers_search"
                icon.source: "icons/settings.png"
                display: Button.TextUnderIcon
                onClicked: sigClickedbtAbout();
            }
        }
    }

    ListView
    {
        id: lvDetect
        anchors.top: actionbar.bottom
        anchors.left: itWin.left
        anchors.right: itWin.right
        focus: true
        header: Rectangle
        {
            width: parent.width
            height: itWin.height/15
            color: "darkblue"
            Text{
                anchors.centerIn: parent;
                color: "lightgray";
                text: "Обнаруженные датчики";
                font.bold: true;
                font.pointSize: 15
            }
        }
        model: ListModel
        { // Здесь будет содержаться список датчиков
            id: lmDetect
            objectName: "lmDetect"
        }
        delegate: Item
        { // Описание представления обнаруженного датчика
            id: itDetect
            height: 60
            width: itWin.width
            Row
            {
                spacing: 2
                Text
                {
                    id: textNumber
                    font.pointSize: 20
                    text: serialNumber
                }
                Column
                {
                    Row
                    {
                        Text
                        {
                            id: textName
                            font.pointSize: 12
                            text: nameDetect
                        }
                        Text
                        {
                            id: textActive
                            font.pointSize: 12
                            text: activeState
                        }
                    }
                    Text
                    {
                        id: textType
                        font.pointSize: 8
                        text: typeDetect
                    }
                    Row
                    {
                        spacing: 6
                        Text
                        {
                            id: textDataManuf
                            font.pointSize: 8
                            text: "Изг.: " + dataManuf
                        }
                        Text
                        {
                            id: textPort
                            font.pointSize: 8
                            text: "Порт: " + port
                        }
                        Text
                        {
                            id:textModbusAddress
                            font.pointSize: 8
                            text: "Адрес Modbus: " + modbusAddress
                            visible: modbusAddress !== ""
                        }
                    }
                }
            }
            MouseArea
            { // Для запуска окна установок
                anchors.fill: parent
                onDoubleClicked:
                {
                    sigNewIndicator(serialNumber)
                }
                hoverEnabled: true
                onEntered: rectIndicate.color = "tomato"
                onExited: rectIndicate.color = "yellow"
                Button
                {
                    id: btOptions
                    anchors.right: parent.right
                    anchors.rightMargin: 2
                    width: itWin.width/10
                    text: "Настройки"
                    onClicked: sigSelectDetectToInit(serialNumber);
                    ToolTip.text: btOptions.text
                    ToolTip.visible: hovered
                }

                Rectangle
                {
                    id: rectIndicate
                    anchors.fill: parent
                    color: "yellow"
                    opacity: 0.2
                    Behavior on color
                    {
                        ColorAnimation
                        {
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }
        }
    }
}

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

    SearchProgress
    {
        id: searchProgress
        objectName: "searchProgress"
        visible: false
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle
    {
        id: actionbar
        height: 100
        color: "lightgray"
        anchors.left: itWin.left
        anchors.right: itWin.right


        Row {
            leftPadding: 20
            spacing: 10

            Button {
                    id: btFind
                    objectName: "btFind"
                    height: 100
                    width: 80
                    hoverEnabled: true
                    background: Rectangle {
                        color: btFind.down ? "lightgray" : (btFind.hovered ? "#c3c3c3" : "lightgray")
                    }
                    text: "Поиск
датчиков"
                    font.capitalization: Font.Capitalize
                    icon.name: "transducers_search"
                    icon.source: "icons/transducers_search.png"
                    display: Button.TextUnderIcon
                    ToolTip.visible: hovered
                    ToolTip.text: "Искать подключенные датчики"
                    onClicked:
                    {
                        searchProgress.visible = true;
                        sigFindDetect();
                        searchProgress.visible = false;
                    }
                }

            Button
            {
                id: btAbout
                height: 100
                width: 80
                hoverEnabled: true
                background: Rectangle {
                    color: btAbout.down ? "lightgray" : (btAbout.hovered ? "#c3c3c3" : "lightgray")
                }
                text: "Настройки
поиска"
                font.capitalization: Font.Capitalize
                icon.name: "transducers_search"
                icon.source: "icons/settings.png"
                display: Button.TextUnderIcon
                ToolTip.visible: hovered
                ToolTip.text: "Выбрать тип датчика и способ поиска"
                onClicked: sigClickedbtAbout();
            }

            ToolSeparator {
                height: 100
            }

            Button {
                    id: btIndicator
                    objectName: "btIndicator"
                    height: 100
                    width: 80
                    hoverEnabled: true
                    background: Rectangle {
                        color: btIndicator.down ? "lightgray" : (btIndicator.hovered ? "#c3c3c3" : "lightgray")
                    }
                    text: "Новый
индикатор"
                    font.capitalization: Font.Capitalize
                    icon.name: "indicator_add"
                    icon.source: "icons/indicator_add.png"
                    display: Button.TextUnderIcon
                    ToolTip.visible: hovered
                    ToolTip.text: "Добавить новый индикатор для найденного датчика"
                    onClicked: sigNewIndicator("Нет"); // Датчик для нового индикатора не выбран
                }

            Button {
                id: btCompose
                height: 100
                width: 110
                hoverEnabled: true
                background: Rectangle {
                    color: btCompose.down ? "lightgray" : (btCompose.hovered ? "#c3c3c3" : "lightgray")
                }
                text: "Компоновать
существующие"
                font.capitalization: Font.Capitalize
                icon.name: "compose"
                icon.source: "icons/compose.png"
                display: Button.TextUnderIcon
                ToolTip.visible: hovered
                ToolTip.text: "Разместить открытые индикаторы в пределах всех доступных экранов"
            }

            Button
            {
                id: btAddAndCompose
                height: 100
                width: 110
                hoverEnabled: true
                background: Rectangle {
                    color: btAddAndCompose.down ? "lightgray" : (btAddAndCompose.hovered ? "#c3c3c3" : "lightgray")
                }
                text: "Добавить и
компоновать"
                font.capitalization: Font.Capitalize
                icon.name: "add_and_compose"
                icon.source: "icons/add_and_compose.png"
                display: Button.TextUnderIcon
                ToolTip.visible: hovered
                ToolTip.text: "Добавить выбранное количество индикаторов и разместить в пределах одного экрана"
            }

            ToolSeparator {
                height: 100
            }

            Button {
                id: btWorkspace
                height: 100
                width: 110
                background: Rectangle {
                    color: btWorkspace.down ? "lightgray" : (btWorkspace.hovered ? "#c3c3c3" : "lightgray")
                }
                text: "Добавить
рабочее место"
                font.capitalization: Font.Capitalize
                icon.name: "workspace"
                icon.source: "icons/workspace.png"
                display: Button.TextUnderIcon
            }

            Column {
                topPadding: 25

                ComboBox {
                    width: 250
                    }
            }


            ToolSeparator {
                height: 100
            }

            Button
            {
                id: btHelp
                height: 100
                width: 100
                background: Rectangle {
                    color: btHelp.down ? "lightgray" : (btHelp.hovered ? "#c3c3c3" : "lightgray")
                }
                Image {
                    source: "icons/micro.png"
                    x:35
                    y:15
                    height: 30
                    width: 30
                }
                Text {
                    text: "Руководство\nпользователя"
                    font: btHelp.font
                    horizontalAlignment: Text.AlignHCenter
                    x: 5
                    y: 48
                }
                font.capitalization: Font.Capitalize
                onClicked: sigClickedbtHelp();
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
    }



    ListView
    {
        id: lvDetect
        anchors.top: actionbar.bottom
        anchors.left: itWin.left
        anchors.right: itWin.right
        anchors.bottom: itWin.bottom
        focus: true
        header: Rectangle
        {
            width: parent.width
            height: itWin.height/15
            color: "#225f78"
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
                onEntered: rectIndicate.color = "gray"
                onExited: rectIndicate.color = "lightgray"
                Button
                {
                    id: btOptions
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    width: 40
                    icon.name: "info"
                    icon.source: "icons/info.png"
                    onClicked: sigSelectDetectToInit(serialNumber);
                    ToolTip.text: "Информация о датчике"
                    ToolTip.visible: hovered
                }

                Rectangle
                {
                    id: rectIndicate
                    anchors.fill: parent
                    color: "lightgray"
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

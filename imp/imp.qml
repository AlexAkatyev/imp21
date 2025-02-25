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

    signal sigClickedGeneralSettings();
    signal sigClickedbtHelp();
    signal sigNewIndicator(string SerialNum); // Серийный номер индикатора, или "Нет", или "DEFAULT_INDICATOR"
    signal sigFillScreenWithIndicators();
    signal sigComposeOpenWindowsInOrder();
    signal sigFindDetect();
    signal sigSelectDetectToInit(string SerialNum);

    ImpStyle
    {
        id: impStyle
    }

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
        color: impStyle.actionbarColor
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
                        color: btFind.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                    }
                    text: "Поиск\nдатчиков"
                    font.capitalization: Font.MixedCase
                    icon.name: "transducers_search"
                    icon.source: "icons/transducers_search.png"
                    display: Button.TextUnderIcon
                    ToolTip.visible: hovered
                    ToolTip.text: "Искать подключенные датчики"
                    onClicked:
                    {
                        sigFindDetect();
                    }
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
                        color: btIndicator.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                    }
                    text: "Новый\nиндикатор"
                    font.capitalization: Font.MixedCase
                    icon.name: "indicator_add"
                    icon.source: "icons/indicator_add.png"
                    display: Button.TextUnderIcon
                    ToolTip.visible: hovered
                    ToolTip.text: "Восстановить индикатор для найденного датчика"
                    onClicked: sigNewIndicator("DEFAULT_INDICATOR");
                }

            Button {
                    id: btRestore
                    height: 100
                    width: 100
                    hoverEnabled: true
                    background: Rectangle {
                        color: btRestore.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                    }
                    text: "Восстановить\nиндикатор"
                    font.capitalization: Font.MixedCase
                    icon.name: "indicator_restore"
                    icon.source: "icons/indicator_restore.png"
                    display: Button.TextUnderIcon
                    ToolTip.visible: hovered
                    ToolTip.text: "Восстановить индикатор с последними настройками"
                    onClicked: sigNewIndicator("Нет"); // Датчик для нового индикатора не выбран
                }

            Button {
                id: btCompose
                height: 100
                width: 110
                hoverEnabled: true
                background: Rectangle {
                    color: btCompose.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                }
                text: "Компоновать\nсуществующие"
                font.capitalization: Font.MixedCase
                icon.name: "compose"
                icon.source: "icons/compose.png"
                display: Button.TextUnderIcon
                ToolTip.visible: hovered
                ToolTip.text: "Разместить открытые индикаторы в пределах всех доступных экранов"
                onClicked: sigComposeOpenWindowsInOrder();
            }

            Button
            {
                id: btAddAndCompose
                height: 100
                width: 100
                hoverEnabled: true
                background: Rectangle {
                    color: btAddAndCompose.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                }
                text: "Добавить и\nкомпоновать"
                font.capitalization: Font.MixedCase
                icon.name: "add_and_compose"
                icon.source: "icons/add_and_compose.png"
                display: Button.TextUnderIcon
                ToolTip.visible: hovered
                ToolTip.text: "Добавить максимальное количество индикаторов и разместить их в пределах всех доступных экранов"
                onClicked: sigFillScreenWithIndicators();
            }

            ToolSeparator {
                height: 100
            }

            Button {
                id: btWorkspace
                height: 100
                width: 110
                background: Rectangle {
                    color: btWorkspace.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                }
                text: "Добавить\nрабочее место"
                font.capitalization: Font.MixedCase
                icon.name: "workspace"
                icon.source: "icons/workspace.png"
                display: Button.TextUnderIcon
            }

            Column {
                topPadding: 25

                ComboBox {
                    id: cbWorkspace
                    height: 50
                    width: 250
                    background: Rectangle {
                        color: cbWorkspace.hovered ? impStyle.hoveredColor : impStyle.windowColor
                        border.color: impStyle.borderColor
                    }
                    }
            }

            Button
            {
                id: btAbout
                height: 100
                width: 110
                hoverEnabled: true
                background: Rectangle {
                    color: btAbout.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                }
                text: "Настройки\nрабочего места"
                font.capitalization: Font.MixedCase
                icon.name: "transducers_search"
                icon.source: "icons/workspace_settings.png"
                display: Button.TextUnderIcon
                ToolTip.visible: hovered
                ToolTip.text: "Выбрать тип датчика и способ поиска"
                onClicked: sigClickedGeneralSettings();
            }
        }

        Row {
            rightPadding: 20
            spacing: 10
            anchors.right: parent.right

            ToolSeparator {
                height: 100

            }

            Button
            {
                id: btHelp
                height: 100
                width: 100
                background: Rectangle {
                    color: btHelp.hovered ? impStyle.hoveredColor : impStyle.actionbarColor
                }
                text: "Руководство\nпользователя"
                font.capitalization: Font.MixedCase
                icon.name: "help"
                icon.source: "icons/help.png"
                display: Button.TextUnderIcon
                onClicked: sigClickedbtHelp();
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
            color: impStyle.chekedColor
            Text{
                anchors.centerIn: parent;
                color: impStyle.windowColor;
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
            height: 80
            width: itWin.width

            Frame {
                anchors.fill: itDetect
                anchors.margins: 1

                Row
                {
                    id: mainRow
                    anchors.verticalCenter: parent.verticalCenter
                    topPadding: 5
                    leftPadding: 10
                    spacing: 10

                    Rectangle {

                        color: impStyle.actionbarColor
                        height: textNumber.height+10
                        width: 150
                        Text
                        {
                            id: textNumber
                            anchors.centerIn: parent
                            font.pointSize: 30
                            text: serialNumber
                        }
                    }

                    Column
                    {
                        anchors.verticalCenter: mainRow.verticalCenter
                        spacing: 5

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
                            font.pointSize: 10
                            text: typeDetect
                        }
                        Row
                        {
                            spacing: 10
                            Text
                            {
                                id: textDataManuf
                                font.pointSize: 10
                                text: "Изг.: " + dataManuf
                            }
                            Text
                            {
                                id: textPort
                                font.pointSize: 10
                                text: "Порт: " + port
                            }
                            Text
                            {
                                id:textModbusAddress
                                font.pointSize: 10
                                text: "Адрес Modbus: " + modbusAddress
                                visible: modbusAddress !== ""
                            }
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
                onEntered: rectIndicate.color = impStyle.chekedColor
                onExited: rectIndicate.color = impStyle.windowColor
                Button
                {
                    id: btOptions
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 10
                    width: 40
                    icon.name: "info"
                    icon.source: "icons/info.png"
                    background: Rectangle {
                        color: btOptions.hovered ? impStyle.hoveredColor : impStyle.windowColor
                        border.color: impStyle.borderColor
                    }
                    onClicked: sigSelectDetectToInit(serialNumber);
                    ToolTip.text: "Информация о датчике"
                    ToolTip.visible: hovered
                }

                Rectangle
                {
                    id: rectIndicate
                    anchors.fill: parent
                    color: impStyle.windowColor
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

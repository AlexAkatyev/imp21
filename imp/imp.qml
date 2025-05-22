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

    property int shortInfoHeight: 90
    property int detailedInfoHeight: 480

    property string strSerialNumber: ""
    property string strActive: ""
    property string strNameDetect: ""
    property string strTypeDetect: ""
    property string strDataManuf: ""
    property string strPort: ""
    property string strModbusAddress: ""
    property string strTypeSettings: ""
    property string strMeasUnit: "___"
    property string strMeasRange: "0 ... 0"
    property string strZeroRange: "0"
    property string strPreSet: "0"
    property string strCurrentDataValue: "!!!"
    property int dataCalibrField: 0
    property string dataCalibTable: ""

    property int iCommand: 0 // 0 - нет команды
                             // 1 - стереть список
                             // 2 - добавить запись
                             // 3 - стереть запись
                             // 4 - коррекция записи (активность датчика)
                             // 5 - коррекция записи (текущее значение)
    property int iSumDetect: 0
    property bool blCalibrate: false // false - measure, true - calibrate

    onICommandChanged: // Получена команда работы со списком датчиков
    {
        var i = 0;
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
                            modbusAddress: strModbusAddress,
                            typeSettings: strTypeSettings,
                            dataMeasUnit: strMeasUnit,
                            dataMeasRange: strMeasRange,
                            dataZeroRange: strZeroRange,
                            dataPreSet: strPreSet,
                            currentDataValue: strCurrentDataValue,
                            maskDataCalibrField: dataCalibrField,
                            calibTable: dataCalibTable
                            });
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
        if (iCommand === 4) // 4 - коррекция записи (активность датчика)
        {
            for (i = 0; i < lmDetect.count; i++)
            {
                if (lmDetect.get(i).serialNumber === strSerialNumber)
                {
                    lmDetect.get(i).activeState = strActive;
                    lvDetect.update();
                    break;
                }
            }
        }
        if (iCommand === 5) // 5 - коррекция записи (текущее значение)
        {
            for (i = 0; i < lmDetect.count; i++)
            {
                if (lmDetect.get(i).serialNumber === strSerialNumber)
                {
                    lmDetect.get(i).currentDataValue = strCurrentDataValue;
                    lvDetect.update();
                    break;
                }
            }
        }

        iCommand = 0; // 0 - нет команды
    }

    signal sigClickedbtHelp();
    signal sigNewIndicator(string SerialNum); // Серийный номер индикатора, или "Нет", или "DEFAULT_INDICATOR"
    signal sigFillScreenWithIndicators();
    signal sigComposeOpenWindowsInOrder();
    signal sigFindDetect();
    signal sigOpenWorkPlaces();
    signal sigWorkPlaceChanged();
    signal sigSetNewUserName(string Id, string UserName);
    signal sigSetNewModbusAddress(string Id, string Address);

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
                onClicked:
                {
                    sigOpenWorkPlaces();
                }
            }

            ComboBox
            {
                id: cbWorkspace
                objectName: "cbWorkspace"
                height: 50
                width: 250
                anchors.verticalCenter: parent.verticalCenter
                background: Rectangle
                {
                    color: cbWorkspace.hovered ? impStyle.hoveredColor : impStyle.windowColor
                    border.color: impStyle.borderColor
                }
                onCurrentIndexChanged: sigWorkPlaceChanged()
            }

            Button
            {
                id: btImpSettingsDialog
                height: 100
                width: 110
                hoverEnabled: true
                background: Rectangle {
                    color: btImpSettingsDialog.hovered || impSettingsDialog.visible ? impStyle.hoveredColor : impStyle.actionbarColor
                }
                text: "Настройки\nрабочего места"
                font.capitalization: Font.MixedCase
                icon.name: "transducers_search"
                icon.source: "icons/workspace_settings.png"
                display: Button.TextUnderIcon
                ToolTip.visible: hovered
                ToolTip.text: "Выбрать тип датчика и способ поиска"
                onClicked:
                {
                    lvDetect.visible = !lvDetect.visible
                    impSettingsDialog.visible = !lvDetect.visible
                }
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
            height: shortInfoHeight
            width: itWin.width

            Behavior on height {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }

            MouseArea
            { // Для запуска окна установок
                anchors.fill: parent
                hoverEnabled: true
                onEntered: rectIndicate.color = impStyle.chekedColor
                onExited: rectIndicate.color = impStyle.windowColor

                Frame
                {
                    id: itemGeneral
                    anchors.fill: parent
                    anchors.margins: 1

                    Item
                    {
                        id: shortInfoIrem
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: -10
                        height: shortInfoHeight - 2 * itemGeneral.anchors.margins

                        Row
                        {
                            id: mainRow
                            anchors.verticalCenter: parent.verticalCenter
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
                            onClicked:
                            {
                                if (itDetect.height == shortInfoHeight)
                                {
                                    itDetect.height = detailedInfoHeight;
                                    detailedItem.visible = true;
                                }
                                else
                                {
                                    itDetect.height = shortInfoHeight;
                                    detailedItem.visible = false;
                                }

                            }
                            ToolTip.text: "Информация о датчике"
                            ToolTip.visible: hovered
                        }

                        Button
                        {
                            id: btNewIndicator
                            anchors.right: btOptions.left
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.rightMargin: btOptions.anchors.rightMargin
                            width: 40
                            icon.name: "info"
                            icon.source: "icons/info.png"
                            background: Rectangle {
                                color: btOptions.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                border.color: impStyle.borderColor
                            }
                            onClicked:
                            {
                                sigNewIndicator(serialNumber)
                            }
                            ToolTip.text: "Открыть в индикаторе"
                            ToolTip.visible: hovered
                        }

                    }

                    Item
                    {
                        id: detailedItem
                        height: itemGeneral.height - shortInfoIrem.height
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        visible: false

                        BepVtSettings
                        {
                            visible: typeSettings == "bep"
                            idNumber: serialNumber
                            detectName: nameDetect
                            measUnit: dataMeasUnit
                            measRange: dataMeasRange
                            zeroRange: dataZeroRange
                            preSet: dataPreSet
                            currentDataMeas: currentDataValue
                            maskCalibrField: maskDataCalibrField
                            mdataCalibTable: calibTable
                            mmodbusAddress: modbusAddress
                        }

                        EmVtSettings
                        {
                            visible: typeSettings == "em"
                            measUnit: dataMeasUnit
                            measRange: dataMeasRange
                            zeroRange: dataZeroRange
                            preSet: dataPreSet
                        }

                    }
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

    ImpSettingsDialog
    {
        id: impSettingsDialog
        objectName: "impSettingsDialog"
        anchors.top: lvDetect.top
        anchors.left: lvDetect.left
        anchors.right: lvDetect.right
        anchors.bottom: lvDetect.bottom
        visible: false
    }
}

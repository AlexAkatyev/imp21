import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.12
import QtQml.Models 2.2
import QtQuick.Layouts 1.3

Item {
    height: 500
    width: 355
    objectName: "ImpSettings"

    property string tcpAdress: ""
    property int iCommand: 0 // 0 - нет команды
                             // 1 - стереть список
                             // 2 - добавить запись
    onICommandChanged: // Получена команда работы со списком датчиков
    {
        if (iCommand === 1) // 1 - стереть список
        {
            lmAddress.clear(); // Очистка списка
        }
        if (iCommand === 2) // 2 - добавить запись
        {
            lmAddress.append({serverAddress : tcpAdress});
        }
        iCommand = 0; // 0 - нет команды
    }
    property string oldAdresses: ""

    signal sigFindModbusTCP(bool enableFind);
    signal sigFindModbus485(bool enableFind);
    signal sigSimRec(bool enableSimRec);
    signal sigAdresses(string listAdresses);

    function enabledFinderCheck()
    {
        cbSearch485.enabled = cbWireSearch.checked;
        cmdAdressbutons.enabled = cbModBusSearch.checked;
        frameAdresses.enabled = cbModBusSearch.checked;
    }


    ImpStyle
    {
        id: impStyle
    }

    Timer
    {
        id: modelUpdater
        objectName: "modelUpdater"
        interval: 1000
        repeat: true
        running: false
        onTriggered:
        {
            var mes = "";
            for (var i = 0; i < lmAddress.count; i++)
            {
                if (i !== 0)
                {
                    mes += "\n";
                }

                mes += lmAddress.get(i).serverAddress;
            }
            if (oldAdresses !== mes)
            {
                oldAdresses = mes;
                sigAdresses(mes);
            }

        }
    }

    Rectangle {
        anchors.fill: parent
        color: impStyle.windowColor

        TabBar {
            id: tbSearchSettings
            width: parent.width
            Material.accent: impStyle.chekedColor

            TabButton {
                id: tbnSearch
                text: "Поиск"
                background: Rectangle {
                    width: parent.width
                    color: tbnSearch.checked ? impStyle.windowColor : impStyle.unChekedTabButtonColor
                }
            }

            TabButton {
                id: tbnWorkspace
                text: "Рабочее место"
                background: Rectangle {
                    width: parent.width
                    color: tbnWorkspace.checked ? impStyle.windowColor : impStyle.unChekedTabButtonColor
                }
            }
        }

        StackLayout {
            width: parent.width
            currentIndex: tbSearchSettings.currentIndex

            Item {
                id: tbnSearchTab

                Column {
                    spacing: 10
                    topPadding: 50
                    padding: 10

                    Frame
                    {
                        id: checkFrame
                        Column {
                            spacing: -10

                            RadioButton {
                                id: cbWireSearch
                                objectName: "cbWireSearch"
                                text: "Искать проводные датчики"
                                checked: true
                                Material.accent: impStyle.chekedColor
                                onReleased:
                                {
                                    enabledFinderCheck();
                                    sigFindModbusTCP(!checked);
                                }
                            }

                            CheckBox {
                                id: cbSearch485
                                objectName: "cbSearch485"
                                text: "Искать датчики по протоколу RS-485"
                                Material.accent: impStyle.chekedColor
                                enabled: cbWireSearch.checked
                                onReleased:
                                {
                                    sigFindModbus485(checked);
                                }
                            }

                            RadioButton {
                                id: cbModBusSearch
                                objectName: "cbModBusSearch"
                                text: "Искать датчики через сервер Modbus TCP"
                                Material.accent: impStyle.chekedColor
                                onReleased:
                                {
                                    enabledFinderCheck();
                                    sigFindModbusTCP(checked)
                                }
                            }
                        }
                    }

                    Text {
                        text: "Адреса"
                        font.pixelSize: 15
                    }

                    Frame
                    {
                        id: frameAdresses
                        height: 200
                        width: checkFrame.width
                        enabled: cbModBusSearch.checked

                        ListView
                        {
                            id: lvAddresses
                            height: 170
                            width: 310
                            Material.accent: impStyle.chekedColor
                            focus: true
                            boundsBehavior: Flickable.StopAtBounds
                                    ScrollBar.vertical: ScrollBar
                                    {
                                        id: scroll
                                        policy: ScrollBar.AlwaysOn
                                        active: ScrollBar.AlwaysOn
                                    }

                            model: ListModel
                            { // Здесь будет содержаться список адресов
                                id: lmAddress
                            }

                            delegate: TextField
                            {
                                width: parent.width - scroll.width
                                height: 50
                                property var view: ListView.view
                                property int itemIndex: index
                                text: serverAddress
                                color:  ListView.isCurrentItem ? impStyle.chekedColor : impStyle.unChekedColor
                                onPressed:
                                {
                                    view.currentIndex = itemIndex;
                                }
                                onTextChanged:
                                {
                                    lmAddress.set(index, {serverAddress : text});
                                }
                            }
                            onCountChanged:
                            {
                                visible = count != 0;
                                emptyWidgets.visible = !visible;
                            }
                            visible: count != 0
                        }

                        Column
                        {
                            id: emptyWidgets
                            visible: lvAddresses.count == 0
                            anchors.verticalCenter: parent.verticalCenter
                            Text {
                                id: txNoAddress
                                text: "Адресов не найдено"
                                font.pixelSize: 12
                            }

                            Image
                            {
                                id: imNoAddress
                                source: "icons/no_adress_added.png"
                                height: 100
                                width: imNoAddress.height
                            }
                        }
                    }
                    Item
                    {
                        id: cmdAdressbutons
                        width: checkFrame.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        enabled: cbModBusSearch.checked

                        Button
                        {
                            id: btAddressAdd
                            anchors.left: parent.left
                            height: 50
                            width: 160
                            text: "Добавить адрес"
                            font.capitalization: Font.MixedCase
                            icon.name: "address_add"
                            icon.source: "icons/address_add.png"
                            background: Rectangle {
                                color: btAddressAdd.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                border.color: impStyle.borderColor
                            }
                            onClicked:
                            {
                                lmAddress.append({serverAddress : "127.0.0.1"});
                            }
                        }

                        Button
                        {
                            id: btAddressRemove
                            anchors.right: parent.right
                            height: 50
                            width: 160
                            text: "Удалить адрес"
                            font.capitalization: Font.MixedCase
                            icon.name: "address_remove"
                            icon.source: "icons/address_remove.png"
                            background: Rectangle
                            {
                                color: btAddressRemove.hovered ? impStyle.hoveredColor : impStyle.windowColor
                                border.color: impStyle.borderColor
                            }
                            onClicked:
                            {
                                var index = lvAddresses.currentIndex;
                                if (index > -1)
                                {
                                    lmAddress.remove(index);
                                }
                            }
                        }
                    }
                }
            }


            Item {
                id: tbnWorkspaceTab

                Column {
                    spacing: 10
                    topPadding: 50
                    padding: 10

                    CheckBox {
                        id: cbSimRec
                        objectName: "cbSimRec"
                        text: "Запись одновременно во всех индикаторах"
                        Material.accent: impStyle.chekedColor
                        onReleased:
                        {
                            sigSimRec(checked);
                        }
                    }
                }
            }
        }
    }
}

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.12
import QtQml.Models 2.2
import QtQuick.Layouts 1.3

Item {
    height: 500
    width: 355
    objectName: "ImpSettings"

    ImpStyle
    {
        id: impStyle
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
                                text: "Искать проводные датчики"
                                checked: true
                                Material.accent: impStyle.chekedColor

                            }

                            CheckBox {
                                id: cbSearch
                                text: "Искать датчики по протоколу RS-485"
                                Material.accent: impStyle.chekedColor
                            }

                            RadioButton {
                                id: cbModBusSearch
                                text: "Искать датчики через сервер Modbus TCP"
                                Material.accent: impStyle.chekedColor
                            }
                        }
                    }

                    Frame
                    {
                        width: checkFrame.width
                        Column
                        {

                            Row
                            {
                                spacing: 10

                                Button
                                {
                                    id: btAddressAdd
                                    height: 50
                                    width: 150
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
                                    height: 50
                                    width: 150
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


                            ListView
                            {
                                id: lvAddresses
                                height: 200
                                width: 310
                                Material.accent: impStyle.chekedColor
                                focus: true

                                header: Text
                                {
                                    text: "Адреса:"
                                }

                                model: ListModel
                                { // Здесь будет содержаться список адресов
                                    id: lmAddress
                                    objectName: "lmAddress"
                                }
                                delegate: TextField
                                {
                                    width: parent.width
                                    height: 50
                                    property var view: ListView.view
                                    property int itemIndex: index
                                    text: serverAddress
                                    color:  ListView.isCurrentItem ? impStyle.chekedColor : impStyle.unChekedColor
                                    onPressed:
                                    {
                                        view.currentIndex = itemIndex;
                                    }
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
                        text: "Запись одновременно во всех индикаторах"
                        Material.accent: impStyle.chekedColor
                    }
                }


            }
        }
    }
}

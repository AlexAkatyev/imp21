import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQml.Models 2.2
import QtQuick.Layouts 1.3

Item {
    height: 500
    width: 340
    objectName: "ImpSettings"

    ImpStyle
    {
        id: impStyle
    }

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"

        TabBar {
            id: tbSearchSettings
            width: parent.width
            Material.accent: impStyle.chekedColor

            TabButton {
                id: tbnSearch
                text: "Поиск"
                background: Rectangle {
                    color: tbnSearch.hovered ? "lightgray" : "#f0f0f0"
                    width: parent.width
                    visible: tbnSearch.not.checked
                }

                    Rectangle {
                    color: tbnSearch.checked ? "#f0f0f0" : "#fafafa"
                    width: parent.width
                }



            }

            TabButton {
                id: tbnWorkspace
                text: "Рабочее место"
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

                    Row {
                        spacing: 10

                        Button
                        {
                            id: btAddressAdd
                            height: 50
                            width: 150
                            text: "Добавить адрес"
                            font.capitalization: Font.Capitalize
                            icon.name: "address_add"
                            icon.source: "icons/address_add.png"
                            background: Rectangle {
                                color: btAddressAdd.down ? "#F0F0F0" : (btAddressAdd.hovered ? "lightgray" : "#F0F0F0")
                                border.color: "gray"
                            }
                        }

                        Button
                        {
                            id: btAddressRemove
                            height: 50
                            width: 150
                            text: "Удалить адрес"
                            font.capitalization: Font.Capitalize
                            icon.name: "address_remove"
                            icon.source: "icons/address_remove.png"
                            background: Rectangle {
                                color: btAddressRemove.down ? "#F0F0F0" : (btAddressRemove.hovered ? "lightgray" : "#F0F0F0")
                                border.color: "gray"
                            }
                        }
                    }

                    TextArea {
                        height: 240
                        width: 310
                        background: Rectangle {
                        border.color: "gray"
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

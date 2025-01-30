import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQml.Models 2.2
import QtQuick.Layouts 1.3

Item {
    height: 450
    width: 330
    objectName: "Настройки"

    Rectangle {
        height: 450
        width: 330
        color: "#F0F0F0"

        Column {
            spacing: 10
            padding: 10

            Column {
                spacing: -10

                RadioButton {
                    id: cbWireSearch
                    text: "Искать проводные датчики"
                    checked: true

                    indicator: Rectangle {
                        implicitWidth: 21
                        implicitHeight: 21
                        x: cbWireSearch.leftPadding
                        y: parent.height / 2 - height / 2
                        radius: 13
                        border.color: cbWireSearch.checked ? "#225f78" : "gray"
                        border.width: 2

                        Rectangle {
                            width: 11
                            height: 11
                            x: 5
                            y: 5
                            radius: 7
                            color: cbWireSearch.down ? "gray" : "#225f78"
                            visible: cbWireSearch.checked
                        }
                    }
                }

                CheckBox {
                    id: cbSearch
                    text: "Искать датчики по протоколу RS-485"

                    indicator: Rectangle {
                        implicitWidth: 19
                        implicitHeight: 19
                        x: cbSearch.leftPadding
                        y: parent.height / 2 - height / 2
                        radius: 3
                        border.color: cbSearch.checked ? "#225f78" : "gray"
                        border.width: 2

                        Rectangle {
                            width: 19
                            height: 19
                            radius: 3
                            color: cbSearch.down ? "gray" : "#225f78"
                            Image {
                                source: "icons/check.png"
                                height: 19
                                width: 19
                            }
                            visible: cbSearch.checked
                        }
                    }
                }

                RadioButton {
                    id: cbModBusSearch
                    text: "Искать датчики через сервер Modbus TCP"

                    indicator: Rectangle {
                        implicitWidth: 21
                        implicitHeight: 21
                        x: cbModBusSearch.leftPadding
                        y: parent.height / 2 - height / 2
                        radius: 13
                        border.color: cbModBusSearch.checked ? "#225f78" : "gray"
                        border.width: 2

                        Rectangle {
                            width: 11
                            height: 11
                            x: 5
                            y: 5
                            radius: 7
                            color: cbModBusSearch.down ? "gray" : "#225f78"
                            visible: cbModBusSearch.checked
                        }
                    }
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
                height: 235
                width: 310
                background: Rectangle {
                border.color: "gray"
                }
            }
        }
    }
}

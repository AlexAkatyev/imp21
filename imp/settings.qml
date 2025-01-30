import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQml.Models 2.2
import QtQuick.Layouts 1.3

Item {
    height: 450
    width: 330
    objectName: "ImpSettings"

    property color chekedColor: "#225f78"

    Rectangle {
        anchors.fill: parent
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
                        border.color: cbWireSearch.checked ? chekedColor : "gray"
                        border.width: 2

                        Rectangle {
                            width: 11
                            height: 11
                            x: 5
                            y: 5
                            radius: 7
                            color: cbWireSearch.down ? "gray" : chekedColor
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
                        border.color: cbSearch.checked ? chekedColor : "gray"
                        border.width: 2

                        Rectangle {
                            width: 19
                            height: 19
                            radius: 3
                            color: cbSearch.down ? "gray" : chekedColor
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
                        border.color: cbModBusSearch.checked ? chekedColor : "gray"
                        border.width: 2

                        Rectangle {
                            width: 11
                            height: 11
                            x: 5
                            y: 5
                            radius: 7
                            color: cbModBusSearch.down ? "gray" : chekedColor
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

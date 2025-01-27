import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQml.Models 2.2

Item {

    Column {
        id: search_group
        spacing: -10
        padding: -10

        RadioButton {
            text: "Искать проводные датчики"
        }

        CheckBox {
            text: "Искать датчики по протоколу RS-485"
        }

        RadioButton {
            text: "Искать датчики через сервер Modbus TCP"
        }

    }

    ToolSeparator {
        height: 120
    }

    Button
    {
        height: 100
        width: 100
        text: "Добавить
адрес"
        font.capitalization: Font.Capitalize
        icon.name: "address_add"
        icon.source: "address_add.png"
        display: Button.TextUnderIcon
    }

    Button
    {
        height: 100
        width: 100
        text: "Удалить
адрес"
        font.capitalization: Font.Capitalize
        icon.name: "address_remove"
        icon.source: "address_remove.png"
        display: Button.TextUnderIcon
    }

    TextArea {
        height: 100
        width: 100
        background: Rectangle {
        border.color: "gray"
        }
    }

}

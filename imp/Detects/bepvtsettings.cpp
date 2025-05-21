#include <cmath>
#include <QQuickWidget>
#include <QTextCodec>

#include "bepvtsettings.h"
#include "vt21detect.h"
#include "WidgetUtil/DetectModelCommands.h"

//Кодировщик шрифта - код для записи строк в датчик
#define CODE_LOCALLY "Windows-1251"

BepVTSettings::BepVTSettings(VT21Detect* parent, QObject* rootUi)
    : AbstractSettingsController(parent, rootUi)
{
  connect(_detect, &VT21Detect::Stopped, this, &BepVTSettings::deleteLater);

  _codec = QTextCodec::codecForName(CODE_LOCALLY);

  // Установка указателей на объекты виджета QML
  _tModbusAddress = _rootUi->findChild<QObject*>("tModbusAddress");
  _tTxtModbusAddress = _rootUi->findChild<QObject*>("tTxtModbusAddress");

  connect(parent, &VT21Detect::NewDataMeas, this, &BepVTSettings::indicateDataMeas);
  // отработка нажатия кнопки сохранение нового имени датчика
  connect(_rootUi, SIGNAL(sigSetNewUserName(QString, QString)), this, SLOT(setNewUserName(QString, QString)));
}

// Заполнение пользовательского окна "Установки" данными датчика
void BepVTSettings::FillQmlWidget()
{
    AbstractSettingsController::FillQmlWidget();

    _rootUi->setProperty("strTypeSettings", "bep");
    // Определение диапазона измерения
    int lrange = _detect->LMeasureInterval();
    int hrange = _detect->HMeasureInterval();
    if (hrange < lrange)
    {
        int temp = hrange;
        hrange = lrange;
        lrange = temp;
    }
    _rootUi->setProperty("strMeasRange", QString::number(lrange) + " ... " + QString::number(hrange));
    // Определение диапазона обнуления
    _rootUi->setProperty("strZeroRange", "±" + QString::number(_detect->ZeroInterval()));
    // Определение диапазона предустанова
    _rootUi->setProperty("strPreSet", "±" + QString::number(_detect->PreSetInterval()));
    // Определение единиц измерения
    _rootUi->setProperty("strMeasUnit", _detect->MeasUnit());
    // Определение калибровочных точек
    auto pmt = static_cast<VT21Detect*>(_detect)->PMTable();
    int calibrField = static_cast<VT21Detect*>(_detect)->CalibrField();
    _rootUi->setProperty("dataCalibrField", calibrField);

    QString calibTable = "";
    for (int i = 0; i < VT21Detect::SumPoint(); ++i)
    {
        int k = 0;
        calibTable.push_back(QString::number(pmt.at(i).at(k++)) + ";");
        calibTable.push_back(QString::number(pmt.at(i).at(k)) + ";");
    }
    _rootUi->setProperty("dataCalibTable", calibTable);
}


void BepVTSettings::indicateDataMeas(long meas)
{
    _rootUi->setProperty("strCurrentDataValue", QString::number(meas));
    _rootUi->setProperty("strSerialNumber", QString::number(_detect->Id()));
    _rootUi->setProperty("iCommand", DETECTS_MODEL_CMDS::UPDATE_CURRENT_VALUE);
}

// Отправка по протоколу MODBUS команды чтения регистров с результатом измерения датчика с выходом RS485
//void BepVTSettings::SendModbusCommand(void)
//{
//    modbusCommandData.DeviceNumber = SetDeviceData.DeviceSerialNum;
//    // Подготовка кадра Modbus
//    if (((SetDeviceData.typeCode & 0xFF0000) >> 16) != 6){
//        // Это не датчик с энкодером
//        const uchar cmd[] = {COMMAND_READ_MEASURE_DATA};
//        modbusCommandData.Data = createModbusCommand(cmd, sizeof(cmd), static_cast<char>(SetDeviceData.modbusAddress));
//    } else {
//        // Это датчик с энкодером
//        const uchar cmd[] = {COMMAND_READ_ENCODER_DATA};
//        modbusCommandData.Data = createModbusCommand(cmd, sizeof(cmd), static_cast<char>(SetDeviceData.modbusAddress));
//    }
//    emit SendData(modbusCommandData);
//}

// Отправка по протоколу MODBUS команды чтения регистров с инициализационной информацией датчика с выходом RS485
//void BepVTSettings::SendReadRegs(void)
//{
//    modbusCommandData.DeviceNumber = SetDeviceData.DeviceSerialNum;
//    // Подготовка кадра Modbus
//    const uchar cmd[] = {COMMAND_READ_REGS};
//    modbusCommandData.Data = createModbusCommand(cmd, sizeof(cmd), static_cast<char>(SetDeviceData.modbusAddress));

//    emit SendData(modbusCommandData); // Отправка команды COMMAND_READ_REGS
//}


void BepVTSettings::setNewUserName(QString number, QString userName)
{
    if (_detect->Id() == number.toInt())
    {
        _detect->SetNewName(userName);
    }
}


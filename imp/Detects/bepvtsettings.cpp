#include <cmath>
#include <QQuickWidget>
#include <QTextCodec>

#include "bepvtsettings.h"
#include "vt21detect.h"

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
  _txtCurrent = _rootUi->findChild<QObject*>("txtCurrent");

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
//  // Определение калибровочных точек
//  auto pmt = _detect->PMTable();
//  int calibrField = _detect->CalibrField();

//  QString color = "grey";
//  if (!(calibrField & 0x0001))
//  {
//    _rootUi->findChild<QObject*>("tfMP10")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP10")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0002))
//  {
//    _rootUi->findChild<QObject*>("tfMP9")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP9")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0004))
//  {
//    _rootUi->findChild<QObject*>("tfMP8")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP8")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0008))
//  {
//    _rootUi->findChild<QObject*>("tfMP7")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP7")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0010))
//  {
//    _rootUi->findChild<QObject*>("tfMP6")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP6")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0020))
//  {
//    _rootUi->findChild<QObject*>("tfMP5")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP5")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0040))
//  {
//    _rootUi->findChild<QObject*>("tfMP4")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP4")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0080))
//  {
//    _rootUi->findChild<QObject*>("tfMP3")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP3")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0100))
//  {
//    _rootUi->findChild<QObject*>("tfMP2")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP2")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0200))
//  {
//    _rootUi->findChild<QObject*>("tfMP1")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfP1")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0400))
//  {
//    _rootUi->findChild<QObject*>("tfMZERO")->setProperty("color", color);
//   _rootUi->findChild<QObject*>("tfZERO")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x0800))
//  {
//    _rootUi->findChild<QObject*>("tfMM1")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM1")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x1000))
//  {
//    _rootUi->findChild<QObject*>("tfMM2")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM2")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x2000))
//  {
//    _rootUi->findChild<QObject*>("tfMM3")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM3")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x4000))
//  {
//    _rootUi->findChild<QObject*>("tfMM4")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM4")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x8000))
//  {
//    _rootUi->findChild<QObject*>("tfMM5")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM5")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x10000))
//  {
//    _rootUi->findChild<QObject*>("tfMM6")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM6")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x20000))
//  {
//    _rootUi->findChild<QObject*>("tfMM7")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM7")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x40000))
//  {
//    _rootUi->findChild<QObject*>("tfMM8")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM8")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x80000))
//  {
//    _rootUi->findChild<QObject*>("tfMM9")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM9")->setProperty("color", color);
//  }
//  if (!(calibrField & 0x100000))
//  {
//    _rootUi->findChild<QObject*>("tfMM10")->setProperty("color", color);
//    _rootUi->findChild<QObject*>("tfM10")->setProperty("color", color);
//  }
//  _rootUi->findChild<QObject*>("tfMP10")->setProperty("text", pmt.at(0).at(0));
//  _rootUi->findChild<QObject*>("tfMP9")->setProperty("text", pmt.at(1).at(0));
//  _rootUi->findChild<QObject*>("tfMP8")->setProperty("text", pmt.at(2).at(0));
//  _rootUi->findChild<QObject*>("tfMP7")->setProperty("text", pmt.at(3).at(0));
//  _rootUi->findChild<QObject*>("tfMP6")->setProperty("text", pmt.at(4).at(0));
//  _rootUi->findChild<QObject*>("tfMP5")->setProperty("text", pmt.at(5).at(0));
//  _rootUi->findChild<QObject*>("tfMP4")->setProperty("text", pmt.at(6).at(0));
//  _rootUi->findChild<QObject*>("tfMP3")->setProperty("text", pmt.at(7).at(0));
//  _rootUi->findChild<QObject*>("tfMP2")->setProperty("text", pmt.at(8).at(0));
//  _rootUi->findChild<QObject*>("tfMP1")->setProperty("text", pmt.at(9).at(0));
//  _rootUi->findChild<QObject*>("tfMZERO")->setProperty("text", pmt.at(10).at(0));
//  _rootUi->findChild<QObject*>("tfMM1")->setProperty("text", pmt.at(11).at(0));
//  _rootUi->findChild<QObject*>("tfMM2")->setProperty("text", pmt.at(12).at(0));
//  _rootUi->findChild<QObject*>("tfMM3")->setProperty("text", pmt.at(13).at(0));
//  _rootUi->findChild<QObject*>("tfMM4")->setProperty("text", pmt.at(14).at(0));
//  _rootUi->findChild<QObject*>("tfMM5")->setProperty("text", pmt.at(15).at(0));
//  _rootUi->findChild<QObject*>("tfMM6")->setProperty("text", pmt.at(16).at(0));
//  _rootUi->findChild<QObject*>("tfMM7")->setProperty("text", pmt.at(17).at(0));
//  _rootUi->findChild<QObject*>("tfMM8")->setProperty("text", pmt.at(18).at(0));
//  _rootUi->findChild<QObject*>("tfMM9")->setProperty("text", pmt.at(19).at(0));
//  _rootUi->findChild<QObject*>("tfMM10")->setProperty("text", pmt.at(20).at(0));
//  _rootUi->findChild<QObject*>("tfP10")->setProperty("text", pmt.at(0).at(1));
//  _rootUi->findChild<QObject*>("tfP9")->setProperty("text", pmt.at(1).at(1));
//  _rootUi->findChild<QObject*>("tfP8")->setProperty("text", pmt.at(2).at(1));
//  _rootUi->findChild<QObject*>("tfP7")->setProperty("text", pmt.at(3).at(1));
//  _rootUi->findChild<QObject*>("tfP6")->setProperty("text", pmt.at(4).at(1));
//  _rootUi->findChild<QObject*>("tfP5")->setProperty("text", pmt.at(5).at(1));
//  _rootUi->findChild<QObject*>("tfP4")->setProperty("text", pmt.at(6).at(1));
//  _rootUi->findChild<QObject*>("tfP3")->setProperty("text", pmt.at(7).at(1));
//  _rootUi->findChild<QObject*>("tfP2")->setProperty("text", pmt.at(8).at(1));
//  _rootUi->findChild<QObject*>("tfP1")->setProperty("text", pmt.at(9).at(1));
//  _rootUi->findChild<QObject*>("tfZERO")->setProperty("text", pmt.at(10).at(1));
//  _rootUi->findChild<QObject*>("tfM1")->setProperty("text", pmt.at(11).at(1));
//  _rootUi->findChild<QObject*>("tfM2")->setProperty("text", pmt.at(12).at(1));
//  _rootUi->findChild<QObject*>("tfM3")->setProperty("text", pmt.at(13).at(1));
//  _rootUi->findChild<QObject*>("tfM4")->setProperty("text", pmt.at(14).at(1));
//  _rootUi->findChild<QObject*>("tfM5")->setProperty("text", pmt.at(15).at(1));
//  _rootUi->findChild<QObject*>("tfM6")->setProperty("text", pmt.at(16).at(1));
//  _rootUi->findChild<QObject*>("tfM7")->setProperty("text", pmt.at(17).at(1));
//  _rootUi->findChild<QObject*>("tfM8")->setProperty("text", pmt.at(18).at(1));
//  _rootUi->findChild<QObject*>("tfM9")->setProperty("text", pmt.at(19).at(1));
//  _rootUi->findChild<QObject*>("tfM10")->setProperty("text", pmt.at(20).at(1));
}


void BepVTSettings::indicateDataMeas(long meas)
{
//  _txtCurrent->setProperty("text", QString::number(meas));
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


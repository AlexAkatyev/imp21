#include <cmath>
#include <QQuickWidget>
#include <QVBoxLayout>
#include <QQuickItem>
#include <QTextCodec>
#include <QDesktopWidget>

#include "bepvtsettings.h"
#include "vt21detect.h"

// Исходные размеры окна установок
const int SIZE_SETTINGS_WINDOW_X = 400;
const int SIZE_SETTINGS_WINDOW_Y = 450;

//Кодировщик шрифта - код для записи строк в датчик
#define CODE_LOCALLY "Windows-1251"


BepVTSettings::BepVTSettings(VT21Detect* parent)
  : QDialog(nullptr)
  , _quickUi(new QQuickWidget)
  , _detect(parent)
{
  setModal(true);
  connect(_detect, &VT21Detect::Stopped, this, &BepVTSettings::deleteLater);

  _codec = QTextCodec::codecForName(CODE_LOCALLY);

  // Размещение окна установок в центр экрана
  QPoint center = QDesktopWidget().availableGeometry().center(); //получаем координаты центра экрана
  center.setX(center.x() - (SIZE_SETTINGS_WINDOW_X/2));
  center.setY(center.y() - (SIZE_SETTINGS_WINDOW_Y/2));
  setMinimumSize(SIZE_SETTINGS_WINDOW_X, SIZE_SETTINGS_WINDOW_Y);
  setMaximumSize(SIZE_SETTINGS_WINDOW_X, SIZE_SETTINGS_WINDOW_Y);
  move(center);

  // Присвоение имени окну
  this->setWindowTitle("Датчик " + _detect->UserName());
  QUrl source("qrc:/bepvtsettings.qml");
  _quickUi->setSource(source);

  QVBoxLayout* pvbx = new QVBoxLayout();

  pvbx->addWidget(_quickUi); // Вставляем QML виджет в лайоут окна
  pvbx->setMargin(0); // Толщина рамки
  setLayout(pvbx); //  Установка лайоута в окно

  // Установка указателей на объекты виджета QML
  _tModbusAddress = _quickUi->rootObject()->findChild<QObject*>("tModbusAddress");
  _tTxtModbusAddress = _quickUi->rootObject()->findChild<QObject*>("tTxtModbusAddress");
  _txtCurrent = _quickUi->rootObject()->findChild<QObject*>("txtCurrent");

  // Вывод на экран имеющейся информации
  fillQmlWidget();

  connect(parent, &VT21Detect::NewDataMeas, this, &BepVTSettings::indicateDataMeas);
  // отработка нажатия кнопки сохранение нового имени датчика
  connect(_quickUi->rootObject(), SIGNAL(saveUserName()), this, SLOT(setNewUserName()));
}

// Заполнение пользовательского окна "Установки" данными датчика
void BepVTSettings::fillQmlWidget()
{
  // Вывод на экран имеющейся информации
  _quickUi->rootObject()->findChild<QObject*>("tfName")->setProperty("text", _detect->UserName());
  _quickUi->rootObject()->findChild<QObject*>("txtSerialNum")->setProperty("text", _detect->Id());
  _quickUi->rootObject()->findChild<QObject*>("txtType")->setProperty("text", _detect->TypeDetect());
  _quickUi->rootObject()->findChild<QObject*>("txtConnect")->setProperty("text", _detect->PortName());
  _tModbusAddress->setProperty("text", _detect->Address());

  _quickUi->rootObject()->findChild<QObject*>("txtData")->setProperty("text", _detect->DateManuf().toString("dd.MM.yyyy"));
  _quickUi->rootObject()->findChild<QObject*>("tfCP")->setProperty("text", QString::number(_detect->CountPeriod()));// Определение числа периодов измерения
  // Определение диапазона измерения
  int lrange = _detect->LMeasureInterval();
  int hrange = _detect->HMeasureInterval();
  if (hrange < lrange)
  {
    int temp = hrange;
    hrange = lrange;
    lrange = temp;
  }
  _quickUi->rootObject()->findChild<QObject*>("tfRange")->setProperty(
        "text", QString::number(lrange) + " ... " + QString::number(hrange));
  // Определение диапазона обнуления
  _quickUi->rootObject()->findChild<QObject*>("tfSetZero")->setProperty(
        "text", "±" + QString::number(_detect->ZeroInterval()));
  // Определение диапазона предустанова
  _quickUi->rootObject()->findChild<QObject*>("tfPreSet")->setProperty(
        "text", "±" + QString::number(_detect->PreSetInterval()));
  // Определение единиц измерения
  _quickUi->rootObject()->findChild<QObject*>("tfUM")->setProperty("text", _detect->MeasUnit());
  // Определение калибровочных точек
  auto pmt = _detect->PMTable();
  int calibrField = _detect->CalibrField();

  QString color = "grey";
  if (!(calibrField & 0x0001))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP10")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP10")->setProperty("color", color);
  }
  if (!(calibrField & 0x0002))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP9")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP9")->setProperty("color", color);
  }
  if (!(calibrField & 0x0004))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP8")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP8")->setProperty("color", color);
  }
  if (!(calibrField & 0x0008))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP7")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP7")->setProperty("color", color);
  }
  if (!(calibrField & 0x0010))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP6")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP6")->setProperty("color", color);
  }
  if (!(calibrField & 0x0020))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP5")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP5")->setProperty("color", color);
  }
  if (!(calibrField & 0x0040))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP4")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP4")->setProperty("color", color);
  }
  if (!(calibrField & 0x0080))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP3")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP3")->setProperty("color", color);
  }
  if (!(calibrField & 0x0100))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP2")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP2")->setProperty("color", color);
  }
  if (!(calibrField & 0x0200))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMP1")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfP1")->setProperty("color", color);
  }
  if (!(calibrField & 0x0400))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMZERO")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfZERO")->setProperty("color", color);
  }
  if (!(calibrField & 0x0800))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM1")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM1")->setProperty("color", color);
  }
  if (!(calibrField & 0x1000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM2")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM2")->setProperty("color", color);
  }
  if (!(calibrField & 0x2000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM3")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM3")->setProperty("color", color);
  }
  if (!(calibrField & 0x4000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM4")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM4")->setProperty("color", color);
  }
  if (!(calibrField & 0x8000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM5")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM5")->setProperty("color", color);
  }
  if (!(calibrField & 0x10000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM6")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM6")->setProperty("color", color);
  }
  if (!(calibrField & 0x20000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM7")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM7")->setProperty("color", color);
  }
  if (!(calibrField & 0x40000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM8")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM8")->setProperty("color", color);
  }
  if (!(calibrField & 0x80000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM9")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM9")->setProperty("color", color);
  }
  if (!(calibrField & 0x100000))
  {
    _quickUi->rootObject()->findChild<QObject*>("tfMM10")->setProperty("color", color);
    _quickUi->rootObject()->findChild<QObject*>("tfM10")->setProperty("color", color);
  }
  _quickUi->rootObject()->findChild<QObject*>("tfMP10")->setProperty("text", pmt.at(0).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP9")->setProperty("text", pmt.at(1).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP8")->setProperty("text", pmt.at(2).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP7")->setProperty("text", pmt.at(3).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP6")->setProperty("text", pmt.at(4).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP5")->setProperty("text", pmt.at(5).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP4")->setProperty("text", pmt.at(6).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP3")->setProperty("text", pmt.at(7).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP2")->setProperty("text", pmt.at(8).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMP1")->setProperty("text", pmt.at(9).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMZERO")->setProperty("text", pmt.at(10).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM1")->setProperty("text", pmt.at(11).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM2")->setProperty("text", pmt.at(12).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM3")->setProperty("text", pmt.at(13).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM4")->setProperty("text", pmt.at(14).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM5")->setProperty("text", pmt.at(15).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM6")->setProperty("text", pmt.at(16).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM7")->setProperty("text", pmt.at(17).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM8")->setProperty("text", pmt.at(18).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM9")->setProperty("text", pmt.at(19).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfMM10")->setProperty("text", pmt.at(20).at(0));
  _quickUi->rootObject()->findChild<QObject*>("tfP10")->setProperty("text", pmt.at(0).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP9")->setProperty("text", pmt.at(1).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP8")->setProperty("text", pmt.at(2).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP7")->setProperty("text", pmt.at(3).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP6")->setProperty("text", pmt.at(4).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP5")->setProperty("text", pmt.at(5).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP4")->setProperty("text", pmt.at(6).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP3")->setProperty("text", pmt.at(7).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP2")->setProperty("text", pmt.at(8).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfP1")->setProperty("text", pmt.at(9).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfZERO")->setProperty("text", pmt.at(10).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM1")->setProperty("text", pmt.at(11).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM2")->setProperty("text", pmt.at(12).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM3")->setProperty("text", pmt.at(13).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM4")->setProperty("text", pmt.at(14).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM5")->setProperty("text", pmt.at(15).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM6")->setProperty("text", pmt.at(16).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM7")->setProperty("text", pmt.at(17).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM8")->setProperty("text", pmt.at(18).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM9")->setProperty("text", pmt.at(19).at(1));
  _quickUi->rootObject()->findChild<QObject*>("tfM10")->setProperty("text", pmt.at(20).at(1));
}


// переопределение события изменения окна
void BepVTSettings::resizeEvent(QResizeEvent* event)
{
  // Масштабирование QML виджета под размер окна
  QSize baseSize(static_cast<int>(_quickUi->rootObject()->width()), static_cast<int>(_quickUi->rootObject()->height()));
  _quickUi->rootObject()->setProperty("scaleX", QVariant(this->width()));
  _quickUi->rootObject()->setProperty("scaleY", QVariant(this->height()));
  event->accept();
}


void BepVTSettings::indicateDataMeas(long meas)
{
  _txtCurrent->setProperty("text", QString::number(meas));
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


void BepVTSettings::setNewUserName()
{
  QVariant vname = _quickUi->rootObject()->findChild<QObject*>("tfName")->property("text");
  _detect->SetNewName(vname.toString());
}


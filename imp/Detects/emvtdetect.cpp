#include <QElapsedTimer>
#include <QApplication>
#include <QTimer>

#include "emvtdetect.h"
#include "emvtsettings.h"
#include "Logger/logger.h"

const int WAIT_FOR_READY_READ = 250;
const int WAIT_START = 250;
const int MAX_WAIT_INIT = WAIT_START + WAIT_FOR_READY_READ + 10;

QByteArray DAT00 = "EM08";
const int AT_STATUS = 4;
const int DEF_MINUS = 45; // "-"
const int DEF_PLUS = 43; // "+"
const int DEF_EQ = 61; // "="
const int DEF_H = 62; // ">"
const int DEF_L = 60; // "<"
const int AT_RES100 = AT_STATUS + 1;
const int AT_RES10 = AT_RES100 + 1;
const int AT_RES1 = AT_RES10 + 1;
const int AT_RES01 = AT_RES1 + 1;
const int AT_BUTTON = AT_RES01 + 1;
const int DEF_FIX = 70; // "F"
const int AT_YEAR10 = AT_BUTTON + 1;
const int AT_YEAR1 = AT_YEAR10 + 1;
const int AT_SN1000 = AT_YEAR1 + 1;
const int AT_SN100 = AT_SN1000 + 1;
const int AT_SN10 = AT_SN100 + 1;
const int AT_SN1 = AT_SN10 + 1;
const int LEN_DATA = 16;


inline int getNumberFromData(char dat, int scale = 1)
{
  return (dat-48) * scale;
}


EmVTDetect::EmVTDetect(QSerialPortInfo portInfo, QObject* parent)
  : VTDetect(portInfo, parent)
{
  _hMeasInterval = 256;
  _lMeasInterval = -256;
  _unitMeasure = "мкм";
  _zeroInterval = 25;
  _preSetInterval = 110;
  _typeDetect = "Электронный модуль типа ЕМ-08";
}


void EmVTDetect::Init()
{
  if (_port->isOpen())
    Logger::GetInstance()->WriteLnLog("Порт " + _port->portName() + " уже используется");
  else if (_port->open(QIODevice::ReadWrite))
  {
    _wdt->start();
    // Установка параметров порта
    _port->setBaudRate(baudRate(), direction());
    _port->setParity(parity());
    _port->setDataBits(dataBits());
    _port->setStopBits(stopBits());
    _port->setReadBufferSize(bufferSize());
    _port->setFlowControl(flowControl());

    _port->waitForReadyRead(WAIT_FOR_READY_READ);
    {
      QElapsedTimer timeWait;
      timeWait.start();
      for(;timeWait.elapsed() < WAIT_START;) // Время ожидания, мсек
        qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
    }
    QByteArray receiveData = _port->readAll();
    getInfoFromData(receiveData);
    if (Id())
    {
      disconnect(_port, &QSerialPort::readyRead, this, &EmVTDetect::routeInput);
      connect(_port, &QSerialPort::readyRead, this, &EmVTDetect::routeInput);
      _wdt->start();
    }
  }
}


QSerialPort::BaudRate EmVTDetect::baudRate()
{
  return QSerialPort::Baud9600;
}


int EmVTDetect::WaitInit()
{
  return MAX_WAIT_INIT;
}


void EmVTDetect::Stop()
{
}


void EmVTDetect::getInfoFromData(QByteArray input)
{
  int i = input.indexOf(DAT00);
  if ( i != -1)
  {
    QByteArray receiveData = input.right(input.size() - i); // trimm receiving
    if (receiveData.size() >= LEN_DATA)
    {
      _serialNumber = getNumberFromData(receiveData.at(AT_YEAR10), 10000)
          + getNumberFromData(receiveData.at(AT_YEAR1), 1000)
          + getNumberFromData(receiveData.at(AT_SN100), 100)
          + getNumberFromData(receiveData.at(AT_SN10), 10)
          + getNumberFromData(receiveData.at(AT_SN1));
      int year = getNumberFromData(receiveData.at(AT_YEAR10), 10)
          + getNumberFromData(receiveData.at(AT_YEAR1))
          + 2000;
      _userName = "ЕМ-08 №" + QString::number(_serialNumber);
      _dateManuf.setDate(year, 1, 1);
      _flagReady = true;
    }
  }
}


void EmVTDetect::routeInput()
{
  if (!_port)
    return;
  _input += _port->readAll();
  int i = _input.indexOf(DAT00);
  if ( i == -1)
    return;
  QByteArray receiveData = _input.right(_input.size() - i); // trimm receiving
  if (receiveData.size() < LEN_DATA)
    return;
  int znak = 1;
  switch (receiveData.at(AT_STATUS))
  {
  case DEF_EQ:
    _measure = 0;
    emit NewMeasure(_measure);
    break;
  case DEF_L:
  case DEF_MINUS:
    znak = -1;
  case DEF_H:
  case DEF_PLUS:
    _measure = getNumberFromData(receiveData.at(AT_RES100), 100)
        + getNumberFromData(receiveData.at(AT_RES10), 10)
        + getNumberFromData(receiveData.at(AT_RES1))
        + getNumberFromData(receiveData.at(AT_RES01)) * 0.1;
    _measure *= znak;
    emit NewMeasure(_measure);
    _flagReady = true;
    _wdt->start();
    break;
  default:
    break;
  }

  setStateButton(receiveData.at(AT_BUTTON) == DEF_FIX);

  _input.clear();
}


void EmVTDetect::ShowSettings()
{
  EmVTSettings* settingsWindow = new EmVTSettings(this);
  settingsWindow->show();
}


int EmVTDetect::bufferSize(SizeBufMode mode)
{
  Q_UNUSED(mode)
  return 3 * LEN_DATA;
}

#include <QTimer>

#include "vtdetect.h"
#include "Logger/logger.h"

const int WDT_INTERVAL = 1000;

VTDetect::VTDetect(QSerialPortInfo portInfo, QObject *parent)
  : ImpAbstractDetect(parent)
  , _port(new QSerialPort(portInfo, this))
{
  _wdt = new QTimer(this);
  _wdt->setInterval(WDT_INTERVAL);
  connect(_wdt,
          &QTimer::timeout,
          this,
          [&]()
  {
    _flagReady = false;
  });
  _wdt->start();
}


void VTDetect::Remove()
{
  Stop();
  _port->close();
  deleteLater();
}


QSerialPort::BaudRate VTDetect::baudRate()
{
  return QSerialPort::Baud38400;
}


QSerialPort::Direction VTDetect:: direction()
{
  return QSerialPort::AllDirections;
}


QSerialPort::Parity VTDetect::parity()
{
  return QSerialPort::NoParity;
}


QSerialPort::DataBits VTDetect::dataBits()
{
  return QSerialPort::Data8;
}


QSerialPort::StopBits VTDetect::stopBits()
{
  return QSerialPort::OneStop;
}


int VTDetect::bufferSize(SizeBufMode mode)
{
  Q_UNUSED(mode);
  return 0; // не ограничен, гарантия отсутствия потери данных
}


QSerialPort::FlowControl VTDetect::flowControl()
{
  return QSerialPort::NoFlowControl;
}


int VTDetect::WaitInit()
{
  return 500;
}


QString VTDetect::PortName()
{
  return _port->portName();
}


bool VTDetect::Ready()
{
  auto portNoError = [](QSerialPort::SerialPortError err) -> bool
  {
    return (err == QSerialPort::NoError || err == QSerialPort::TimeoutError);
  };

  QSerialPort::SerialPortError error = _port ? _port->error() : QSerialPort::UnknownError;
  if (!portNoError(error) && _port)
    Logger::GetInstance()->WriteLnLog("Ready Порт " + _port->portName() + " : Ошибка " + QString::number(error));
  else if (_port == nullptr)
    Logger::GetInstance()->WriteLnLog("Ready Датчик " + QString::number(Id()) + " не готов. Порт не определен.");
  if (!_flagReady)
    Logger::GetInstance()->WriteLnLog("Ready Датчик " + QString::number(Id()) + " - не принимает данные.");
  if (!_port)
    Logger::GetInstance()->WriteLnLog("Ready Датчик " + QString::number(Id()) + " - нет порта.");
  else if (!_port->isOpen())
    Logger::GetInstance()->WriteLnLog("Ready Датчик " + QString::number(Id()) + " - порт закрыт.");
  return _flagReady
      && _port
      && _port->isOpen()
      && portNoError(error);
}

#include "vtdetect.h"

VTDetect::VTDetect(QSerialPortInfo portInfo, QObject *parent)
  : QObject(parent)
  , _measure(0)
  , _port(new QSerialPort(portInfo, this))
  , _serialNumber(0)
  , _typeDetect(tr("Не определен"))
  , _dateManuf(QDate::currentDate())
  , _hMeasInterval(0)
  , _lMeasInterval(0)
  , _zeroInterval(0)
  , _preSetInterval(0)
  , _unitMeasure("mkm")
  , _userName("")
  , _flagReady(false)
  , _address(0)
{
}


void VTDetect::Init()
{

}


void VTDetect::Stop()
{
  emit Stopped();
}


void VTDetect::Remove()
{
  Stop();
  _port->close();
  deleteLater();
}


float VTDetect::CurrentMeasure()
{
  return _measure;
}


int VTDetect::Id()
{
  return _serialNumber;
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

int VTDetect::bufferSize()
{
  return 1024;
}

QSerialPort::FlowControl VTDetect::flowControl()
{
  return QSerialPort::NoFlowControl;
}


int VTDetect::WaitInit()
{
  return 500;
}


QString VTDetect::TypeDetect()
{
  return _typeDetect;
}


QDate VTDetect::DateManuf()
{
  return _dateManuf;
}

QString VTDetect::UserName()
{
  return _userName;
}


QString VTDetect::PortName()
{
  return _port->portName();
}


QString VTDetect::MeasUnit()
{
  return _unitMeasure;
}


int VTDetect::HMeasureInterval()
{
  return _hMeasInterval;
}


int VTDetect::LMeasureInterval()
{
  return _lMeasInterval;
}


int VTDetect::ZeroInterval()
{
  return _zeroInterval;
}


int VTDetect::PreSetInterval()
{
  return _preSetInterval;
}


void VTDetect::ShowSettings()
{

}


void VTDetect::SetNewName(QString newName)
{
  Q_UNUSED(newName)
}


bool VTDetect::Ready()
{
  int error = _port ? _port->error() : QSerialPort::UnknownError;
  return _flagReady
      && _port
      && _port->isOpen()
      && (error == QSerialPort::NoError || error == QSerialPort::TimeoutError);
}


QString VTDetect::Address()
{
  return _address == 0 ? "" : QString::number(_address);
}



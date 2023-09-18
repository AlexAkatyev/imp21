
#include <QSerialPort>
#include <QTimer>

#include <QDebug>

#include "serialportlocator.h"

SerialPortLocator::SerialPortLocator(QSerialPortInfo info, QObject *parent)
  : QObject(parent)
  , _port(new QSerialPort(info, this))
{
  if (_port->open(QIODevice::ReadWrite))
  {
    // Установка параметров порта
    _port->setBaudRate(QSerialPort::Baud38400, QSerialPort::AllDirections);
    _port->setParity(QSerialPort::NoParity);
    _port->setDataBits(QSerialPort::Data8);
    _port->setStopBits(QSerialPort::OneStop);
    _port->setReadBufferSize(5000);
    _port->setFlowControl(QSerialPort::NoFlowControl);
    _port->waitForReadyRead(50);
    //emit Received(_port->readAll());
  }

  connect(_port, &QSerialPort::readyRead, this, [&]()
  {
    emit Received(_port->readAll());
  });
}


bool SerialPortLocator::Ready()
{
  bool result = _port && _port->isOpen();
  return result;
}


void SerialPortLocator::Write(QByteArray message)
{
  if (_port->isOpen())
  {
    //qDebug()  << "send : " << _port->portName() << " : " << message;
    _port->write(message);
    _port->flush();
  }
}


void SerialPortLocator::Remove()
{
  _port->close();
  deleteLater();
}


QString SerialPortLocator::PortName()
{
  return _port->portName();
}



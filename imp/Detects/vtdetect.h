#ifndef VTDETECT_H
#define VTDETECT_H
/*
 *
 * This is universal abstract Vipp-tehnika detect
 *
 * */


#include <QSerialPortInfo>
#include <QSerialPort>

#include "Detects/impabstractdetect.h"

class QTimer;

enum SizeBufMode {FREE_MODE, INIT_MODE, MEAS_MODE};

class VTDetect : public ImpAbstractDetect
{
  Q_OBJECT
public:
  explicit VTDetect(QSerialPortInfo portInfo, QObject *parent);
  void Remove() override;
  int WaitInit() override;
  QString PortName() override;
  bool Ready() override;
  QString Address() override;

protected:
  QSerialPort* _port;
  QTimer* _wdt;

  virtual QSerialPort::BaudRate baudRate();
  virtual QSerialPort::Direction direction();
  virtual QSerialPort::Parity parity();
  virtual QSerialPort::DataBits dataBits();
  virtual QSerialPort::StopBits stopBits();
  virtual int bufferSize(SizeBufMode mode = SizeBufMode::FREE_MODE);
  virtual QSerialPort::FlowControl flowControl();

};

#endif // VTDETECT_H

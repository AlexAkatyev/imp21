#ifndef VTDETECT_H
#define VTDETECT_H
/*
 *
 * This is universal abstract Vipp-tehnika detect
 *
 * */


#include <QObject>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDate>

class VTDetect : public QObject
{
  Q_OBJECT
public:
  explicit VTDetect(QSerialPortInfo portInfo, QObject *parent);
  virtual void Init();
  virtual void Stop();
  void Remove();
  float CurrentMeasure();
  int Id();
  virtual int WaitInit();
  QString TypeDetect();
  QDate DateManuf();
  QString UserName();
  QString PortName();
  QString MeasUnit();
  int HMeasureInterval();
  int LMeasureInterval();
  int ZeroInterval();
  int PreSetInterval();
  virtual void ShowSettings();
  virtual void SetNewName(QString);
  bool Ready();
  virtual QString Address();

signals:
  void NewMeasure(float);
  void Stopped();
  void UserNameChanged();
  void FixMeasure();

protected:
  float _measure;
  QSerialPort* _port;
  int _serialNumber;
  QString _typeDetect;
  QDate _dateManuf;
  int _hMeasInterval;
  int _lMeasInterval;
  int _zeroInterval;
  int _preSetInterval;
  QString _unitMeasure;
  QString _userName;
  bool _flagReady;
  char _address;

  virtual QSerialPort::BaudRate baudRate();
  virtual QSerialPort::Direction direction();
  virtual QSerialPort::Parity parity();
  virtual QSerialPort::DataBits dataBits();
  virtual QSerialPort::StopBits stopBits();
  virtual int bufferSize();
  virtual QSerialPort::FlowControl flowControl();

};

#endif // VTDETECT_H

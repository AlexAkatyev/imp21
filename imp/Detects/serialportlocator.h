#ifndef SERIALPORTLOCATOR_H
#define SERIALPORTLOCATOR_H

#include <QObject>
#include <QSerialPortInfo>

class QSerialPort;

class SerialPortLocator : public QObject
{
  Q_OBJECT
public:
  explicit SerialPortLocator(QSerialPortInfo info, QObject *parent);
  bool Ready();
  void Write(QByteArray message);
  void Remove();
  QString PortName();

signals:
  void Received(QByteArray message);

private:
  QSerialPort* _port;

};

#endif // SERIALPORTLOCATOR_H

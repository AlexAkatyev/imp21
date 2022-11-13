#ifndef EMVTDETECT_H
#define EMVTDETECT_H

#include "vtdetect.h"

class QTimer;

class EmVTDetect : public VTDetect
{
  Q_OBJECT
public:
  explicit EmVTDetect(QSerialPortInfo portInfo, QObject *parent);
  void Init() override;
  void Stop() override;
  QSerialPort::BaudRate baudRate() override;
  int WaitInit() override;
  void ShowSettings() override;

private:
  void getInfoFromData(QByteArray input);
  void routeInput();

  QTimer* _wdt;
  QByteArray _input;
};

#endif // EMVTDETECT_H

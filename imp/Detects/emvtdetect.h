#ifndef EMVTDETECT_H
#define EMVTDETECT_H

#include "vtdetect.h"

class EmVTDetect : public VTDetect
{
  Q_OBJECT
public:
  explicit EmVTDetect(QSerialPortInfo portInfo, QObject *parent);
  void Init() override;
  void Stop() override;
  QSerialPort::BaudRate baudRate() override;
  int WaitInit() override;
  void CreateSettingsController(QObject* rooiUi) override;

private:
  void getInfoFromData(QByteArray input);
  void routeInput();
  int bufferSize(SizeBufMode mode = SizeBufMode::FREE_MODE) override;

  QByteArray _input;
};

#endif // EMVTDETECT_H

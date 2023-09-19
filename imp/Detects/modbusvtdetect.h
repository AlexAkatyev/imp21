#ifndef MODBUSVTDETECT_H
#define MODBUSVTDETECT_H

#include "vt21detect.h"

class SerialPortLocator;
class QTimer;

class ModbusVTDetect : public VT21Detect
{
  Q_OBJECT
public:
  explicit ModbusVTDetect(QSerialPortInfo portInfo, QObject *parent = nullptr);
  void Init() override;
  void Stop() override;
  int WaitInit() override;
  void ShowSettings() override;
  void SetNewName(QString newName) override;
  void SetAddres(int addres);
  void SetLocator(SerialPortLocator* locator);
  QString Address() override;

private:
  //char _addres;
  SerialPortLocator* _locator;
  std::vector<std::vector<int>> _currentReadRegisters; // reg, length
  QTimer* _measTimer;
  QByteArray _inputBuffer;
  QByteArray _baDataInput;


  void receveInputMessage(QByteArray message);
  void readRegisters(int reg, int length);
  void fillParameters(int startReg, int len, QByteArray& data);
};

#endif // MODBUSVTDETECT_H

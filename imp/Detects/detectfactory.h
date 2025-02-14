#ifndef DETECTFACTORY_H
#define DETECTFACTORY_H

#include <QObject>

#include "Detects/impabstractdetect.h"

class SerialPortLocator;
class MBTcpLocator;

class DetectFactory : public QObject
{
  Q_OBJECT
public:
  static DetectFactory* Instance(QObject *parent = nullptr);
  ~DetectFactory();
  int AvailablePorts();
  int FindingTime();
  void StartFindOfDetects();
  std::vector<ImpAbstractDetect*> VTDetects();
  std::vector<ImpAbstractDetect*> TestDetects();

signals:
  void readyOfDetects();

private:
  std::vector<SerialPortLocator*> _mbSpLocators;
  std::vector<MBTcpLocator*> _mbTcpLocators;
  std::vector<ImpAbstractDetect*> _detects;

  explicit DetectFactory(QObject *parent);
  void comVTDetects();
  void tcpVTDetects();
  void waitElapsed(int ms);
  void readRequest(MBTcpLocator* s, int startAddress, quint16 numberOfEntries);
  void sendReadyOfDetects();
};

#endif // DETECTFACTORY_H

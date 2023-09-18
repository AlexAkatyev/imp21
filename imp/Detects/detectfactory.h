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
  std::vector<ImpAbstractDetect*> VTDetects();

private:
  std::vector<SerialPortLocator*> _mbSpLocators;
  std::vector<MBTcpLocator*> _mbTcpLocators;

  explicit DetectFactory(QObject *parent);
  std::vector<ImpAbstractDetect*> ComVTDetects();
  std::vector<ImpAbstractDetect*> TcpVTDetects();
  void waitElapsed(int ms);
  void readRequest(MBTcpLocator* s, int startAddress, quint16 numberOfEntries);
};

#endif // DETECTFACTORY_H

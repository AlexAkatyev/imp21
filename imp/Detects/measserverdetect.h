#ifndef MEASSERVERDETECT_H
#define MEASSERVERDETECT_H

#include "Detects/impabstractdetect.h"

class MBTcpLocator;

class MeasServerDetect : public ImpAbstractDetect
{
  Q_OBJECT
public:
  explicit MeasServerDetect(MBTcpLocator* locator, int countD, QObject* parent);
  void Init() override;
  QString PortName() override;
  void ShowSettings() override;

private:
  MBTcpLocator* _locator;
  int _numberD;
  int _counter;
};

#endif // MEASSERVERDETECT_H

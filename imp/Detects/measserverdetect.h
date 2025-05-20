#ifndef MEASSERVERDETECT_H
#define MEASSERVERDETECT_H

#include "Detects/impabstractdetect.h"

class QTimer;
class MBTcpLocator;

class MeasServerDetect : public ImpAbstractDetect
{
  Q_OBJECT
public:
  explicit MeasServerDetect(MBTcpLocator* locator, int countD, QObject* parent);
  void Init() override;
  QString PortName() override;
  void CreateSettingsController(QObject* rootUi) override;
  void ReportReadyWrite(bool ready) override;

private:
  void updateState();

  MBTcpLocator* _locator;
  int _numberD;
  int _counterWDT;
  QTimer* _stateTimer;
};

#endif // MEASSERVERDETECT_H

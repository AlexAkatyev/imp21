#ifndef BEPVTDETECT_H
#define BEPVTDETECT_H

#include "vt21detect.h"

class QTimer;

class BepVTDetect : public VT21Detect
{
  Q_OBJECT
public:
  explicit BepVTDetect(QSerialPortInfo portInfo, QObject *parent = nullptr);
  void Init() override;
  void Stop() override;
  int WaitInit() override;
  void ShowSettings() override;
  void SetNewName(QString newName) override;

protected:
  void getMeas();

private:
  bool defSerialNumber(const QByteArray& mas);
  bool defTypeDetect(const QByteArray& mas);
  bool defDataManufDetect(const QByteArray& mas);
  bool defCountPeriod(const QByteArray& mas);
  bool defMeasureInterval(const QByteArray& mas);
  bool defZeroInterval(const QByteArray& mas);
  bool defPreSetInterval(const QByteArray& mas);
  bool defUnitMeasure(const QByteArray& mas);
  bool defUserName(const QByteArray& mas);
  bool defCalibrField(const QByteArray& mas);
  bool defCalibrateDataTable(const QByteArray& baStream);

  QTimer* _measTimer;
};

#endif // BEPVTDETECT_H

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
  void CreateSettingsController(QObject* rootUi) override;
  void SetNewName(QString newName) override;
  void SetAddress(int address) override;

protected:
  void getMeas();

private:
  bool defSerialNumber(const QByteArray& mas);
  bool defTypeDetect(const QByteArray& mas);
  bool defDataManufDetect(const QByteArray& mas);
  bool defAddress(const QByteArray& mas);
  bool defMeasureInterval(const QByteArray& mas);
  bool defZeroInterval(const QByteArray& mas);
  bool defPreSetInterval(const QByteArray& mas);
  bool defUnitMeasure(const QByteArray& mas);
  bool defUserName(const QByteArray& mas);
  bool defCalibrField(const QByteArray& mas);
  bool defCalibrateDataTable(const QByteArray& baStream);
  int bufferSize(SizeBufMode mode = SizeBufMode::FREE_MODE) override;
  void sendSaveCMD();

  QTimer* _measTimer;
};


class TestBepVTDetect: public BepVTDetect
{
  Q_OBJECT
public:
  explicit TestBepVTDetect(QSerialPortInfo portInfo, QObject *parent = nullptr);
};

#endif // BEPVTDETECT_H

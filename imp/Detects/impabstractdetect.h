#ifndef IMPABSTRACTDETECT_H
#define IMPABSTRACTDETECT_H

#include <QObject>
#include <QDate>

class ImpAbstractDetect : public QObject
{
  Q_OBJECT
public:
  explicit ImpAbstractDetect(QObject* parent);
  virtual void Init();
  virtual void Stop();
  virtual void Remove();
  float CurrentMeasure();
  int Id();
  virtual int WaitInit();
  QString TypeDetect();
  QDate DateManuf();
  QString UserName();
  virtual QString PortName();
  QString MeasUnit();
  int HMeasureInterval();
  int LMeasureInterval();
  int ZeroInterval();
  int PreSetInterval();
  virtual void ShowSettings();
  virtual void SetNewName(QString);
  virtual bool Ready();
  virtual QString Address();

signals:
  void NewMeasure(float);
  void Stopped();
  void UserNameChanged();
  void FixMeasure();

protected:
  float _measure;
  int _serialNumber;
  QString _typeDetect;
  QDate _dateManuf;
  QString _unitMeasure;
  QString _userName;
  int _hMeasInterval;
  int _lMeasInterval;
  int _zeroInterval;
  int _preSetInterval;
  bool _flagReady;

};

#endif // IMPABSTRACTDETECT_H

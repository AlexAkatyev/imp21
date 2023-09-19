#ifndef MBTCPLOCATOR_H
#define MBTCPLOCATOR_H

#include <QModbusTcpClient>
#include <QDate>

class QModbusReply;
class QTimer;

class MBTcpLocator : public QModbusTcpClient
{
public:
  MBTcpLocator(QObject* parent);
  void OnReadReady(QModbusReply* reply);
  void Init();
  bool IsMBTcpLocator();
  int CountDetects();
  int DetectSerialNumber(int numberD);
  QString DetectType(int numberD);
  QDate DetectDateManuf(int numberD);
  QString UnitMeasure(int numberD);
  QString NameDetect(int numberD);
  int MeasInterval(int numberD);
  int SetZero(int numberD);
  int PreSet(int numberD);
  float Measure(int numberD);

private:
  std::map<qint16, qint16> _regs;
  QTimer* _initTimer;
  int _initStep;

  void readRequest(int startAddress, quint16 numberOfEntries);
  void initContinue();
  int regData(qint16 reg, int numberD);
  QString dataToString(qint16 reg, int numberD, int length);
};

#endif // MBTCPLOCATOR_H

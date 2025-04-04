#ifndef MBTCPLOCATOR_H
#define MBTCPLOCATOR_H

#include <QModbusTcpClient>
#include <QDate>

class QModbusReply;
class QTimer;
class QUdpSocket;

class MBTcpLocator : public QModbusTcpClient
{
  Q_OBJECT
public:
  MBTcpLocator(QObject* parent);
  void OnReadReady(QModbusReply* reply);
  void Init();
  bool IsMBTcpLocator();
  int CountDetects();
  int DetectId(int numberD);
  bool ActivityState(int numberD);
  QString DetectType(int numberD);
  QDate DetectDateManuf(int numberD);
  QString UnitMeasure(int numberD);
  QString NameDetect(int numberD);
  int MeasLoLimitInterval(int numberD);
  int MeasHiLimitInterval(int numberD);
  int SetZero(int numberD);
  int PreSet(int numberD);
  float Measure(int numberD);
  QString PortName();
  void ReportReadyWrite(int numberD, bool ready);

signals:
  void ReadyMeasure(int id, float meas);
  void PedalPressed(bool press, int id);

private:
  std::map<qint16, qint16> _regs;
  QTimer* _initTimer;
  QTimer* _stateTimer;
  int _initStep;
  int _stateIndex;
  QUdpSocket* _socket;
  bool _setUdpConnect;

  void readRequest(int startAddress, quint16 numberOfEntries);
  void initContinue();
  int regData(qint16 reg, int numberD);
  int regDataWrite(qint16 reg, int numberD);
  QByteArray dataToByteArray(qint16 reg, int numberD, int length);
  void readFromUdpSocket();
  int numberD(int id);
  float getFloatFromRegMeas(qint16 reg, int numberD);

};

#endif // MBTCPLOCATOR_H

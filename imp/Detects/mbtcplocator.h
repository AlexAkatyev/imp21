#ifndef MBTCPLOCATOR_H
#define MBTCPLOCATOR_H

#include <QModbusTcpClient>

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

private:
  std::map<qint16, qint16> _regs;
  QTimer* _initTimer;
  int _initStep;

  void readRequest(int startAddress, quint16 numberOfEntries);
  void initContinue();
};

#endif // MBTCPLOCATOR_H

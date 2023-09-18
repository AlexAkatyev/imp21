#include <QModbusReply>
#include <QModbusDataUnit>
#include <QTimer>

#include "mbtcplocator.h"
#include "measservermap.h"
#include "impdef.h"
#include "Logger/logger.h"

const int INIT_INTERVAL = 500;

MBTcpLocator::MBTcpLocator(QObject* parent)
  : QModbusTcpClient(parent)
  , _initTimer(new QTimer(this))
  , _initStep(-2)
{
  _initTimer->setInterval(INIT_INTERVAL);
  connect(_initTimer, &QTimer::timeout, this, &MBTcpLocator::initContinue);
}


void MBTcpLocator::OnReadReady(QModbusReply* reply)
{
  if (!reply)
    return;

  if (reply->error() == QModbusDevice::NoError)
  {
    const QModbusDataUnit unit = reply->result();
    int total = unit.valueCount();
    for (int i = 0; i < total; ++i)
    {
      _regs[unit.startAddress() + i + 1] = unit.value(i);
      Logger::GetInstance()->WriteLnLog(QString("[%1] = %2").arg(unit.startAddress() + i + 1).arg(unit.value(i)));
    }
  }

  reply->deleteLater();
}


void MBTcpLocator::Init()
{
  readRequest(REG_FACTORY_CODE1, 16);
  Logger::GetInstance()->WriteLnLog(QString("Request. step %1. count detects %2").arg(_initStep).arg(CountDetects()));
  ++_initStep;
  _initTimer->start();
}


void MBTcpLocator::initContinue()
{
  if (_initStep == -1)
  {
    readRequest(REG_LIST_ID, CountDetects());
    Logger::GetInstance()->WriteLnLog(QString("Request. step %1. count detects %2").arg(_initStep).arg(CountDetects()));
    ++_initStep;
  }
  else if (_initStep < CountDetects())
  {
    readRequest(REG_CURRENT_MEAS + LEN_STEP_DETECT * _initStep, LEN_DATA_DETECT);
    Logger::GetInstance()->WriteLnLog(QString("Request. step %1. count detects %2").arg(_initStep).arg(CountDetects()));
    ++_initStep;
  }
  else
  {
    _initTimer->stop();
    Logger::GetInstance()->WriteLnLog(QString("Init timer stopped. step %1. count detects %2").arg(_initStep).arg(CountDetects()));
  }
}


void MBTcpLocator::readRequest(int startAddress, quint16 numberOfEntries)
{
  if (numberOfEntries == 0)
    return;
  if (auto* reply = sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress - 1, numberOfEntries), DEF_ID_DEVICE))
  {
      if (!reply->isFinished())
          connect(reply, &QModbusReply::finished, this, [=]()
          {
            OnReadReady(reply);
          });
      else
          delete reply; // broadcast replies return immediately
  }
}


bool MBTcpLocator::IsMBTcpLocator()
{
  return _regs[REG_FACTORY_CODE1] == FACTORY_CODE1
         && _regs[REG_FACTORY_CODE2] == FACTORY_CODE2
         && _regs[REG_PRODUCT_CODE] == PRODUCT_CODE;
}


int MBTcpLocator::CountDetects()
{
  if (IsMBTcpLocator())
    return _regs[REG_DETECTS_COUNT];
  else
    return 0;
}


int MBTcpLocator::DetectSerialNumber(int numberD)
{
  return 0;
}




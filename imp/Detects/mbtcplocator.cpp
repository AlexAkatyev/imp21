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
  return _regs[REG_LIST_ID + numberD];
}


int MBTcpLocator::regData(qint16 reg, int numberD)
{
  return numberD * LEN_STEP_DETECT + reg;
}


QString MBTcpLocator::DetectType(int numberD)
{
  QString result = "Тип не определен";
  switch (_regs[regData(REG_TYPE_DETECT, numberD)])
  {
  case 1:
    result = "Датчик с частотным преобразованием";
    break;
  case 2:
    result = "Датчик с синхронным детектированием";
    break;
  case 3:
    result = "Датчик с частотным преобразованием с ADG419";
    break;
  case 4:
    result = "Датчик с манометрическое преобразованием";
    break;
  case 5:
    result = "Датчик для вискозиметра АКВ-2В";
    break;
  }
  return result;
}


QDate MBTcpLocator::DetectDateManuf(int numberD)
{
  QDate result;

  uint dm = _regs[regData(REG_DATE_MANUF, numberD)];
  int month = static_cast<unsigned char>(dm);
  int day = static_cast<unsigned char>(dm >> 8);
  int year = _regs[regData(REG_YEAR_MANUF, numberD)];
  bool error = !result.setDate(year, month, day);
  if (error)
    result.setDate(2021, 9, 1); // Дата начала изготовления этого типа датчиков
  return result;
}


QString MBTcpLocator::UnitMeasure(int numberD)
{
  return dataToString(REG_UNIT_DETECT, numberD, LEN_UNIT_DETECT);
}


QString MBTcpLocator::NameDetect(int numberD)
{
  return dataToString(REG_NAME_DETECT, numberD, LEN_NAME_DETECT);
}


QString MBTcpLocator::dataToString(qint16 reg, int numberD, int length)
{
  QString result;
  for (int i = 0; i < length; ++i)
  {
    uint d = _regs[regData(reg + i, numberD)];
    result.push_back(static_cast<char>(d >> 8));
    result.push_back(static_cast<char>(d));
  }
  return result.trimmed();
}


int MBTcpLocator::MeasInterval(int numberD)
{
  return abs(_regs[regData(REG_INTERVAL, numberD)]);
}


int MBTcpLocator::SetZero(int numberD)
{
  return _regs[regData(REG_SET_ZERO, numberD)];
}


int MBTcpLocator::PreSet(int numberD)
{
  return _regs[regData(REG_PRESET, numberD)];
}


float MBTcpLocator::Measure(int numberD)
{
  qint32 h = _regs[regData(REG_CURRENT_MEAS, numberD)] << 16;
  qint32 l = _regs[regData(REG_CURRENT_MEAS + 1, numberD)] & 0xFFFF;
  h += l;
  float result = h;
  return result/1000;
}

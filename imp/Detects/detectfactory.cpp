#include <QApplication>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QElapsedTimer>
#include <QModbusTcpClient>
#include <QModbusDataUnit>

#include "detectfactory.h"
#include "Detects/bepvtdetect.h"
#include "Detects/serialportlocator.h"
#include "Detects/modbusvtdetect.h"
#include "Detects/emvtdetect.h"
#include "Logger/logger.h"
#include "impsettings.h"
#include "impdef.h"
#include "UtilLib/modbus.h"
#include "measservermap.h"
#include "Detects/mbtcplocator.h"
#include "Detects/measserverdetect.h"

const int BEP_WAIT_INIT = 1600;
const int EM_WAIT_INIT = 1000;
const int MODBUS_WAIT_INIT = 2000;
const int MBTCP_WAIT_INIT = 6000;
const int MBTCDETECT_WAIT_INIT = 50;
std::vector<int> V{BEP_WAIT_INIT, EM_WAIT_INIT, MODBUS_WAIT_INIT + 1000, MBTCP_WAIT_INIT};
const int MAX_WAIT_INIT = *std::max_element(V.begin(), V.end());


DetectFactory* DetectFactory::Instance(QObject* parent)
{
  static DetectFactory* df = new DetectFactory(parent);
  return df;
}

DetectFactory::DetectFactory(QObject *parent)
  : QObject(parent)
{
  _detects.clear();
}


DetectFactory::~DetectFactory()
{
  for (auto d : _mbTcpLocators)
  {
    d->blockSignals(true);
    d->disconnectDevice();
  }
}


int DetectFactory::AvailablePorts()
{
  return QSerialPortInfo::availablePorts().size();
}


int DetectFactory::FindingTime()
{
  return AvailablePorts() * MAX_WAIT_INIT;
}


void DetectFactory::StartFindOfDetects()
{
  _detects.clear();
  if (ImpSettings::Instance()->Value(ImpKeys::EN_MODBUS_TCP).toBool())
  {
    tcpVTDetects();
    if (_detects.empty())
    {
      comVTDetects();
    }
  }
  else
  {
    comVTDetects();
    if (_detects.empty())
    {
      tcpVTDetects();
    }
  }

  sendReadyOfDetects();
}


std::vector<ImpAbstractDetect*> DetectFactory::VTDetects()
{
  return _detects;
}


void DetectFactory::comVTDetects()
{
  Logger* logger = Logger::GetInstance();

  QList<QSerialPortInfo> ListPort;
  ListPort = QSerialPortInfo::availablePorts();
  logger->WriteLnLog("Поиск проводных датчиков");
  if (ListPort.size() > 0) // Что-то делаем, если вообще есть устройства
  {
    // Получаем от системы список COM-портов, на которых сидят устройства
    for (QSerialPortInfo& info : ListPort)
    {
      logger->WriteLnLog("Порт " + info.portName());
      // BepVTDetect
      VTDetect* bepVTD = new BepVTDetect(info, parent());
      bepVTD->Init();
      if (bepVTD->Id())
      {
        _detects.push_back(bepVTD);
        logger->WriteLnLog("Это датчик USB");
        continue;
      }
      else
        bepVTD->Remove();


      // EmVTDetect
      logger->WriteLnLog("Ищу измерительную головку ЭМ-08");
      VTDetect* em = new EmVTDetect(info, parent());
      em->Init();
      if (em->Id())
      {
        _detects.push_back(em);
        logger->WriteLnLog("Это измерительная головка ЭМ-08");
        continue;
      }
      else
        em->Remove();


      // ModbusVTDetect
      if (ImpSettings::Instance()->Value(ImpKeys::EN_RS_485).toBool())
      {
        Logger::GetInstance()->WriteLnLog("Ищу ModbusVTDetect в " + info.portName());
        bool notFind = true;
        bool newLocator = false;
        SerialPortLocator* locator = nullptr;
        for (auto& l : _mbSpLocators)
          if (l->PortName() == info.portName())
          {
            locator = l;
            break;
          }
        if (locator == nullptr)
        {
          locator = new SerialPortLocator(info, parent());
          newLocator = true;
        }
        if (locator->Ready())
        {
          std::vector<ModbusVTDetect*> detects;
          for (int i = 17; i < 18; ++i) // (int i = 1; i < 256; ++i)
          {
            ModbusVTDetect* modbusVTD = new ModbusVTDetect(info, parent());
            detects.push_back(modbusVTD);
            modbusVTD->SetAddress(i);
            modbusVTD->SetLocator(locator);
            modbusVTD->Init();
          }
          waitElapsed(MODBUS_WAIT_INIT);
          for (auto detect : detects)
          {
            if (detect->Id())
            {
              _detects.push_back(detect);
              logger->WriteLnLog("Это датчик RS-485");
              notFind = false;
              continue;
            }
            else
              detect->deleteLater();
          }
        }
        if (newLocator && notFind)
          locator->Remove();
        else if (newLocator)
          _mbSpLocators.push_back(locator);
        Logger::GetInstance()->WriteLnLog("Окончил поиски ModbusVTDetect в " + info.portName());
      }
    }
  }
}


void DetectFactory::sendReadyOfDetects()
{
  emit readyOfDetects();
}


void DetectFactory::tcpVTDetects()
{
  Logger::GetInstance()->WriteLnLog("новый поиск серверов");
  // refresh list of sockets
  QStringList slAddr = ImpSettings::Instance()->Value(ImpKeys::LIST_MB_ADDR).toStringList();
  if (slAddr.isEmpty())
    slAddr << DEF_MB_SERVER;
  for (QString socketAddr : slAddr)
  {
    MBTcpLocator* client = nullptr;
    for (auto& s : _mbTcpLocators)
      if (s->connectionParameter(QModbusDevice::NetworkAddressParameter).toString() == socketAddr)
      {
        client = s;
        break;
      }
    if (client == nullptr)
    {
      Logger::GetInstance()->WriteLnLog("Начало поиска сервера на " + socketAddr);
      client = new MBTcpLocator(parent());
      client->setConnectionParameter(QModbusDevice::NetworkPortParameter, DEF_MB_TCP_PORT);
      client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, socketAddr);
      client->setTimeout(MBTCP_WAIT_INIT * 0.95);
      client->setNumberOfRetries(10);
      _mbTcpLocators.push_back(client);
      connect(client, &MBTcpLocator::stateChanged, this, [=](QModbusDevice::State state)
      {

        Logger::GetInstance()->WriteLnLog("client " + client->PortName() + " : state : " + QString::number(state));
        if (state == QModbusDevice::ConnectedState)
        {
          client->Init();
          Logger::GetInstance()->WriteLnLog("Подключились. Инициализация");
        }
      });
      client->connectDevice();
    }
    else
    {
      if (client->state() == QModbusDevice::ConnectedState)
        client->Init();
      else
        client->connectDevice();
    }
  }

  waitElapsed(MBTCP_WAIT_INIT);

  bool createdDetect = false;
  int countTcpDetects = 0;
  for (MBTcpLocator* s : _mbTcpLocators)
    for (int i = 0; i < s->CountDetects(); ++i)
    {
      MeasServerDetect* msd = new MeasServerDetect(s, i, parent());
      createdDetect = true;
      msd->Init();
      _detects.push_back(msd);
      ++countTcpDetects;
    }
  if (createdDetect)
  {
    waitElapsed(std::max(MBTCDETECT_WAIT_INIT * countTcpDetects / 10, MBTCDETECT_WAIT_INIT));
  }
}


void DetectFactory::waitElapsed(int ms)
{
  QElapsedTimer timew;
  timew.start();
  while(timew.elapsed() < ms) // Время ожидания, мсек
    qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
}


void DetectFactory::readRequest(MBTcpLocator* s, int startAddress, quint16 numberOfEntries)
{
  if (auto* reply = s->sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress - 1, numberOfEntries), DEF_ID_DEVICE))
  {
      if (!reply->isFinished())
          connect(reply, &QModbusReply::finished, this, [=]()
          {
            s->OnReadReady(reply);
          });
      else
          delete reply; // broadcast replies return immediately
  }
}


std::vector<ImpAbstractDetect*> DetectFactory::TestDetects()
{
  std::vector<ImpAbstractDetect*> testDetects;
  EmVTDetect* d1 = new EmVTDetect(QSerialPortInfo("COM50"), parent());
  EmVTDetect* d2 = new EmVTDetect(QSerialPortInfo("COM51"), parent());
  EmVTDetect* d3 = new EmVTDetect(QSerialPortInfo("COM52"), parent());
  TestBepVTDetect* d4 = new TestBepVTDetect(QSerialPortInfo("COM53"), parent());
  testDetects.push_back(d1);
  testDetects.push_back(d2);
  testDetects.push_back(d3);
  testDetects.push_back(d4);
  return testDetects;
}

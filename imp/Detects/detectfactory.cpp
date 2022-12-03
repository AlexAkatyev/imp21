#include <QApplication>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QElapsedTimer>

#include "detectfactory.h"
#include "Detects/bepvtdetect.h"
#include "Detects/serialportlocator.h"
#include "Detects/modbusvtdetect.h"
#include "Detects/emvtdetect.h"
#include "Logger/logger.h"

const int BEP_WAIT_INIT = 1000;
const int EM_WAIT_INIT = 1000;
const int MODBUS_WAIT_INIT = 2000;
std::vector<int> V{BEP_WAIT_INIT, EM_WAIT_INIT, MODBUS_WAIT_INIT + 1000};
const int MAX_WAIT_INIT = *std::max_element(V.begin(), V.end());


DetectFactory::DetectFactory(QObject *parent)
  : QObject(parent)
{

}


int DetectFactory::AvailablePorts()
{
  return QSerialPortInfo::availablePorts().size();
}


int DetectFactory::FindingTime()
{
  return AvailablePorts() * MAX_WAIT_INIT;
}


std::vector<VTDetect*> DetectFactory::VTDetects()
{
  Logger* logger = Logger::GetInstance();
  std::vector<VTDetect*> result;

  QList<QSerialPortInfo> ListPort;
  ListPort = QSerialPortInfo::availablePorts();
  logger->WriteLnLog("Поиск датчиков");
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
        result.push_back(bepVTD);
        logger->WriteLnLog("Это датчик USB");
        continue;
      }
      else
        bepVTD->Remove();


      // EmVTDetect
      VTDetect* em = new EmVTDetect(info, parent());
      em->Init();
      if (em->Id())
      {
        result.push_back(em);
        logger->WriteLnLog("Это измерительная головка ЭМ-08");
        continue;
      }
      else
        em->Remove();


      // ModbusVTDetect
      SerialPortLocator* locator = new SerialPortLocator(info, parent());
      bool notFind = true;
      if (locator->Ready())
      {
        std::vector<ModbusVTDetect*> detects;
        for (int i = 1; i < 256; ++i)
        {
          ModbusVTDetect* modbusVTD = new ModbusVTDetect(info, parent());
          detects.push_back(modbusVTD);
          modbusVTD->SetAddres(i);
          modbusVTD->SetLocator(locator);
          modbusVTD->Init();
        }
        {
          QElapsedTimer timew;
          timew.start();
          while(timew.elapsed() < MODBUS_WAIT_INIT) // Время ожидания, мсек
            qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
        }
        for (auto detect : detects)
        {
          if (detect->Id())
          {
            result.push_back(detect);
            logger->WriteLnLog("Это датчик RS-485");
            notFind = false;
            continue;
          }
          else
            detect->deleteLater();
        }
      }
      if (notFind)
        locator->Remove();

    }
  }

  return result;
}


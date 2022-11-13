#include <QTimer>

#include "modbusvtdetect.h"
#include "UtilLib/modbus.h"
#include "serialportlocator.h"
#include "bepvtsettings.h"

#include <QDebug>

const int MIN_ADR = 0;
const int MAX_ADR = 0xFF;


ModbusVTDetect::ModbusVTDetect(QSerialPortInfo portInfo, QObject *parent)
  : VT21Detect(portInfo, parent)
  , _locator(nullptr)
  , _measTimer(new QTimer(this))
{
  _calibrField = -1;
  _currentReadRegisters.clear();
  _measTimer->setInterval(500);
  connect(_measTimer, &QTimer::timeout, this, [&]()
  {
    readRegisters(0x0025, 2);
  });
  _inputBuffer.clear();
}


void ModbusVTDetect::SetAddres(int addres)
{
  if (addres >= MIN_ADR && addres <= MAX_ADR)
    _address = (char)addres;
}


void ModbusVTDetect::SetLocator(SerialPortLocator* locator)
{
  if (_locator)
    disconnect(_locator, &SerialPortLocator::Received, this, &ModbusVTDetect::receveInputMessage);
  _locator = locator;
  _flagReady &= _locator != nullptr;
  if (_locator)
    connect(_locator, &SerialPortLocator::Received, this, &ModbusVTDetect::receveInputMessage);
}


void ModbusVTDetect::Init()
{
  if (_address < MIN_ADR || _locator == nullptr)
    return;
  readRegisters(0, 0x6F);
}


void ModbusVTDetect::readRegisters(int reg, int length)
{
  _currentReadRegisters.push_back({reg, length});
  if (_currentReadRegisters.size() == 1)
    _locator->Write(ModbusCommandRead(_currentReadRegisters.at(0).at(0),
                                      _currentReadRegisters.at(0).at(1),
                                      _address));
}


void ModbusVTDetect::Stop()
{
  _measTimer->stop();
}


int ModbusVTDetect::WaitInit()
{
  return 0;
}


void ModbusVTDetect::ShowSettings()
{
  BepVTSettings* settingsWindow = new BepVTSettings(this);
  settingsWindow->show();
}


void ModbusVTDetect::SetNewName(QString newName)
{
  Q_UNUSED(newName)
}


void ModbusVTDetect::receveInputMessage(QByteArray message)
{
  if (_currentReadRegisters.empty()) // ничего не жду
    return;

  //qDebug() << "receive " << QString::number(_address) << " : " << message;

  _inputBuffer += message;
  while (_inputBuffer.size() > 1
         && _inputBuffer.at(0) != _address
         && (_inputBuffer.at(1) != COMMAND_READ_REGS || _inputBuffer.at(1) != ERROR_READ_REGS))
    _inputBuffer.remove(0, 1);

  while (_inputBuffer.size() > 5)
  {
//    MessageIndex indexes = GetSizeFirstReadMessage(inputBuffer);
//    if (indexes.begin == indexes.end)
//      return;

//    if (inputBuffer.at(indexes.begin) == _addres)
//      inputBuffer.remove(0, indexes.begin);
//    else
//    {
//      inputBuffer.remove(0, indexes.end);
//      continue;
//    }
    int len = _currentReadRegisters.at(0).at(1);
    int reg = _currentReadRegisters.at(0).at(0);
    if (_inputBuffer.at(1) == COMMAND_READ_REGS)
    {
      int sizeMessage = 3 + 2 * len + 2;
      if (_inputBuffer.size() < sizeMessage) // мало осталось
        return;
      _currentReadRegisters.erase(_currentReadRegisters.begin());

      //qDebug() << "fillParam " << QString::number(_address) << " : " << inputBuffer;
      fillParameters(reg, len, _inputBuffer);

      _inputBuffer.remove(0, sizeMessage);
      if (_currentReadRegisters.size()) // отправка следующей команды на чтение, если есть
        _locator->Write(ModbusCommandRead(_currentReadRegisters.at(0).at(0),
                                          _currentReadRegisters.at(0).at(1),
                                          _address));
    }
    else if (_inputBuffer.at(1) == ERROR_READ_REGS)
    {
      int sizeMessage = 5;
      if (_inputBuffer.size() < sizeMessage) // мало осталось
        return;
      _inputBuffer.remove(0, sizeMessage);
      _currentReadRegisters.erase(_currentReadRegisters.begin());
      readRegisters(reg, len);
    }
  }
}


void ModbusVTDetect::fillParameters(int startReg, int len, QByteArray& data)
{
  int reg = startReg;
  int end = 3 + 2 * len + 2;
  int d;
  long long dataMeas = 0;
  bool flDataMeas = false;
  for (int i = 3; i < end; ++i)
  {
    switch (reg++)
    {
    case 0x0000:
      _flagReady = data.at(i++) == static_cast<char>(0xFE);
      _flagReady &= data.at(i) == static_cast<char>(0xDC);
      break;
    case 0x0001:
      _flagReady &= data.at(i++) == static_cast<char>(0xBA);
      _flagReady &= data.at(i) == static_cast<char>(0x98);
      break;
    case 0x0002:
      d = data.at(i++);
      d += data.at(i) << 8;
      _serialNumber = d;
      break;
    case 0x0003:
      ++i;
      _typeDetect.clear();
      d = static_cast<unsigned char>(data.at(i));
      if (d == 1)
        _typeDetect.append("Датчик с частотным преобразованием");
      if (d == 2)
        _typeDetect.append("Датчик с синхронным детектированием");
      if (d == 3)
        _typeDetect.append("Датчик с частотным преобразованием");
      if (d == 4)
        _typeDetect.append("Датчик с манометрическим преобразованием");
      break;
    case 0x0004:
      d = static_cast<unsigned char>(data.at(i));
      if (d == 0)
        _typeDetect.append(" с ADG419");
      if (d == 1)
        _typeDetect.append(" с ADG419 и RS-232");
      ++i;
      break;
    case 0x0005:
    {
      int day = static_cast<unsigned char>(data.at(i++)); // день
      int month = static_cast<unsigned char>(data.at(i)); // месяц
      int year = _dateManuf.year();
      _dateManuf.setDate(year, month, day);
    }
      break;
    case 0x0006:
    {
      int day = _dateManuf.day(); // день
      int month = _dateManuf.month(); // месяц
      int year = static_cast<unsigned char>(data.at(i++))*100; // век
      year += static_cast<unsigned char>(data.at(i)); // год
      _dateManuf.setDate(year, month, day);
    }
      break;
    case 0x0007:
      d = data.at(i++);
      d += data.at(i) << 8;
      _countPeriod = d;
      break;
    case 0x0008:
      d = data.at(i++);
      d += data.at(i) << 8;
      _hMeasInterval = d;
      break;
    case 0x0009: // единица измерения
      if (i+16 < end)
      {
        QByteArray baUnitMes;
        baUnitMes.clear();
        for (int k = 0; k < 16; ++k)
          baUnitMes.push_back(data.at(i+k));
        _unitMeasure = getLocallyString(baUnitMes);
        ++i;
      }
      break;
    case 0x0011: // имя датчика
      if (i+32 < end)
      {
        QByteArray baUserName;
        baUserName.clear();
        for (int k = 0; k < 32; ++k)
          baUserName.push_back(data.at(i+k));
        _userName = getLocallyString(baUserName);
        ++i;
      }
      break;
    case 0x0021:
      d = data.at(i++);
      d += data.at(i) << 8;
      _zeroInterval = d;
      break;
    case 0x0022:
      d = data.at(i++);
      d += data.at(i) << 8;
      _preSetInterval = d;
      break;
    case 0x0025:
      dataMeas = data.at(i++);
      dataMeas += data.at(i) << 8;
      flDataMeas = true;
      break;
    case 0x0026:
      dataMeas += data.at(i++) << 24;
      dataMeas += data.at(i) << 16;
      if (flDataMeas)
      {
        if (dataMeas > 0x7FFFFFFF) // Значение отрицательное
            dataMeas = dataMeas - 0x0100000000;
        calcCalibrateResult(static_cast<int>(dataMeas));
      }
      break;
    case 0x0030: // калибровочная таблица
    {
      std::vector<int> index = {1, 0, 5, 4, 3, 2};
      int sizePunkt = index.size();
      if (i + sizePunkt * SumPoint() < end)
      {
        QByteArray baData;
        int sumPoint = SumPoint();
        for (int k = 0; k < sumPoint; ++k)
          for (int ind : index)
            baData.push_back(data.at(i + sizePunkt * k + ind));
        if (!fillCalibrateDataTable(baData, 2, 4))
        {
          defHMeasureInterval();
          defLMeasureInterval();
          _measTimer->start();
        }
        ++i;
      }
    }
      break;
    default:
      ++i;
      break;
    }
  }
}


QString ModbusVTDetect::Address()
{
  return QString::number(_address);
}


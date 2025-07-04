#include <QApplication>
#include <QElapsedTimer>
#include <QTimer>

#include "bepvtdetect.h"
#include "bepvtsettings.h"

#include "Logger/logger.h"
#include "UtilLib/utillib.h"

// Количество калибровочных точек
const int SUM_POINT = VT21Detect::SumPoint();

const int WAIT_OF_DTR = 100;
const int WAIT_OF_ANSWER_WAIT = 300;
const int WAIT_OF_ANSWER_INIT = 1000;//500
const int WAIT_FOR_READY_READ = 300;
const int WAIT_OF_ANSWER_SAVE = 700;
//const int WAIT_BEFORE_CLOSE_PORT = 500;
const int UPDATE_MEAS = 100;

/*  ---------------------------------------------
 *  ---------------------------------------------
 *  Описание протокола взаимодействия с датчиками
 *  ---------------------------------------------
 *--------------------------------------------  */

// Описание команд датчиков
#define COMMAND_INIT_DETECT         "INIT"
#define COMMAND_WAIT_DETECT         "WAIT"
#define COMMAND_SAVE_DETECT         "SAVE"

// Конец команд INIT и SAVE
#define END_SAVE1                   0x55
#define END_SAVE2                   0x55

// Описание ответа на INIT
// Header
const int AT_HEADERS =              0;
#define ANSWER_INIT_DETECT1         0xDD
#define ANSWER_INIT_DETECT2         0xCC
#define ANSWER_INIT_DETECT3         0xBB
#define ANSWER_INIT_DETECT4         0xAA
const int LEN_HEADERS =             4;
// SerialNum
const int AT_SERIAL_NUMBER =        AT_HEADERS+LEN_HEADERS;
const int LEN_SERIAL_NUMBER =       2;

const int AT_TYPE_DETECT =          AT_SERIAL_NUMBER+LEN_SERIAL_NUMBER;
const int LEN_TYPE_DETECT =         3;
// FirmwareVersion $08, $00, $03
const int AT_INTERNAL_VER =         AT_TYPE_DETECT+LEN_TYPE_DETECT;
const int LEN_INTERNAL_VER =        3;
// ReleaseDate
// пример: $0A,$09,$14,$0E – 10 сентября 2014г.
const int AT_DATA_MANUF =           AT_INTERNAL_VER+LEN_INTERNAL_VER;
const int LEN_DATA_MANUF =          4;
// MeasureInterval Количество измерительных интервалов в периоде
const int AT_MODBUS_ADDRESS = AT_DATA_MANUF+LEN_DATA_MANUF;
const int LEN_MODBUS_ADDRESS = 2;
// RangeBorder Диапазон измерения
const int AT_LIMIT_MESS =           AT_MODBUS_ADDRESS+LEN_MODBUS_ADDRESS;
const int LEN_LIMIT_MESS =          2;
// Диапазон обнуления
const int AT_ZERO_SET =             AT_LIMIT_MESS+LEN_LIMIT_MESS;
const int LEN_ZERO_SET =            2;
// Диапазон предустанова
const int AT_PRE_SET =              AT_ZERO_SET+LEN_ZERO_SET;
const int LEN_PRE_SET =             2;
// Unit
const int AT_MESS1 =                AT_PRE_SET+LEN_PRE_SET;
const int LEN_MESS1 =               4;
// CalibrationTable -10 .... 10
const int AT_CALIBR =               AT_MESS1+LEN_MESS1;
const int LEN_POINT =               2;
const int LEN_POIN2 =               4;
const int LEN_MEASSURE_POINT =      4;
const int LEN_CALIBR =              (LEN_POINT+LEN_MEASSURE_POINT)*SUM_POINT;
const int LEN_CALIB2 =              (LEN_POIN2+LEN_MEASSURE_POINT)*SUM_POINT;
// Name
const int AT_NAME_DETECT =          AT_CALIBR+LEN_CALIBR;
const int AT_NAME_DETEC2 =          AT_CALIBR+LEN_CALIB2;
const int LEN_NAME_DETECT =         16;
// битовое поле точек калибровки (младший бит – точка калибровки +10 и т.д.)
/*
0 – калибровка не проводилась,
1 – калибровка проводилась
*/
const int AT_CALIBR_FIELD =         AT_NAME_DETECT+LEN_NAME_DETECT;
const int AT_CALIBR_FIEL2 =         AT_NAME_DETEC2+LEN_NAME_DETECT;
const int LEN_CALIBR_FIELD =        4;
// Footer
const int AT_END_OF_INIT =          AT_CALIBR_FIELD+LEN_CALIBR_FIELD;
const int AT_END_OF_INI2 =          AT_CALIBR_FIEL2+LEN_CALIBR_FIELD;
const int LEN_END_OF_INIT =         2;
const int LEN_ANSWER_INIT =         AT_END_OF_INIT+LEN_END_OF_INIT;                                // Размер INIT
const int LEN_ANSWER_INI2 =         AT_END_OF_INI2+LEN_END_OF_INIT;                                // Размер INIT


// Описание сообщения MEASUREMENT
// Header
//const int AT_HEADER_MEAS  =         0;
#define HEADER_MEAS1                0xBF
#define HEADER_MEAS2                0xB5
#define HEADER_MEAS3                0xD5
#define HEADER_MEAS4                0xBD
const int LEN_HEADER_MEAS=          4;
const int LEN_N_MEAS =              4;
const int LEN_MEAS =                LEN_HEADER_MEAS + LEN_N_MEAS + LEN_N_MEAS;


int QByteArrayAtLenToInt(const QByteArray& mas, int index, int length)
{
  int i = 0;
  for (int count = 0; count < length; ++count)
  {
    i = i << 8;
    i += static_cast<unsigned char>(mas.at(index + count));
  }
  return i;
}


BepVTDetect::BepVTDetect(QSerialPortInfo portInfo, QObject *parent)
  : VT21Detect(portInfo, parent)
  , _measTimer(new QTimer())
{
  _measTimer->setInterval(UPDATE_MEAS);
  connect(_measTimer, &QTimer::timeout, this, &BepVTDetect::getMeas);
}


void BepVTDetect::Init()
{
  Logger* logger = Logger::GetInstance();

  if (_port->isOpen())
  {
    logger->WriteLnLog("Порт " + _port->portName() + " уже используется");
    return;
  }

  logger->WriteLnLog("Открытие порта " + _port->portName());
  if (_port->open(QIODevice::ReadWrite))
  {
    logger->WriteLnLog("Инициализация");
    // Установка параметров порта
    _port->setBaudRate(baudRate(), direction());
    _port->setParity(parity());
    _port->setDataBits(dataBits());
    _port->setStopBits(stopBits());
    _port->setReadBufferSize(bufferSize(SizeBufMode::INIT_MODE));
    _port->setFlowControl(flowControl());

    _port->setDataTerminalReady(true);

    _port->waitForReadyRead(WAIT_FOR_READY_READ); // ожидаем готовность порта принимать

    {
      QElapsedTimer timeDTR;
      timeDTR.start();
      for(;timeDTR.elapsed() < WAIT_OF_DTR;) // Время ожидания, мсек
        qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
    }

    _port->write(COMMAND_WAIT_DETECT);
    _port->flush();
    logger->WriteLnLog("Отправил WAIT");
    {
      QElapsedTimer timew;
      timew.start();
      for(;timew.elapsed() < WAIT_OF_ANSWER_WAIT;) // Время ожидания, мсек
        qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
    }

    QByteArray receiveData;
    // Отправка команды
    //_port->waitForReadyRead(WAIT_FOR_READY_READ); // ожидаем готовность порта принимать
    _port->write(COMMAND_INIT_DETECT);
    //_port->flush();
    _port->waitForBytesWritten(WAIT_OF_ANSWER_WAIT);
    logger->WriteLnLog("Отправил INIT");

    // Выдержка для получения данных с порта для анализа
    {
      QElapsedTimer timei;
      timei.start();
      for(;timei.elapsed() < WAIT_OF_ANSWER_INIT;) // Время ожидания, мсек
        qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
    }
    // Читаем данные с порта
    logger->WriteLnLog("Подождал");
    receiveData = _port->readAll();
    logger->WriteLnLog("Прочитал буфер");
    logger->WriteBytes(receiveData);
    logger->WriteLnLog("Ошибка порта : " + QString::number(_port->error()));

    int i = receiveData.indexOf(ANSWER_INIT_DETECT1);
    if ( i != -1
        && static_cast<unsigned char>(receiveData.at(i+1)) == ANSWER_INIT_DETECT2
        && static_cast<unsigned char>(receiveData.at(i+2)) == ANSWER_INIT_DETECT3
        && static_cast<unsigned char>(receiveData.at(i+3)) == ANSWER_INIT_DETECT4)
    {
      _wdt->start();
      _flagReady = true;

      receiveData = receiveData.right(receiveData.size() - i); // trimm receiving
      bool error = defSerialNumber(receiveData)
          | defTypeDetect(receiveData)
          | defDataManufDetect(receiveData)
          | defAddress(receiveData)
          | defMeasureInterval(receiveData)
          | defZeroInterval(receiveData)
          | defPreSetInterval(receiveData)
          | defUnitMeasure(receiveData)
          | defUserName(receiveData)
          | defCalibrField(receiveData)
          | defCalibrateDataTable(receiveData);

      if (error)
        _serialNumber = 0;
      else
      { // датчик начинает работать на прием измерений
        _port->setReadBufferSize(bufferSize(SizeBufMode::MEAS_MODE));
        _measTimer->start();
      }
    }
  }
  else
    logger->WriteLnLog("Порт не доступен");
}


void BepVTDetect::Stop()
{
  if (_port->isOpen())
  {
    _port->write(COMMAND_WAIT_DETECT);
    _port->flush();
    _measTimer->stop();
    _flagReady = false;
    emit Stopped();
    Logger::GetInstance()->WriteLnLog("Закрыл порт " + _port->portName());
  }
}


void BepVTDetect::getMeas()
{
  QByteArray header;
  header += HEADER_MEAS1;
  header += HEADER_MEAS2;
  header += HEADER_MEAS3;
  header += HEADER_MEAS4;
  QByteArray receiveData = _port->readAll();

  int i = receiveData.lastIndexOf(header);
  if ( i != -1
       && ((i + LEN_MEAS) <= receiveData.size()))
  {
    long N1 = 0;
    long N2 = 0;
    i += LEN_HEADER_MEAS;
    for (int index = 0; index < LEN_N_MEAS; ++index)
    {
      N1 = N1 << 8;
      N2 = N2 << 8;
      N1 += static_cast<unsigned char>(receiveData.at(index + i));
      N2 += static_cast<unsigned char>(receiveData.at(index + i + LEN_N_MEAS));
    }
    long long dataMeas = static_cast<long long>(N1 - N2);
    if (dataMeas > 0x7FFFFFFF) // Значение отрицательное
        dataMeas = dataMeas - 0x0100000000;
    _flagReady = true;
    _wdt->start();
    calcCalibrateResult(static_cast<int>(dataMeas));
  }
}


int BepVTDetect::WaitInit()
{
  return WAIT_OF_DTR
      + WAIT_OF_ANSWER_WAIT
      + WAIT_OF_ANSWER_INIT
      + WAIT_FOR_READY_READ;
}


bool BepVTDetect::defSerialNumber(const QByteArray& mas)
{
  bool error = true;
  if (mas.size() > AT_SERIAL_NUMBER + LEN_SERIAL_NUMBER)
  {
    _serialNumber = QByteArrayAtLenToInt(mas, AT_SERIAL_NUMBER,LEN_SERIAL_NUMBER);
    error = false;
  }
  return error;
}


bool BepVTDetect::defTypeDetect(const QByteArray& mas)
{
  bool error = false;
  QString typeDetect;
  if (mas.size() > AT_TYPE_DETECT + LEN_TYPE_DETECT)
  {
    int kod = static_cast<unsigned char>(mas.at(AT_TYPE_DETECT));
    int kod1 = static_cast<unsigned char>(mas.at(AT_TYPE_DETECT + 1));
    int kod2 = static_cast<unsigned char>(mas.at(AT_TYPE_DETECT + 2));
    switch (kod)
    {
    case 1:
      typeDetect.append("Датчик с частотным преобразованием");
      break;
    case 2:
      typeDetect.append("Датчик с синхронным детектированием");
      break;
    case 3:
      typeDetect.append("Датчик с частотным преобразованием");
      switch (kod1)
      {
      case 0:
        typeDetect.append(" с ADG419");
        if (kod2 == 44)
        {
          typeDetect.append(" точный");
          _currency = 2;
        }
        break;
      case 1:
        typeDetect.append(" с ADG419 и RS-232");
        if (kod2 == 44)
        {
          typeDetect.append(" точный");
          _currency = 2;
        }
        break;
      default:
        if (kod2 == 44)
        {
          typeDetect.append(" точный");
          _currency = 2;
        }
        break;
      }
      break;
    case 4:
      typeDetect.append("Датчик с манометрическим преобразованием");
      break;
    case 5:
      typeDetect.append("Индикаторная головка цифровая ЕМ08");
      _currency = 2;
      break;
    default:
      typeDetect.append("Тип датчика не определен");
      break;
    }
    _typeDetect = typeDetect;
  }
  else
    error = true;
  return error;
}


bool BepVTDetect::defDataManufDetect(const QByteArray& mas)
{
  bool error = true;
  if (mas.size() > AT_DATA_MANUF + LEN_DATA_MANUF)
  {
    int day = static_cast<unsigned char>(mas.at(AT_DATA_MANUF)); // день
    int month = static_cast<unsigned char>(mas.at(AT_DATA_MANUF + 1)); // месяц
    int year = static_cast<unsigned char>(mas.at(AT_DATA_MANUF + 2))*100; // век
    year += static_cast<unsigned char>(mas.at(AT_DATA_MANUF + 3)); // год
    error = !_dateManuf.setDate(year, month, day);
    if (error)
    {
      _dateManuf.setDate(2021, 9, 1); // Дата начала изготовления этого типа датчиков
      error = false;
    }
  }
  return error;
}


bool BepVTDetect::defAddress(const QByteArray& mas)
{
  bool error = true;
  if (mas.size() > AT_MODBUS_ADDRESS + LEN_MODBUS_ADDRESS)
  {
    _address = QByteArrayAtLenToInt(mas, AT_MODBUS_ADDRESS, LEN_MODBUS_ADDRESS);
    error = false;
  }
  return error;
}


bool BepVTDetect::defMeasureInterval(const QByteArray& mas)
{
  Q_UNUSED(mas)
  return false;
}


bool BepVTDetect::defZeroInterval(const QByteArray& mas)
{
  bool error = true;
  if (mas.size() > AT_ZERO_SET + LEN_ZERO_SET)
  {
    _zeroInterval = QByteArrayAtLenToInt(mas, AT_ZERO_SET, LEN_ZERO_SET);
    error = false;
  }
  return error;
}


bool BepVTDetect::defPreSetInterval(const QByteArray& mas)
{
  bool error = true;
  if (mas.size() > AT_PRE_SET + LEN_PRE_SET)
  {
    _preSetInterval = QByteArrayAtLenToInt(mas, AT_PRE_SET, LEN_PRE_SET);
    error = false;
  }
  return error;
}


bool BepVTDetect::defUnitMeasure(const QByteArray& mas)
{
  bool error = true;
  if (mas.size() > AT_MESS1 + LEN_MESS1)
  {
    _unitMeasure = getLocallyString(mas.mid(AT_MESS1, LEN_MESS1));
    error = false;
  }
  return error;
}


bool BepVTDetect::defUserName(const QByteArray& mas)
{
  bool error = true;
  int atNameDetect = _currency == 2 ? AT_NAME_DETEC2 : AT_NAME_DETECT;
  if (mas.size() > atNameDetect + LEN_NAME_DETECT)
  {
      _userName = getLocallyString(mas.mid(atNameDetect, LEN_NAME_DETECT));
      error = false;
  }
  return error;
}


bool BepVTDetect::defCalibrField(const QByteArray& mas)
{
  bool error = true;
  int atCalibrField = _currency == 2 ? AT_CALIBR_FIEL2 : AT_CALIBR_FIELD;
  if (mas.size() > atCalibrField + LEN_CALIBR_FIELD)
  {
    _calibrField = QByteArrayAtLenToInt(mas, atCalibrField, LEN_CALIBR_FIELD);
    error = false;
  }
  return error;
}


// Формирование калибровочной таблицы
bool BepVTDetect::defCalibrateDataTable(const QByteArray& baStream)
{
  bool error = false;
  int lenPoint = _currency == 2 ? LEN_POIN2 : LEN_POINT;
  int lenCalibr = _currency == 2 ? LEN_CALIB2 : LEN_CALIBR;
  if (baStream.size() < AT_CALIBR + lenCalibr)
    error = true;
  else
    error = fillCalibrateDataTable(baStream.mid(AT_CALIBR, lenCalibr),
                                   lenPoint,
                                   LEN_MEASSURE_POINT);

  if (!error)
  {
    defHMeasureInterval(_currency == 2 ? 100 : 1);
    defLMeasureInterval(_currency == 2 ? 100 : 1);
  }

  return error;
}


void BepVTDetect::CreateSettingsController(QObject* rootUi)
{
    _settingsController = new BepVTSettings(this, rootUi);
}


void BepVTDetect::SetAddress(int address)
{
    if (address == 0)
    {
        return;
    }
    _address = address;
    sendSaveCMD();
}


void BepVTDetect::SetNewName(QString newName)
{
    if (newName.isEmpty())
    {
        return;
    }
    _userName = newName;
    sendSaveCMD();
}


void BepVTDetect::sendSaveCMD()
{
  QByteArray mail = COMMAND_SAVE_DETECT;
  auto pushInt2 = [&](int data)
  {
    mail.push_back(data >> 8);
    mail.push_back(data);
  };
  auto pushInt4 = [&](int data)
  {
    mail.push_back(data >> 24);
    mail.push_back(data >> 16);
    mail.push_back(data >> 8);
    mail.push_back(data);
  };

  _port->write(COMMAND_WAIT_DETECT);
  _port->flush();
  {
    QElapsedTimer timew;
    timew.start();
    for(;timew.elapsed() < WAIT_OF_ANSWER_WAIT;) // Время ожидания, мсек
      qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
  }

  pushInt2(Address().toInt());
  pushInt2(HMeasureInterval());
  pushInt2(ZeroInterval());
  pushInt2(PreSetInterval());
  QString measUnit = MeasUnit();
  mail.push_back(measUnit.toUtf8());
  for (int i = measUnit.size(); i < LEN_MESS1; ++i)
    mail.push_back(0x20);

  for (int i = 0; i < SUM_POINT; ++i)
  {
    if (_currency == 2)
      pushInt4(_pmt.at(i).at(0));
    else
      pushInt2(_pmt.at(i).at(0));
    pushInt4(_pmt.at(i).at(1));
  }

  QByteArray nname = setLocallyString(_userName.left(LEN_NAME_DETECT));
  mail.push_back(nname);
  for (int i = nname.length(); i < LEN_NAME_DETECT; ++i)
    mail.push_back(0x20);
  pushInt4(CalibrField());
  mail.push_back(END_SAVE1);
  mail.push_back(END_SAVE2);
  _port->write(mail);
  _port->flush();
  {
    QElapsedTimer timew;
    timew.start();
    for(;timew.elapsed() < WAIT_OF_ANSWER_SAVE;) // Время ожидания, мсек
      qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
  }
    _port->write(COMMAND_WAIT_DETECT);
    _port->flush();
    {
      QElapsedTimer timew;
      timew.start();
      for(;timew.elapsed() < WAIT_OF_ANSWER_WAIT;) // Время ожидания, мсек
        qApp->processEvents();// Ждем ответа, но обрабатываем возможные события
    }
  _port->write(COMMAND_INIT_DETECT);
  _port->flush();

    emit DetectPropertyChanged();
}


int BepVTDetect::bufferSize(SizeBufMode mode)
{
  if (mode == SizeBufMode::MEAS_MODE)
    return 2 * LEN_MEAS;
  else if (mode == SizeBufMode::INIT_MODE)
    return 2 * LEN_MEAS + qMax(LEN_ANSWER_INIT, LEN_ANSWER_INI2);
  return VTDetect::bufferSize(mode);
}



TestBepVTDetect::TestBepVTDetect(QSerialPortInfo portInfo, QObject *parent)
  : BepVTDetect(portInfo, parent)
{
  _userName = "test bep detect";
  _unitMeasure = "_no_";
  _serialNumber = 21000;
  _typeDetect = "тестовый датчик. нет аппаратуры";
  _dateManuf.setDate(2021, 9, 1);
  _address = 17;
  _zeroInterval = 50;
  _preSetInterval = 150;
  _currency = 2;

  int lenPoint = _currency == 2 ? LEN_POIN2 : LEN_POINT;
  QByteArray data;
  int d1 = 0;
  int dd1 = 1000;
  int d2 = 0;
  int dd2 = 10000;
  for (int i = 0; i < SUM_POINT; ++i)
  {
    data.append((char)(d1 >> 24));
    data.append((char)(d1 >> 16));
    data.append((char)(d1 >> 8));
    data.append((char)(d1));
    data.append((char)(d2 >> 24));
    data.append((char)(d2 >> 16));
    data.append((char)(d2 >> 8));
    data.append((char)(d2));
    d1 += dd1;
    d2 += dd2;
  }
  fillCalibrateDataTable(data,
                         lenPoint,
                         LEN_MEASSURE_POINT);
}


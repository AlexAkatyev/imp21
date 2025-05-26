#include <QtWidgets>
#include <QQuickWidget>
#include <QGraphicsAnchorLayout>
#include <QQuickItem>
#include <QPrinter>
#include <QPrintDialog>

#include <cmath>

#include "indicator.h"
#include "UtilLib/utillib.h"
#include "Detects/vtdetect.h"
#include "imp.h"
#include "checkInputNumberIF/checkInputNumberIF.h"
#include "indsettings.h"
#include "postmessagesender.h"
#include "formulatree/formulafactory.h"
#include "formulatree/formulanode.h"

// Время ожидания ответа датчиков
#define TIME_OF_WAIT_DETECT 5000

// Размер приемного буфера
#define SIZE_RECEIVE_BUFFER 300
// Размер уменьшения буфера при переполнении
#define DECREMENT_BUFFER 50

// Время обновления показаний для индикатора
#define UPDATE_RESULT_TIME 50

// Длина истории измерений для фильтра
#define LEN_HISTORY 10


// Исходные размеры окна индикатора
const int SIZE_INDICATOR_WINDOW_X = 450;
const int SIZE_INDICATOR_WINDOW_Y = 400;

const int MINIMAL_HEIGHT = 450;
const int MINIMAL_WIDTH = 400;

const int WATCH_DOG_ACTIVE = 100;

// Периодичность считывания показаний датчиков
const int WATCH_DOG_INTERVAL = 100;

const int PRECISION_SCALE = 3;
const int PRECISION_INCREMENT = 7;
const int PRECISION_RANGE = 7;
const int PRECISION_DIVISION = 6;

float getMeasForTransform(float mr, int ttu)
{
    float result = 0;
    switch (ttu)
    {
    case 1: // мкм - мм
        result = mr / 1000;
        break;
    case 2: // мкм - дюйм
        result = mr / 25400;
        break;
    case 3: // мкм/м -> угловая секунда
        result = 3600 * atanf(mr / 1000000) * 180 / M_PI;
        break;
    default:
        result = mr;
        break;
    }
    return result;
}


Indicator::Indicator
(
    QWidget* parent
    , QString uuid
    , int identificator
    , ImpAbstractDetect* baseDetect
    , bool defOptions
)
  : QWidget(nullptr, Qt::Window)
  , _parent(static_cast<Imp*>(parent))
  , _idIndicator(identificator)
  , _wdtActivateControl(new QTimer(this))
  , _stateActivateWindow(false)
  , _iSender(false)
  , _quickUi(new QQuickWidget)
  , _formulaComplete(true)
  , _detect1(nullptr)
  , _detect2(nullptr)
  , _transGauge(UnitMKM)
  , _complexFormula(nullptr)
  , _complexFormulaEnable(false)
  , _complexFormulaComplete(false)
{
  PostMessageSender::Instance(parent);
  _zeroShifts = {0, 0};
  _settings = new IndSettings(uuid + "indicator" + QString::number(_idIndicator) + ".ini", this);

  // Размещение окна индикатора в центр экрана
  loadSettingsWindow();

  // Включение QML виджета
  QUrl source("qrc:/indicator.qml");
  _quickUi->setSource(source);

  // контейнер для QML виджета
  QVBoxLayout* pvbx = new QVBoxLayout();

  pvbx->setMargin(0); // Толщина рамки
  pvbx->addWidget(_quickUi); // Вставляем QML виджет в лайоут окна
  setLayout(pvbx); //  Установка лайоута в окно

  // Установка указателей на объекты виджета QML
  _tfFactor1 = _quickUi->rootObject()->findChild<QObject*>("tfFactor1");
  _cbListDetect1 = _quickUi->rootObject()->findChild<QObject*>("cbListDetect1");
  _tfIncert1 = _quickUi->rootObject()->findChild<QObject*>("tfIncert1");
  _tfFactor2 = _quickUi->rootObject()->findChild<QObject*>("tfFactor2");
  _cbListDetect2 = _quickUi->rootObject()->findChild<QObject*>("cbListDetect2");
  _tfIncert2 = _quickUi->rootObject()->findChild<QObject*>("tfIncert2");
  _tfDivider = _quickUi->rootObject()->findChild<QObject*>("tfDivider");
  _tfName = _quickUi->rootObject()->findChild<QObject*>("tfName");
  _tfUnitPoint = _quickUi->rootObject()->findChild<QObject*>("tfUnitPoint");
  _tfHiLimit = _quickUi->rootObject()->findChild<QObject*>("tfHiLimit");
  _tfLoLimit = _quickUi->rootObject()->findChild<QObject*>("tfLoLimit");
  _tfPriemka = _quickUi->rootObject()->findChild<QObject*>("tfPriemka");
  _tfNumberCharPoint = _quickUi->rootObject()->findChild<QObject*>("tfNumberCharPoint");
  _tfPeriod = _quickUi->rootObject()->findChild<QObject*>("tfPeriod");
  _tfSumPoint = _quickUi->rootObject()->findChild<QObject*>("tfSumPoint");
  _rbHandMode = _quickUi->rootObject()->findChild<QObject*>("rbHandMode");
  _rbAutoMode = _quickUi->rootObject()->findChild<QObject*>("rbAutoMode");
  _tfStatPeriod = _quickUi->rootObject()->findChild<QObject*>("tfStatPeriod");

  // Указатель для входных данных индикатора
  _inputIndicator = _quickUi->rootObject()->findChild<QObject*>("inputIndicator");

  // Установка формулы
  connect(_quickUi->rootObject(), SIGNAL(sigChangeFormula()), this, SLOT(setFormula()));
  // Обновление цены деления
  connect(_quickUi->rootObject(), SIGNAL(sigGetDivisionValue()), this, SLOT(getDivisionValue()));
  // Обновление допусков на индикаторе
  connect(_quickUi->rootObject(), SIGNAL(sigChangeLimit()), this, SLOT(changeLimit()));
  // Обновление характера показаний на индикаторе
  connect(_quickUi->rootObject(), SIGNAL(sigChangeIndication()), this, SLOT(changeIndication()));
  // Установка 0 значения датчиков
  connect(_quickUi->rootObject(), SIGNAL(sigSetZeroShift()), this, SLOT(setZeroShifts()));
  // выбор файла автосохранения
  // Установка имени
  connect(_quickUi->rootObject(), SIGNAL(sigNameEntered()), this, SLOT(setWindowName()));
  // отработка нажатия кнопки DATA
  connect(_quickUi->rootObject(), SIGNAL(sigReleaseData()), this, SIGNAL(sigDataPressed()));
  // передача измерения в Windows
  connect(_quickUi->rootObject(), SIGNAL(sigSendMeasurementMessage()), this, SLOT(sendMessage()));
  // изменения в единице измерения
  connect(_inputIndicator, SIGNAL(sigChangeTransGauge()), this, SLOT(changedTransGauge()));
  // изменения в сложной формуле
  connect(_quickUi->rootObject(), SIGNAL(analyseComplexFormula(QString)), this, SLOT(createComplexFormula(QString)));
  connect(_quickUi->rootObject(), SIGNAL(sigEnableComplexFormula(bool)), this, SLOT(enableComplexFormula(bool)));

  // получение информиции о имеющихся датчиках от главного окна
  connect(_parent, SIGNAL(sigFindDetect()), this, SLOT(setComboListDetect()));
  setComboListDetect(); // Сразу список надо передать в страницу ФОРМУЛА

  QTimer* timerUpdateIndicator = new QTimer(this);
  connect(timerUpdateIndicator, SIGNAL(timeout()), this, SLOT(updateResult()));
  timerUpdateIndicator->setInterval(UPDATE_RESULT_TIME);
  timerUpdateIndicator->start();

  // Настройка таймеров периодического чтения результатов измерения датчиков
  _timerWatchDog = new QTimer(this);
  connect(_timerWatchDog, SIGNAL(timeout()), this, SLOT(watchDogControl()));
  _timerWatchDog->setInterval(WATCH_DOG_INTERVAL);
  _timerWatchDog->start();

  // Сообщение о закрытии индикатора
  connect(this, SIGNAL(sigCloseIndicator(int)), _parent, SLOT(deleteIndicator(int)));

  // Загрузка настроек
  QString message = loadSettingsIndicator(defOptions);

  _measuredLogs.clear();

  if (baseDetect)
  {
    setTitle("Индикатор");
    _detect1 = baseDetect;
    _detect2 = nullptr;
    setComboListDetect();
  }

  if (_detect1 || _detect2) // Заказана формула индикатора
    setFormula();

  setMinimumSize(QSize(MINIMAL_WIDTH, MINIMAL_HEIGHT));
  this->show(); // Окно выводим на экран

    connect(this, &Indicator::sigActivatedWindow, _parent, &Imp::RaiseIndicators);
    connect(_wdtActivateControl, &QTimer::timeout, this, [=]()
    {
        bool activeWindow = isActiveWindow();
        if (activeWindow && !_stateActivateWindow)
        {
            _iSender = true;
            emit sigActivatedWindow();
            _iSender = false;
        }
        _stateActivateWindow = activeWindow;
    });
    _wdtActivateControl->start(WATCH_DOG_ACTIVE);

  if (message.length() != 0)
  {
      QMessageBox::information
              (
                  this
                  , Imp::MessageTitle()
                  , message
              );
  }
}


int Indicator::Id()
{
  return _idIndicator;
}


QString Indicator::EmptyDetect()
{
    return "Нет";
}


// переопределение события изменения окна
void Indicator::resizeEvent(QResizeEvent *event)
{
  // Масштабирование QML виджета под размер окна
  QSize baseSize(static_cast<int>(_quickUi->rootObject()->width()), static_cast<int>(_quickUi->rootObject()->height()));
  _quickUi->rootObject()->setProperty("scaleX", QVariant(this->width()));
  _quickUi->rootObject()->setProperty("scaleY", QVariant(this->height()));
  event->accept();
}


// Определение цены деления
void Indicator::getDivisionValue(void)
{
  QVariant varTemp;
  int iAccurDivision = 0;
  varTemp = _tfUnitPoint->property("text");
  InputNumber inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_UNSIGNED_FLOAT, PRECISION_DIVISION);
  if (inTemp.InfoError.isEmpty())
  { // Ошибок нет
    float  fTemp = inTemp.fNumber;
    //if (fTemp == 0) fTemp = 1;
    if (qFuzzyCompare(fTemp, 0)) fTemp = 1;
    _inputIndicator->setProperty("unitPoint", fTemp);
    fTemp *= 100000;
    if (round(static_cast<double>(fTemp)/100000) != round(static_cast<double>(fTemp))/100000) iAccurDivision = 1;
    if (round(static_cast<double>(fTemp)/10000) != round(static_cast<double>(fTemp))/10000) iAccurDivision = 2;
    if (round(static_cast<double>(fTemp)/1000) != round(static_cast<double>(fTemp))/1000) iAccurDivision = 3;
    if (round(static_cast<double>(fTemp)/100) != round(static_cast<double>(fTemp))/100) iAccurDivision = 4;
    if (round(static_cast<double>(fTemp)/10) != round(static_cast<double>(fTemp))/10) iAccurDivision = 5;
    _inputIndicator->setProperty("accurDivision", iAccurDivision);
  }
  else
  { // В введенном поле есть ошибки
    QMessageBox::warning(this, "Цена деления", inTemp.InfoError);
  }
}


// Изменение допусков на индикаторе
void Indicator::changeLimit(void)
{
  float fHiLimit = static_cast<float>(INT_MAX);
  float fLoLimit = static_cast<float>(INT_MIN);
  QVariant varTemp = _tfHiLimit->property("text");
  InputNumber inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT, PRECISION_RANGE);
  if (inTemp.InfoError.isEmpty())
  { // Ошибок нет
    fHiLimit = inTemp.fNumber;
  }
  else
  { // В введенном поле есть ошибки
    QMessageBox::warning(this, "Верхнее предельное отклонение", inTemp.InfoError);
  }
  varTemp = _tfLoLimit->property("text");
  inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT, PRECISION_RANGE);
  if (inTemp.InfoError.isEmpty())
  { // Ошибок нет
    fLoLimit = inTemp.fNumber;
  }
  else
  { // В введенном поле есть ошибки
    QMessageBox::warning(this, "Нижнее предельное отклонение", inTemp.InfoError);
  }
  if (fHiLimit > fLoLimit)
  { // Границы введены корректно
    _inputIndicator->setProperty("highLimit", fHiLimit);
    _inputIndicator->setProperty("lowLimit", fLoLimit);
  }
  //if (fHiLimit == fLoLimit)
  if (qFuzzyCompare(fHiLimit, fLoLimit))
  { // Границы введены не корректно
    QMessageBox::warning(this, "Значения предельных отклонений",
                         "Предельные отклонения\n не должны иметь одинаковые значения");
  }
  if (fHiLimit < fLoLimit)
  { // Границы введены не корректно
    QMessageBox::warning(this,
                         "Значения предельных отклонений",
                         "Верхнее предельное отклонение\n не может быть меньше\n нижнего предельного отклонения");
  }
  varTemp = _tfPriemka->property("text");
  inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT, PRECISION_RANGE);
  if (inTemp.InfoError.isEmpty() && inTemp.fNumber >= 0) // Ошибок нет
    _inputIndicator->setProperty("priemka", inTemp.fNumber);
  else // В введенном поле есть ошибки
  {
    QMessageBox::warning(this,
                         "Приемочная граница",
                         inTemp.InfoError + "\nЗначение должно быть не меньше 0.");
    QVariant priemka = _inputIndicator->property("priemka");
    _tfPriemka->setProperty("text", priemka);
  }
}


// Изменение способа показаний
void Indicator::changeIndication(void)
{
  // Ввод времени периода, мс
  QVariant varTemp = _tfPeriod->property("text");
  InputNumber inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_UNSIGNED_INTEGER);
  if (inTemp.InfoError.isEmpty())
  { // Ошибок нет
    _periodMean = inTemp.iNumber;
    if (_periodMean > LEN_MAD * UPDATE_RESULT_TIME)
    {
      QMessageBox::warning(this,
                           "Период усреднения цифрового индикатора",
                           "Значение не может быть более " + QString::number(LEN_MAD * UPDATE_RESULT_TIME));
      _periodMean = LEN_MAD * UPDATE_RESULT_TIME;
      _tfPeriod->setProperty("text", QString::number(_periodMean));
    }
    _lenMean = static_cast<int>(round(_periodMean / UPDATE_RESULT_TIME));
    _lenMean = _lenMean == 0 ? 1 : _lenMean; // Минимальное значение - 1
    for (int i=0; i<_lenMean; i++) flHistoryMean[i] = 0; // Очистка истории
  }
  else
  { // В введенном поле есть ошибки
    QMessageBox::warning(this, "Период усреднения цифрового индикатора", inTemp.InfoError);
  }
}


// Установка новой формулы
void Indicator::setFormula(void)
{
  if (_complexFormulaEnable)
  {
    return;
  }
  QVariant varTemp;
  InputNumber inTemps1, inTemps2, inTempi1, inTempi2, inTempD;
  _formulaComplete = false; // Формула не готова
  bool error = false;
  // Множитель 1 ------------------------------------------------
  varTemp = _tfFactor1->property("text");
  inTemps1 = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT, PRECISION_SCALE);
  if (inTemps1.InfoError.isEmpty() == false)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Множитель 1", inTemps1.InfoError);
  }
  // Множитель 2 ------------------------------------------------
  varTemp = _tfFactor2->property("text");
  inTemps2 = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT, PRECISION_SCALE);
  if (inTemps2.InfoError.isEmpty() == false)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Множитель 2", inTemps2.InfoError);
  }
  // Слагаемое 1 ------------------------------------------------
  varTemp = _tfIncert1->property("text");
  inTempi1 = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT, PRECISION_INCREMENT);
  if (inTempi1.InfoError.isEmpty() == false)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Слагаемое 1", inTempi1.InfoError);
  }
  // Слагаемое 2 ------------------------------------------------
  varTemp = _tfIncert2->property("text");
  inTempi2 = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT, PRECISION_INCREMENT);
  if (inTempi2.InfoError.isEmpty() == false)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Слагаемое 2", inTempi2.InfoError);
  }
  // Общий делитель ---------------------------------------------
  varTemp = _tfDivider->property("text");
  inTempD = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT, PRECISION_SCALE);
  if (!inTempD.InfoError.isEmpty())
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Общий делитель", inTempD.InfoError);
  }
  if (abs(inTempD.fNumber) < 0.01)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Общий делитель", "Недопустимое значение, близкое к 0");
  }


  // Датчик 1
  qApp->processEvents();
  QString strCurrent1 = _cbListDetect1->property("currentText").toString();
  if (_detect1)
    disconnect(_detect1, &VTDetect::PressedButton, this, &Indicator::RunButtonRelease);
  _detect1 = _parent->DetectAtName(strCurrent1);
  if (_detect1)
    connect(_detect1, &VTDetect::PressedButton, this, &Indicator::RunButtonRelease);

  // Датчик 2
  qApp->processEvents();
  QString strCurrent2 = _cbListDetect2->property("currentText").toString();
  if (_detect2)
    disconnect(_detect2, &VTDetect::PressedButton, this, &Indicator::RunButtonRelease);
  _detect2 = _parent->DetectAtName(strCurrent2);
  if (_detect2)
    connect(_detect2, &VTDetect::PressedButton, this, &Indicator::RunButtonRelease);
  // Проверки
  QString unit1 = _detect1 ? _detect1->MeasUnit() : "";
  QString unit2 = _detect2 ? _detect2->MeasUnit() : "";
  error |= (unit1 != unit2) && !unit1.isEmpty() && !unit2.isEmpty();
  if (!error)
  { // Отсутствуют ошибки при вводе чисел
    _scale1 = inTemps1.fNumber;
    _scale2 = inTemps2.fNumber;
    _increment1 = inTempi1.fNumber;
    _increment2 = inTempi2.fNumber;
    _divider = inTempD.fNumber;
    _inputIndicator->setProperty("messUnitDetect", unit1.isEmpty() ? unit2 : unit1);
    _formulaComplete = true; // Формула готова
    setWorkIndicators();
  }

}


void Indicator::RunButtonRelease()
{
  QMetaObject::invokeMethod(_quickUi->rootObject(), "releaseData", Qt::QueuedConnection);
}


// Индикация работающих датчиков
void Indicator::setWorkIndicators()
{
  bool en1 = false;
  bool en2 = false;
  if (_complexFormulaEnable)
  {
    en1 = true;
  }
  else
  {
    en1 = _detect1 != nullptr;
    en2 = _detect2 != nullptr;
  }
  _inputIndicator->setProperty("blDetect1EnableInput", en1);
  _inputIndicator->setProperty("blDetect2EnableInput", en2);
}


// Обновление списка датчиков
void Indicator::setComboListDetect(void)
{
  // Обновили список в combobox
  QStringList slCLD1 = createListByDetect2();
  QStringList slCLD2 = createListByDetect2();
  _cbListDetect1->setProperty("model", slCLD1);
  _cbListDetect2->setProperty("model", slCLD2);

  // По ключу выставляем активный датчик
  int index1 = 0; // Датчик по умолчанию исчез из списка
  if (_detect1)
    index1 = currentIndex2ByName(_detect1->UserName());
  _cbListDetect1->setProperty("currentIndex", index1);

  int index2 = 0; // Датчик по умолчанию исчез из списка
  if (_detect2)
    index2 = currentIndex2ByName(_detect2->UserName());
  _cbListDetect2->setProperty("currentIndex", index2);

  setWorkIndicators();
}


QStringList Indicator::createListByDetect1()
{
  QStringList slCLD;
  slCLD << _parent->DetectNames();
  if (slCLD.size() == 0)
    slCLD << EmptyDetect();
  return slCLD;
}


QStringList Indicator::createListByDetect2()
{
  QStringList slCLD;
  slCLD << EmptyDetect();
  slCLD += _parent->DetectNames();
  return slCLD;
}


int Indicator::currentIndex1ByName(QString nameDetect)
{
  QStringList slCLD = createListByDetect1();
  int index = 0;
  while (index < slCLD.size())
  {
    if (slCLD.at(index) == nameDetect) break;
    index++;
  }
  return index >= slCLD.size() ? 0 : index;
}


int Indicator::currentIndex2ByName(QString nameDetect)
{
  QStringList slCLD = createListByDetect2();
  int index = 1;
  while (index < slCLD.size())
  {
    if (slCLD.at(index) == nameDetect) break;
    index++;
  }
  return index >= slCLD.size() ? 0 : index;
}


// Вывод от датчиков на индикатор
void Indicator::updateResult(void)
{
  int i;
  float sum;
  float flResult = calculateResult();
  if (_lenMean > 1)
  { // Получаем новое усреднение
    sum = 0;
    for (i=_lenMean-2; i>-1; i--)
    {
      flHistoryMean[i+1] = flHistoryMean[i];
      sum += flHistoryMean[i];
    }
    flHistoryMean[++i] = flResult;
    sum += flResult;
    flResult = sum / _lenMean; // Вот вам и усреднение
  }
  _inputIndicator->setProperty("messReal", flResult);
  enableSetZero();
}


float Indicator::calculateChannel(int number)
{
  float result = 0;
  switch (number) {
  case 1:
    result = _scale1 * (_detect1 ? getMeasForTransform(_detect1->CurrentMeasure(), static_cast<int>(_transGauge)) - _zeroShifts[0] : 0) + _increment1;
    break;
  case 2:
    result = _scale2 * (_detect2 ? getMeasForTransform(_detect2->CurrentMeasure(), static_cast<int>(_transGauge)) - _zeroShifts[1] : 0) + _increment2;
    break;
  default:
    break;
  }
  return result;
}


// Расчет показаний датчиков по формуле
float Indicator::calculateResult()
{
  float result = 0;
  float beforeSet = _inputIndicator->property("beforeSet").toFloat();
  if (_complexFormulaEnable)
  {
    result = _complexFormulaComplete ? (_complexFormula->Get(getMeasForTransform, static_cast<int>(_transGauge)) / _divider + beforeSet) : 0;
  }
  else
  {
    result = _formulaComplete
           ? ((calculateChannel(1) + calculateChannel(2))/_divider + beforeSet)
           : 0;
  }

  return result;
}


// Разрешение обнуления показаний
void Indicator::enableSetZero()
{
  if (_formulaComplete)
  {
    bool enableSetZero = true;
    for (auto detect : {_detect1, _detect2})
      if (detect)
        enableSetZero &= abs(detect->CurrentMeasure()) <= detect->ZeroInterval();
    _inputIndicator->setProperty("enableSetZero", enableSetZero);
  }
}


// Закрытие окна - реакция на указание от других программ
void Indicator::CloseMyIndicator(void)
{
  _timerWatchDog->stop();
  // сигнал дочерним окнам
  emit sigCloseMyIndicator();
  // сохранение файла инициализации
  saveSettingsIndicator();
  this->deleteLater();
}


// переопределение события закрытия окна (нажатием на крестик) - реакция на указания пользователя
void Indicator::closeEvent(QCloseEvent *event)
{
  _timerWatchDog->stop();
  event->accept();
  // сообщение о закрытии пользователем
  emit sigCloseIndicator(_idIndicator);    // родителю
  emit sigCloseMyIndicator();             // детям
  // сохранение файла инициализации - последние изменения
  saveSettingsIndicator();
}


// сохранение настроек
void Indicator::saveSettingsIndicator()
{
  // Сохранение параметров окна
  QRect winGeometry = geometry();
  _settings->SetValue(IndKeys::WIN_X, winGeometry.x());
  _settings->SetValue(IndKeys::WIN_Y, winGeometry.y());
  _settings->SetValue(IndKeys::WIN_WIDTH, winGeometry.width());
  _settings->SetValue(IndKeys::WIN_HEIGHT, winGeometry.height());

  // сохранение формулы
  _settings->SetValue(IndKeys::INDICATOR_NAME, _userTitle);
  _settings->SetValue(IndKeys::SCALE1, _tfFactor1->property("text"));
  _settings->SetValue(IndKeys::DETECT1, _detect1 ? _detect1->Id() : 0);
  _settings->SetValue(IndKeys::INCREMENT1, _tfIncert1->property("text"));
  _settings->SetValue(IndKeys::SCALE2, _tfFactor2->property("text"));
  _settings->SetValue(IndKeys::DETECT2, _detect2 ? _detect2->Id() : 0);
  _settings->SetValue(IndKeys::INCREMENT2, _tfIncert2->property("text"));

  _settings->SetValue(IndKeys::DIVIDER, _tfDivider->property("text"));
  _settings->SetValue(IndKeys::BEFORESET, _inputIndicator->property("beforeSet"));
  _settings->SetValue(IndKeys::DOPUSK, _inputIndicator->property("dopusk"));
  _settings->SetValue(IndKeys::PERIOD, _tfPeriod->property("text").toInt());
  _settings->SetValue(IndKeys::COMPLEX_FORMULA_ENABLE, _complexFormulaEnable);

  // настройка шкалы
  _settings->SetValue(IndKeys::UNITPOINT, _inputIndicator->property("unitPoint"));
  _settings->SetValue(IndKeys::HIGHLIMIT, _tfHiLimit->property("text"));
  _settings->SetValue(IndKeys::LOWLIMIT, _tfLoLimit->property("text"));
  _settings->SetValue(IndKeys::PRIEMKA, _inputIndicator->property("priemka"));
  _settings->SetValue(IndKeys::ACCURACY, _inputIndicator->property("accuracy"));
  _settings->SetValue(IndKeys::ACCUR_DIVISION, _inputIndicator->property("accurDivision"));

  // Режим измерения
  _settings->SetValue(IndKeys::IND_MEAS_MODE, _quickUi->rootObject()->property("deviationMode"));

  // Преобразование шкалы
  _settings->SetValue(IndKeys::TRANS_GAUGE, _transGauge);

  // Режимы сортировки
  QObject* qmlWidget = _quickUi->rootObject()->findChild<QObject*>("cbSortFormula");
  _settings->SetValue(IndKeys::SORT_CHF, qmlWidget->property("checked"));
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("countGroupsF");
  _settings->SetValue(IndKeys::GROUPSF, qmlWidget->property("value").toInt());

  // способ отображения показаний
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("cbStrelka");
  _settings->SetValue(IndKeys::STRELKA, qmlWidget->property("checked"));

  // автосохранение
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("automaticSave");
  _settings->SetValue(IndKeys::AUTO_SAVE_ENABLE,  qmlWidget->property("checked").toBool());
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("tfAutoSave");
  _settings->SetValue(IndKeys::AUTO_SAVE_PERIOD,  qmlWidget->property("text").toInt());
}


void Indicator::loadSettingsWindow()
{
  QRect windowGeometry = QRect(_settings->Value(IndKeys::WIN_X).toInt(),
                               _settings->Value(IndKeys::WIN_Y).toInt(),
                               _settings->Value(IndKeys::WIN_WIDTH).toInt(),
                               _settings->Value(IndKeys::WIN_HEIGHT).toInt());
  setGeometry(windowGeometry);
}


// Чтение установок
QString Indicator::loadSettingsIndicator(bool defOptions)
{
    QString message = "";
  if (defOptions)
  {
    _settings->LockDefaultValues();
  }
  QVariant v = _settings->Value(IndKeys::SCALE1);
  _scale1 = v.toDouble();
  _tfFactor1->setProperty("text", v);
  int idDetect1 = _settings->Value(IndKeys::DETECT1).toInt();
  if (idDetect1 == 0)
  { // датчик не используется
    _cbListDetect1->setProperty("currentIndex", 0);
    _detect1 = nullptr;
    _inputIndicator->setProperty("blDetect1EnableInput", false);
  }
  else
  { // датчик указан
    // По ключу выставляем активный датчик
    _detect1 = _parent->DetectAtId(idDetect1);
    if (_detect1)
    { // Датчик остался в списке
      QStringList slTemp;
      int index = currentIndex2ByName(_detect1->UserName());
      _cbListDetect1->setProperty("currentIndex", index);
      _inputIndicator->setProperty("blDetect1EnableInput", index != 0);
    }
    else
    { // Датчик исчез из списка
      _cbListDetect1->setProperty("currentIndex", 0);
      _inputIndicator->setProperty("blDetect1EnableInput", false);
    }
  }
  v = _settings->Value(IndKeys::INCREMENT1);
  _increment1 = v.toDouble();
  _tfIncert1->setProperty("text", v);

  v = _settings->Value(IndKeys::SCALE2);
  _scale2 = v.toDouble();
  _tfFactor2->setProperty("text", v);
  int idDetect2 = _settings->Value(IndKeys::DETECT2).toInt();
  if (idDetect2 == 0)
  { // датчик не используется
    _cbListDetect2->setProperty("currentIndex", 0);
    _detect2 = nullptr;
    _inputIndicator->setProperty("blDetect2EnableInput", false);
  }
  else
  { // датчик указан
    // По ключу выставляем активный датчик
    _detect2 = _parent->DetectAtId(idDetect2);
    if (_detect2)
    { // Датчик остался в списке
      QStringList slTemp;
      int index = currentIndex2ByName(_detect2->UserName());
      _cbListDetect2->setProperty("currentIndex", index);
      _inputIndicator->setProperty("blDetect2EnableInput", index != 0);
    }
    else
    { // Датчик исчез из списка
      _cbListDetect2->setProperty("currentIndex", 0);
      _inputIndicator->setProperty("blDetect2EnableInput", false);
    }
  }
  v = _settings->Value(IndKeys::INCREMENT2);
  _increment2 = v.toDouble();
  _tfIncert2->setProperty("text", v);
  v = _settings->Value(IndKeys::DIVIDER);
  _divider = v.toFloat();
  _tfDivider->setProperty("text", v);
  _inputIndicator->setProperty("beforeSet", _settings->Value(IndKeys::BEFORESET));
  _inputIndicator->setProperty("dopusk", _settings->Value(IndKeys::DOPUSK));
  _periodMean = _settings->Value(IndKeys::PERIOD).toInt();
  if (_periodMean > LEN_MAD * UPDATE_RESULT_TIME)
  {
    _periodMean = LEN_MAD * UPDATE_RESULT_TIME;
  }
  _lenMean = static_cast<int>(round(_periodMean / UPDATE_RESULT_TIME));
  _lenMean = _lenMean == 0 ? 1 : _lenMean; // Минимальное значение - 1
  for (int i=0; i<_lenMean; i++) flHistoryMean[i] = 0; // Очистка истории
  _tfPeriod->setProperty("text", QString::number(_periodMean));

  QQuickItem* root = _quickUi->rootObject();
  QObject* complexFormula = root->findChild<QObject*>("complexFormula");
  v = _settings->Value(IndKeys::COMPLEX_FORMULA_EXPRESSION);
  complexFormula->setProperty("text", v);
  createComplexFormula(v.toString());
  QObject* cbComplexFormula = root->findChild<QObject*>("cbComplexFormula");
  v = _settings->Value(IndKeys::COMPLEX_FORMULA_ENABLE);
  cbComplexFormula->setProperty("checked", v);
  _complexFormulaEnable = v.toBool();

  _inputIndicator->setProperty("unitPoint", _settings->Value(IndKeys::UNITPOINT));
  _tfUnitPoint->setProperty("text", _settings->Value(IndKeys::UNITPOINT));

  v = _settings->Value(IndKeys::HIGHLIMIT);
  _inputIndicator->setProperty("highLimit", v);
  _tfHiLimit->setProperty("text", v);
  _quickUi->rootObject()->findChild<QObject*>("tfHiLevelF")->setProperty("text",v);

  v = _settings->Value(IndKeys::LOWLIMIT);
  _inputIndicator->setProperty("lowLimit", v);
  _tfLoLimit->setProperty("text", v);
  _quickUi->rootObject()->findChild<QObject*>("tfLoLevelF")->setProperty("text", v);

  _inputIndicator->setProperty("priemka", _settings->Value(IndKeys::PRIEMKA));
  _tfPriemka->setProperty("text", _settings->Value(IndKeys::PRIEMKA));
  int accuracy = _settings->Value(IndKeys::ACCURACY).toInt();
  if (accuracy < 0)
    accuracy = 0;
  _inputIndicator->setProperty("accuracy", accuracy);
  _inputIndicator->setProperty("accurDivision", _settings->Value(IndKeys::ACCUR_DIVISION));
  _quickUi->rootObject()->setProperty("deviationMode",  _settings->Value(IndKeys::IND_MEAS_MODE));
  setTitle(_settings->Value(IndKeys::INDICATOR_NAME).toString());
  v = _settings->Value(IndKeys::TRANS_GAUGE);
  _transGauge = static_cast<TransToUnit>(v.toInt());
  _inputIndicator->setProperty("transGauge", v);

  v = _settings->Value(IndKeys::STRELKA);
   _quickUi->rootObject()->findChild<QObject*>("cbStrelka")->setProperty("checked", v);
   _quickUi->rootObject()->findChild<QObject*>("cbZifra")->setProperty("checked", !v.toBool());

  QObject* qmlWidget = _quickUi->rootObject()->findChild<QObject*>("countGroupsF");
  qmlWidget->setProperty("value", _settings->Value(IndKeys::GROUPSF));
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("cbSortFormula");
  qmlWidget->setProperty("checked", _settings->Value(IndKeys::SORT_CHF));

  // автосохранение
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("automaticSave");
  qmlWidget->setProperty("checked",  _settings->Value(IndKeys::AUTO_SAVE_ENABLE));
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("timerSave");
  qmlWidget->setProperty("running",  _settings->Value(IndKeys::AUTO_SAVE_ENABLE));
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("tfAutoSave");
  qmlWidget->setProperty("text",  _settings->Value(IndKeys::AUTO_SAVE_PERIOD));

  _settings->UnlockDefaultValues();

  if (idDetect1 != 0
      && _detect1 == nullptr)
  {
      message += "\nНе обнаружен датчик зав. № " + QString::number(idDetect1);
  }
  if (idDetect2 != 0
      && _detect2 == nullptr)
  {
      message += "\nНе обнаружен датчик зав. № " + QString::number(idDetect2);
  }
  return message;
}


void Indicator::setTitle(QString indicatorName)
{
  setWindowTitleMain(indicatorName);
  _tfName->setProperty("text", indicatorName);
}


void Indicator::setWindowName()
{
  setWindowTitleMain(_tfName->property("text").toString());
}


void Indicator::setWindowTitleMain(QString name)
{
  _userTitle = name;
  setWindowTitle(_userTitle + " : " + QString::number(_idIndicator + 1));
}


// Периодическое чтение результата измерения датчика 1
void Indicator::watchDogControl(void)
{
  if (_complexFormulaEnable)
  {
    if (_complexFormulaComplete)
    {
      _inputIndicator->setProperty("blOverRange1", _complexFormula->HiOverRange() || _complexFormula->LoOverRange());
      _inputIndicator->setProperty("blDetect1WorkInput", _complexFormula->DetectsReady());
      return;
    }
    else
    {
      return;
    }
  }

  if (_detect1)
  {
    float meas1 = _detect1->CurrentMeasure();
    _inputIndicator->setProperty("blOverRange1", (meas1 > _detect1->HMeasureInterval()) || (meas1 < _detect1->LMeasureInterval()));
    _inputIndicator->setProperty("blDetect1WorkInput", _detect1->Ready());
  }

  if (_detect2)
  {
    float meas2 = _detect2->CurrentMeasure();
    _inputIndicator->setProperty("blOverRange2", (meas2 > _detect2->HMeasureInterval()) || (meas2 < _detect2->LMeasureInterval()));
    _inputIndicator->setProperty("blDetect2WorkInput", _detect2->Ready());
  }
  else
  {
    _inputIndicator->setProperty("blOverRange2", false);
    _inputIndicator->setProperty("blDetect2EnableInput", false);
  }
}


void Indicator::sendMessage()
{
  ImpMessage message = ImpMessage();
  message.Sender = ImpMessageDataSender::Indicator;
  message.SenderId = _idIndicator + 1;
  QObject* root = _quickUi->rootObject();
  bool cbSortFormula = root->property("cbSortFormula").toBool();
  if (cbSortFormula)
  {
    message.Caption = ImpMessageDataCaption::SelectGroup;
    message.GroupNumber = root->property("groupNumber").toInt();
  }
  else
  {
    bool deviationMode = root->property("deviationMode").toBool();
    if (deviationMode)
    {
      message.Caption = ImpMessageDataCaption::MinMax;
      message.Min = root->property("measMin").toFloat();
      message.Max = root->property("measMax").toFloat();
    }
    else
    {
      message.Caption = ImpMessageDataCaption::Measure;
      message.Measure = root->property("sendData").toFloat();
    }
  }
  PostMessageSender::Instance(this->parent())->Do(message);
}


void Indicator::setZeroShifts()
{
  bool setZero = _quickUi->rootObject()->property("setZero").toBool();
  if (setZero)
  {
    _zeroShifts[0] = _detect1 ? _detect1->CurrentMeasure() : 0;
    _zeroShifts[1] = _detect2 ? _detect2->CurrentMeasure() : 0;
  }
  else
  {
    _zeroShifts[0] = 0;
    _zeroShifts[1] = 0;
  }
}


void Indicator::changedTransGauge()
{
  _transGauge = static_cast<TransToUnit>(_inputIndicator->property("transGauge").toInt());
}


void Indicator::createComplexFormula(QString inputText)
{
  _settings->SetValue(IndKeys::COMPLEX_FORMULA_EXPRESSION, inputText);

  const int STATUS_FORMULA_EMPTY = 0;
  const int STATUS_FORMULA_OK = 1;
  const int STATUS_FORMULA_ERROR = 2;
  int status = inputText.isEmpty() ? STATUS_FORMULA_EMPTY : STATUS_FORMULA_OK;

  QString statusMessage;
  if (status == STATUS_FORMULA_EMPTY)
  {
    statusMessage = "Формула не задана";
  }
  else
  {
    statusMessage = "Формула задана верно";
  }

  bool error = false;
  QString textError;
  if (status)
  {
    _complexFormula = FormulaFactory::Instance()->Do(inputText, &error, &textError);
    _complexFormulaComplete = !error;
    status = error ? STATUS_FORMULA_ERROR : STATUS_FORMULA_OK;
    if (error)
    {
      statusMessage = textError;
    }
  }

  QObject* fMessage = _quickUi->rootObject()->findChild<QObject*>("formulaMessage");
  fMessage->setProperty("status", status);
  fMessage->setProperty("text", QVariant(statusMessage));
}


void Indicator::enableComplexFormula(bool en)
{
  _complexFormulaEnable = en;
  _inputIndicator->setProperty("messUnitDetect", "мкм");
}


std::vector<int> Indicator::GetDefaultSize()
{
    std::vector<int> defaultSize = {MINIMAL_WIDTH, MINIMAL_HEIGHT};
    return defaultSize;
}


void Indicator::ReiseWindow()
{
    if (_iSender)
    {
        return;
    }
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();
    activateWindow();
    _stateActivateWindow = true;
}

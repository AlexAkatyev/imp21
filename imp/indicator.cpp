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

#include <Xlsx/Workbook.h>
using namespace SimpleXlsx;

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
const int SIZE_INDICATOR_WINDOW_X = 374;
const int SIZE_INDICATOR_WINDOW_Y = 458;
//const int SIZE_INDICATOR_WINDOW_Y2 = (SIZE_INDICATOR_WINDOW_Y/2);

const int MINIMAL_HEIGHT = 458;
const int MINIMAL_WIDTH = 374;

// Периодичность считывания показаний датчиков
const int WATCH_DOG_INTERVAL = 100;

const int PRECISION_SCALE = 3;
const int PRECISION_INCREMENT = 7;
const int PRECISION_RANGE = 7;
const int PRECISION_DIVISION = 6;

Indicator::Indicator(QWidget* parent, int identificator, ImpAbstractDetect* baseDetect)
  : QWidget(nullptr, Qt::Window)
  , _parent(static_cast<Imp*>(parent))
  , _idIndicator(identificator)
  , _quickUi(new QQuickWidget)
  , _formulaComlete(true)
  , _detect1(nullptr)
  , _detect2(nullptr)
  , _transGauge(UnitMKM)
{
  _zeroShifts = {0, 0};
  _settings = new IndSettings("indicator" + QString::number(_idIndicator) + ".ini", this);

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
  _tStatChart = _quickUi->rootObject()->findChild<QObject*>("statChart");

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
  // Сохранение измерений
  connect(_quickUi->rootObject(), SIGNAL(sigClickedSave()), this, SLOT(saveMeas()));
  connect(_quickUi->rootObject(), SIGNAL(sigAutoSave()), this, SLOT(autoSaveToXLSX()));
  // Установка 0 значения датчиков
  connect(_quickUi->rootObject(), SIGNAL(sigSetZeroShift()), this, SLOT(setZeroShifts()));
  // выбор файла автосохранения
  connect(_quickUi->rootObject(), SIGNAL(sigPeekFile()), this, SLOT(selectAutoSaveFile()));
  // Установка имени
  connect(_quickUi->rootObject(), SIGNAL(sigNameEntered()), this, SLOT(setWindowName()));
  // отработка нажатия кнопки печати
  connect(_tStatChart, SIGNAL(sigClickedPrint()), this, SLOT(printChart()));
  // отработка нажатия кнопки сохранение CSV
  connect(_tStatChart, SIGNAL(sigClickedSaveCSV()), this, SLOT(saveChartToCSV()));
  // отработка нажатия кнопки сохранение XLS
  connect(_tStatChart, SIGNAL(sigClickedSaveXLS()), this, SLOT(saveChartToXLS()));
  // отработка нажатия кнопки DATA
  connect(_quickUi->rootObject(), SIGNAL(sigReleaseData()), this, SIGNAL(sigDataPressed()));
  // передача измерения в Windows
  connect(_quickUi->rootObject(), SIGNAL(sigSendMeasurementMessage()), this, SLOT(sendWmAppMessage()));
  // изменения в единице измерения
  connect(_inputIndicator, SIGNAL(sigChangeTransGauge()), this, SLOT(changedTransGauge()));

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
  loadSettingsIndicator();

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
}


int Indicator::Id()
{
  return _idIndicator;
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
  QVariant varTemp;
  InputNumber inTemps1, inTemps2, inTempi1, inTempi2, inTempD;
  _formulaComlete = false; // Формула не готова
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
    _formulaComlete = true; // Формула готова
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
  _inputIndicator->setProperty("blDetect1EnableInput", _detect1 != nullptr);
  _inputIndicator->setProperty("blDetect2EnableInput", _detect2 != nullptr);
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
    slCLD << "Нет";
  return slCLD;
}


QStringList Indicator::createListByDetect2()
{
  QStringList slCLD;
  slCLD << "Нет";
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
    result = _scale1 * (_detect1 ? getMeasForTransform(_detect1->CurrentMeasure()) - _zeroShifts[0] : 0) + _increment1;
    break;
  case 2:
    result = _scale2 * (_detect2 ? getMeasForTransform(_detect2->CurrentMeasure()) - _zeroShifts[1] : 0) + _increment2;
    break;
  default:
    break;
  }
  return result;
}


// Расчет показаний датчиков по формуле
float Indicator::calculateResult()
{
  return _formulaComlete
        ? ((calculateChannel(1) + calculateChannel(2))/_divider + _inputIndicator->property("beforeSet").toFloat())
        : 0;
}


// Разрешение обнуления показаний
void Indicator::enableSetZero()
{
  if (_formulaComlete)
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
  _settings->SetValue(IndKeys::GROUPSF, qmlWidget->property("text").toString().toInt());

  // автосохранение
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("automaticSave");
  _settings->SetValue(IndKeys::AUTO_SAVE_ENABLE,  qmlWidget->property("checked").toBool());
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("tfAutoSave");
  _settings->SetValue(IndKeys::AUTO_SAVE_PERIOD,  qmlWidget->property("text").toInt());
  _settings->SetValue(IndKeys::AUTO_SAVE_FILE,  _autoSaveFile);
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
bool Indicator::loadSettingsIndicator()
{
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

  QObject* qmlWidget = _quickUi->rootObject()->findChild<QObject*>("countGroupsF");
  qmlWidget->setProperty("text", _settings->Value(IndKeys::GROUPSF));
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("cbSortFormula");
  qmlWidget->setProperty("checked", _settings->Value(IndKeys::SORT_CHF));

  // автосохранение
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("automaticSave");
  qmlWidget->setProperty("checked",  _settings->Value(IndKeys::AUTO_SAVE_ENABLE));
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("timerSave");
  qmlWidget->setProperty("running",  _settings->Value(IndKeys::AUTO_SAVE_ENABLE));
  qmlWidget = _quickUi->rootObject()->findChild<QObject*>("tfAutoSave");
  qmlWidget->setProperty("text",  _settings->Value(IndKeys::AUTO_SAVE_PERIOD));
  _autoSaveFile = _settings->Value(IndKeys::AUTO_SAVE_FILE).toString();

  return true;
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


// сохранение графика
void Indicator::saveMeas(void)
{
    QString strFileName;
    QFileDialog fdCreateTable(this);
    fdCreateTable.setAcceptMode(QFileDialog::AcceptSave); // Для сохранения файла
    fdCreateTable.setFileMode(QFileDialog::AnyFile); // Для выбора несуществующего файла
    fdCreateTable.setNameFilters({"Таблица XLSX (*.xlsx)"
                                  , "Таблица CSV (*.csv)"});
    //fdCreateTable.setDefaultSuffix("csv");
    fdCreateTable.setViewMode(QFileDialog::List); // Файлы представляются в виде списка
    if (fdCreateTable.exec() == QDialog::Accepted) //Файл выбран
    {
      strFileName = fdCreateTable.selectedFiles().first();
      _measuredLogs.push_back(_quickUi->rootObject()->property("textcsv").toString());
      if (strFileName.contains(".csv", Qt::CaseInsensitive))
        saveToCSV(strFileName);
      else if (strFileName.contains(".xls", Qt::CaseInsensitive))
        saveToXLS(strFileName);
       _quickUi->rootObject()->setProperty("clearMeasdata", true);
    }
}


// печать графика
void Indicator::printChart(void)
{
    QPrinter* pprinter = new QPrinter;
    QPrintDialog printDialog(pprinter, this);
    printDialog.setMinMax(1, 1); // печать будет только на одном листе
    if (printDialog.exec() == QDialog::Accepted)
    {
        QPainter painter(pprinter);
        QRect rectPrint = painter.viewport(); // Определение области печати принтера
        QPixmap pix(_quickUi->size()); // Определяем размер печатаемого объекта
        QSize sizePix = pix.size();
        sizePix.scale(rectPrint.size(), Qt::KeepAspectRatio); // масштабируем размер изображения под область печати
        painter.setViewport(rectPrint.x(), rectPrint.y()+((rectPrint.height()-sizePix.height())/2), sizePix.width(), sizePix.height()); // устанавливаем область печати
        _quickUi->render(&pix); // скидываем изображение виджета в pixmap
        painter.setWindow(pix.rect());// устанавливаем размер объекта для печати
        painter.drawPixmap(0, 0, pix);
    }
    delete pprinter; // удаление объекта
}


// сохранение графика
void Indicator::saveChartToCSV()
{
    QFileDialog fdCreateCSV(this);
    fdCreateCSV.setAcceptMode(QFileDialog::AcceptSave); // Для сохранения файла
    fdCreateCSV.setFileMode(QFileDialog::AnyFile); // Для выбора несуществующего файла
    fdCreateCSV.setNameFilter("Таблица (*.csv)");
    fdCreateCSV.setDefaultSuffix("csv");
    fdCreateCSV.setViewMode(QFileDialog::List); // Файлы представляются в виде списка
    if (fdCreateCSV.exec() == QDialog::Accepted) //Файл выбран
    {
      _measuredLogs.push_back(_tStatChart->property("textcsv").toString());
      saveToCSV(fdCreateCSV.selectedFiles().first());
    }
}


void Indicator::saveToCSV(QString fileName)
{
  QFile file(fileName);
  if (file.open(QFile::WriteOnly | QFile::Text))
  {
      QTextStream out(&file);
      for (QString text : _measuredLogs)
        out << convertStringToCorrectCSV(text + "\n\n");
      file.close();
      _measuredLogs.clear();
  }
  else
    QMessageBox::warning(this,
                         "Проблема записи",
                         QString("Невозможна запись в файл %1:\n%2.").arg(QDir::toNativeSeparators(fileName), file.errorString()));
}


void Indicator::saveChartToXLS()
{
  QFileDialog fdCreateXLS(this);
  fdCreateXLS.setAcceptMode(QFileDialog::AcceptSave); // Для сохранения файла
  fdCreateXLS.setFileMode(QFileDialog::AnyFile); // Для выбора несуществующего файла
  fdCreateXLS.setNameFilter("Таблица (*.xlsx)");
  fdCreateXLS.setDefaultSuffix("xlsx");
  fdCreateXLS.setViewMode(QFileDialog::List); // Файлы представляются в виде списка
  if (fdCreateXLS.exec() == QDialog::Accepted) //Файл выбран
  {
    _measuredLogs.push_back(_tStatChart->property("textcsv").toString());
    saveToXLS(fdCreateXLS.selectedFiles().first());
  }
}


void Indicator::saveToXLS(QString fileName)
{
  if (QFile::exists(fileName))
    if (!QFile::remove(fileName))
      return;

  if (_measuredLogs.empty())
    return;

  CWorkbook book( "Imp21" );
  std::vector<ColumnWidth> ColWidth;
  ColWidth.push_back(ColumnWidth(0, 0, 30));

  int pageNumber = 1;
  for (QString text : _measuredLogs)
  {
    QString out = text;
    QStringList linen = out.split("\n");
    if (linen.size() == 0)
      continue;

    QString pageName = "Лист " + QString::number(pageNumber++);
    CWorksheet& sheet = book.AddSheet(pageName.toStdString(), ColWidth);

    int row = 1;
    for (auto& line : linen)
    {
      int col = 1;
      QStringList cells = line.split(QLatin1Char(';'));
      sheet.BeginRow();
      for (auto& cell : cells)
        if (row > 9 && col > 1)
          sheet.AddCell(cell.toFloat());
        else
          sheet.AddCell(cell.toStdString());
      sheet.EndRow();
      ++row;
    }
  }

  book.Save(fileName.toStdString());
  _measuredLogs.clear();
}


void Indicator::selectAutoSaveFile()
{
  QString autoSaveFile = QFileDialog::getSaveFileName(this
                                                      , "Выберите файл для автосохранения"
                                                      , _autoSaveFile
                                                      , "Excel file (*.xlsx)");
  if (!autoSaveFile.isEmpty())
    _autoSaveFile = autoSaveFile;
}


void Indicator::autoSaveToXLSX()
{
  _measuredLogs.push_back(_quickUi->rootObject()->property("textcsv").toString());
  saveToXLS(_autoSaveFile);
}


void Indicator::sendWmAppMessage()
{
  PostMessageSender::Instance(_parent)->Do
      (
        DataSender::Indicator
        , _idIndicator + 1
        , _quickUi->rootObject()->property("sendData").toFloat()
      );
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


float Indicator::getMeasForTransform(float mr)
{
    float result = 0;
    switch (_transGauge)
    {
    case UnitMM: // мкм - мм
        result = mr / 1000;
        break;
    case UnitInch: // мкм - дюйм
        result = mr / 25400;
        break;
    case UnitAngleSec: // мкм/м -> угловая секунда
        result = 3600 * atanf(mr / 1000000) * 180 / M_PI;
        break;
    default:
        result = mr;

        break;
    }
    return result;
}


void Indicator::changedTransGauge()
{
  _transGauge = static_cast<TransToUnit>(_inputIndicator->property("transGauge").toInt());
}

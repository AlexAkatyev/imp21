#include <QtWidgets>
#include <QQuickWidget>
#include <QDomNode>
#include <QGraphicsAnchorLayout>
#include <QQuickItem>
#include <QPrinter>
#include <QPrintDialog>
#include <QAxObject>


#include "indicator.h"
#include "UtilLib/utillib.h"
#include "Detects/vtdetect.h"
#include "imp.h"
#include "checkInputNumberIF/checkInputNumberIF.h"

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

// Поля файла настроек
#define DOM_ROOT "indicator-ini"
#define DOM_WINDOW "window"
#define DOM_TOPLEFTPOINT "topLeftPoint"
#define DOM_X "x"
#define DOM_Y "y"
#define DOM_SIZE "size"
#define DOM_WIDTH "width"
#define DOM_HEIGHT "height"
#define DOM_FORMULA "formula"
#define DOM_NAME "name"
#define DOM_SCALE1 "scale1"
#define DOM_DETECT1 "detect1"
#define DOM_INCREMENT1 "increment1"
#define DOM_SCALE2 "scale2"
#define DOM_DETECT2 "detect2"
#define DOM_INCREMENT2 "increment2"
#define DOM_DIVIDER "divider"
#define DOM_BEFORESET "beforeset"
#define DOM_DOPUSK "dopusk"
#define DOM_GAUGE "gauge"
#define DOM_UNITPOINT "unitpoint"
#define DOM_HIGHLIMIT "highlimit"
#define DOM_LOWLIMIT "lowlimit"
#define DOM_PRIEMKA "priemka"
#define DOM_ACCURACY "accuracy"
#define DOM_PERIOD "period"
#define DOM_ACCUR_DIVISION "accur_division"
#define DOM_MEAS_MODE "meas_mode"
#define DOM_TRANS_GAUGE "transGauge"
#define DOM_SORT "sort"
#define DOM_GROUPSF "groupsf"
#define DOM_SORT_CHF "scf"

// Исходные размеры окна индикатора
const int SIZE_INDICATOR_WINDOW_X = 440;
const int SIZE_INDICATOR_WINDOW_Y = 480;
//const int SIZE_INDICATOR_WINDOW_Y2 = (SIZE_INDICATOR_WINDOW_Y/2);

const int MINIMAL_HEIGHT = 300;
const int MINIMAL_WIDTH = 300;

// Периодичность считывания показаний датчиков
const int WATCH_DOG_INTERVAL = 200;


Indicator::Indicator(QWidget* parent, int identificator, ImpAbstractDetect* baseDetect)
  : QWidget(nullptr, Qt::Window)
  , _parent(static_cast<Imp*>(parent))
  , _idIndicator(identificator)
  , _quickUi(new QQuickWidget)
  , _formulaComlete(true)
  , _detect1(nullptr)
  , _detect2(nullptr)
{
  _fileSettingsIndicator = new QFile("indicator" + QString::number(_idIndicator) + ".xml"); // Указание файла с параметрами установок

  // Размещение окна индикатора в центр экрана
  QPoint center = QDesktopWidget().availableGeometry().center(); //получаем координаты центра экрана
  center.setX(center.x() - (SIZE_INDICATOR_WINDOW_X/2));
  center.setY(center.y() - (SIZE_INDICATOR_WINDOW_Y/2));
  resize(SIZE_INDICATOR_WINDOW_X, SIZE_INDICATOR_WINDOW_Y);
  move(center);

  // Включение QML виджета
  QUrl source("qrc:/indicator.qml");
  _quickUi->setSource(source);

  // контейнер для QML виджета
  QVBoxLayout* pvbx = new QVBoxLayout();

  pvbx->setMargin(0); // Толщина рамки
  pvbx->addWidget(_quickUi); // Вставляем QML виджет в лайоут окна
  //pvbx->addWidget(pcbMode);
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
  // Установка имени
  connect(_quickUi->rootObject(), SIGNAL(sigNameEntered()), this, SLOT(setWindowName()));
  // отработка нажатия кнопки печати
  connect(_tStatChart, SIGNAL(sigClickedPrint()), this, SLOT(printChart()));
  // отработка нажатия кнопки сохранение CSV
  connect(_tStatChart, SIGNAL(sigClickedSaveCSV()), this, SLOT(saveChartToCSV()));
  // отработка нажатия кнопки сохранение XLS
  connect(_tStatChart, SIGNAL(sigClickedSaveXLS()), this, SLOT(saveChartToXLS()));

  // получение информиции о имеющихся датчиках от главного окна
  connect(_parent, SIGNAL(sigFindDetect()), this, SLOT(setComboListDetect()));
  setComboListDetect(); // Сразу список надо передать в страницу ФОРМУЛА

  // Запуск периодического обновления показаний датчика на шкале
  // Если есть файл инициализации, то эти данные обновятся
  QString indicatorName = "Индикатор " + QString::number(_idIndicator);
  // Присвоение имени окну
  setTitle(indicatorName);
  _tfUnitPoint->setProperty("text", "5");
  _tfHiLimit->setProperty("text", "50");
  _tfLoLimit->setProperty("text", "-50");
  _tfNumberCharPoint->setProperty("currentIndex ", 1);
  _detect1 = baseDetect;
  if (_detect1 != nullptr)
  {
    int index = currentIndex1ByName(_detect1->UserName());
    _cbListDetect1->setProperty("currentIndex", index);
  }
  _detect2 = nullptr;
  _increment1 = 0;
  _tfIncert1->setProperty("text", _increment1);
  _increment2 = 0;
  _tfIncert2->setProperty("text", _increment2);
  _scale1 = 1;
  _tfFactor1->setProperty("text", _scale1);
  _scale2 = 1;
  _tfFactor2->setProperty("text", _scale2);
  _divider = 1;
  _tfDivider->setProperty("text", _divider);
  _inputIndicator->setProperty("transGauge", 0);

  QTimer* timerUpdateIndicator = new QTimer(this);
  connect(timerUpdateIndicator, SIGNAL(timeout()), this, SLOT(updateResult()));
  timerUpdateIndicator->setInterval(UPDATE_RESULT_TIME);
  timerUpdateIndicator->start();

  // Настройка таймеров периодического чтения результатов измерения датчиков
  _timerWatchDog = new QTimer(this);
  connect(_timerWatchDog, SIGNAL(timeout()), this, SLOT(watchDogControl()));
  _timerWatchDog->setInterval(WATCH_DOG_INTERVAL);
  _timerWatchDog->start();

  // Использование параметров показаний по умолчанию
  _periodMean = 0;
  _lenMean = 1;

  // Сообщение о закрытии индикатора
  connect(this, SIGNAL(sigCloseIndicator(int)), _parent, SLOT(deleteIndicator(int)));


  // Загрузка старых настроек, если есть
  if (_fileSettingsIndicator->open(QIODevice::ReadOnly))
  {
    QString errorString;
    int errorLine = 0;
    int errorColumn = 0;
    QDomDocument ddSettingsIndicator;
    if (ddSettingsIndicator.setContent(_fileSettingsIndicator, true, &errorString, &errorLine, &errorColumn))
    { // ошибок нет
      _fileSettingsIndicator->close();
      QDomElement root = ddSettingsIndicator.documentElement();
      if (root.tagName() == DOM_ROOT) // структура правильная, заголовок свой, идем по дереву
        traverseNodeIndicator(&root);
    }
    else // ошибка в файле инициализации
      _fileSettingsIndicator->close();
  }

  if (baseDetect)
  {
    setTitle(QString::number(baseDetect->Id()));
    _detect1 = baseDetect;
    _detect2 = nullptr;
    setComboListDetect();
  }

  if (_detect1 || _detect2) // Заказана формула индикатора
    setFormula();

  setMinimumSize(QSize(MINIMAL_WIDTH, MINIMAL_HEIGHT));
  this->show(); // Окно выводим на экран
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
  InputNumber inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_UNSIGNED_FLOAT, 6);
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
  InputNumber inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT);
  if (inTemp.InfoError.isEmpty())
  { // Ошибок нет
    fHiLimit = inTemp.fNumber;
  }
  else
  { // В введенном поле есть ошибки
    QMessageBox::warning(this, "Верхнее предельное отклонение", inTemp.InfoError);
  }
  varTemp = _tfLoLimit->property("text");
  inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT);
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
  inTemp = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT);
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
  inTemps1 = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT);
  if (inTemps1.InfoError.isEmpty() == false)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Множитель 1", inTemps1.InfoError);
  }
  // Множитель 2 ------------------------------------------------
  varTemp = _tfFactor2->property("text");
  inTemps2 = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT);
  if (inTemps2.InfoError.isEmpty() == false)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Множитель 2", inTemps2.InfoError);
  }
  // Слагаемое 1 ------------------------------------------------
  varTemp = _tfIncert1->property("text");
  inTempi1 = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT);
  if (inTempi1.InfoError.isEmpty() == false)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Слагаемое 1", inTempi1.InfoError);
  }
  // Слагаемое 2 ------------------------------------------------
  varTemp = _tfIncert2->property("text");
  inTempi2 = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT);
  if (inTempi2.InfoError.isEmpty() == false)
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this, "Слагаемое 2", inTempi2.InfoError);
  }
  // Общий делитель ---------------------------------------------
  varTemp = _tfDivider->property("text");
  inTempD = checkInputNumberIF(varTemp.toString(), NUMBER_FLOAT);
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
    disconnect(_detect1, &VTDetect::FixMeasure, this, &Indicator::runButtonRelease);
  _detect1 = _parent->DetectAtName(strCurrent1);
  if (_detect1)
    connect(_detect1, &VTDetect::FixMeasure, this, &Indicator::runButtonRelease);
  if (_detect1 && abs(inTempi1.fNumber) > _detect1->PreSetInterval())
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this,
                         "Недопустимое значение предустанова для датчика 1",
                         "Значение предустанова для датчика 1 должно быть не более " + QString::number(_detect1->PreSetInterval()));
  }
  // Датчик 2
  qApp->processEvents();
  QString strCurrent2 = _cbListDetect2->property("currentText").toString();
  if (_detect2)
    disconnect(_detect2, &VTDetect::FixMeasure, this, &Indicator::runButtonRelease);
  _detect2 = _parent->DetectAtName(strCurrent2);
  if (_detect2)
    connect(_detect2, &VTDetect::FixMeasure, this, &Indicator::runButtonRelease);
  if (_detect2 && abs(inTempi2.fNumber) > _detect2->PreSetInterval())
  { // В введенном поле есть ошибки
    error = true;
    QMessageBox::warning(this,
                         "Недопустимое значение предустанова для датчика 2",
                         "Значение предустанова для датчика 2 должно быть не более " + QString::number(_detect2->PreSetInterval()));
  }
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


void Indicator::runButtonRelease()
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
  QStringList slCLD1 = createListByDetect1();
  QStringList slCLD2 = createListByDetect2();
  _cbListDetect1->setProperty("model", slCLD1);
  _cbListDetect2->setProperty("model", slCLD2);

  // По ключу выставляем активный датчик
  int index1 = 0; // Датчик по умолчанию исчез из списка
  if (_detect1)
  {
    if (_detect1->Ready()) // Датчик остался в списке и жив
      index1 = currentIndex1ByName(_detect1->UserName());
    else // Датчик в процессе удаления
      _detect1 = nullptr;
  }
  _cbListDetect1->setProperty("currentIndex", index1);

  int index2 = 0; // Датчик по умолчанию исчез из списка
  if (_detect2 != nullptr)
  {
    if (_detect2->Ready()) // Датчик остался в списке и жив
      index2 = currentIndex2ByName(_detect2->UserName());
    else // Датчик в процессе удаления
      _detect2 = nullptr;
  }
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
  _inputIndicator->setProperty("mDetect1", calculateChannel(1));
  _inputIndicator->setProperty("mDetect2", calculateChannel(2));
  enableSetZero();
}


float Indicator::calculateChannel(int number)
{
  float result = 0;
  switch (number) {
  case 1:
    result = _scale1 * (_detect1 ? _detect1->CurrentMeasure() : 0) + _increment1;
    break;
  case 2:
    result = _scale2 * (_detect2 ? _detect2->CurrentMeasure() : 0) + _increment2;
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
  // удаление файла инициализации
  //_fileSettingsIndicator->remove(); // никогда не удалять
  // сохранение файла инициализации - последние изменения
  saveSettingsIndicator();
}


// сохранение настроек
void Indicator::saveSettingsIndicator(void)
{
  QDomDocument doc;
  QDomElement root = doc.createElement(DOM_ROOT);
  doc.appendChild(root);

  // Сохранение параметров окна
  QDomElement window = doc.createElement(DOM_WINDOW);
  QDomElement topLeftPoint = doc.createElement(DOM_TOPLEFTPOINT);
  QDomElement topLeftPointX = doc.createElement(DOM_X);
  QDomElement topLeftPointY = doc.createElement(DOM_Y);
  QDomText topLeftPointXText = doc.createTextNode(QString().setNum(frameGeometry().x()));
  QDomText topLeftPointYText = doc.createTextNode(QString().setNum(frameGeometry().y()));
  QDomElement size = doc.createElement(DOM_SIZE);
  QDomElement width = doc.createElement(DOM_WIDTH);
  QDomText widthText = doc.createTextNode(QString().setNum(geometry().width()));
  QDomElement height = doc.createElement(DOM_HEIGHT);
  QDomText heightText = doc.createTextNode(QString().setNum(geometry().height()));

  root.appendChild(window);
  window.appendChild(topLeftPoint);
  topLeftPoint.appendChild(topLeftPointX);
  topLeftPointX.appendChild(topLeftPointXText);
  topLeftPoint.appendChild(topLeftPointY);
  topLeftPointY.appendChild(topLeftPointYText);
  window.appendChild(size);
  size.appendChild(width);
  width.appendChild(widthText);
  size.appendChild(height);
  height.appendChild(heightText);

  // сохранение формулы
  QDomElement formula = doc.createElement(DOM_FORMULA);
  QDomElement dename = doc.createElement(DOM_NAME);
  QDomText dtname = doc.createTextNode(this->windowTitle());
  QDomElement descale1 = doc.createElement(DOM_SCALE1);
  QDomText dtscale1 = doc.createTextNode(QString::number(static_cast<double>(_scale1)));
  QDomElement dedetect1 = doc.createElement(DOM_DETECT1);
  QDomText dtdetect1 = doc.createTextNode(_detect1 ? QString::number(_detect1->Id()) : "0");
  QDomElement deincrement1 = doc.createElement(DOM_INCREMENT1);
  QDomText dtincrement1 = doc.createTextNode(QString::number(static_cast<double>(_increment1)));
  QDomElement descale2 = doc.createElement(DOM_SCALE2);
  QDomText dtscale2 = doc.createTextNode(QString::number(static_cast<double>(_scale2)));
  QDomElement dedetect2 = doc.createElement(DOM_DETECT2);
  QDomText dtdetect2 = doc.createTextNode(_detect2 ? QString::number(_detect2->Id()) : "0");
  QDomElement deincrement2 = doc.createElement(DOM_INCREMENT2);
  QDomText dtincrement2 = doc.createTextNode(QString::number(static_cast<double>(_increment2)));
  QDomElement dedivider = doc.createElement(DOM_DIVIDER);
  QDomText dtdivider = doc.createTextNode(QString::number(static_cast<double>(_divider)));
  QDomElement debeforeset = doc.createElement(DOM_BEFORESET);
  QDomText dtbeforeset = doc.createTextNode(QString::number(_inputIndicator->property("beforeSet").toDouble()));
  QDomElement dedopusk = doc.createElement(DOM_DOPUSK);
  QDomText dtdopusk = doc.createTextNode(_inputIndicator->property("dopusk").toBool() ? "true" : "false");
  QDomElement deperiod = doc.createElement(DOM_PERIOD);
  QDomText dtperiod = doc.createTextNode(_tfPeriod->property("text").toString());

  root.appendChild(formula);
  formula.appendChild(dename);
  dename.appendChild(dtname);
  formula.appendChild(descale1);
  descale1.appendChild(dtscale1);
  formula.appendChild(dedetect1);
  dedetect1.appendChild(dtdetect1);
  formula.appendChild(deincrement1);
  deincrement1.appendChild(dtincrement1);
  formula.appendChild(descale2);
  descale2.appendChild(dtscale2);
  formula.appendChild(dedetect2);
  dedetect2.appendChild(dtdetect2);
  formula.appendChild(deincrement2);
  deincrement2.appendChild(dtincrement2);
  formula.appendChild(dedivider);
  dedivider.appendChild(dtdivider);
  formula.appendChild(debeforeset);
  debeforeset.appendChild(dtbeforeset);
  formula.appendChild(dedopusk);
  dedopusk.appendChild(dtdopusk);
  formula.appendChild(deperiod);
  deperiod.appendChild(dtperiod);

  // настройка шкалы
  float flTemp;
  QDomElement gauge = doc.createElement(DOM_GAUGE);
  QDomElement deunitpoint = doc.createElement(DOM_UNITPOINT);
  flTemp = _inputIndicator->property("unitPoint").toFloat();
  flTemp = roundf(flTemp*100000)/100000;
  QDomText dtunitpoint = doc.createTextNode(QString::number(static_cast<double>(flTemp)));
  QDomElement dehilimit = doc.createElement(DOM_HIGHLIMIT);
  flTemp = _inputIndicator->property("highLimit").toFloat();
  flTemp = roundf(flTemp*100)/100;
  QDomText dthilimit = doc.createTextNode(QString::number(static_cast<double>(flTemp)));
  QDomElement delolimit = doc.createElement(DOM_LOWLIMIT);
  flTemp = _inputIndicator->property("lowLimit").toFloat();
  flTemp = roundf(flTemp*100)/100;
  QDomText dtlolimit = doc.createTextNode(QString::number(static_cast<double>(flTemp)));
  QDomElement depriemka = doc.createElement(DOM_PRIEMKA);
  flTemp = _inputIndicator->property("priemka").toFloat();
  flTemp = roundf(flTemp*100)/100;
  QDomText dtpriemka = doc.createTextNode(QString::number(static_cast<double>(flTemp)));
  QDomElement deaccuracy = doc.createElement(DOM_ACCURACY);
  QDomText dtaccuracy = doc.createTextNode(_inputIndicator->property("accuracy").toString());
  QDomElement deaccurdivision = doc.createElement(DOM_ACCUR_DIVISION);
  QDomText dtaccurdivision = doc.createTextNode(_inputIndicator->property("accurDivision").toString());

  root.appendChild(gauge);
  gauge.appendChild(deunitpoint);
  deunitpoint.appendChild(dtunitpoint);
  gauge.appendChild(dehilimit);
  dehilimit.appendChild(dthilimit);
  gauge.appendChild(delolimit);
  delolimit.appendChild(dtlolimit);
  gauge.appendChild(depriemka);
  depriemka.appendChild(dtpriemka);
  gauge.appendChild(deaccuracy);
  deaccuracy.appendChild(dtaccuracy);
  gauge.appendChild(deaccurdivision);
  deaccurdivision.appendChild(dtaccurdivision);

  // Режим измерения
  QDomElement demode = doc.createElement(DOM_MEAS_MODE);
  int imode = _quickUi->rootObject()->property("deviationMode").toBool() ? 1 : 0; // автомат/ручной режим
  QDomText dtmode = doc.createTextNode(QString::number(imode));
  root.appendChild(demode);
  demode.appendChild(dtmode);

  // Преобразование шкалы
  QDomElement detrans = doc.createElement(DOM_TRANS_GAUGE);
  QDomText dttrans = doc.createTextNode(_inputIndicator->property("transGauge").toString());
  root.appendChild(detrans);
  detrans.appendChild(dttrans);

  // Режимы сортировки
  QDomElement desort = doc.createElement(DOM_SORT);
  root.appendChild(desort);
  std::vector<std::pair<QString, QString>> sortIniSelect =
  {
    {DOM_SORT_CHF, "cbSortFormula"}
  };
  for (auto& para : sortIniSelect)
  {
    QDomElement de = doc.createElement(para.first);
    QObject* qmlWidget = _quickUi->rootObject()->findChild<QObject*>(para.second);
    QDomText dt = doc.createTextNode(qmlWidget->property("checked").toBool() ? "true" : "false");
    desort.appendChild(de);
    de.appendChild(dt);
  }
  std::vector<std::pair<QString, QString>> sortIni =
  {
    {DOM_GROUPSF, "countGroupsF"}
  };
  for (auto& para : sortIni)
  {
    QDomElement de = doc.createElement(para.first);
    QObject* qmlWidget = _quickUi->rootObject()->findChild<QObject*>(para.second);
    QDomText dt = doc.createTextNode(qmlWidget->property("text").toString());
    desort.appendChild(de);
    de.appendChild(dt);
  }

  // сохранение структуры в файл
  if (!_fileSettingsIndicator->open(QIODevice::WriteOnly | QIODevice::Text))
    return;
  QTextStream out(_fileSettingsIndicator);
  doc.save(out, 4);
  _fileSettingsIndicator->close();
}


// Чтение установок
void Indicator::traverseNodeIndicator(QDomNode* node)
{
  QDomElement deTemp;
  QDomNode domNode = node->firstChild();
  while(!domNode.isNull())
  {
    QDomElement domElement = domNode.toElement();
    if(!domElement.isNull())
    {
      if(domElement.tagName() == DOM_WINDOW)
      {
        QDomElement sizeElement =  domElement.firstChildElement(DOM_SIZE);
        QDomElement sizeWidthElement =  sizeElement.firstChildElement(DOM_WIDTH);
        QDomElement sizeHeightElement =  sizeElement.firstChildElement(DOM_HEIGHT);
        QDomElement topLeftPointElement =  domElement.firstChildElement(DOM_TOPLEFTPOINT);
        QDomElement topLeftPointXElement =  topLeftPointElement.firstChildElement(DOM_X);
        QDomElement topLeftPointYElement =  topLeftPointElement.firstChildElement(DOM_Y);
        // Устанавливаем размер и положение окна программы
        QRect windowGeomerty = QRect(	topLeftPointXElement.text().toInt(),
                                        topLeftPointYElement.text().toInt(),
                                        sizeWidthElement.text().toInt(),
                                        sizeHeightElement.text().toInt());
        this->move(windowGeomerty.topLeft());
        this->resize(windowGeomerty.size());
      }
      if (domElement.tagName() == DOM_SCALE1)
      {
        _scale1 = domElement.text().toFloat();
        _tfFactor1->setProperty("text", round(static_cast<double>(_scale1)*100)/100);
      }
      if (domElement.tagName() == DOM_DETECT1)
      {
        QString idDetect1 = domElement.text();
        if (idDetect1 == "0")
        { // датчик не используется
          _cbListDetect1->setProperty("currentIndex", 0);
          _detect1 = nullptr;
          _inputIndicator->setProperty("blDetect1EnableInput", false);
        }
        else
        { // датчик указан
          // По ключу выставляем активный датчик
          _detect1 = _parent->DetectAtId(idDetect1.toInt());
          if (_detect1)
          { // Датчик остался в списке
            QStringList slTemp;
            int index = currentIndex1ByName(_detect1->UserName());
            _cbListDetect1->setProperty("currentIndex", index);
            _inputIndicator->setProperty("blDetect1EnableInput", index != 0);
          }
          else
          { // Датчик исчез из списка
            _cbListDetect1->setProperty("currentIndex", 0);
            _inputIndicator->setProperty("blDetect1EnableInput", false);
          }
        }
      }
      if (domElement.tagName() == DOM_INCREMENT1)
      {
        _increment1 = domElement.text().toFloat();
        _tfIncert1->setProperty("text", round(static_cast<double>(_increment1)*100)/100);
      }
      if (domElement.tagName() == DOM_SCALE2)
      {
        _scale2 = domElement.text().toFloat();
        _tfFactor2->setProperty("text", round(static_cast<double>(_scale2)*100)/100);
      }
      if (domElement.tagName() == DOM_DETECT2)
      {
        QString idDetect2 = domElement.text();
        if (idDetect2 == "0")
        { // датчик не используется
          _cbListDetect2->setProperty("currentIndex", 0);
          _detect2 = nullptr;
          _inputIndicator->setProperty("blDetect2EnableInput", false);
        }
        else
        { // датчик указан
          // По ключу выставляем активный датчик
          _detect2 = _parent->DetectAtId(idDetect2.toInt());
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
      }
      if (domElement.tagName() == DOM_INCREMENT2)
      {
        _increment2 = domElement.text().toFloat();
        _tfIncert2->setProperty("text", round(static_cast<double>(_increment2)*100)/100);
      }
      if (domElement.tagName() == DOM_DIVIDER)
      {
        _divider = domElement.text().toFloat();
        _tfDivider->setProperty("text", round(static_cast<double>(_divider)*100)/100);
      }
      if (domElement.tagName() == DOM_BEFORESET)
        _inputIndicator->setProperty("beforeSet", domElement.text().toFloat());
      if (domElement.tagName() == DOM_DOPUSK)
        _inputIndicator->setProperty("dopusk", domElement.text() == "true");
      if (domElement.tagName() == DOM_PERIOD)
      {
        _periodMean = domElement.text().toInt();
        if (_periodMean > LEN_MAD * UPDATE_RESULT_TIME)
        {
          _periodMean = LEN_MAD * UPDATE_RESULT_TIME;
        }
        _lenMean = static_cast<int>(round(_periodMean / UPDATE_RESULT_TIME));
        _lenMean = _lenMean == 0 ? 1 : _lenMean; // Минимальное значение - 1
        for (int i=0; i<_lenMean; i++) flHistoryMean[i] = 0; // Очистка истории
        _tfPeriod->setProperty("text", QString::number(_periodMean));
      }
      if (domElement.tagName() == DOM_UNITPOINT)
      {
        _inputIndicator->setProperty("unitPoint", domElement.text().toFloat());
        _tfUnitPoint->setProperty("text", domElement.text());
      }
      if (domElement.tagName() == DOM_HIGHLIMIT)
      {
        _inputIndicator->setProperty("highLimit", domElement.text().toFloat());
        _tfHiLimit->setProperty("text", domElement.text());
      }
      if (domElement.tagName() == DOM_LOWLIMIT)
      {
        _inputIndicator->setProperty("lowLimit", domElement.text().toFloat());
        _tfLoLimit->setProperty("text", domElement.text());
      }
      if (domElement.tagName() == DOM_PRIEMKA)
      {
        _inputIndicator->setProperty("priemka", domElement.text().toFloat());
        _tfPriemka->setProperty("text", domElement.text());
      }
      if (domElement.tagName() == DOM_ACCURACY)
      {
        float accuracy = domElement.text().toFloat();
        if (accuracy < 0)
          accuracy = 0;
        _inputIndicator->setProperty("accuracy", accuracy);
      }
      if (domElement.tagName() == DOM_ACCUR_DIVISION)
      {
        _inputIndicator->setProperty("accurDivision", domElement.text().toInt());
      }
      if (domElement.tagName() == DOM_MEAS_MODE)
        if (domElement.text().toInt())
          _quickUi->rootObject()->setProperty("deviationMode", true);
      if (domElement.tagName() == DOM_NAME)
        setTitle(domElement.text());
      if (domElement.tagName() == DOM_TRANS_GAUGE)
        _inputIndicator->setProperty("transGauge", domElement.text().toInt());
      if(domElement.tagName() == DOM_SORT)
      {
        std::vector<std::pair<QString, QString>> sortIni =
        {
          {DOM_GROUPSF, "countGroupsF"}
        };
        for (auto& para : sortIni)
        {
          QDomElement de =  domElement.firstChildElement(para.first);
          QObject* qmlWidget = _quickUi->rootObject()->findChild<QObject*>(para.second);
          qmlWidget->setProperty("text", de.text().toInt());
        }
        std::vector<std::pair<QString, QString>> sortIniSelect =
        {
          {DOM_SORT_CHF, "cbSortFormula"}
        };
        for (auto& para : sortIniSelect)
        {
          QDomElement de =  domElement.firstChildElement(para.first);
          QObject* qmlWidget = _quickUi->rootObject()->findChild<QObject*>(para.second);
          qmlWidget->setProperty("checked", de.text() == "true");
        }
      }
    }
    traverseNodeIndicator(&domNode);
    domNode = domNode.nextSibling();
  }

}


void Indicator::setTitle(QString indicatorName)
{
  _tfName->setProperty("text", indicatorName);
  this->setWindowTitle(indicatorName);
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
}


// сохранение графика
void Indicator::saveMeas(void)
{
    QString strFileName;
    QFileDialog fdCreateTable(this);
    fdCreateTable.setAcceptMode(QFileDialog::AcceptSave); // Для сохранения файла
    fdCreateTable.setFileMode(QFileDialog::AnyFile); // Для выбора несуществующего файла
    fdCreateTable.setNameFilters({"Таблица CSV (*.csv)", "Таблица XLSX (*.xlsx)"});
    //fdCreateTable.setDefaultSuffix("csv");
    fdCreateTable.setViewMode(QFileDialog::List); // Файлы представляются в виде списка
    if (fdCreateTable.exec() == QDialog::Accepted) //Файл выбран
    {
      strFileName = fdCreateTable.selectedFiles().first();
      QString text = _quickUi->rootObject()->property("textcsv").toString();
      if (strFileName.contains(".csv", Qt::CaseInsensitive))
        saveToCSV(strFileName, text);
      else if (strFileName.contains(".xls", Qt::CaseInsensitive))
        saveToXLS(strFileName, text);
    }
}


void Indicator::setWindowName(void)
{
    QString indicatorName = _tfName->property("text").toString();
    this->setWindowTitle(indicatorName);
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
      saveToCSV(fdCreateCSV.selectedFiles().first(), _tStatChart->property("textcsv").toString());
}


void Indicator::saveToCSV(QString fileName, QString text)
{
  QFile file(fileName);
  if (file.open(QFile::WriteOnly | QFile::Text))
  {
      QTextStream out(&file);
      out << convertStringToCorrectCSV(text);
      file.close();
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
    saveToXLS(fdCreateXLS.selectedFiles().first(), _tStatChart->property("textcsv").toString());
}


void Indicator::saveToXLS(QString fileName, QString text)
{
  QString out = text;
  QStringList linen = out.split("\n");
  if (linen.size() == 0)
    return;

  if (QFile::exists(fileName))
    QFile::remove(fileName);

  // получаем указатель на Excel
  QAxObject* mExcel = new QAxObject("Excel.Application",this);
  mExcel->setProperty("DisplayAlerts", 0); // Игнорировать сообщения
  // на книги
  QAxObject* workbooks = mExcel->querySubObject("Workbooks");
  // на директорию, откуда грузить книг
  workbooks->dynamicCall("Add()");
  QAxObject * workbook = mExcel-> querySubObject ("ActiveWorkBook");
  // указываем, какой лист выбрать
  QAxObject* sheet = workbook->querySubObject("Worksheets(int)", 1);

  int row = 1;
  for (auto& line : linen)
  {
    int col = 1;
    QStringList cells = line.split(QLatin1Char(';'));
    for (auto& cell : cells)
      if (row > 9 && col > 1)
        setFloatToCell(sheet, row, col++, cell.toFloat());
      else
        setValueToCell(sheet, row, col++, cell);
    ++row;
  }

  // освобождение памяти
  delete sheet;
  workbook->dynamicCall("SaveAs(const QString &)", QDir::toNativeSeparators(fileName));
  workbook->dynamicCall("Close()");
  delete workbook;
  //закрываем книги
  delete workbooks;
  //закрываем Excel
  mExcel->dynamicCall("Quit()");
  mExcel->setProperty("DisplayAlerts", 1);
  delete mExcel;
}


void Indicator::setValueToCell(QAxObject* sheet, int r, int c, QString text)
{
  sheet->querySubObject("Cells(int,int)", r, c)->setProperty("Value", text);
}

void Indicator::setFloatToCell(QAxObject* sheet, int r, int c, float f)
{
  sheet->querySubObject("Cells(int,int)", r, c)->setProperty("Value", f);
}




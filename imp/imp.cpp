/*
 *
 * Описание функций главного окна
 *
 * */
#include <sstream>

#include "imp.h"

#include <QQuickItem>

#include <QTimer>
#include <QMessageBox>
#include <QTime>
#include <QHash>
#include <QQuickWidget>
#include <QVBoxLayout>

#include "about/about.h"
#include "indicator.h"
#include "versionNo.h"
#include "Detects/detectfactory.h"
#include "Logger/logger.h"
#include "impsettings.h"
#include "impsettingsdialog.h"

// Пауза после запуска программы перед поиском датчиков
const int PAUSE_BEFORE_FIND_DETECT = 500;

// Параметры индикации поиска датчиков
const int INTERVAL_TIMER = 100;
// обновление статуса активности датчика
const int INTERVAL_UPDATE = 400;

// Максимальное количество индикаторов
const int MAX_INDICATOR = 256;

// Версия программы
const int VERSION_MAJOR = DEF_VERSION_MAJOR;
const int VERSION_MINOR = DEF_VERSION_MINOR;
const int VERSION_PATCH = DEF_VERSION_PATCH;
const int VERSION_TEST  = DEF_VERSION_TEST;

// Исходные размеры окна
const int SIZE_WINDOW_WIDTH = 1270;
const int SIZE_WINDOW_HEIGTH = 500;

const char* HELP_INFO = "build\\html\\index.html";
const QString KEY_DEF_OPTIONS = "DEFAULT_INDICATOR";

// Конструктор главного окна
Imp::Imp(QWidget* parent)
    : QWidget(parent)
    , pQuickUi(new QQuickWidget)
    , _indicateTimer(new QTimer(this))
{
    Logger::GetInstance(this); // Запуск журнала

    // Загрузка параметров из файла установок
    _useIndicators.clear(); // подготовка к загрузке множества индикаторов
    _indicators.clear();
    _flagRunIndicators = true;
    LoadSettingsGeneral();
    setMinimumSize(SIZE_WINDOW_WIDTH, SIZE_WINDOW_HEIGTH);

    // Присвоение имени окну
    this->setWindowTitle(QString("ИМП  v.%1.%2.%3").arg(DEF_VERSION_MAJOR).arg(DEF_VERSION_MINOR).arg(DEF_VERSION_PATCH));
    QUrl source("qrc:/imp.qml");
    pQuickUi->setSource(source);

    QVBoxLayout* pvbx = new QVBoxLayout();

    pvbx->addWidget(pQuickUi); // Вставляем QML виджет в лайоут окна
    pvbx->setMargin(0); // Толщина рамки
    setLayout(pvbx); //  Установка лайоута в окно

    // Установка указателей на объекты виджета QML
    pitWin = pQuickUi->rootObject();
    ppbFind = pQuickUi->rootObject()->findChild<QObject*>("pbFind");
    ptextComment = pQuickUi->rootObject()->findChild<QObject*>("textComment");
    pbtFind = pQuickUi->rootObject()->findChild<QObject*>("btFind");
    pbtIndicator = pQuickUi->rootObject()->findChild<QObject*>("btIndicator");

    // Отработка команды: Помощь/Справка
    connect(pQuickUi->rootObject(), SIGNAL(sigClickedbtHelp()), this, SLOT(showHelp()));

    // Отработка команды: Помощь/О программе
    connect(pQuickUi->rootObject(), SIGNAL(sigClickedbtAbout()), this, SLOT(showAbout()));

    // Отработка команды: Новый индикатор
    connect(pQuickUi->rootObject(), SIGNAL(sigNewIndicator(QString)), this, SLOT(createNewIndicator(QString)));

    // Отработка двойного щелчка по записи датчика, открытие окна установок датчика
    connect(pQuickUi->rootObject(), SIGNAL(sigSelectDetectToInit(QString)), this, SLOT(createNewSettings(QString)));

    // Отработка команды: Поиск датчиков
    connect(pQuickUi->rootObject(), SIGNAL(sigFindDetect()), this, SLOT(findDetect()));

    // Поиск датчиков при запуске программы через паузу
    TimerBeforeFound = new QTimer(this);
    TimerBeforeFound->setSingleShot(true);
    TimerBeforeFound->setInterval(PAUSE_BEFORE_FIND_DETECT);
    connect(TimerBeforeFound, &QTimer::timeout, this, &Imp::findDetect);

    _indicateTimer->setInterval(INTERVAL_TIMER);
    connect(_indicateTimer, &QTimer::timeout, this, &Imp::indicateFindCOMDetect);

    _timerUpdaterActiveStatus = new QTimer(this);
    _timerUpdaterActiveStatus->setInterval(INTERVAL_UPDATE);
    connect(_timerUpdaterActiveStatus, &QTimer::timeout, this, &Imp::changeActiveStatusToTable);

    if (ImpSettings::Instance(this)->Value(ImpKeys::DEBUG_GUI_MODE).toBool())
    {
      _flagRunIndicators = false;
      _detects = DetectFactory::Instance(this)->TestDetects();
      reWriteDetectsToTable();
      pitWin->setProperty("iCommand", 4);
    }
    else
    {
      // Поиск датчиков при запуске программы после паузы, чтобы оформилось главное окно
      TimerBeforeFound->start();
    }
}


// переопределение события изменения окна
void Imp::resizeEvent(QResizeEvent* event)
{
    // Масштабирование QML виджета под размер окна
    pQuickUi->rootObject()->setProperty("scaleX", QVariant(this->width()));
    pQuickUi->rootObject()->setProperty("scaleY", QVariant(this->height()));
    event->accept();
}


// Закрытие главного окна
void Imp::closeEvent(QCloseEvent* e)
{
    //Сохранение параметров окна
    SaveSettingsGeneral();
    emit sigCloseSettingsWindows();
    emit sigCloseIndicatorWindows();
    emit sigCloseGeneralWindow(); // Сигнализируем, что главное окно закрывается
    for (auto detect : _detects)
      detect->Stop();
    e->accept();
}


// Загрузка параметров из файла установок
bool Imp::LoadSettingsGeneral()
{
  ImpSettings* settings = ImpSettings::Instance(this);
  QRect windowGeometry = QRect(settings->Value(ImpKeys::WIN_X).toInt(),
                               settings->Value(ImpKeys::WIN_Y).toInt(),
                               settings->Value(ImpKeys::WIN_WIDTH).toInt(),
                               settings->Value(ImpKeys::WIN_HEIGHT).toInt());
  this->setGeometry(windowGeometry);
  QStringList lInd = settings->Value(ImpKeys::INDICATORS).toStringList();
  for (QString num : lInd)
  {
    _useIndicators.insert(num.toInt());
  }
  return true;
}


// Сохранение параметров установок в файл
void Imp::SaveSettingsGeneral()
{
  ImpSettings* settings = ImpSettings::Instance(this);
  QRect winGeometry = geometry();
  settings->SetValue(ImpKeys::WIN_X, winGeometry.x());
  settings->SetValue(ImpKeys::WIN_Y, winGeometry.y());
  settings->SetValue(ImpKeys::WIN_WIDTH, winGeometry.width());
  settings->SetValue(ImpKeys::WIN_HEIGHT, winGeometry.height());
  QStringList indficators;
  for (int num = 0; num < MAX_INDICATOR; num++)
    if (_useIndicators.contains(num) == true)
      indficators << QString::number(num);
  settings->SetValue(ImpKeys::INDICATORS, indficators);
}


// Запуск поиска датчиков
void Imp::findDetect()
{
    QVariant enableNewIndiator;
    _timerUpdaterActiveStatus->stop();

    pbtFind->setProperty("enabled", false); // Чтобы не было накладывающихся поисков
    if(_flagRunIndicators)
    {
        enableNewIndiator = pbtIndicator->property("enabled");
        pbtIndicator->setProperty("enabled", false); // при первом поиске датчиков индикаторы не создавать
    }

    pitWin->setProperty("iCommand", 1); // Команда очистки таблицы
    ptextComment->setProperty("text", "Найдено датчиков: " + QString::number(0));

    FindDetects();

    pbtFind->setProperty("enabled", true); // Разрешаем повторный поиск датчиков
    ptextComment->setProperty("text", " Найдено датчиков: " + QString::number(_detects.size()));

    if(_flagRunIndicators)
        pbtIndicator->setProperty("enabled", enableNewIndiator); // первый поиск датчиков завершен

    reWriteDetectsToTable();
    for (auto detect : _detects)
      connect(detect, &ImpAbstractDetect::UserNameChanged, this, [&]()
      {
        reWriteDetectsToTable();
        emit sigFindDetect();
      });

    if(_flagRunIndicators)
    { // при запуске программы открытие старых индикаторов
        for (int j = 0; j < MAX_INDICATOR; j++)
            if (_useIndicators.contains(j) == true)
                createIndicator(j, nullptr, false);
        _flagRunIndicators = false; // старые индикаторы больше не запускать
    }
    emit sigFindDetect();
    _timerUpdaterActiveStatus->start();
}


// Индикация процесса поиска датчиков
void Imp::indicateFindCOMDetect()
{
  _uiCounter = _uiCounter + INTERVAL_TIMER;
  ptextComment->setProperty("text", "Поиск датчиков");
  ppbFind->setProperty("value", (static_cast<float>(_uiCounter))/(_uiLength));
}


void Imp::reWriteDetectsToTable()
{
  pitWin->setProperty("iCommand", 1); // clear list of detects

  for (ImpAbstractDetect* detect : _detects)
  {
    // Передача описания найденного датчика в графический интерфейс
    pitWin->setProperty("strSerialNumber", QString::number(detect->Id()));
    pitWin->setProperty("strNameDetect", detect->UserName());
    pitWin->setProperty("strActive", detect->ActiveStateInfo());
    pitWin->setProperty("strTypeDetect", detect->TypeDetect());
    pitWin->setProperty("strDataManuf", detect->DateManuf().toString("dd.MM.yyyy"));
    pitWin->setProperty("strPort", detect->PortName());
    pitWin->setProperty("strModbusAddress", detect->Address());
    pitWin->setProperty("iCommand", 2); // Команда на добавление записи
  }
}


void Imp::changeActiveStatusToTable()
{
  for (ImpAbstractDetect* d : _detects)
  {
    if (d->ActiveStatusChanged())
    {
      pitWin->setProperty("strSerialNumber", QString::number(d->Id()));
      pitWin->setProperty("strActive", d->ActiveStateInfo());
      pitWin->setProperty("iCommand", 4); // Команда на коррекцию записи
    }
  }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Поиск датчиков на COM-портах
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Imp::FindDetects(void)
{
  // удалить из списка исчезнувшие датчики
  std::vector<ImpAbstractDetect*> det;
  for (ImpAbstractDetect* d: _detects)
    det.push_back(d);
  _detects.clear();
  for (ImpAbstractDetect* d: det)
    if (d->Ready())
      _detects.push_back(d);
    else
      d->Remove();
  qApp->processEvents();

  // искать новые датчики
  DetectFactory* dfactory = DetectFactory::Instance(this);
  static bool setConnect = false;

  if (!setConnect)
  {
    connect(dfactory, &DetectFactory::readyOfDetects, this, [=]()
    {
      for (ImpAbstractDetect* newDetect : dfactory->VTDetects())
      {
        bool haveId = false;
        for (auto currently : _detects)
          if (currently->Id() == newDetect->Id())
          {
            haveId = true;
            break;
          }
        if (haveId)
          newDetect->deleteLater();
        else
          _detects.push_back(newDetect);
      }

      _indicateTimer->stop();
      ptextComment->setProperty("text", "");
      ppbFind->setProperty("value", 1);
    });

    setConnect = true;
  }


  if (dfactory->AvailablePorts())
  {
    // Индикация времени поиска USB датчиков
    _uiCounter = 0;
    _uiLength = dfactory->FindingTime();
    ptextComment->setProperty("text", "Поиск датчиков");
    ppbFind->setProperty("value", _uiCounter);
    _indicateTimer->start();

    dfactory->StartFindOfDetects();
  }
}


// Создание класса для работы с установками датчика
void Imp::createNewSettings(QString idDetect)//,
{
  qApp->processEvents();
  ImpAbstractDetect* detect = DetectAtId(idDetect.toInt());
  if (detect)
    detect->ShowSettings(geometry());
}


// Создание нового индикатора
void Imp::createNewIndicator(QString strNDS)
{
  qApp->processEvents();
  int id = strNDS.toInt();
  ImpAbstractDetect* baseDetect = nullptr;
  if (id)
  {
    for (auto detect : _detects)
    {
      if (id == detect->Id())
      {
        baseDetect = detect;
        break;
      }
    }
  }

    if (!_flagRunIndicators) // Первый поиск датчиков и запуск старых индикаторов завершен
    {
        int j;
        // Поиск свободного идентификатора
        for (j = 0; j < MAX_INDICATOR; j++)
            if (_useIndicators.contains(j) == false)
                break;
        _useIndicators.insert(j);
        createIndicator(j, baseDetect, strNDS == KEY_DEF_OPTIONS);
    }
}


// Удаление индикатора из множества
void Imp::deleteIndicator(int idInd)
{
    _useIndicators.erase(_useIndicators.find(idInd));
    for (auto it = _indicators.begin(); it != _indicators.end(); )
    {
      if ((*it)->Id() == idInd)
      {
        it = _indicators.erase(it);
      }
      else
      {
        ++it;
      }
    }
}


void Imp::createIndicator(int index, ImpAbstractDetect* baseDetect, bool defOption)
{
  qApp->processEvents();
    Indicator* ind;
    ind = new Indicator(this,
                        index, // Номер индикатора
                        baseDetect, // ссылка на датчик
                        defOption);
    _indicators.push_back(ind);
    connect(ind, &Indicator::sigDataPressed, this, [=]()
    {
      ImpSettings* settings = ImpSettings::Instance(this);
      bool translate = settings->Value(ImpKeys::RECORDING_IN_ALL_INDICATORS).toBool();
      if (!translate)
      {
        return;
      }
      Indicator* ind = static_cast<Indicator*>(sender());
      int senderId = ind->Id();
      for (Indicator* i : _indicators)
      {
        if (i->Id() != senderId)
        {
          i->RunButtonRelease();
        }
      }
    });
    // Закрытие индикатора при закрытии главного окна
    connect(this, &Imp::sigCloseIndicatorWindows, ind, &Indicator::CloseMyIndicator);
}


void Imp::showHelp()
{
  std::stringstream stream;
  stream << HELP_INFO;
  system(stream.str().c_str());
}


void Imp::showAbout()
{
  static ImpSettingsDialog* sDialog = new ImpSettingsDialog(this);
  sDialog->UpdatePosition(geometry());
  sDialog->show();
}


QStringList Imp::DetectNames()
{
  QStringList result;
  for (auto detect : _detects)
    result.push_back(detect->UserName());
  return result;
}


ImpAbstractDetect* Imp::DetectAtId(int id)
{
  ImpAbstractDetect* result = nullptr;
  for (auto detect : _detects)
    if (id == detect->Id())
    {
      result = detect;
      break;
    }
  return result;
}


ImpAbstractDetect* Imp::DetectAtName(QString idName)
{
  ImpAbstractDetect* result = nullptr;
  for (auto detect : _detects)
    if (idName == detect->UserName())
    {
      result = detect;
      break;
    }
  return result;
}


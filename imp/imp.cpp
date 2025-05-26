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
#include <QApplication>
#include <QScreen>
#include <QWindow>

#include "indicator.h"
#include "versionNo.h"
#include "Detects/detectfactory.h"
#include "Logger/logger.h"
#include "impsettings.h"
#include "formulatree/formulafactory.h"
#include "workplacesmodel.h"
#include "settingseditordialog.h"
#include "WidgetUtil/DetectModelCommands.h"

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
const int SIZE_WINDOW_TITLE = 25;

const char* HELP_INFO = "build\\html\\index.html";
const QString KEY_DEF_OPTIONS = "DEFAULT_INDICATOR";
const QString NO_UUID = "";
const QString MESSAGE_TITLE = "ИМП";

// Конструктор главного окна
Imp::Imp(QWidget* parent)
    : QWidget(parent)
    , _pQuickUi(new QQuickWidget)
    , _indicateTimer(new QTimer(this))
{
    Logger::GetInstance(this); // Запуск журнала
    FormulaFactory::Instance(this);

    // Загрузка параметров из файла установок
    _useIndicators.clear(); // подготовка к загрузке множества индикаторов
    _indicators.clear();
    _flagRunIndicators = true;
    _uuid = NO_UUID;
    LoadSettingsGeneral();
    setMinimumSize(SIZE_WINDOW_WIDTH, SIZE_WINDOW_HEIGTH);

    // Присвоение имени окну
    this->setWindowTitle(QString("ИМП  v.%1.%2.%3").arg(DEF_VERSION_MAJOR).arg(DEF_VERSION_MINOR).arg(DEF_VERSION_PATCH));
    QUrl source("qrc:/imp.qml");
    _pQuickUi->setSource(source);

    QVBoxLayout* pvbx = new QVBoxLayout();

    pvbx->addWidget(_pQuickUi); // Вставляем QML виджет в лайоут окна
    pvbx->setMargin(0); // Толщина рамки
    setLayout(pvbx); //  Установка лайоута в окно

    // Установка указателей на объекты виджета QML
    _pitWin = _pQuickUi->rootObject();
    _ppbFind = _pQuickUi->rootObject()->findChild<QObject*>("pbFind");
    _ptextComment = _pQuickUi->rootObject()->findChild<QObject*>("textComment");
    _pbtFind = _pQuickUi->rootObject()->findChild<QObject*>("btFind");
    _pbtIndicator = _pQuickUi->rootObject()->findChild<QObject*>("btIndicator");

    // Отработка команды: Помощь/Справка
    connect(_pQuickUi->rootObject(), SIGNAL(sigClickedbtHelp()), this, SLOT(showHelp()));

    // Отработка команды: Новый индикатор
    connect(_pQuickUi->rootObject(), SIGNAL(sigNewIndicator(QString)), this, SLOT(createNewIndicator(QString)));

    // Отработка команды: Поиск датчиков
    connect(_pQuickUi->rootObject(), SIGNAL(sigFindDetect()), this, SLOT(findDetect()));

    //Отработка команды: Заполнить экран окнами индикаторов
    connect(_pQuickUi->rootObject(), SIGNAL(sigFillScreenWithIndicators()), this, SLOT(fillScreenWithIndicators()));

    //Отработка команды: Разместить отрытые окна по порядку
    connect(_pQuickUi->rootObject(), SIGNAL(sigComposeOpenWindowsInOrder()), this, SLOT(composeOpenWindowsInOrder()));

    // Загрузка рабочего места
    updateCbWorkPlaces();
    connect(_pQuickUi->rootObject(), SIGNAL(sigOpenWorkPlaces()), this, SLOT(openWorkPlacesEditor()));

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
      for (ImpAbstractDetect* d : _detects)
      {
          d->CreateSettingsController(_pitWin);
      }
      reWriteDetectsToTable();
      _pitWin->setProperty("iCommand", DETECTS_MODEL_CMDS::UPDATE_ACTIVE_STATE);
    }
    else
    {
      // Поиск датчиков при запуске программы после паузы, чтобы оформилось главное окно
      TimerBeforeFound->start();
    }
}


QString Imp::MessageTitle()
{
    return MESSAGE_TITLE;
}


// переопределение события изменения окна
void Imp::resizeEvent(QResizeEvent* event)
{
    // Масштабирование QML виджета под размер окна
    _pQuickUi->rootObject()->setProperty("scaleX", QVariant(this->width()));
    _pQuickUi->rootObject()->setProperty("scaleY", QVariant(this->height()));
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
  _uuid = settings->CurrentUuid();
  fillUseIndicators();
  return true;
}


void Imp::fillUseIndicators()
{
    QStringList lInd = ImpSettings::Instance(this)->Value(ImpKeys::INDICATORS).toStringList();
    _useIndicators.clear();
    for (QString num : lInd)
    {
      _useIndicators.insert(num.toInt());
    }
}


// Сохранение параметров установок в файл
void Imp::SaveSettingsGeneral()
{
  ImpSettings* settings = ImpSettings::Instance(this);
  QStringList indficators;
  for (int num = 0; num < MAX_INDICATOR; num++)
    if (_useIndicators.contains(num) == true)
      indficators << QString::number(num);
  settings->SetValue(ImpKeys::INDICATORS, indficators);
  settings->SaveWorkPlacesModel();
  QRect winGeometry = geometry();
  settings->SetValue(ImpKeys::WIN_X, winGeometry.x());
  settings->SetValue(ImpKeys::WIN_Y, winGeometry.y());
  settings->SetValue(ImpKeys::WIN_WIDTH, winGeometry.width());
  settings->SetValue(ImpKeys::WIN_HEIGHT, winGeometry.height());
}


void Imp::linkIni()
{
  ImpSettings* settings = ImpSettings::Instance(this);
  QObject* root = _pQuickUi->rootObject();
  QObject* impSettingsDialog = root->findChild<QObject*>("impSettingsDialog");

  QObject* cbModBusSearch = root->findChild<QObject*>("cbModBusSearch");
  cbModBusSearch->setProperty("checked", settings->Value(ImpKeys::EN_MODBUS_TCP).toBool());
  connect(impSettingsDialog, SIGNAL(sigFindModbusTCP(bool)), this, SLOT(setIniFindModbusTCP(bool)));

  QObject* cbSearch485 = root->findChild<QObject*>("cbSearch485");
  cbSearch485->setProperty("checked", settings->Value(ImpKeys::EN_RS_485).toBool());
  connect(impSettingsDialog, SIGNAL(sigFindModbus485(bool)), this, SLOT(setIniFindModbus485(bool)));

  QObject* cbSimRec = root->findChild<QObject*>("cbSimRec");
  cbSimRec->setProperty("checked", settings->Value(ImpKeys::RECORDING_IN_ALL_INDICATORS).toBool());
  connect(impSettingsDialog, SIGNAL(sigSimRec(bool)), this, SLOT(setRecordingInAllIndicators(bool)));

  QStringList sl = settings->Value(ImpKeys::LIST_MB_ADDR).toStringList();
  impSettingsDialog->setProperty("iCommand", DETECTS_MODEL_CMDS::CLEAR_LIST); // clear list of adresses
  for (QString adress : sl)
  {
    impSettingsDialog->setProperty("tcpAdress", adress);
    impSettingsDialog->setProperty("iCommand", DETECTS_MODEL_CMDS::ADD_RECORD); // Команда на добавление записи
  }
  connect(impSettingsDialog, SIGNAL(sigAdresses(QString)), this, SLOT(setModbusAdresses(QString)));
  QObject* modelUpdater = root->findChild<QObject*>("modelUpdater");
  modelUpdater->setProperty("running", true);
}


void Imp::setIniFindModbusTCP(bool en)
{
  ImpSettings* settings = ImpSettings::Instance(parent());
  settings->SetValue(ImpKeys::EN_MODBUS_TCP, en);
}


void Imp::setIniFindModbus485(bool en)
{
  ImpSettings* settings = ImpSettings::Instance(parent());
  settings->SetValue(ImpKeys::EN_RS_485, en);
}


void Imp::setRecordingInAllIndicators(bool en)
{
  ImpSettings* settings = ImpSettings::Instance(parent());
  settings->SetValue(ImpKeys::RECORDING_IN_ALL_INDICATORS, en);
}


void Imp::setModbusAdresses(QString adresses)
{
  ImpSettings* settings = ImpSettings::Instance(parent());
  QStringList sl = adresses.split("\n");
  settings->SetValue(ImpKeys::LIST_MB_ADDR, sl);
}


// Запуск поиска датчиков
void Imp::findDetect()
{
    QObject* findProgressBar = _pQuickUi->rootObject()->findChild<QObject*>("searchProgress");
    findProgressBar->setProperty("visible", true);

    QVariant enableNewIndiator;
    _timerUpdaterActiveStatus->stop();

    _pbtFind->setProperty("enabled", false); // Чтобы не было накладывающихся поисков
    if(_flagRunIndicators)
    {
        enableNewIndiator = _pbtIndicator->property("enabled");
        _pbtIndicator->setProperty("enabled", false); // при первом поиске датчиков индикаторы не создавать
    }

    _pitWin->setProperty("iCommand", DETECTS_MODEL_CMDS::CLEAR_LIST); // Команда очистки таблицы
    _ptextComment->setProperty("text", "Найдено датчиков: " + QString::number(0));

    FindDetects();

    _pbtFind->setProperty("enabled", true); // Разрешаем повторный поиск датчиков
    _ptextComment->setProperty("text", " Найдено датчиков: " + QString::number(_detects.size()));

    if(_flagRunIndicators)
        _pbtIndicator->setProperty("enabled", enableNewIndiator); // первый поиск датчиков завершен

    reWriteDetectsToTable();
    for (auto detect : _detects)
      connect(detect, &ImpAbstractDetect::DetectPropertyChanged, this, [&]()
      {
        reWriteDetectsToTable();
        emit sigFindDetect();
      });
    // !!!!!!!!!!!!!!!!!!!!!!
    runIndicators(); // !!!!! при запуске программы открытие старых индикаторов
    // !!!!!!!!!!!!!!!!!!!!!!
    emit sigFindDetect();
    _timerUpdaterActiveStatus->start();

    findProgressBar->setProperty("visible", false);
}


void Imp::runIndicators()
{
    if (_flagRunIndicators)
    {
        for (int j = 0; j < MAX_INDICATOR; j++)
            if (_useIndicators.contains(j) == true)
            {
              createIndicator(j, nullptr, false);
            }
        _flagRunIndicators = false; // старые индикаторы больше не запускать
    }
}


// Индикация процесса поиска датчиков
void Imp::indicateFindCOMDetect()
{
  _uiCounter = _uiCounter + INTERVAL_TIMER;
  _ptextComment->setProperty("text", "Поиск датчиков");
  _ppbFind->setProperty("value", (static_cast<float>(_uiCounter))/(_uiLength));
}


void Imp::reWriteDetectsToTable()
{
  _pitWin->setProperty("iCommand", DETECTS_MODEL_CMDS::CLEAR_LIST); // clear list of detects

  for (ImpAbstractDetect* detect : _detects)
  {
    // Передача описания найденного датчика в графический интерфейс
      detect->LoadDataToQmlWidget();
    _pitWin->setProperty("iCommand", DETECTS_MODEL_CMDS::ADD_RECORD); // Команда на добавление записи
  }
}


void Imp::changeActiveStatusToTable()
{
  for (ImpAbstractDetect* d : _detects)
  {
    if (d->ActiveStatusChanged())
    {
      _pitWin->setProperty("strSerialNumber", QString::number(d->Id()));
      _pitWin->setProperty("strActive", d->ActiveStateInfo());
      _pitWin->setProperty("iCommand", DETECTS_MODEL_CMDS::UPDATE_ACTIVE_STATE); // Команда на коррекцию записи
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
    {
        d->CreateSettingsController(_pitWin);
        _detects.push_back(d);
    }
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
        {
            newDetect->CreateSettingsController(_pitWin);
            _detects.push_back(newDetect);
        }
      }

      _indicateTimer->stop();
      _ptextComment->setProperty("text", "");
      _ppbFind->setProperty("value", 1);
    });

    setConnect = true;
  }


  if (dfactory->AvailablePorts())
  {
    // Индикация времени поиска USB датчиков
    _uiCounter = 0;
    _uiLength = dfactory->FindingTime();
    _ptextComment->setProperty("text", "Поиск датчиков");
    _ppbFind->setProperty("value", _uiCounter);
    _indicateTimer->start();

    dfactory->StartFindOfDetects();
  }
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

//Заполнение экрана индикаторами
void Imp::fillScreenWithIndicators()
{
  qApp->processEvents();
  QList<int> indexList;
  int index;

  for (index = 0; index < MAX_INDICATOR; index++)
  {
      auto result = std::find_if(_indicators.begin(), _indicators.end(), [=](Indicator* ind)
      {
          return (ind != nullptr) ? (ind->Id() == index) : true;
      });
      if (result == _indicators.end())
      {
          indexList.push_back(index);
      }
  }
  createScreenIndicators(indexList, nullptr);
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


//создание индикаторов для заполнения экрана(-ов)
void Imp::createScreenIndicators(QList<int> indexList, ImpAbstractDetect* baseDetect)
{
    composeOpenWindowsInOrder();

    Indicator* ind;

    int countScreen = QApplication::screens().count();

    std::map<int, QSize> _sizeWindowMap;
    QList<QScreen *> screen = QApplication::screens();

    std::vector<int> sizeWindowIndicator = ind->GetDefaultSize();
    int sizeWidthIndicator = sizeWindowIndicator.at(0);
    int sizeHeigthIndicator = sizeWindowIndicator.at(1);

    int numberInd = 0;

    for(int i = 0; i < countScreen; i++){
        _sizeWindowMap[i] = screen[i]->availableSize();

        int numberOfWindowsColumns = static_cast<int>((_sizeWindowMap[i].width())/sizeWidthIndicator);
        int numberOfWindowsRows = static_cast<int>((_sizeWindowMap[i].height())/sizeHeigthIndicator);

        int numberIndicators = numberOfWindowsRows * numberOfWindowsColumns + numberInd;

        int startSreenX = screen[i]->geometry().x();
        int startSreenY = screen[i]->geometry().y();

        int indicatorsCount = _indicators.size();
        for(int row = 0; row < numberOfWindowsRows; row++)
        {
            for(int column = 0; column < numberOfWindowsColumns; column++)
            {
                if ((column + row * numberOfWindowsColumns) < indicatorsCount)
                {
                    continue;
                }
                ind = new Indicator(this,
                                    ImpSettings::Instance()->CurrentUuid(),
                                    indexList[numberInd], // Номер индикатора
                                    baseDetect); // ссылка на датчик

                ind->setGeometry(startSreenX+sizeWidthIndicator*column,
                                 startSreenY+SIZE_WINDOW_TITLE+(row*sizeHeigthIndicator+SIZE_WINDOW_TITLE*row),
                                 0,
                                 0);

                _useIndicators.insert(indexList[numberInd]);
                _indicators.push_back(ind);

                if(numberInd < numberIndicators){
                    numberInd++;
                }
                // Закрытие индикаторов при завершении работы приложения
                connect(this, &Imp::sigCloseIndicatorWindows, ind, &Indicator::CloseMyIndicator);
            }
        }
    }
}


void Imp::createIndicator(int index, ImpAbstractDetect* baseDetect, bool defOption)
{
  qApp->processEvents();
    Indicator* ind;
    ind = new Indicator(this,
                        ImpSettings::Instance()->CurrentUuid(),
                        index, // Номер индикатора
                        baseDetect, // ссылка на датчик
                        defOption);
    _indicators.push_back(ind);
    connect(ind, &Indicator::sigDataPressed, this, [=]()
    {
      ImpSettings* settings = ImpSettings::Instance(this);
      bool translate = settings->GetWorkPlacesModel()->RecordingInAllIndicators(settings->Value(ImpKeys::ACTIVE_WORKPLACE).toInt());
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


//Размещение открытых индикаторов по экрану
void Imp::composeOpenWindowsInOrder()
{
    qApp->processEvents();

        int countScreen = QApplication::screens().count();

        std::map<int, QSize> _sizeScreenMap;
        QList<QScreen *> screen = QApplication::screens();

        Indicator* ind;
        //узнаем размеры окна индикатора по умолчанию
        std::vector<int> sizeWindowIndicator = ind->GetDefaultSize();
        int sizeWidthIndicator = sizeWindowIndicator.at(0);
        int sizeHeigthIndicator = sizeWindowIndicator.at(1);
        //узнаем количество открытых окон
        int countOpenIndicators = _indicators.size();
        int allWindows = 0;

        for(int i = 0; i < countScreen; i++)
        {
            //узнаем размеры экрана
            _sizeScreenMap[i] = screen[i]->availableSize();
            int numberOfWindowsColumns = static_cast<int>((_sizeScreenMap[i].width())/sizeWidthIndicator);
            int numberOfWindowsRows = static_cast<int>((_sizeScreenMap[i].height())/sizeHeigthIndicator);
            //сколько может быть всего окон на экранах
            allWindows = allWindows + numberOfWindowsRows * numberOfWindowsColumns;
        }

        if(countOpenIndicators <= allWindows)
        {
            int currentScreen = 0;
            int row = 0;
            int column = 0;
            bool recalculateWindowSize = true;
            //количество столбцов
            int numberOfWindowsColumns = static_cast<int>((_sizeScreenMap[currentScreen].width()) / sizeWidthIndicator);
            //количество строк
            int numberOfWindowsRows = static_cast<int>((_sizeScreenMap[currentScreen].height()) / sizeHeigthIndicator);
            int startSreenX;
            int startSreenY;
            int newStartColumn;
            int newStartRow;

            for (auto indicator : _indicators)
            {
                if (column > numberOfWindowsColumns - 1)
                {
                    if (numberOfWindowsRows - 1 > row)
                    {
                        row++;
                    }
                    else
                    {
                        currentScreen++;
                        row = 0;
                        recalculateWindowSize = true;
                    }
                    column = 0;
                }

                if (recalculateWindowSize)
                {
                    recalculateWindowSize = false;
                    numberOfWindowsColumns = static_cast<int>((_sizeScreenMap[currentScreen].width()) / sizeWidthIndicator);
                    numberOfWindowsRows = static_cast<int>((_sizeScreenMap[currentScreen].height()) / sizeHeigthIndicator);
                    //начало currentScreen-того экрана
                    startSreenX = screen[currentScreen]->geometry().x();
                    startSreenY = screen[currentScreen]->geometry().y();
                    newStartColumn = startSreenX;
                    newStartRow = startSreenY;
                }


                ind = indicator;

                startSreenX = newStartColumn + column * sizeWidthIndicator;
                startSreenY = newStartRow + row * sizeHeigthIndicator + SIZE_WINDOW_TITLE * row;
                // отрисовка start
                ind->setGeometry(startSreenX,
                                 startSreenY + SIZE_WINDOW_TITLE,
                                 sizeWidthIndicator,
                                 sizeHeigthIndicator);
                column++;
                // отрисовка finish
            }
        }
        else
        {
            QMessageBox::information(this, MESSAGE_TITLE,"Нет возможности распределить все открытые окна.");
        }
}

void Imp::showHelp()
{
  std::stringstream stream;
  stream << HELP_INFO;
  system(stream.str().c_str());
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
  if (result == nullptr
      && idName != Indicator::EmptyDetect())
  {
      QMessageBox::information
              (
                  this
                  , MESSAGE_TITLE
                  , "Не обнаружен датчик " + idName
              );
  }
  return result;
}


void Imp::openWorkPlacesEditor()
{
    SettingsEditorDialog* editor = new SettingsEditorDialog(this);
    connect(editor, &QWidget::destroyed, this, &Imp::updateCbWorkPlaces);
    editor->SetModel(ImpSettings::Instance()->GetWorkPlacesModel());
    editor->show();
}


void Imp::updateCbWorkPlaces()
{
    QObject* root = _pQuickUi->rootObject();
    if (root != nullptr)
    {
        disconnect(root, SIGNAL(sigWorkPlaceChanged()), this, SLOT(workPlaceChanged()));
    }
    QObject* cbWorkPlaces = _pQuickUi->rootObject()->findChild<QObject*>("cbWorkspace");
    ImpSettings* settings = ImpSettings::Instance(this);
    int modelCount = settings->GetWorkPlacesModel()->WorkPlacesNames().length();
    cbWorkPlaces->setProperty("model", settings->GetWorkPlacesModel()->WorkPlacesNames());
    int activeRow = settings->Value(ImpKeys::ACTIVE_WORKPLACE).toInt();
    if (activeRow >= modelCount)
    {
        activeRow = modelCount - 1;
        settings->SetValue(ImpKeys::ACTIVE_WORKPLACE, activeRow);
    }
    cbWorkPlaces->setProperty("currentIndex", activeRow);
    linkIni();
    connect(_pQuickUi->rootObject(), SIGNAL(sigWorkPlaceChanged()), this, SLOT(workPlaceChanged()));
}


void Imp::workPlaceChanged()
{
    QObject* cbWorkPlaces = _pQuickUi->rootObject()->findChild<QObject*>("cbWorkspace");
    ImpSettings* settings = ImpSettings::Instance(this);
    settings->SetValue(ImpKeys::ACTIVE_WORKPLACE, cbWorkPlaces->property("currentIndex").toInt());
    linkIni();
    if (_uuid == settings->CurrentUuid())
    {
        return;
    }
    _uuid = settings->CurrentUuid();
    emit sigCloseIndicatorWindows();
    _indicators.clear();
    fillUseIndicators();
    _flagRunIndicators = true;
    runIndicators();
}

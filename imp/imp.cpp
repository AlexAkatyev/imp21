/*
 *
 * Описание функций главного окна
 *
 * */
#include "imp.h"

#include <QQuickItem>

#include <QTimer>
#include <QFile>
#include <QMessageBox>
#include <QtXml>
#include <QTime>
#include <QHash>
#include <QQuickWidget>
#include <QVBoxLayout>

#include "about/about.h"
#include "HelpBrowser/HelpBrowser.h"
#include "indicator.h"
//#include "command_detect.h"
#include "versionNo.h"
#include "Detects/detectfactory.h"
#include "Logger/logger.h"

// Пауза после запуска программы перед поиском датчиков
const int PAUSE_BEFORE_FIND_DETECT = 500;

// Параметры индикации поиска датчиков
const int INTERVAL_TIMER = 100;

// Максимальное количество индикаторов
const int MAX_INDICATOR = 256;

// Версия программы
const int VERSION_MAJOR = DEF_VERSION_MAJOR;
const int VERSION_MINOR = DEF_VERSION_MINOR;
const int VERSION_PATCH = DEF_VERSION_PATCH;

// Исходные размеры окна
const int SIZE_WINDOW_WIDTH = 320;
const int SIZE_WINDOW_HEIGTH = 480;

// Конструктор главного окна
Imp::Imp(QWidget* parent)
    : QWidget(parent)
    , pQuickUi(new QQuickWidget)
{
    Logger::GetInstance(this); // Запуск журнала

    fileSettingsGeneral = new QFile("imp.xml"); // Указание файла с параметрами установок

    // Загрузка параметров из файла установок
    _useIndicators.clear(); // подготовка к загрузке множества индикаторов
    _flagRunIndicators = true;
    if (LoadSettingsGeneral(fileSettingsGeneral))
    { /* Файл без ошибки загружен */
        QDomElement domElement = ddSettingsGeneral.documentElement();
        traverseNodeGeneral(&domElement);
    }
    else /* Файла нет или загружен с ошибкой */
    {
        // Размещение окна в центр экрана
        QPoint center = QDesktopWidget().availableGeometry().center(); //получаем координаты центра экрана
        center.setX(center.x() - (SIZE_WINDOW_WIDTH/2));
        center.setY(center.y() - (SIZE_WINDOW_HEIGTH/2));
        resize(SIZE_WINDOW_WIDTH, SIZE_WINDOW_HEIGTH);
        move(center);
    }

    // Присвоение имени окну
    this->setWindowTitle("ИМП 21");
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

    // Поиск датчиков при запуске программы после паузы, чтобы оформилось главное окно
    TimerBeforeFound->start();
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
    SaveSettingsGeneral(fileSettingsGeneral);
    emit sigCloseSettingsWindows();
    emit sigCloseIndicatorWindows();
    emit sigCloseGeneralWindow(); // Сигнализируем, что главное окно закрывается
    for (auto detect : _detects)
      detect->Stop();
    e->accept();
}

// Загрузка параметров из файла установок
bool Imp::LoadSettingsGeneral(QFile* file)
{
    QString errorString;
    int errorLine = 0;
    int errorColumn = 0;
    ddSettingsGeneral = QDomDocument("ini");
    if (!file->open(QIODevice::ReadOnly))
        return false;
    bool iniError = !ddSettingsGeneral.setContent(file, true, &errorString, &errorLine, &errorColumn);
    file->close();
    if (iniError)
        return false; // ошибка в файле инициализации

    QDomElement root = ddSettingsGeneral.documentElement();

    if (root.tagName() != "imp-ini")
        return false;

    return true;
}


// Использование установок в главном окне
void Imp::traverseNodeGeneral(QDomNode* node, int acceptFlag)
{
   QDomNode domNode = node->firstChild();

   while(!domNode.isNull())
   {
       if(domNode.isElement())
       {
           QDomElement domElement = domNode.toElement();
           if(!domElement.isNull()) {
               if(domElement.tagName() == "window" &&
                  (acceptFlag & 0x1))
               {
                   QDomElement sizeElement =  domElement.firstChildElement("size");
                   QDomElement sizeWidthElement =  sizeElement.firstChildElement("width");
                   QDomElement sizeHeightElement =  sizeElement.firstChildElement("height");
                   QDomElement topLeftPointElement =  domElement.firstChildElement("topLeftPoint");
                   QDomElement topLeftPointXElement =  topLeftPointElement.firstChildElement("x");
                   QDomElement topLeftPointYElement =  topLeftPointElement.firstChildElement("y");
                   // Устанавливаем размер и положение окна программы
                   QRect windowGeometry = QRect(topLeftPointXElement.text().toInt(),
                                                topLeftPointYElement.text().toInt(),
                                                sizeWidthElement.text().toInt(),
                                                sizeHeightElement.text().toInt());
                   this->setGeometry(windowGeometry);
               }

               if(domElement.tagName() == "i" &&
                  (acceptFlag & 0x4))
               {
                   _useIndicators.insert(domElement.text().toInt());
               }
           }
       }
       traverseNodeGeneral(&domNode, acceptFlag);
       domNode = domNode.nextSibling();
   }
}


// Сохранение параметров установок в файл
void Imp::SaveSettingsGeneral(QFile* file)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("imp-ini");
    doc.appendChild(root);

    // Сохранение позиции окна
    QDomElement window = doc.createElement("window");
    QDomElement topLeftPoint = doc.createElement("topLeftPoint");
    QDomElement topLeftPointX = doc.createElement("x");
    QDomElement topLeftPointY = doc.createElement("y");
    QDomText topLeftPointXText = doc.createTextNode(QString().setNum(geometry().x()));
    QDomText topLeftPointYText = doc.createTextNode(QString().setNum(geometry().y()));
    QDomElement size = doc.createElement("size");
    QDomElement width = doc.createElement("width");
    QDomText widthText = doc.createTextNode(QString().setNum(geometry().width()));
    QDomElement height = doc.createElement("height");
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


    // Сохраненеие языка пользователя
    QDomElement mylanguage = doc.createElement("language");

    // Сохранение списка открытых индикаторов
    QDomElement deIndicators = doc.createElement("indicators");
    root.appendChild(deIndicators);
    for (int num = 0; num < MAX_INDICATOR; num++)
    {
        if (_useIndicators.contains(num) == true)
        {
            QDomElement deIndicator = doc.createElement("i");
            deIndicators.appendChild(deIndicator);
            QDomText dtIndicator = doc.createTextNode(QString::number(num));
            deIndicator.appendChild(dtIndicator);
        }
    }

    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(file);
    doc.save(out, 4);
    file->close();

}


// Запуск поиска датчиков
void Imp::findDetect()
{
    QVariant enableNewIndiator;

    pbtFind->setProperty("enabled", false); // Чтобы не было накладывающихся поисков
    if(_flagRunIndicators)
    {
        enableNewIndiator = pbtIndicator->property("enabled");
        pbtIndicator->setProperty("enabled", false); // при первом поиске датчиков индикаторы не создавать
    }

    pitWin->setProperty("iCommand", 1); // Команда очистки таблицы
    ptextComment->setProperty("text", "Найдено датчиков: " + QString::number(0));

    FindCOMDetect();

    pbtFind->setProperty("enabled", true); // Разрешаем повторный поиск датчиков
    ptextComment->setProperty("text", " Найдено датчиков: " + QString::number(_detects.size()));

    if(_flagRunIndicators)
        pbtIndicator->setProperty("enabled", enableNewIndiator); // первый поиск датчиков завершен

    reWriteDetectsToTable();
    for (auto detect : _detects)
      connect(detect, &VTDetect::UserNameChanged, this, [&]()
      {
        reWriteDetectsToTable();
        emit sigFindDetect();
      });

    if(_flagRunIndicators)
    { // при запуске программы открытие старых индикаторов
        for (int j = 0; j < MAX_INDICATOR; j++)
            if (_useIndicators.contains(j) == true)
                createIndicator(j);
        _flagRunIndicators = false; // старые индикаторы больше не запускать
    }
    emit sigFindDetect();
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

  for (VTDetect* detect : _detects)
  {
    // Передача описания найденного датчика в графический интерфейс
    pitWin->setProperty("strSerialNumber", QString::number(detect->Id()));
    pitWin->setProperty("strNameDetect", detect->UserName());
    pitWin->setProperty("strTypeDetect", detect->TypeDetect());
    pitWin->setProperty("strDataManuf", detect->DateManuf().toString("dd.MM.yyyy"));
    pitWin->setProperty("strPort", detect->PortName());
    pitWin->setProperty("strModbusAddress", detect->Address());
    pitWin->setProperty("iCommand", 2); // Команда на добавление записи
  }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Поиск датчиков на COM-портах
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Imp::FindCOMDetect(void)
{
  // удалить из списка исчезнувшие датчики
  std::vector<VTDetect*> det;
  for (VTDetect* d: _detects)
    det.push_back(d);
  _detects.clear();
  for (VTDetect* d: det)
    if (d->Ready())
      _detects.push_back(d);
    else
      d->deleteLater();

  // искать новые датчики
  DetectFactory* dfactory = new DetectFactory(this);
  if (dfactory->AvailablePorts())
  {
    // Индикация времени поиска USB датчиков
    QTimer* indicateTimer = new QTimer(this);
    indicateTimer->setInterval(INTERVAL_TIMER);
    _uiCounter = 0;
    _uiLength = dfactory->FindingTime();
    connect(indicateTimer, &QTimer::timeout, this, &Imp::indicateFindCOMDetect);
    ptextComment->setProperty("text", "Поиск датчиков");
    ppbFind->setProperty("value", _uiCounter);
    indicateTimer->start();

    for (VTDetect* d : dfactory->VTDetects())
      _detects.push_back(d);

    indicateTimer->stop();
    indicateTimer->deleteLater();
    ptextComment->setProperty("text", "");
    ppbFind->setProperty("value", 1);
  }
  dfactory->deleteLater();
}


// Создание класса для работы с установками датчика
void Imp::createNewSettings(QString idDetect)//,
{
  VTDetect* detect = DetectAtId(idDetect.toInt());
  if (detect)
    detect->ShowSettings();
}


// Создание нового индикатора
void Imp::createNewIndicator(QString strNDS)
{
  int id = strNDS.toInt();
  VTDetect* baseDetect = nullptr;
  if (id)
    for (auto detect : _detects)
      if (id == detect->Id())
      {
        baseDetect = detect;
        break;
      }

    if (!_flagRunIndicators) // Первый поиск датчиков и запуск старых индикаторов завершен
    {
        int j;
        // Поиск свободного идентификатора
        for (j = 0; j < MAX_INDICATOR; j++)
            if (_useIndicators.contains(j) == false)
                break;
        _useIndicators.insert(j);
        createIndicator(j, baseDetect);
    }
}


// Удаление индикатора из множества
void Imp::deleteIndicator(int idInd)
{
    _useIndicators.erase(_useIndicators.find(idInd));
}


void Imp::createIndicator(int index, VTDetect* baseDetect)
{
    Indicator* ind;
    ind = new Indicator(this,
                        index, // Номер индикатора
                        baseDetect); // ссылка на датчик
    // Закрытие индикатора при закрытии главного окна
    connect(this, &Imp::sigCloseIndicatorWindows, ind, &Indicator::CloseMyIndicator);
}


void Imp::showHelp()
{
  HelpBrowser* helpWindow = new HelpBrowser(this, ":/", "index.htm");
  helpWindow->show();
}


void Imp::showAbout()
{
  AboutDialog* aboutWindow = new AboutDialog(this, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  aboutWindow->show();
}


QStringList Imp::DetectNames()
{
  QStringList result;
  for (auto detect : _detects)
    result.push_back(detect->UserName());
  return result;
}


VTDetect* Imp::DetectAtId(int id)
{
  VTDetect* result = nullptr;
  for (auto detect : _detects)
    if (id == detect->Id())
    {
      result = detect;
      break;
    }
  return result;
}


VTDetect* Imp::DetectAtName(QString idName)
{
  VTDetect* result = nullptr;
  for (auto detect : _detects)
    if (idName == detect->UserName())
    {
      result = detect;
      break;
    }
  return result;
}


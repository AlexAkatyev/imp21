#include <cmath>
#include <QQuickWidget>
#include <QVBoxLayout>
#include <QQuickItem>
#include <QTextCodec>
#include <QDesktopWidget>

#include "emvtsettings.h"
#include "impabstractdetect.h"

// Исходные размеры окна установок
const int SIZE_SETTINGS_WINDOW_X = 400;
const int SIZE_SETTINGS_WINDOW_Y = 450;

//Кодировщик шрифта - код для записи строк в датчик
#define CODE_LOCALLY "Windows-1251"


EmVTSettings::EmVTSettings(ImpAbstractDetect* parent, int imageCode)
  : CenterDialog(nullptr)
  , _quickUi(new QQuickWidget)
  , _imageCode(imageCode)
  , _detect(parent)
{
  setModal(true);
  connect(_detect, &ImpAbstractDetect::Stopped, this, &EmVTSettings::deleteLater);

  _codec = QTextCodec::codecForName(CODE_LOCALLY);

  setSize(SIZE_SETTINGS_WINDOW_X, SIZE_SETTINGS_WINDOW_Y);

  // Присвоение имени окну
  this->setWindowTitle("Датчик " + _detect->UserName() + ",  № " + QString::number(_detect->Id()));
  QUrl source("qrc:/emvtsettings.qml");
  _quickUi->setSource(source);

  QVBoxLayout* pvbx = new QVBoxLayout();

  pvbx->addWidget(_quickUi); // Вставляем QML виджет в лайоут окна
  pvbx->setMargin(0); // Толщина рамки
  setLayout(pvbx); //  Установка лайоута в окно

  // Вывод на экран имеющейся информации
  fillQmlWidget();
}

// Заполнение пользовательского окна "Установки" данными датчика
void EmVTSettings::fillQmlWidget()
{
  // Вывод на экран имеющейся информации
  _quickUi->rootObject()->findChild<QObject*>("txtSerialNum")->setProperty("text", _detect->Id());
  _quickUi->rootObject()->findChild<QObject*>("txtType")->setProperty("text", _detect->TypeDetect());
  _quickUi->rootObject()->findChild<QObject*>("txtConnect")->setProperty("text", _detect->PortName());

  _quickUi->rootObject()->findChild<QObject*>("txtData")->setProperty("text", _detect->DateManuf().toString("dd.MM.yyyy"));
  // Определение диапазона измерения
  int lrange = _detect->LMeasureInterval();
  int hrange = _detect->HMeasureInterval();
  if (hrange < lrange)
  {
    int temp = hrange;
    hrange = lrange;
    lrange = temp;
  }
  _quickUi->rootObject()->findChild<QObject*>("tfRange")->setProperty(
        "text", QString::number(lrange) + " ... " + QString::number(hrange));
  // Определение диапазона обнуления
  _quickUi->rootObject()->findChild<QObject*>("tfSetZero")->setProperty(
        "text", "±" + QString::number(_detect->ZeroInterval()));
  // Определение диапазона предустанова
  _quickUi->rootObject()->findChild<QObject*>("tfPreSet")->setProperty(
        "text", "±" + QString::number(_detect->PreSetInterval()));
  // Определение единиц измерения
  _quickUi->rootObject()->findChild<QObject*>("tfUM")->setProperty("text", _detect->MeasUnit());

  // logo
  if (_imageCode == 1)
    _quickUi->rootObject()->findChild<QObject*>("imLogo")->setProperty("source", "qrc:/logoNIR.bmp");
}


// переопределение события изменения окна
void EmVTSettings::resizeEvent(QResizeEvent* event)
{
  // Масштабирование QML виджета под размер окна
  QSize baseSize(static_cast<int>(_quickUi->rootObject()->width()), static_cast<int>(_quickUi->rootObject()->height()));
  _quickUi->rootObject()->setProperty("scaleX", QVariant(this->width()));
  _quickUi->rootObject()->setProperty("scaleY", QVariant(this->height()));
  event->accept();
}


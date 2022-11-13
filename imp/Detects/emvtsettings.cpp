#include <cmath>
#include <QQuickWidget>
#include <QVBoxLayout>
#include <QQuickItem>
#include <QTextCodec>
#include <QDesktopWidget>

#include "emvtsettings.h"
#include "emvtdetect.h"

// Исходные размеры окна установок
const int SIZE_SETTINGS_WINDOW_X = 320;
const int SIZE_SETTINGS_WINDOW_Y = 480;

//Кодировщик шрифта - код для записи строк в датчик
#define CODE_LOCALLY "Windows-1251"


EmVTSettings::EmVTSettings(EmVTDetect* parent)
  : QWidget(nullptr)
  , _quickUi(new QQuickWidget)
  , _detect(parent)
{
  connect(_detect, &EmVTDetect::Stopped, this, &EmVTSettings::deleteLater);

  _codec = QTextCodec::codecForName(CODE_LOCALLY);

  // Размещение окна установок в центр экрана
  QPoint center = QDesktopWidget().availableGeometry().center(); //получаем координаты центра экрана
  center.setX(center.x() - (SIZE_SETTINGS_WINDOW_X/2));
  center.setY(center.y() - (SIZE_SETTINGS_WINDOW_Y/2));
  resize(SIZE_SETTINGS_WINDOW_X, SIZE_SETTINGS_WINDOW_Y);
  move(center);

  // Присвоение имени окну
  this->setWindowTitle(tr("Датчик ") + _detect->UserName());
  QUrl source("qrc:emvtsettings.qml");
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


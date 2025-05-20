#include <cmath>
#include <QQuickWidget>
#include <QTextCodec>

#include "emvtsettings.h"
#include "impabstractdetect.h"

//Кодировщик шрифта - код для записи строк в датчик
#define CODE_LOCALLY "Windows-1251"


EmVTSettings::EmVTSettings(ImpAbstractDetect* parent, QObject* rootUi, int imageCode)
    : AbstractSettingsController(parent, rootUi)
    , _imageCode(imageCode)
{
  connect(_detect, &ImpAbstractDetect::Stopped, this, &EmVTSettings::deleteLater);

  _codec = QTextCodec::codecForName(CODE_LOCALLY);
}

// Заполнение пользовательского окна "Установки" данными датчика
void EmVTSettings::FillQmlWidget()
{
  // Вывод на экран имеющейся информации
    AbstractSettingsController::FillQmlWidget();
    _rootUi->setProperty("strTypeSettings", "em");

//  _rootUi->findChild<QObject*>("txtSerialNum")->setProperty("text", _detect->Id());
//  _rootUi->findChild<QObject*>("txtType")->setProperty("text", _detect->TypeDetect());
//  _rootUi->findChild<QObject*>("txtConnect")->setProperty("text", _detect->PortName());

//  _rootUi->findChild<QObject*>("txtData")->setProperty("text", _detect->DateManuf().toString("dd.MM.yyyy"));
//  // Определение диапазона измерения
//  int lrange = _detect->LMeasureInterval();
//  int hrange = _detect->HMeasureInterval();
//  if (hrange < lrange)
//  {
//    int temp = hrange;
//    hrange = lrange;
//    lrange = temp;
//  }
//  _rootUi->findChild<QObject*>("tfRange")->setProperty(
//        "text", QString::number(lrange) + " ... " + QString::number(hrange));
//  // Определение диапазона обнуления
//  _rootUi->findChild<QObject*>("tfSetZero")->setProperty(
//        "text", "±" + QString::number(_detect->ZeroInterval()));
//  // Определение диапазона предустанова
//  _rootUi->findChild<QObject*>("tfPreSet")->setProperty(
//        "text", "±" + QString::number(_detect->PreSetInterval()));
//  // Определение единиц измерения
//  _rootUi->findChild<QObject*>("tfUM")->setProperty("text", _detect->MeasUnit());

//  // logo
//  if (_imageCode == 1)
//    _rootUi->findChild<QObject*>("imLogo")->setProperty("source", "qrc:/logoNIR.bmp");
}

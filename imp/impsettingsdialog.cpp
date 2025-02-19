#include <QQuickWidget>
#include <QVBoxLayout>
#include <QRect>
#include <QQuickItem>

#include "impsettingsdialog.h"
#include "WidgetUtil/centerdialog.h"
#include "../impsettings.h"


ImpSettingsDialog::ImpSettingsDialog(QWidget* parent)
  : CenterDialog(parent)
{
  QQuickWidget* pQuickUi = new QQuickWidget(this);

  QUrl source("qrc:/settings.qml");
  pQuickUi->setSource(source);

  QVBoxLayout* pvbx = new QVBoxLayout();

  pvbx->addWidget(pQuickUi); // Вставляем QML виджет в лайоут окна
  pvbx->setMargin(0); // Толщина рамки
  setLayout(pvbx); //  Установка лайоута в окно
  setSize(pQuickUi->property("width").toInt(), pQuickUi->property("height").toInt());

  linkIni(pQuickUi);
}


void ImpSettingsDialog::linkIni(QQuickWidget* ui)
{
  ImpSettings* settings = ImpSettings::Instance(parent());
  QQuickItem* root = ui->rootObject();

  QObject* cbModBusSearch = root->findChild<QObject*>("cbModBusSearch");
  cbModBusSearch->setProperty("checked", settings->Value(ImpKeys::EN_MODBUS_TCP).toBool());
  connect(root, SIGNAL(sigFindModbusTCP(bool)), this, SLOT(setIniFindModbusTCP(bool)));

  QObject* cbSearch485 = root->findChild<QObject*>("cbSearch485");
  cbSearch485->setProperty("checked", settings->Value(ImpKeys::EN_RS_485).toBool());
  connect(root, SIGNAL(sigFindModbus485(bool)), this, SLOT(setIniFindModbus485(bool)));

  QObject* cbSimRec = root->findChild<QObject*>("cbSimRec");
  cbSimRec->setProperty("checked", settings->Value(ImpKeys::RECORDING_IN_ALL_INDICATORS).toBool());
  connect(root, SIGNAL(sigSimRec(bool)), this, SLOT(setRecordingInAllIndicators(bool)));

  QStringList sl = settings->Value(ImpKeys::LIST_MB_ADDR).toStringList();
  root->setProperty("iCommand", 1); // clear list of adresses
  for (QString adress : sl)
  {
    root->setProperty("tcpAdress", adress);
    root->setProperty("iCommand", 2); // Команда на добавление записи
  }
}


void ImpSettingsDialog::setIniFindModbusTCP(bool en)
{
  ImpSettings* settings = ImpSettings::Instance(parent());
  settings->SetValue(ImpKeys::EN_MODBUS_TCP, en);
}


void ImpSettingsDialog::setIniFindModbus485(bool en)
{
  ImpSettings* settings = ImpSettings::Instance(parent());
  settings->SetValue(ImpKeys::EN_RS_485, en);
}


void ImpSettingsDialog::setRecordingInAllIndicators(bool en)
{
  ImpSettings* settings = ImpSettings::Instance(parent());
  settings->SetValue(ImpKeys::RECORDING_IN_ALL_INDICATORS, en);
}

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
  ImpSettings * settings = ImpSettings::Instance(parent());
  //QObject* cbWireSearch = ui->rootObject()->findChild<QObject*>("cbWireSearch");
  //cbWireSearch->setProperty("checked", !settings->Value(ImpKeys::EN_MODBUS_TCP).toBool());

  QObject* cbModBusSearch = ui->rootObject()->findChild<QObject*>("cbModBusSearch");
  cbModBusSearch->setProperty("checked", settings->Value(ImpKeys::EN_MODBUS_TCP).toBool());
  //connect(ui->rootObject(), SIGNAL(sigClickedbtAbout()), this, SLOT(showAbout()));
}

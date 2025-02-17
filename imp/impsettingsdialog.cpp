#include <QQuickWidget>
#include <QVBoxLayout>
#include <QRect>

#include "impsettingsdialog.h"
#include "WidgetUtil/centerdialog.h"


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
}

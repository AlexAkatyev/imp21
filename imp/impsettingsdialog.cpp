#include <QQuickWidget>
#include <QVBoxLayout>

#include "impsettingsdialog.h"


ImpSettingsDialog::ImpSettingsDialog(QWidget* parent)
  : QDialog(parent)
{
  QQuickWidget* pQuickUi = new QQuickWidget(this);

  QUrl source("qrc:/settings.qml");
  pQuickUi->setSource(source);

  QVBoxLayout* pvbx = new QVBoxLayout();

  pvbx->addWidget(pQuickUi); // Вставляем QML виджет в лайоут окна
  pvbx->setMargin(0); // Толщина рамки
  setLayout(pvbx); //  Установка лайоута в окно
  setGeometry(100, 100, 600, 600);
}

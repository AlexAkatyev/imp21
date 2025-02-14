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
  int height = pQuickUi->property("height").toInt();
  int width = pQuickUi->property("width").toInt();
  int ax = parent->x() + (parent->width() - width) / 2;
  int ay = parent->y() + (parent->height() - height) / 2;
  setGeometry(ax, ay, width, height);
  setMinimumSize(width, height);
  setMaximumSize(width, height);
}

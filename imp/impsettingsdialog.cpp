#include <QQuickWidget>
#include <QVBoxLayout>
#include <QRect>

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
  _height = pQuickUi->property("height").toInt();
  _width = pQuickUi->property("width").toInt();
  setMinimumSize(_width, _height);
  setMaximumSize(_width, _height);
}


void ImpSettingsDialog::UpdatePosition(QRect parentGeometry)
{
  int ax = parentGeometry.x() + (parentGeometry.width() - _width) / 2;
  int ay = parentGeometry.y() + (parentGeometry.height() - _height) / 2;
  setGeometry(ax, ay, _width, _height);
}

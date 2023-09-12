
/*
 * about.cpp
 *
 *  Created on: 01.03.2017
 *      Author: Aleksey Akatyev
 *
 * Индицирует информацию об изготовителе
 *
 */


#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QListWidget>

#include "about.h"
#include "../impsettings.h"

AboutDialog::AboutDialog(QWidget* parent, int V_MAJOR, int V_MINOR, int V_PATCH, ImpSettings* settings)
  : QDialog(parent)
  , _settings(settings)
{
  QLabel* about = new QLabel("Измеритель микроперемещений.");
  QLabel* copyright = new QLabel(QString("2023 ООО ИМЦ Микро"));
  QLabel* developed = new QLabel(QString("Разработано:\n    ООО Новые инжененрные решения\n     ies-ltd.com"));
  QLabel* support = new QLabel("Техническая поддержка: imcmikro@mail.ru");
  QLabel* webAdress = new QLabel("www.imcmikro.ru");
  QLabel* thanks = new QLabel(QString(""));
  QLabel* version = new QLabel(QString("Версия %1.%2.%3").arg(V_MAJOR).arg(V_MINOR).arg(V_PATCH));
  QVBoxLayout* aboutLayout = new QVBoxLayout;
  aboutLayout->setSizeConstraint(QLayout::SetFixedSize);
  aboutLayout->addWidget(version);
  aboutLayout->addWidget(about);
  aboutLayout->addWidget(copyright);
  aboutLayout->addWidget(developed);
  aboutLayout->addWidget(thanks);
  aboutLayout->addWidget(support);
  aboutLayout->addWidget(webAdress);
  QWidget* aboutWidget = new QWidget;
  aboutWidget->setLayout(aboutLayout);


  QCheckBox* cbDetRS485 = new QCheckBox("Искать датчики по интерфейсу RS-485");
  cbDetRS485->setCheckState(_settings->Value(ImpKeys::EN_RS_485).toBool() ? Qt::Checked : Qt::Unchecked);
  connect(cbDetRS485, &QCheckBox::stateChanged, this, [=]()
  {
    _settings->SetValue(ImpKeys::EN_RS_485, cbDetRS485->checkState() == Qt::Checked);
  });
  QCheckBox* cbDetRadioChannel = new QCheckBox("Искать датчики через сервер Modbus TCP");
  cbDetRadioChannel->setCheckState(_settings->Value(ImpKeys::EN_MODBUS_TCP).toBool() ? Qt::Checked : Qt::Unchecked);
  connect(cbDetRadioChannel, &QCheckBox::stateChanged, this, [=]()
  {
    _settings->SetValue(ImpKeys::EN_MODBUS_TCP, cbDetRadioChannel->checkState() == Qt::Checked);
  });
  QListWidget* lwAddr = new QListWidget;
  lwAddr->addItems(_settings->Value(ImpKeys::LIST_MB_ADDR).toStringList());
  QVBoxLayout* settingsLayout = new QVBoxLayout;
  settingsLayout->setSizeConstraint(QLayout::SetFixedSize);
  settingsLayout->addWidget(cbDetRS485);
  settingsLayout->addWidget(cbDetRadioChannel);
  settingsLayout->addWidget(lwAddr);
  QWidget* settingsWidget = new QWidget;
  settingsWidget->setLayout(settingsLayout);


  QTabWidget* tab = new QTabWidget;
  tab->addTab(aboutWidget, "Тех. поддержка");
  tab->addTab(settingsWidget, "Настройки");

  QVBoxLayout* generalLayout = new QVBoxLayout;
  generalLayout->addWidget(tab);
  setLayout(generalLayout);

  setWindowTitle("О программе");
}

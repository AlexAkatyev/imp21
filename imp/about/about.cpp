
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
#include <QHBoxLayout>
#include <QCheckBox>
#include <QListWidget>
#include <QPushButton>

#include "about.h"
#include "../impsettings.h"
#include "../impdef.h"

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

  QPushButton* btnAdd = new QPushButton("Добавить адрес");
  QPushButton* btnRemove = new QPushButton("Удалить адрес");
  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(btnAdd);
  buttonLayout->addWidget(btnRemove);

  QListWidget* lwAddr = new QListWidget;
  lwAddr->addItems(_settings->Value(ImpKeys::LIST_MB_ADDR).toStringList());
  connect(lwAddr, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item)
  {
    item->setFlags(Qt::ItemIsEditable  | Qt::ItemIsEnabled);
  });
  connect(lwAddr, &QListWidget::itemChanged, this, [=]()
  {
    saveAdresses(lwAddr);
  });
  connect(btnAdd, &QAbstractButton::pressed, this, [=]()
  {
    lwAddr->addItem(DEF_MB_SERVER);
  });
  connect(btnRemove, &QAbstractButton::pressed, this, [=]()
  {
    lwAddr->takeItem(lwAddr->currentRow());
    saveAdresses(lwAddr);
  });

  QVBoxLayout* settingsLayout = new QVBoxLayout;
  settingsLayout->setSizeConstraint(QLayout::SetFixedSize);
  settingsLayout->addWidget(cbDetRS485);
  settingsLayout->addWidget(cbDetRadioChannel);
  settingsLayout->addLayout(buttonLayout);
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


void AboutDialog::saveAdresses(QListWidget* lw)
{
  QStringList sl;
  for (int i = 0; i < lw->count(); ++i)
    sl << lw->item(i)->data(Qt::DisplayRole).toString();
  _settings->SetValue(ImpKeys::LIST_MB_ADDR, sl);
}


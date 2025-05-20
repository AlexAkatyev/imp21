#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

#include "abstractsettingscontroller.h"

class VT21Detect;

class BepVTSettings : public AbstractSettingsController
{
    Q_OBJECT
public:
    explicit BepVTSettings(VT21Detect* parent, QObject* rootUi);
    void FillQmlWidget() override;

private slots:
  void setNewUserName(QString number, QString userName);

private:
    // Включение переменных виджета QML
    // Указатели на объекты QML виджета
    // Окно установок +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _tModbusAddress;
    QObject* _tTxtModbusAddress;
    // Окно калибровки ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _txtCurrent;

    // Кодировщик для датчика
    QTextCodec* _codec;

    void indicateDataMeas(long dataMeas);
};

#endif // SETTINGS_H

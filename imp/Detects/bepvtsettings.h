#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

#include "WidgetUtil/centerdialog.h"

class QQuickWidget;
class VT21Detect;


class BepVTSettings
    : public CenterDialog
{
    Q_OBJECT
public:
    explicit BepVTSettings(VT21Detect* parent);

private slots:
  void setNewUserName();

private:
    // Включение переменных виджета QML
    // Указатель на виджет qml в окне класса
    QQuickWidget* _quickUi;
    // Указатели на объекты QML виджета
    // Окно установок +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _tModbusAddress;
    QObject* _tTxtModbusAddress;
    // Окно калибровки ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QObject* _txtCurrent;

    VT21Detect* _detect;
    // переопределение события изменения окна
    void resizeEvent(QResizeEvent*);

    // Кодировщик для датчика
    QTextCodec* _codec;

    void fillQmlWidget();   // Заполнение пользовательского окна "Установки" данными датчика

    void indicateDataMeas(long dataMeas);
};

#endif // SETTINGS_H

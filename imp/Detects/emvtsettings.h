#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

#include "abstractsettingscontroller.h"

class ImpAbstractDetect;

class EmVTSettings : public AbstractSettingsController
{
    Q_OBJECT
public:
    explicit EmVTSettings(ImpAbstractDetect* parent, QObject* _rootUi, int imageCode = 0);
    void FillQmlWidget() override;

private:
    // Включение переменных виджета QML
    // Указатель на виджет qml в окне класса
    int _imageCode;

    // Кодировщик для датчика
    QTextCodec* _codec;

    void fillQmlWidget();   // Заполнение пользовательского окна "Установки" данными датчика
};

#endif // SETTINGS_H

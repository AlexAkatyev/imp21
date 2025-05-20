#ifndef ABSTRACTSETTINGSCONTROLLER_H
#define ABSTRACTSETTINGSCONTROLLER_H

#include <QObject>

class ImpAbstractDetect;

class AbstractSettingsController : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSettingsController(ImpAbstractDetect* parent, QObject* rootUi);
    virtual void FillQmlWidget();

protected:
    ImpAbstractDetect* _detect;
    // Указатель на виджет qml в окне класса
    QObject* _rootUi;
};

#endif // ABSTRACTSETTINGSCONTROLLER_H

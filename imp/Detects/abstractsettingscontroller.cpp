#include <QVariant>

#include "abstractsettingscontroller.h"
#include "impabstractdetect.h"

AbstractSettingsController::AbstractSettingsController(ImpAbstractDetect* parent, QObject* rootUi)
    : QObject(parent)
    , _detect(parent)
    , _rootUi(rootUi)
{

}


void AbstractSettingsController::FillQmlWidget()
{
    _rootUi->setProperty("strSerialNumber", QString::number(_detect->Id()));
    _rootUi->setProperty("strNameDetect", _detect->UserName());
    _rootUi->setProperty("strActive", _detect->ActiveStateInfo());
    _rootUi->setProperty("strTypeDetect", _detect->TypeDetect());
    _rootUi->setProperty("strDataManuf", _detect->DateManuf().toString("dd.MM.yyyy"));
    _rootUi->setProperty("strPort", _detect->PortName());
    _rootUi->setProperty("strModbusAddress", _detect->Address());
}

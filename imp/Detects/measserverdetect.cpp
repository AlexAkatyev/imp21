#include <QVariant>

#include "measserverdetect.h"
#include "mbtcplocator.h"
#include "emvtsettings.h"


MeasServerDetect::MeasServerDetect(MBTcpLocator* locator, int countD, QObject* parent)
  : ImpAbstractDetect(parent)
  , _locator(locator)
  , _numberD(countD)
{

}


void MeasServerDetect::Init()
{
  _serialNumber = _locator->DetectSerialNumber(_numberD);
  _typeDetect = _locator->DetectType(_numberD);
  _dateManuf = _locator->DetectDateManuf(_numberD);
  _unitMeasure = _locator->UnitMeasure(_numberD);
  _userName = _locator->NameDetect(_numberD);
  _hMeasInterval = _locator->MeasInterval(_numberD);
  _lMeasInterval = -_locator->MeasInterval(_numberD);
  _zeroInterval = _locator->SetZero(_numberD);
  _preSetInterval = _locator->PreSet(_numberD);
  _flagReady = true;
  _measure = _locator->Measure(_numberD);
}


QString MeasServerDetect::PortName()
{
  QString result = _locator->connectionParameter(QModbusDevice::NetworkAddressParameter).toString();
  result.push_back(":502");
  return result;
}

void MeasServerDetect::ShowSettings()
{
  EmVTSettings* settingsWindow = new EmVTSettings(this, 1);
  settingsWindow->show();
}


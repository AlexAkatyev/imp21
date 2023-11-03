#include <QVariant>

#include "measserverdetect.h"
#include "mbtcplocator.h"
#include "emvtsettings.h"


MeasServerDetect::MeasServerDetect(MBTcpLocator* locator, int countD, QObject* parent)
  : ImpAbstractDetect(parent)
  , _locator(locator)
  , _numberD(countD)
  , _counter(0)
{
  connect(_locator, &MBTcpLocator::ReadyMeasure, this, [=](int id, int counter, float meas)
  {
    if (id == Id())
      if (counter > _counter
          || (counter < 50 && _counter > 200) )
      {
        _measure = meas;
        emit NewMeasure(meas);
        _counter = counter;
      }
  });
}


void MeasServerDetect::Init()
{
  _serialNumber = _locator->DetectId(_numberD);
  _typeDetect = _locator->DetectType(_numberD);
  _dateManuf = _locator->DetectDateManuf(_numberD);
  _unitMeasure = _locator->UnitMeasure(_numberD);
  _userName = _locator->NameDetect(_numberD);
  _hMeasInterval = _locator->MeasHiLimitInterval(_numberD);
  _lMeasInterval = _locator->MeasLoLimitInterval(_numberD);
  _zeroInterval = _locator->SetZero(_numberD);
  _preSetInterval = _locator->PreSet(_numberD);
  _flagReady = true;
  _measure = _locator->Measure(_numberD);
}


QString MeasServerDetect::PortName()
{
  QString result = _locator->PortName();
  result.push_back(" уст-во " + QString::number(_numberD));
  return result;
}


void MeasServerDetect::ShowSettings()
{
  EmVTSettings* settingsWindow = new EmVTSettings(this, 1);
  settingsWindow->show();
}


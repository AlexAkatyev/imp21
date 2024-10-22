#include "impabstractdetect.h"

ImpAbstractDetect::ImpAbstractDetect(QObject *parent)
  : QObject(parent)
  , _measure(0)
  , _serialNumber(0)
  , _typeDetect("Не определен")
  , _dateManuf(QDate::currentDate())
  , _unitMeasure("mkm")
  , _userName("")
  , _hMeasInterval(0)
  , _lMeasInterval(0)
  , _zeroInterval(0)
  , _preSetInterval(0)
  , _flagReady(false)
  , _activeStatus(false)
  , _activeStatusChanged(false)
  , _prevStateButton(false)
{

}


void ImpAbstractDetect::Init()
{

}


void ImpAbstractDetect::Stop()
{
  emit Stopped();
}


void ImpAbstractDetect::Remove()
{
  Stop();
  deleteLater();
}


float ImpAbstractDetect::CurrentMeasure()
{
  return _measure;
}


int ImpAbstractDetect::Id()
{
  return _serialNumber;
}


int ImpAbstractDetect::WaitInit()
{
  return 1000;
}


QString ImpAbstractDetect::TypeDetect()
{
  return _typeDetect;
}


QDate ImpAbstractDetect::DateManuf()
{
  return _dateManuf;
}


QString ImpAbstractDetect::UserName()
{
  return _userName;
}


QString ImpAbstractDetect::ActiveStateInfo()
{
  return _activeStatus ? "" : "(не активен)";
}


bool ImpAbstractDetect::ActiveStatusChanged()
{
  bool result = _activeStatusChanged;
  _activeStatusChanged = false;
  return result;
}


QString ImpAbstractDetect::PortName()
{
  return "";
}


QString ImpAbstractDetect::MeasUnit()
{
  return _unitMeasure;
}


int ImpAbstractDetect::HMeasureInterval()
{
  return _hMeasInterval;
}


int ImpAbstractDetect::LMeasureInterval()
{
  return _lMeasInterval;
}


int ImpAbstractDetect::ZeroInterval()
{
  return _zeroInterval;
}


int ImpAbstractDetect::PreSetInterval()
{
  return _preSetInterval;
}


void ImpAbstractDetect::ShowSettings()
{

}


void ImpAbstractDetect::SetNewName(QString newName)
{
  Q_UNUSED(newName)
}


bool ImpAbstractDetect::Ready()
{
  return _flagReady;
}


QString ImpAbstractDetect::Address()
{
  return "";
}


void ImpAbstractDetect::setStateButton(bool press)
{
  if (_prevStateButton)
  {
    if (press)
      emit HoldedButton();
    else
      emit ReleasedButton();
  }
  else
  {
    if (press)
      emit PressedButton();
  }

  _prevStateButton = press;
}

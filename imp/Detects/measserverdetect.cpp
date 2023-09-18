#include "measserverdetect.h"
#include "mbtcplocator.h"


MeasServerDetect::MeasServerDetect(MBTcpLocator* locator, int countD, QObject* parent)
  : ImpAbstractDetect(parent)
  , _locator(locator)
  , _numberD(countD)
{

}


void MeasServerDetect::Init()
{
  _serialNumber = _locator->DetectSerialNumber(_numberD);
//  QString _typeDetect;
//  QDate _dateManuf;
//  QString _unitMeasure;
//  QString _userName;
//  int _hMeasInterval;
//  int _lMeasInterval;
//  int _zeroInterval;
//  int _preSetInterval;
  _flagReady = true;
}




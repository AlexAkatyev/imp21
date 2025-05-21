
#include "vt21detect.h"

// Количество калибровочных точек
const int SUM_POINT = 21;


VT21Detect::VT21Detect(QSerialPortInfo portInfo, QObject *parent)
  : VTDetect(portInfo, parent)
  , _calibrField(0)
  , _currency(0)
  , _address(0)
{

}


int VT21Detect::CalibrField()
{
  return _calibrField;
}


std::vector<std::vector<int>> VT21Detect::PMTable()
{
  if (_currency == 2)
  {
    std::vector<std::vector<int>> ppmt;
    for (auto& pm : _pmt)
    {
      std::vector<int> a;
      a.push_back(pm[0]/100);
      a.push_back(pm[1]);
      ppmt.push_back(a);
    }
    return ppmt;
  }
  else
    return _pmt;
}


// Результат измерения датчика с учетом калибровки
void VT21Detect::calcCalibrateResult(int measData)
{
  emit NewDataMeas(measData);
  if (_cdt.size() < SUM_POINT-1)
    return;
  double result = 0;
  int index = 0;
  for (; index < SUM_POINT-1; ++index)
  {
    if (_cdt.at(index).lmess < _cdt.at(index).rmess)
    {
      if (index == 0)
        if (measData <= _cdt.at(index).rmess) break;
      if (measData >=  _cdt.at(index).lmess)
        if (measData <=  _cdt.at(index).rmess) break;
    }
    else
    {
      if (index == 0)
        if (measData >=  _cdt.at(index).rmess) break;
      if (measData <=  _cdt.at(index).lmess)
        if (measData >=  _cdt.at(index).rmess) break;
    }
  }
  if (index == SUM_POINT-1)
    --index;
  result =  _cdt.at(index).alfa * static_cast<double>(measData) +  _cdt.at(index).beta;
  _measure = static_cast<float>(result/1000);
  emit NewMeasure(_measure);
}


int VT21Detect::SumPoint()
{
  return SUM_POINT;
}


int reduceDivider(int d)
{
  if (d == 0)
    return 1;
  else if (d < 0)
    return -d;
  else
    return d;
}


void VT21Detect::defHMeasureInterval(int divider)
{
  int result = INT_MIN;
  for (auto pm : _pmt)
    if (result < pm.at(0))
      result = pm.at(0);
  _hMeasInterval = result / reduceDivider(divider);
}


void VT21Detect::defLMeasureInterval(int divider)
{
  int result = INT_MAX;
  for (auto pm : _pmt)
    if (result > pm.at(0))
      result = pm.at(0);
  _lMeasInterval = result / reduceDivider(divider);
}


bool VT21Detect::fillCalibrateDataTable(QByteArray baData, int lenPoint, int lenMeasPoint)
{
  bool error = false;

  _cdt.clear();
  _pmt.clear();
  int point2 = 0;
  int mess2 = 0;

  for (int j = 0; j < SUM_POINT - 1; ++j)
  {
    // Калибровочные точки
    int point1 = 0;
    point2 = 0;
    for (int k=0; k<lenPoint; k++)
    {
      point1 = point1 << 8;
      point1 += static_cast<unsigned char>(baData.at(j*(lenPoint+lenMeasPoint) + k));
      point2 = point2 << 8;
      point2 += static_cast<unsigned char>(baData.at((j+1)*(lenPoint+lenMeasPoint) + k));
    }
    if (lenPoint == 2)
    {
      if (point1 > 0x7FFF)
        point1 = point1 - 0x010000;
      if (point2 > 0x7FFF)
        point2 = point2 - 0x010000;
    }
    else
    {
      if (point1 > 0x7FFFFFFF)
        point1 *= -1;
      if (point2 > 0x7FFFFFFF)
        point2 *= -1;
    }


    // Калибровочное значение
    int mess1 = 0;
    mess2 = 0;
    for (int k = 0; k < lenMeasPoint; ++k)
    {
      mess1 = mess1 << 8;
      mess1 += static_cast<unsigned char>(baData.at(j*(lenPoint+lenMeasPoint) + lenPoint + k));
      mess2 = mess2 << 8;
      mess2 += static_cast<unsigned char>(baData.at((j+1)*(lenPoint+lenMeasPoint) + lenPoint + k));
    }
    if (mess1 == mess2)
      return true; // error
    CalibrateData cd;
    cd.lmess = mess1;
    cd.rmess = mess2;
    // расчет функции
    double dPoint1 = static_cast<double>(point1);
    double dPoint2 = static_cast<double>(point2);
    if (_currency == 2)
    {
      dPoint1 /= 100;
      dPoint2 /= 100;
    }
    cd.alfa = (1000*(dPoint2 - dPoint1)) / (static_cast<double>(mess2 - mess1));
    cd.beta = (dPoint1 - ((cd.alfa * (static_cast<double>(mess1))) / 1000)) * 1000;
    _cdt.push_back(cd);

    std::vector<int> punkt;
    punkt.push_back(point1);
    punkt.push_back(mess1);
    _pmt.push_back(punkt);
  }
  std::vector<int> punkt;
  punkt.push_back(point2);
  punkt.push_back(mess2);
  _pmt.push_back(punkt);

  return error;
}


QString VT21Detect::Address()
{
    return _address == 0 ? "" : QString::number(_address);
}


void VT21Detect::SetAddress(int address)
{
    Q_UNUSED(address);
}

#ifndef VT21DETECT_H
#define VT21DETECT_H

#include <vtdetect.h>

struct CalibrateData
{ // y = alfa * x + beta, if lopoint < x < hipoint
    double alfa;
    double beta;
    int lmess;
    int rmess;
};


class VT21Detect : public VTDetect
{
  Q_OBJECT
public:
  explicit VT21Detect(QSerialPortInfo portInfo, QObject *parent = nullptr);
  int CountPeriod();
  std::vector<std::vector<int>> PMTable();
  int CalibrField();
  virtual void SetAddress(int address);
  QString Address() override;
  static int SumPoint();

signals:
  void NewDataMeas(int dataMeas);

 protected:
  void calcCalibrateResult(int measData);
  void defHMeasureInterval(int divider = 1);
  void defLMeasureInterval(int divider = 1);
  bool fillCalibrateDataTable(QByteArray baData, int lenPoint, int lenMeasPoint);

  int _countPeriod;
  int _calibrField;
  std::vector<std::vector<int>> _pmt; // промежуточная калибровочная таблица для окна настроек
  std::vector<CalibrateData> _cdt; // CalibrateDataTable Аппроксимационные функции между калибровочными точками
  int _currency;
  char _address;

};

#endif // VT21DETECT_H

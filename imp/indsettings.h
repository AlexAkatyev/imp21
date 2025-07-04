#ifndef INDSETTINGS_H
#define INDSETTINGS_H
#include <QObject>

class QSettings;

enum IndKeys {WIN_X
              , WIN_Y
              , WIN_WIDTH
              , WIN_HEIGHT
              , INDICATOR_NAME
              , SCALE1
              , DETECT1
              , INCREMENT1
              , SCALE2
              , DETECT2
              , INCREMENT2
              , DIVIDER
              , BEFORESET
              , DOPUSK
              , UNITPOINT
              , HIGHLIMIT
              , LOWLIMIT
              , PRIEMKA
              , ACCURACY
              , PERIOD
              , ACCUR_DIVISION
              , IND_MEAS_MODE
              , TRANS_GAUGE
              , STRELKA
              , GROUPSF
              , SORT_CHF
              , NUMBER_CHAR_POINT
              , AUTO_SAVE_ENABLE
              , AUTO_SAVE_PERIOD
              , COMPLEX_FORMULA_ENABLE
              , COMPLEX_FORMULA_EXPRESSION
             };

class IndSettings : public QObject
{
public:
  IndSettings(QString setFileName, QObject* parent);
  void SetValue(IndKeys key, QVariant data);
  QVariant Value(IndKeys key);
  void LockDefaultValues();
  void UnlockDefaultValues();

private:
  QSettings* _settings;

  QString keyFromCode(IndKeys c);
  QVariant defaultValues(IndKeys c);

  bool _defaultLock;
};

#endif // INDSETTINGS_H


#include <QSettings>
#include <QGuiApplication>
#include <QScreen>

#include "indsettings.h"

// Исходные размеры окна
const int SIZE_WINDOW_WIDTH = 450;
const int SIZE_WINDOW_HEIGTH = 400;


IndSettings::IndSettings(QString setFileName, QObject* parent)
  : QObject(parent)
  , _settings(new QSettings(setFileName, QSettings::IniFormat, this))
  , _defaultLock(false)
{
}


void IndSettings::SetValue(IndKeys key, QVariant data)
{
  _settings->setValue(keyFromCode(key), data);
}


QVariant IndSettings::Value(IndKeys key)
{
  if (_defaultLock)
  {
    return defaultValues(key);
  }
  else
  {
    return _settings->value(keyFromCode(key), defaultValues(key));
  }
}


QString IndSettings::keyFromCode(IndKeys c)
{
  switch (c)
  {
  case WIN_X:
    return "x";
    break;
  case WIN_Y:
    return "y";
    break;
  case WIN_WIDTH:
    return "width";
    break;
  case WIN_HEIGHT:
    return "height";
    break;
  case INDICATOR_NAME:
    return "formula_name";
    break;
  case SCALE1:
    return "scale1";
    break;
  case DETECT1:
    return "detect1";
    break;
  case INCREMENT1:
    return "increment1";
    break;
  case SCALE2:
    return "scale2";
    break;
  case DETECT2:
    return "detect2";
    break;
  case INCREMENT2:
    return "increment2";
    break;
  case DIVIDER:
    return "divider";
    break;
  case BEFORESET:
    return "beforeset";
    break;
  case DOPUSK:
    return "dopusk";
    break;
  case UNITPOINT:
    return "unitpoint";
    break;
  case HIGHLIMIT:
    return "highlimit";
    break;
  case LOWLIMIT:
    return "lowlimit";
    break;
  case PRIEMKA:
    return "priemka";
    break;
  case ACCURACY:
    return "accuracy";
    break;
  case PERIOD:
    return "period";
    break;
  case ACCUR_DIVISION:
    return "accur_division";
    break;
  case IND_MEAS_MODE:
    return "meas_mode";
    break;
  case TRANS_GAUGE:
    return "transGauge";
    break;
  case STRELKA:
    return "strelka";
    break;
  case GROUPSF:
    return "groupsf";
    break;
  case SORT_CHF:
    return "scf";
    break;
  case NUMBER_CHAR_POINT:
    return "number_char_point";
    break;
  case AUTO_SAVE_ENABLE:
    return "auto_save_enable";
    break;
  case AUTO_SAVE_PERIOD:
    return "auto_save_period";
    break;
  case COMPLEX_FORMULA_ENABLE:
    return "complex_formula_enable";
    break;
  case COMPLEX_FORMULA_EXPRESSION:
    return "complex_formula_expression";
    break;
  default:
    return "";
  }
}


QVariant IndSettings::defaultValues(IndKeys c)
{
  auto getWindowPos = [](bool hor)
  {
    QList<QScreen*> screens = QGuiApplication::screens();
    QPoint center = screens.at(0)->availableGeometry().center();
    if (hor)
      return screens.isEmpty() ? 0 : (center.x() - (SIZE_WINDOW_WIDTH/2));
    else
      return screens.isEmpty() ? 0 : (center.y() - (SIZE_WINDOW_HEIGTH/2));
  };

  switch (c)
  {
  case WIN_X:
    return getWindowPos(true);
    break;
  case WIN_Y:
    return getWindowPos(false);
    break;
  case WIN_WIDTH:
    return SIZE_WINDOW_WIDTH;
    break;
  case WIN_HEIGHT:
    return SIZE_WINDOW_HEIGTH;
    break;
  case INDICATOR_NAME:
    return "Индикатор";
    break;
  case UNITPOINT:
    return 5;
    break;
  case HIGHLIMIT:
    return 50;
    break;
  case LOWLIMIT:
    return -50;
    break;
  case NUMBER_CHAR_POINT:
    return 1;
    break;
  case SCALE1:
    return 1;
    break;
  case DETECT1:
    return 0;
    break;
  case INCREMENT1:
    return 0;
    break;
  case SCALE2:
    return 1;
    break;
  case DETECT2:
    return 0;
    break;
  case INCREMENT2:
    return 0;
    break;
  case DIVIDER:
    return 1;
    break;
  case TRANS_GAUGE:
    return 0;
    break;
  case STRELKA:
    return true;
    break;
  case GROUPSF:
    return 1;
    break;
  case PERIOD:
    return 0;
    break;
  case PRIEMKA:
    return 0;
    break;
  case ACCURACY:
    return 1;
    break;
  case ACCUR_DIVISION:
    return 0;
    break;
  case IND_MEAS_MODE:
    return false;
    break;
  case DOPUSK:
    return true;
    break;
  case AUTO_SAVE_ENABLE:
    return false;
    break;
  case AUTO_SAVE_PERIOD:
    return 10;
    break;
  case COMPLEX_FORMULA_ENABLE:
    return false;
    break;
  case COMPLEX_FORMULA_EXPRESSION:
    return "";
    break;
  default:
    return QVariant();
  }
}


void IndSettings::LockDefaultValues()
{
  _defaultLock = true;
}


void IndSettings::UnlockDefaultValues()
{
  _defaultLock = false;
}

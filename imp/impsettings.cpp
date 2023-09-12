
#include <QSettings>
#include <QGuiApplication>
#include <QScreen>

#include "impsettings.h"

// Исходные размеры окна
const int SIZE_WINDOW_WIDTH = 320;
const int SIZE_WINDOW_HEIGTH = 480;


ImpSettings::ImpSettings(QString setFileName, QObject* parent)
  : QObject(parent)
  , _settings(new QSettings(setFileName, QSettings::IniFormat, this))
{

}


void ImpSettings::SetValue(ImpKeys key, QVariant data)
{
  _settings->setValue(keyFromCode(key), data);
}


QVariant ImpSettings::Value(ImpKeys key)
{
  return _settings->value(keyFromCode(key), defaultValues(key));
}


QString ImpSettings::keyFromCode(ImpKeys c)
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
  case INDICATORS:
    return "indicators";
    break;
  default:
    return "";
  }
}


QVariant ImpSettings::defaultValues(ImpKeys c)
{
  static QList<QScreen*> screens = QGuiApplication::screens();
  static QPoint center = screens.at(0)->availableGeometry().center();
  static int x = screens.isEmpty() ? 0 : (center.x() - (SIZE_WINDOW_WIDTH/2));
  static int y = screens.isEmpty() ? 0 : (center.y() - (SIZE_WINDOW_HEIGTH/2));

  switch (c)
  {
  case WIN_X:
  {
    return x;
    break;
  }
  case WIN_Y:
  {
    return y;
    break;
  }
  case WIN_WIDTH:
    return SIZE_WINDOW_WIDTH;
    break;
  case WIN_HEIGHT:
    return SIZE_WINDOW_HEIGTH;
    break;
  case INDICATORS:
    return QStringList();
    break;
  default:
    return QVariant();
  }
}


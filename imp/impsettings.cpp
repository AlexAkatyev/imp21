
#include <QSettings>
#include <QGuiApplication>
#include <QScreen>

#include "impsettings.h"
#include "impdef.h"
#include "workplacesmodel.h"

// Исходные размеры окна
const int SIZE_WINDOW_WIDTH = 320;
const int SIZE_WINDOW_HEIGTH = 480;


ImpSettings* ImpSettings::Instance(QObject* parent)
{
  static ImpSettings* s = new ImpSettings("imp.ini", parent);
  return s;
}


ImpSettings::ImpSettings(QString setFileName, QObject* parent)
  : QObject(parent)
  , _settings(new QSettings(setFileName, QSettings::IniFormat, this))
  , _setModel(new WorkPlacesModel(parent))
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
  case EN_RS_485:
    return "en_rs_485";
    break;
  case EN_MODBUS_TCP:
    return "en_modbus_tcp";
    break;
  case LIST_MB_ADDR:
    return "list_mb_addr";
    break;
  case RECORDING_IN_ALL_INDICATORS:
    return "recording_in_all_indicators";
    break;
  case DEBUG_GUI_MODE:
    return "debug_gui_mode";
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
  case EN_RS_485:
    return false;
    break;
  case EN_MODBUS_TCP:
    return false;
    break;
  case LIST_MB_ADDR:
  {
    QStringList sl;
    sl << DEF_MB_SERVER;
    return sl;
    break;
  }
  case RECORDING_IN_ALL_INDICATORS:
    return false;
    break;
  case DEBUG_GUI_MODE:
    return false;
    break;
  default:
    return QVariant();
  }
}


WorkPlacesModel* ImpSettings::GetWorkPlacesModel()
{
    return _setModel;
}


void ImpSettings::SaveWorkPlacesModel()
{

}

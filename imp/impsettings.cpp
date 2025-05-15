
#include <QSettings>
#include <QGuiApplication>
#include <QScreen>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "impsettings.h"
#include "impdef.h"
#include "workplacesmodel.h"

// Исходные размеры окна
const int SIZE_WINDOW_WIDTH = 320;
const int SIZE_WINDOW_HEIGTH = 480;
const QString WP_NAME = "wpname";
const QString WP_UUID = "uuid";
const QString WP_MB_ADDRESSES = "list_mb_addr";
const QString WP_RECI = "recording_in_all_indicators";
const QString WP_ENMBTCP = "en_modbus_tcp";
const QString WP_ENRS485 = "en_rs_485";
const QString WP_ARRAY = "workplaces";


ImpSettings* ImpSettings::Instance(QObject* parent)
{
  static ImpSettings* s = new ImpSettings("imp.ini", "impini.json", parent);
  return s;
}


ImpSettings::ImpSettings(QString setFileName, QString setJsonFileName, QObject* parent)
  : QObject(parent)
  , _settings(new QSettings(setFileName, QSettings::IniFormat, this))
  , _setModel(new WorkPlacesModel(parent))
  , _jsonIniFile(setJsonFileName)
{
    QFile file(_jsonIniFile);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument jDoc(QJsonDocument::fromJson(data));
        QJsonObject jobj = jDoc.object();
        QJsonArray array = jobj.value(WP_ARRAY).toArray();
        for (int i = 0; i < array.size(); ++i)
        {
            QJsonValue v = array[i];
            _setModel->AddRecord
                    (
                        v[WP_NAME].toString()
                        , v[WP_RECI].toBool()
                        , v[WP_ENMBTCP].toBool()
                        , v[WP_ENRS485].toBool()
                        , v[WP_MB_ADDRESSES].toVariant().toStringList()
                        , v[WP_UUID].toString()
                    );
        }

    }
}


void ImpSettings::SetValue(ImpKeys key, QVariant data)
{
    switch (key)
    {
    case ImpKeys::EN_MODBUS_TCP:
        _setModel->SetEnModbusTCP(Value(ImpKeys::ACTIVE_WORKPLACE).toInt(), data.toBool());
        break;
    case ImpKeys::EN_RS_485:
        _setModel->SetEnRS485(Value(ImpKeys::ACTIVE_WORKPLACE).toInt(), data.toBool());
        break;
    case ImpKeys::RECORDING_IN_ALL_INDICATORS:
        _setModel->SetRecordingInAllIndicators(Value(ImpKeys::ACTIVE_WORKPLACE).toInt(), data.toBool());
        break;
    case LIST_MB_ADDR:
        _setModel->SetModbusAddresses(Value(ImpKeys::ACTIVE_WORKPLACE).toInt(), data.toStringList());
        break;
    default:
        _settings->setValue(keyFromCode(key), data);
        break;
    }
}


QVariant ImpSettings::Value(ImpKeys key)
{
    switch (key)
    {
    case ImpKeys::EN_MODBUS_TCP:
        return _setModel->EnModbusTCP(Value(ImpKeys::ACTIVE_WORKPLACE).toInt());
        break;
    case ImpKeys::EN_RS_485:
        return _setModel->EnRS485(Value(ImpKeys::ACTIVE_WORKPLACE).toInt());
        break;
    case ImpKeys::RECORDING_IN_ALL_INDICATORS:
        return _setModel->RecordingInAllIndicators(Value(ImpKeys::ACTIVE_WORKPLACE).toInt());
        break;
    case LIST_MB_ADDR:
        return _setModel->ModbusAddresses(Value(ImpKeys::ACTIVE_WORKPLACE).toInt());
        break;
    default:
        return _settings->value(keyFromCode(key), defaultValues(key));
        break;
    }
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
  case DEBUG_GUI_MODE:
    return "debug_gui_mode";
    break;
  case ACTIVE_WORKPLACE:
      return "active_workplace";
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
  case DEBUG_GUI_MODE:
    return false;
    break;
  case ACTIVE_WORKPLACE:
      return 0;
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
    QFile file(_jsonIniFile);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QJsonArray wpArray = QJsonArray();
        QStringList names = _setModel->WorkPlacesNames();
        for (int i = 0; i < names.length(); ++i)
        {
            QJsonObject textObject;
            textObject[WP_NAME] = names[i];
            textObject[WP_RECI] = _setModel->RecordingInAllIndicators(i);
            textObject[WP_ENRS485] = _setModel->EnRS485(i);
            textObject[WP_ENMBTCP] = _setModel->EnModbusTCP(i);
            textObject[WP_UUID] = _setModel->GetUuid(i);
            QJsonArray mba = QJsonArray();
            for (QString adr : _setModel->ModbusAddresses(i))
            {
                mba.append(adr);
            }
            textObject[WP_MB_ADDRESSES] = mba;
            wpArray.append(textObject);
        }
        QJsonObject wpObj = QJsonObject();
        wpObj[WP_ARRAY] = wpArray;
        file.write(QJsonDocument(wpObj).toJson(QJsonDocument::Indented));
        file.close();
    }
}

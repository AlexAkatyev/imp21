#ifndef IMPSETTINGS_H
#define IMPSETTINGS_H
#include <QObject>

class QSettings;
class WorkPlacesModel;

enum ImpKeys {WIN_X
              , WIN_Y
              , WIN_WIDTH
              , WIN_HEIGHT
              , INDICATORS
              , EN_RS_485
              , EN_MODBUS_TCP
              , LIST_MB_ADDR
              , DEBUG_GUI_MODE
              , ACTIVE_WORKPLACE
             };

class ImpSettings : public QObject
{
public:
  static ImpSettings* Instance(QObject* parent = nullptr);
  void SetValue(ImpKeys key, QVariant data);
  QVariant Value(ImpKeys key);
  WorkPlacesModel* GetWorkPlacesModel();
  void SaveWorkPlacesModel();

private:
  ImpSettings(QString setFileName, QString setJsonFileName, QObject* parent);
  QSettings* _settings;
  WorkPlacesModel* _setModel;
  QString _jsonIniFile;

  QString keyFromCode(ImpKeys c);
  QVariant defaultValues(ImpKeys c);
};

#endif // IMPSETTINGS_H

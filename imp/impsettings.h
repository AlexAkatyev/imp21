#ifndef IMPSETTINGS_H
#define IMPSETTINGS_H
#include <QObject>

class QSettings;

enum ImpKeys {WIN_X
              , WIN_Y
              , WIN_WIDTH
              , WIN_HEIGHT
              , INDICATORS
              , EN_RS_485
              , EN_MODBUS_TCP
              , LIST_MB_ADDR
              , RECORDING_IN_ALL_INDICATORS
             };

class ImpSettings : public QObject
{
public:
  static ImpSettings* Instance(QObject* parent = nullptr);
  void SetValue(ImpKeys key, QVariant data);
  QVariant Value(ImpKeys key);

private:
  ImpSettings(QString setFileName, QObject* parent);
  QSettings* _settings;

  QString keyFromCode(ImpKeys c);
  QVariant defaultValues(ImpKeys c);
};

#endif // IMPSETTINGS_H

#ifndef IMPSETTINGSDIALOG_H
#define IMPSETTINGSDIALOG_H

#include <WidgetUtil/centerdialog.h>

class QQuickWidget;

class ImpSettingsDialog
    : public CenterDialog
{
  Q_OBJECT
public:
  ImpSettingsDialog(QWidget* parent = nullptr);

private slots:
  void setIniFindModbusTCP(bool en);
  void setIniFindModbus485(bool en);
  void setRecordingInAllIndicators(bool en);

private:
  void linkIni(QQuickWidget* ui);
};

#endif // IMPSETTINGSDIALOG_H

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

private:
  void linkIni(QQuickWidget* ui);

private slots:
  void setIniFindModbusTCP(bool en);
  void setIniFindModbus485(bool en);
};

#endif // IMPSETTINGSDIALOG_H

#ifndef IMPSETTINGSDIALOG_H
#define IMPSETTINGSDIALOG_H

#include <WidgetUtil/centerdialog.h>

class ImpSettingsDialog
    : public CenterDialog
{
  Q_OBJECT
public:
  ImpSettingsDialog(QWidget* parent = nullptr);
};

#endif // IMPSETTINGSDIALOG_H

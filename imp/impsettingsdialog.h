#ifndef IMPSETTINGSDIALOG_H
#define IMPSETTINGSDIALOG_H

#include <QDialog>

class ImpSettingsDialog
    : public QDialog
{
  Q_OBJECT
public:
  ImpSettingsDialog(QWidget* parent = nullptr);
  void UpdatePosition(QRect parentGeometry);

private:
  int _width;
  int _height;
};

#endif // IMPSETTINGSDIALOG_H

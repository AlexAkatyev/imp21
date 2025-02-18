#ifndef CENTERDIALOG_H
#define CENTERDIALOG_H

#include <QDialog>

class CenterDialog : public QDialog
{
public:
  CenterDialog(QWidget* parent);
  void UpdatePosition(QRect parentGeometry);

protected:
  void setSize(int w, int h);

private:
  int _width;
  int _height;
};

#endif // CENTERDIALOG_H

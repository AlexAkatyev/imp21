#include "centerdialog.h"

CenterDialog::CenterDialog(QWidget* parent)
  : QDialog(parent)
{
  setModal(true);
}


void CenterDialog::setSize(int w, int h)
{
  _height = h;
  _width = w;
  setMinimumSize(_width, _height);
  setMaximumSize(_width, _height);
}


void CenterDialog::UpdatePosition(QRect parentGeometry)
{
  int ax = parentGeometry.x() + (parentGeometry.width() - _width) / 2;
  int ay = parentGeometry.y() + (parentGeometry.height() - _height) / 2;
  setGeometry(ax, ay, _width, _height);
}

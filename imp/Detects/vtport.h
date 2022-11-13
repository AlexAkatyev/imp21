#ifndef VTPORT_H
#define VTPORT_H

#include <QIODevice>

class VTPort : public QIODevice
{
  Q_OBJECT
public:
  explicit VTPort(QObject* parent);
  virtual bool IsHaveVTDetect() = 0;

};

#endif // VTPORT_H

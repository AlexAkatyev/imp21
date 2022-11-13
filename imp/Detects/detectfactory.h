#ifndef DETECTFACTORY_H
#define DETECTFACTORY_H

#include <QObject>

#include "Detects/vtdetect.h"

class VTDetect;

class DetectFactory : public QObject
{
  Q_OBJECT
public:
  explicit DetectFactory(QObject *parent);
  int AvailablePorts();
  int FindingTime();
  std::vector<VTDetect*> VTDetects();

};

#endif // DETECTFACTORY_H

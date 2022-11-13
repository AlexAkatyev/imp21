#ifndef PORTFACTORY_H
#define PORTFACTORY_H

#include <QObject>

class PortFactory : public QObject
{
  Q_OBJECT
public:
  explicit PortFactory(QObject* parent = nullptr);

signals:

};

#endif // PORTFACTORY_H

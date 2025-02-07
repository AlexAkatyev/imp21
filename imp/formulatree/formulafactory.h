#ifndef FORMULAFACTORY_H
#define FORMULAFACTORY_H

#include <QObject>

class FormulaNode;

class FormulaFactory : public QObject
{
  Q_OBJECT
public:
  explicit FormulaFactory(QObject *parent);

  FormulaNode* Do(QString input, bool& error, QString& textError);
};

#endif // FORMULAFACTORY_H

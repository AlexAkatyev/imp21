#ifndef FORMULAFACTORY_H
#define FORMULAFACTORY_H

#include <QObject>

class FormulaNode;
class ImpAbstractDetect;
class Imp;

class FormulaFactory : public QObject
{
  Q_OBJECT
public:
  static FormulaFactory* Instance(Imp* imp = nullptr);
  void operator=(const FormulaFactory&) = delete;
  FormulaNode* Do(QString input, bool* error, QString* textError);

private:
  FormulaFactory(Imp* imp);

  FormulaNode* stapleDo(QString input, bool* error, QString* textError);
  FormulaNode* getExpression(QString input, bool* error, QString* textError);
  ImpAbstractDetect* getDetect(QString input, bool* error, QString* textError);

  Imp* _imp;
};

#endif // FORMULAFACTORY_H

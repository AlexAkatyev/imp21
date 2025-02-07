#include "formulafactory.h"
#include "formulanode.h"

FormulaFactory::FormulaFactory(QObject *parent) : QObject(parent)
{

}


FormulaNode* FormulaFactory::Do(QString input, bool& error, QString& textError)
{
  FormulaNode* result = new FormulaNode(parent());
  QString strLH;
  return result;
}

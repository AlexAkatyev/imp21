#ifndef FORMULANODE_H
#define FORMULANODE_H

#include <QObject>

class FormulaNode : public QObject
{
  Q_OBJECT
public:
  explicit FormulaNode(QObject *parent = nullptr);

  double Get();

private:
  enum Operator
  {
    ADD
    , SUB
    , MUL
    , DIV
  };
  FormulaNode* _lh;
  FormulaNode* _rh;
  Operator _op;
};

#endif // FORMULANODE_H

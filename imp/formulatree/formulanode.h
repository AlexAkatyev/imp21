#ifndef FORMULANODE_H
#define FORMULANODE_H

#include <QObject>

class ImpAbstractDetect;

class FormulaNode : public QObject
{
  Q_OBJECT
public:
  enum Operator
  {
    ADD
    , SUB
    , MUL
    , DIV
  };
  explicit FormulaNode(QObject *parent = nullptr);

  void SetL(float);
  void SetL(FormulaNode*);
  void SetL(ImpAbstractDetect*);
  void SetR(float);
  void SetR(FormulaNode*);
  void SetR(ImpAbstractDetect*);
  void SetOp(Operator);

  float Get();

private:
  Operator _op;
  FormulaNode* _lh;
  FormulaNode* _rh;
  float _lValue;
  float _rValue;
  ImpAbstractDetect* _lDetect;
  ImpAbstractDetect* _rDetect;
};

#endif // FORMULANODE_H

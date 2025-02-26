#ifndef FORMULANODE_H
#define FORMULANODE_H

#include <QObject>

class ImpAbstractDetect;
enum FormulaNodeOperator
{
  ADD
  , SUB
  , MUL
  , DIV
};

class FormulaNode : public QObject
{
  Q_OBJECT
public:
  explicit FormulaNode(QObject *parent = nullptr);

  void SetL(float);
  void SetL(FormulaNode*);
  void SetL(ImpAbstractDetect*);
  void SetR(float);
  void SetR(FormulaNode*);
  void SetR(ImpAbstractDetect*);
  void SetOp(FormulaNodeOperator);

  float Get(std::function<float(float, int)>, int);
  bool HiOverRange();
  bool LoOverRange();

private:
  FormulaNodeOperator _op;
  FormulaNode* _lh;
  FormulaNode* _rh;
  float _lValue;
  float _rValue;
  ImpAbstractDetect* _lDetect;
  ImpAbstractDetect* _rDetect;
};

#endif // FORMULANODE_H

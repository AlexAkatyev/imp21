#include "formulanode.h"
#include "impabstractdetect.h"

FormulaNode::FormulaNode(QObject *parent)
  : QObject(parent)
  , _op(FormulaNodeOperator::ADD)
  , _lh(nullptr)
  , _rh(nullptr)
  , _lValue(0.0)
  , _rValue(0.0)
  , _lDetect(nullptr)
  , _rDetect(nullptr)
{
}


float FormulaNode::Get()
{
  float lh = _lValue;
  if (_lh)
  {
    lh = _lh->Get();
  }
  else if (_lDetect)
  {
    lh = _lDetect->CurrentMeasure();
  }
  float rh = _rValue;
  if (_rh)
  {
    rh = _rh->Get();
  }
  else if (_rDetect)
  {
    rh = _rDetect->CurrentMeasure();
  }
  switch (_op)
  {
  case FormulaNodeOperator::ADD:
    lh += rh;
    break;
  case FormulaNodeOperator::SUB:
    lh -= rh;
    break;
  case FormulaNodeOperator::MUL:
    lh *= rh;
    break;
  case FormulaNodeOperator::DIV:
    if (rh == 0)
    {
      lh = __FLT_MAX__;
    }
    else
    {
      lh /= rh;
    }
    break;
  }
  return lh;
}


void FormulaNode::SetL(float d)
{
  _lValue = d;
}


void FormulaNode::SetL(FormulaNode* d)
{
  _lh = d;
}


void FormulaNode::SetL(ImpAbstractDetect* d)
{
  _lDetect = d;
}


void FormulaNode::SetR(float d)
{
  _rValue = d;
}


void FormulaNode::SetR(FormulaNode* d)
{
  _rh = d;
}


void FormulaNode::SetR(ImpAbstractDetect* d)
{
  _rDetect = d;
}


void FormulaNode::SetOp(FormulaNodeOperator d)
{
  _op = d;
}

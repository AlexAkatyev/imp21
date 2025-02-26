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


float FormulaNode::Get(std::function<float(float, int)> transformation, int prop)
{
  float lh = _lValue;
  if (_lh)
  {
    lh = _lh->Get(transformation, prop);
  }
  else if (_lDetect)
  {
    lh = transformation(_lDetect->CurrentMeasure(), prop);
  }
  float rh = _rValue;
  if (_rh)
  {
    rh = _rh->Get(transformation, prop);
  }
  else if (_rDetect)
  {
    rh = transformation(_rDetect->CurrentMeasure(), prop);
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


bool FormulaNode::HiOverRange()
{
  bool hiOverRange = false;
  if (_lh)
  {
    hiOverRange |= _lh->HiOverRange();
  }
  else if (_lDetect)
  {
    hiOverRange |= _lDetect->CurrentMeasure() > _lDetect->HMeasureInterval();
  }
  if (!hiOverRange)
  {
    if (_rh)
    {
      hiOverRange |= _rh->HiOverRange();
    }
    else if (_rDetect)
    {
      hiOverRange |= _rDetect->CurrentMeasure() > _rDetect->HMeasureInterval();
    }
  }
  return hiOverRange;
}


bool FormulaNode::LoOverRange()
{
  bool loOverRange = false;
  if (_lh)
  {
    loOverRange |= _lh->HiOverRange();
  }
  else if (_lDetect)
  {
    loOverRange |= _lDetect->CurrentMeasure() < _lDetect->LMeasureInterval();
  }
  if (!loOverRange)
  {
    if (_rh)
    {
      loOverRange |= _rh->HiOverRange();
    }
    else if (_rDetect)
    {
      loOverRange |= _rDetect->CurrentMeasure() < _rDetect->LMeasureInterval();
    }
  }
  return loOverRange;
}


bool FormulaNode::DetectsReady()
{
  bool ready = true;
  if (_lh)
  {
    ready &= _lh->DetectsReady();
  }
  else if (_lDetect)
  {
    ready &= _lDetect->Ready();
  }
  if (_rh)
  {
    ready &= _rh->DetectsReady();
  }
  else if (_rDetect)
  {
    ready &= _rDetect->Ready();
  }
  return ready;
}

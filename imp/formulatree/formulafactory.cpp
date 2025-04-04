#include "formulafactory.h"
#include "formulanode.h"
#include "imp.h"

const char BEGIN_STAPLE = '(';
const char END_STAPLE = ')';

bool isExpression(QString input)
{
  return input.contains('+')
      || input.contains('-')
      || input.contains('*')
      || input.contains('/');
}


int getBeginForLastEndStaple(QString str)
{
  int lh = -1;
  int rh = str.lastIndexOf(END_STAPLE);
  if (rh == -1)
  {
    return lh;
  }
  int depth = 0;
  for (int i = rh - 1; i > -1; --i)
  {
    if (str[i] == END_STAPLE)
    {
      ++depth;
    }
    else if (str[i] == BEGIN_STAPLE)
    {
      if (depth == 0)
      {
        lh = i;
        break;
      }
      else
      {
        --depth;
      }
    }
  }
  return lh;
}


FormulaFactory* FormulaFactory::Instance(Imp* imp)
{
  static FormulaFactory* singleton = nullptr;
  if (singleton == nullptr
      && imp != nullptr)
  {
    singleton = new FormulaFactory(imp);
  }
  return singleton;
}


FormulaFactory::FormulaFactory(Imp* imp)
  : QObject(imp)
  , _imp(imp)
{
}


FormulaNode* FormulaFactory::Do(QString input, bool* error, QString* textError)
{
  QString clInput = input.trimmed();
  // обработка скобок
  if (clInput.contains('(') || clInput.contains(')'))
  {
    return stapleDo(clInput, error, textError);
  }
  // скобок нет
  return getExpression(clInput, error, textError);
}


FormulaNode* FormulaFactory::stapleDo(QString clInput, bool* error, QString* textError)
{
  FormulaNode* result = new FormulaNode(parent());
  int leftCount = clInput.count(BEGIN_STAPLE);
  int rightCount = clInput.count(END_STAPLE);
  if (leftCount != rightCount)
  {
    *textError = "Открывающие и закрывающие скобки не спарены";
    *error = true;
    return result;
  }
  int ls = getBeginForLastEndStaple(clInput);
  int rs = clInput.lastIndexOf(END_STAPLE);
  if (rs != -1)
  {
    if (ls == -1)
    {
      *textError = "Открывающие и закрывающие скобки не спарены";
      *error = true;
      return result;
    }
    if (ls >= rs)
    {
      *textError = "Не правильная последовательность скобок";
      *error = true;
      return result;
    }
    if (ls == 0 && rs == clInput.length() - 1)
    {
      result->deleteLater();
      return Do(clInput.mid(ls + 1, rs - 1), error, textError);
    }
    int lp = clInput.indexOf('+');
    if (lp != -1 && lp < ls)
    {
      result->SetOp(FormulaNodeOperator::ADD);
      result->SetL(Do(clInput.mid(0, lp), error, textError));
      result->SetR(Do(clInput.mid(lp + 1), error, textError));
      return result;
    }
    int lm = clInput.indexOf('-');
    if (lm != -1 && lm < ls)
    {
      result->SetOp(FormulaNodeOperator::SUB);
      result->SetL(Do(clInput.mid(0, lm), error, textError));
      result->SetR(Do(clInput.mid(lm + 1), error, textError));
      return result;
    }
    int rp = clInput.lastIndexOf('+');
    if (rp != -1 && rp > rs)
    {
      result->SetOp(FormulaNodeOperator::ADD);
      result->SetL(Do(clInput.mid(0, rp), error, textError));
      result->SetR(Do(clInput.mid(rp + 1), error, textError));
      return result;
    }
    int rm = clInput.lastIndexOf('-');
    if (rm != -1 && rm > rs)
    {
      result->SetOp(FormulaNodeOperator::SUB);
      result->SetL(Do(clInput.mid(0, rm), error, textError));
      result->SetR(Do(clInput.mid(rm + 1), error, textError));
      return result;
    }
    int lmul = clInput.indexOf('*');
    if (lmul != -1 && lmul < ls)
    {
      result->SetOp(FormulaNodeOperator::MUL);
      result->SetL(Do(clInput.mid(0, lmul), error, textError));
      result->SetR(Do(clInput.mid(lmul + 1), error, textError));
      return result;
    }
    int ld = clInput.indexOf('/');
    if (ld != -1 && ld < ls)
    {
      result->SetOp(FormulaNodeOperator::DIV);
      result->SetL(Do(clInput.mid(0, ld), error, textError));
      result->SetR(Do(clInput.mid(ld + 1), error, textError));
      return result;
    }
    int rmul = clInput.lastIndexOf('*');
    if (rmul != -1 && rmul > rs)
    {
      result->SetOp(FormulaNodeOperator::MUL);
      result->SetL(Do(clInput.mid(0, rmul), error, textError));
      result->SetR(Do(clInput.mid(rmul + 1), error, textError));
      return result;
    }
    int rd = clInput.lastIndexOf('/');
    if (rd != -1 && rd > rs)
    {
      result->SetOp(FormulaNodeOperator::DIV);
      result->SetL(Do(clInput.mid(0, rd), error, textError));
      result->SetR(Do(clInput.mid(rd + 1), error, textError));
      return result;
    }
  }
  if (ls != 1) // rs == -1
  {
    *textError = "Открывающие и закрывающие скобки не спарены";
    *error = true;
    return result;
  }
  *textError = "Ошибка обработки скобок";
  *error = true;
  return result;
}


FormulaNode* FormulaFactory::getExpression(QString clInput, bool* error, QString* textError)
{
  FormulaNode* result = new FormulaNode(parent());
  QString strLH;
  QString strRH;

  bool numeric;
  float data = clInput.toFloat(&numeric);
  if (numeric)
  {
    result->SetL(data);
    return result;
  }

  if (clInput.contains('+'))
  {
    int rp = clInput.lastIndexOf('+');
    strLH = clInput.mid(0, rp);
    strRH = clInput.mid(rp + 1);
  }
  else if (clInput.contains('-'))
  {
    int rp = clInput.lastIndexOf('-');
    result->SetOp(FormulaNodeOperator::SUB);
    strLH = clInput.mid(0, rp);
    strRH = clInput.mid(rp + 1);
  }
  else if (clInput.contains('*'))
  {
    int rp = clInput.lastIndexOf('*');
    result->SetOp(FormulaNodeOperator::MUL);
    strLH = clInput.mid(0, rp);
    strRH = clInput.mid(rp + 1);
  }
  else if (clInput.contains('/'))
  {
    int rp = clInput.lastIndexOf('/');
    result->SetOp(FormulaNodeOperator::DIV);
    strLH = clInput.mid(0, rp);
    strRH = clInput.mid(rp + 1);
  }
  else
  {
    if (clInput.isEmpty())
    {
      *error = true;
      *textError = "Некорректная запись формулы";
    }
    else
    {
      // возможно, здесь записан датчик
      result->SetL(getDetect(clInput, error, textError));
    }
  }
  if (!*error)
  {
    data = strLH.toFloat(&numeric);
    if (strLH.isEmpty())
    {
      *error = true;
      *textError = "Некорректная запись формулы слева";
    }
    else if (numeric)
    {
      result->SetL(data);
    }
    else if (isExpression(strLH))
    {
      result->SetL(getExpression(strLH, error, textError));
    }
    else
    {
      result->SetL(getDetect(strLH, error, textError));
    }

    data = strRH.toFloat(&numeric);
    if (strRH.isEmpty())
    {
      *error = true;
      *textError = "Некорректная запись формулы справа";
    }
    else if (numeric)
    {
      result->SetR(data);
    }
    else if (isExpression(strRH))
    {
      result->SetR(getExpression(strRH, error, textError));
    }
    else
    {
      result->SetR(getDetect(strRH, error, textError));
    }
  }
  return result;
}


ImpAbstractDetect* FormulaFactory::getDetect(QString clInput, bool* error, QString* textError)
{
  if (clInput.length() < 2)
  {
    *error = true;
    *textError = "неверная запись датчика : " + clInput;
    return nullptr;
  }
  else
  {
    if (clInput.at(0) == "#")
    {
      bool numeric;
      int number = clInput.mid(1).toInt(&numeric);
      if (!numeric)
      {
        *error = true;
        *textError = "неверная запись датчика : " + clInput;
        return nullptr;
      }
      if (_imp != nullptr)
      {
        ImpAbstractDetect* detect = _imp->DetectAtId(number);
        if (detect)
        {
          return detect;
        }
        else
        {
          *error = true;
          *textError = "датчик  " + clInput + " не обнаружен";
          return nullptr;
        }
      }
      else
      {
        *error = true;
        *textError = "Не удается найти список датчиков";
        return nullptr;
      }
    }
    else
    {
      *error = true;
      *textError = "неверная запись датчика : " + clInput;
      return nullptr;
    }
  }
  return nullptr;
}

#include "formulafactory.h"
#include "formulanode.h"

FormulaFactory::FormulaFactory(QObject *parent) : QObject(parent)
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
  return signDo(clInput, error, textError);
}


FormulaNode* FormulaFactory::stapleDo(QString clInput, bool* error, QString* textError)
{
  FormulaNode* result = new FormulaNode(parent());
  int ls = clInput.indexOf('(');
  int rs = clInput.lastIndexOf(')');
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


FormulaNode* FormulaFactory::signDo(QString clInput, bool* error, QString* textError)
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
    *error = true;
    *textError = "Не допустимые символы в формуле";
  }
  if (!*error)
  {
    data = strLH.toFloat(&numeric);
    if (numeric)
    {
      result->SetL(data);
    }
    else
    {
      result->SetL(signDo(strLH, error, textError));
    }
    data = strRH.toFloat(&numeric);
    if (numeric)
    {
      result->SetR(data);
    }
    else
    {
      result->SetR(signDo(strRH, error, textError));
    }
  }
  return result;
}

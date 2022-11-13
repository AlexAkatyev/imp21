
#include "checkInputNumberIF.h"

InputNumber checkInputNumberIF(QString strInput, NumberType t_num, int count)
{
  QString drobi;
  for (int i = 0; i < count; ++i)
    drobi.push_back("[0-9]?");
  InputNumber result;
  result.InfoError = "";
  result.fNumber = 0;
  result.iNumber = 0;
  QString stroka = strInput;
  stroka = stroka.trimmed(); // Удаление пробельных символов в начале и в конце строки (' ','\t','\n','\v','\f','\r')
  stroka.replace(",", "."); // Замена всех запятых на точки
  if (stroka.isEmpty())
  { // Строка пуста
    result.InfoError = "Поле ввода пусто";
  }
  else
  {
    if (t_num == NUMBER_INTEGER)
    {
      QRegExp regInt("[\\-|\\+]?[0-9]+");
      if (regInt.exactMatch(stroka))
      { // строка соответствует формату
        result.iNumber = stroka.toInt();
        result.fNumber = result.iNumber;
      }
      else
      { // строка формату не соответствует
        result.InfoError = "Содержимое поля ввода не соответствует\n формату целого числа";
      }
    }
    if (t_num == NUMBER_UNSIGNED_INTEGER)
    {
      QRegExp regUInt("[0-9]+");
      if (regUInt.exactMatch(stroka))
      { // строка соответствует формату
        result.iNumber = stroka.toInt();
        result.fNumber = result.iNumber;
      }
      else
      { // строка формату не соответствует
        result.InfoError = "Содержимое поля ввода не соответствует\n формату целого числа без знака";
      }
    }
    if (t_num == NUMBER_FLOAT)
    {
      QRegExp regFloat("[\\-|\\+]?[0-9]+[\\.]?" + drobi);
      if (regFloat.exactMatch(stroka))
      { // строка соответствует формату
        result.fNumber = stroka.toFloat();
      }
      else
      { // строка формату не соответствует
        result.InfoError = "Содержимое поля ввода не соответствует\n формату десятичной дроби.\nДолжно быть не более "
                           + QString::number(count)
                           + " знаков после запятой";
      }
    }
    if (t_num == NUMBER_UNSIGNED_FLOAT)
    {
      QRegExp regUFloat("[0-9]+[\\.]?" + drobi);
      if (regUFloat.exactMatch(stroka))
      { // строка соответствует формату
        result.fNumber = stroka.toFloat();
      }
      else
      { // строка формату не соответствует
        result.InfoError = "Содержимое поля ввода не соответствует\n формату десятичной дроби без знака.\nДолжно быть не более "
                           + QString::number(count)
                           + " знаков после запятой";
      }
    }
  }
  return result;
}

#include "utillib.h"

// Коррекция строки для вывода чисел в правильном формате
QString convertStringToCorrectCSV(QString strInput)
{
    QString result = strInput;
    for (int i=1; i<strInput.size()-1; i++)
    {
        if (result.at(i)==".") result.replace(i, 1, ",");
    }
    return result;
}


std::vector<QByteArray> splitByteArray(QByteArray input, QByteArray delimiter)
{
  std::vector<QByteArray> result;
  int currentIndex = 0;
  while (currentIndex < input.size())
  {
    int i = input.indexOf(delimiter, currentIndex);
    if (i == currentIndex)
      currentIndex += delimiter.size();
    else if (i > currentIndex)
    {
      QByteArray currentArray;
      while (i != currentIndex)
        currentArray.push_back(input.at(currentIndex++));
      result.push_back(currentArray);
    }
    else if (i == -1)
    {
      QByteArray currentArray;
      while (currentIndex < input.size())
        currentArray.push_back(input.at(currentIndex++));
      if (currentArray.size())
        result.push_back(currentArray);
    }
  }
  return result;
}



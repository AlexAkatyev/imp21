#include <QTextCodec>

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


QString getLocallyString(QByteArray baData, QByteArray codeLocally)
{
  QByteArray data = baData.replace('\0',' '); // Для корректной работы со строкой
  QTextCodec* codec = QTextCodec::codecForName(codeLocally);
  QString stroka = codec->toUnicode(data);
  return stroka.trimmed();
}


QByteArray setLocallyString(QString stroka, QByteArray codeLocally)
{
  QTextCodec* codec = QTextCodec::codecForName(codeLocally);
  return codec->fromUnicode(stroka);
}



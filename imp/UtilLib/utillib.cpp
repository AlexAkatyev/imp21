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

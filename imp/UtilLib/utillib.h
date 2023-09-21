/*

Это библиотека функций для преобразования форматов

*/

#include <vector>

#include <QString>
#include <QByteArray>

// Коррекция строки для вывода чисел в правильном формате
QString convertStringToCorrectCSV(QString strInput);

std::vector<QByteArray> splitByteArray(QByteArray input, QByteArray delimiter);


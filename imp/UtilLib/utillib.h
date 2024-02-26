/*

Это библиотека функций для преобразования форматов

*/

#include <vector>

#include <QString>
#include <QByteArray>

// Коррекция строки для вывода чисел в правильном формате
QString convertStringToCorrectCSV(QString strInput);

std::vector<QByteArray> splitByteArray(QByteArray input, QByteArray delimiter);

// convert Locally String
#define CODE_LOCALLY_DEF "Windows-1251"
QString getLocallyString(QByteArray baData, QByteArray codeLocally = CODE_LOCALLY_DEF);
QByteArray setLocallyString(QString stroka, QByteArray codeLocally = CODE_LOCALLY_DEF);

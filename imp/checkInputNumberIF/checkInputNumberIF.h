/*
 * 
 *   Функция проверки правильности ввода числа
 * 
 *                                        Автор А.П. Акатьев
 * 
 * 
 */
#pragma once

#include <QObject>

// Описание структуры на выходе функции
struct InputNumber
{
    QString InfoError;
    int iNumber;
    float fNumber;
};

enum NumberType {NUMBER_INTEGER, NUMBER_UNSIGNED_INTEGER, NUMBER_FLOAT, NUMBER_UNSIGNED_FLOAT};

// Описание функции
InputNumber checkInputNumberIF  (QString strInput // Введенная информация
                                 , NumberType t_num // Ожидаемый тип числа
                                 , int count = 2 // Количество знаков после запятой для десятичных дробей
                                );

/*
 * 
 *   ������� �������� ������������ ����� �����
 * 
 *                                        ����� �.�. �������
 * 
 * 
 */
#pragma once

#include <QObject>

// �������� ��������� �� ������ �������
struct InputNumber
{
    QString InfoError;
    int iNumber;
    float fNumber;
};

enum NumberType {NUMBER_INTEGER, NUMBER_UNSIGNED_INTEGER, NUMBER_FLOAT, NUMBER_UNSIGNED_FLOAT};

// �������� �������
InputNumber checkInputNumberIF  (QString strInput // ��������� ����������
                                 , NumberType t_num // ��������� ��� �����
                                 , int count = 2 // ���������� ������ ����� ������� ��� ���������� ������
                                );

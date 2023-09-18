#pragma once

#include <vector>
#include <QByteArray>

// Описание команд датчиков по протоколу MODBUS
// Чтение всех регистров с информацией о датчике
// Кадр должен быть дополнен 2 байтами CRC
const char COMMAND_READ_REGS  = 0x03;
const char ERROR_READ_REGS    = 0x83;
const char COMMAND_WRITE_REG  = 0x06;
const char ERROR_WRITE_REG    = 0x86;
const char COMMAND_WRITE_REGS = 0x10;
const char ERROR_WRITE_REGS   = 0x90;

struct MessageIndex
{
  int begin;
  int end;
};

// Предоставление общих функций пользователям
// Функция вычисления контрольной суммы CRC для формирования кадра MODBUS
//unsigned short Crc_chk(const char* data, unsigned int length);
// Функция создания MODBUS посылки - возвращает QByteArray, содержащий посылку
QByteArray ModbusCommandRead(int startReg, int regs, char modbusAddress);
std::vector<QByteArray> ModbusCommandsWrite(int startReg, QByteArray data, char modbusAddress);

MessageIndex GetSizeFirstReadMessage(QByteArray& input);


// MODBUS TCP
QByteArray baFromInt16(qint16 i);

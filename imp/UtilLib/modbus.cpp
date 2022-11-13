
/*
 * modbus.cpp
 *
 *  Created on: 02.05.2025
 *      Author: Aleksey Akatyev
 *
 * Предоставляет пользовательские функции для работы с датчиками modbus
 *
 */

#include "modbus.h"


// Функция вычисления контрольной суммы CRC для формирования кадра MODBUS
unsigned short Crc_chk(const char* data, unsigned int length)
{
    unsigned short reg_crc = 0xFFFF;
    while(length > 0){
        unsigned char temp = static_cast<unsigned char>(*data);
        reg_crc ^= static_cast<unsigned short>(temp);
        for(int j=0;j<8;j++){
            if (reg_crc & 0x0001) {reg_crc = (reg_crc >> 1)^0xA001;}
            else reg_crc = reg_crc >> 1;
        }
        data++;
        length--;
    }
    return reg_crc;
}


// Функция создания MODBUS посылки. Адрес начала передается в cmd, подставляется адрес MODBUS и добавляются два байта контрольной суммы CRC.
// Возвращается QByteArray с сформированным сообщением
QByteArray ModbusCommandRead(int startReg, int regs, char modbusAddress)
{
  QByteArray baTemp;
  baTemp.push_back(modbusAddress); // Установка Modbus Address
  baTemp.push_back(COMMAND_READ_REGS);
  baTemp.append(static_cast<char>((startReg & 0xFF00) >> 8));
  baTemp.append(static_cast<char>(startReg));
  baTemp.append(static_cast<char>((regs & 0xFF00) >> 8));
  baTemp.append(static_cast<char>(regs));
  unsigned short crc = Crc_chk(baTemp.constData(), static_cast<unsigned int>(baTemp.size()));
  baTemp.append(static_cast<char>(crc));                    // Установка low byte CRC
  baTemp.append(static_cast<char>((crc & 0xFF00) >> 8));    // Установка hi byte CRC
  return baTemp;
}


std::vector<QByteArray> ModbusCommandsWrite(int startReg, QByteArray data, char modbusAddress)
{
  std::vector<QByteArray> result;
  QByteArray nettoData;
  for (auto d : data)
    nettoData.push_back(d);
  int messageStartReg = startReg;
  while (nettoData.size() > 0)
  {
    QByteArray baTemp;
    baTemp.push_back(modbusAddress); // Установка Modbus Address
    if (nettoData.size() < 3)
    {
      baTemp.push_back(COMMAND_WRITE_REG);
      baTemp.append(static_cast<char>(messageStartReg));
      baTemp.append(static_cast<char>((messageStartReg & 0xFF00) >> 8));
      if (nettoData.size() == 1)
        nettoData.push_back(char(0));
      baTemp.push_back(nettoData);
      nettoData.clear();
    }
    else
    {
      baTemp.push_back(COMMAND_WRITE_REGS);
      baTemp.append(static_cast<char>(messageStartReg));
      baTemp.append(static_cast<char>((messageStartReg & 0xFF00) >> 8));
      int countByte = nettoData.size();
      countByte = countByte > 246 ? 246 : countByte;
      if (countByte % 2)
        --countByte;
      int countElement = countByte / 2;
      baTemp.append(static_cast<char>(countElement));
      baTemp.append(static_cast<char>((countElement & 0xFF00) >> 8));
      baTemp.append(static_cast<char>(countByte));
      for (int i = 0; i < countByte; ++i)
        baTemp.push_back(nettoData.at(i));
      nettoData = nettoData.remove(0, countByte);
      messageStartReg += countElement;
    }
    unsigned short crc = Crc_chk(baTemp.constData(), static_cast<unsigned int>(baTemp.size()));
    baTemp.append(static_cast<char>(crc));                    // Установка low byte CRC
    baTemp.append(static_cast<char>((crc & 0xFF00) >> 8));    // Установка hi byte CRC
    result.push_back(baTemp);
  }
  return result;
}


MessageIndex GetSizeFirstReadMessage(QByteArray& input)
{
  MessageIndex result;
  for (int i = 0; i < input.size(); ++i)
  {
    result.begin = i;
    result.end = i;
    if (input.at(i+1) & 0x80
        && input.size() >= i + 5
        && (char)input.at(i+2) < 0x0C)
    {
      int crr = (char)input.at(i+5) << 8;
      crr += input.at(i+4);
      int crc =  Crc_chk(input.begin()+i, 3);
      if (crr == crc)
      {
        result.end = i + 5;
        return result;
      }
    }
    if ((input.at(i+1) == COMMAND_READ_REGS
        || input.at(i+1) == COMMAND_WRITE_REG
        || input.at(i+1) == COMMAND_WRITE_REGS)
        && input.size() >= i + 3 + (char)input.at(i+2) + 2)
    {
      int end = i + 3 + (char)input.at(i+2) + 2;
      int crr = input.at(end) << 8;
      crr += input.at(end-1);
      int crc =  Crc_chk(input.begin()+i, 3 + input.at(i+2));
      if (crr == crc)
      {
        result.end = end;
        return result;
      }
    }
  }
  return result;
}

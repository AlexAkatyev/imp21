#include <math.h>

#include "impmessagecreator.h"

const int DATA_SCALE = 1000;
const char HEADER_C1 = 0x55;
const char HEADER_C2 = 0xAA;

ImpMessageCreator::ImpMessageCreator(QObject* parent)
  : QObject(parent)
{
}


QByteArray getSeparator()
{
  QByteArray result;
  for (int i = 0; i < 3; ++i)
  {
    result.push_back(HEADER_C1);
    result.push_back(HEADER_C2);
  }
  return result;
}


QByteArray getMeasure(ImpMessage message)
{
  QByteArray result;
  result.push_back(static_cast<char>(ImpMessageDataCaption::Measure));
  int measure = static_cast<int>(std::roundf(message.Measure * DATA_SCALE));
  result.push_back(static_cast<char>(measure >> 24));
  result.push_back(static_cast<char>(measure >> 16));
  result.push_back(static_cast<char>(measure >> 8));
  result.push_back(static_cast<char>(measure));
  return result;
}


QByteArray getMinMax(ImpMessage message)
{
  QByteArray result;
  result.push_back(static_cast<char>(ImpMessageDataCaption::MinMax));
  int min = static_cast<int>(std::roundf(message.Min * DATA_SCALE));
  result.push_back(static_cast<char>(min >> 24));
  result.push_back(static_cast<char>(min >> 16));
  result.push_back(static_cast<char>(min >> 8));
  result.push_back(static_cast<char>(min));
  int max = static_cast<int>(std::roundf(message.Max * DATA_SCALE));
  result.push_back(static_cast<char>(max >> 24));
  result.push_back(static_cast<char>(max >> 16));
  result.push_back(static_cast<char>(max >> 8));
  result.push_back(static_cast<char>(max));
  return result;
}


QByteArray getSelectGroup(ImpMessage message)
{
  QByteArray result;
  result.push_back(static_cast<char>(ImpMessageDataCaption::SelectGroup));
  result.push_back(static_cast<char>(message.SelectGroup));
  return result;
}


QByteArray indicatorMessage(ImpMessage message)
{
  QByteArray result;
  result.push_back(static_cast<char>(message.SenderId));
  if (message.Caption == ImpMessageDataCaption::Measure)
  {
    result.push_back(getMeasure(message));
  }
  else if (message.Caption == ImpMessageDataCaption::MinMax)
  {
    result.push_back(getMinMax(message));
  }
  else if (message.Caption == ImpMessageDataCaption::SelectGroup)
  {
    result.push_back(getSelectGroup(message));
  }
  else
  {
    result.push_back(static_cast<char>(ImpMessageDataCaption::Undefined));
  }
  return result;
}


QByteArray detectMessage(ImpMessage message)
{
  QByteArray result;
  result.push_back(static_cast<char>(message.SenderId));
  result.push_back(getMeasure(message));
  return result;
}


QByteArray ImpMessageCreator::Do(ImpMessage message)
{
  QByteArray result;
  QByteArray separator = getSeparator();
  result.push_back(separator);
  if (message.Sender == ImpMessageDataSender::Indicator)
  {
    result += static_cast<char>(message.Sender) + indicatorMessage(message);
  }
  else if (message.Sender == ImpMessageDataSender::Meter)
  {
    result += static_cast<char>(message.Sender) + detectMessage(message);
  }
  else
  {
    result.push_back(ImpMessageDataSender::Incognito);
  }
  return result + separator;
}

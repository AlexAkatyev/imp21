#ifndef IMPMESSAGECREATOR_H
#define IMPMESSAGECREATOR_H

#include <QObject>

enum ImpMessageDataSender
{
  Indicator = 0x00
  , Meter = 0x01
  , DataEmpty = 0xFF
};

enum ImpMessageDataCaption
{
  Measure = 0x00
  , MinMax = 0x01
  , SelectGroup = 0x02
  , Undefined = 0xFF
};

struct ImpMessage
{
  ImpMessageDataSender Sender;
  ImpMessageDataCaption Caption;
  int SenderId;
  float Measure;
  float Max;
  float Min;
  int GroupNumber;
  ImpMessage()
  {
    Sender = ImpMessageDataSender::Meter;
    Caption = ImpMessageDataCaption::Measure;
    SenderId = 0;
    Measure = 0;
  }
};

class ImpMessageCreator : public QObject
{
public:
  explicit ImpMessageCreator(QObject *parent = nullptr);
  QByteArray Do(ImpMessage);

  static ImpMessage EmptyMessage();
};

#endif // IMPMESSAGECREATOR_H

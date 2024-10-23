#ifndef POSTMESSAGESENDER_H
#define POSTMESSAGESENDER_H

enum DataSender
{
  Indicator
  , Meter
};


class PostMessageSender
{
public:
  static PostMessageSender* Instance();
  void Do(DataSender sender, int id, float data);

private:
  PostMessageSender();
  int getMesId(DataSender sender);
};

#endif // POSTMESSAGESENDER_H

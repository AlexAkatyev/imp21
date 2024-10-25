#ifndef POSTMESSAGESENDER_H
#define POSTMESSAGESENDER_H
#include <QObject>
#include <list>

class QTimer;

enum DataSender
{
  Indicator
  , Meter
};



class PostMessageSender : public QObject
{
Q_OBJECT
public:
  static PostMessageSender* Instance(QObject* parent);
  void Do(DataSender sender, int id, float data);

private:
  struct sendData
  {
    DataSender sender;
    int id;
    float data;
  };

  PostMessageSender(QObject* parent);
  int getMesId(DataSender sender, int repeater);
  void send();

  QTimer* _senderTimer;
  std::list<sendData> _sendData;
};

#endif // POSTMESSAGESENDER_H

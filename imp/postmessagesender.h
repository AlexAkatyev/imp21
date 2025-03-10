#ifndef POSTMESSAGESENDER_H
#define POSTMESSAGESENDER_H
#include <list>

#include <QObject>

#include "ImpMessage/impmessagecreator.h"

class QTimer;
class QTcpServer;
class QTcpSocket;

class PostMessageSender : public QObject
{
Q_OBJECT
  enum ExcelRequest
  {
    Used = 0
    , NotUsed = 1
    , Empty = 2
  };

public:
  static PostMessageSender* Instance(QObject* parent);
  void Do(ImpMessage message);

private:
  PostMessageSender(QObject* parent);

  void doNewConnection();
  bool isListening();
  void slotServerRead();
  void slotClientDisconnected();

  void send();
  void writeTimeOut();
  QByteArray createPost(ImpMessage);

  QTcpServer* _mTcpServer;
  QTimer* _waitingTimer;
  std::list<ImpMessage> _sendData;
  bool _isListen;
  QTcpSocket* _mSocket;
  ExcelRequest _request;
};

#endif // POSTMESSAGESENDER_H

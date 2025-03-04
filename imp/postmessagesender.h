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
  enum ExcelAnswer
  {
    Passed
    , Error
    , Waiting
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
  QTimer* _senderTimer;
  QTimer* _waitingTimer;
  std::list<ImpMessage> _sendData;
  bool _isListen;
  QTcpSocket* _mSocket;
  ExcelAnswer _answer;
};

#endif // POSTMESSAGESENDER_H

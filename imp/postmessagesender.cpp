#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QDebug>

#include "postmessagesender.h"

const int MESSAGE_PORT = 32789; // 0x8015
const int WAIT_RECEIVER = 10000;

PostMessageSender::PostMessageSender(QObject* parent)
  : QObject(parent)
  , _mTcpServer(new QTcpServer(this))
  , _waitingTimer(new QTimer(this))
  , _sendData(std::list<ImpMessage>())
  , _isListen(false)
  , _mSockets(std::list<QTcpSocket*>())
  , _request(ExcelRequest::Empty)
{
  _waitingTimer->setInterval(WAIT_RECEIVER);
  connect
      (
        _waitingTimer
        , &QTimer::timeout
        , this
        , &PostMessageSender::writeTimeOut
      );

  isListening();
  connect(_mTcpServer, &QTcpServer::newConnection, this, &PostMessageSender::doNewConnection);
}


PostMessageSender* PostMessageSender::Instance(QObject* parent)
{
  static PostMessageSender* instance = nullptr;
  if (instance == nullptr)
  {
    instance = new PostMessageSender(parent);
    qDebug() << "create sender" << parent;
  }
  return instance;
}


bool PostMessageSender::isListening()
{
  if (!_isListen)
  {

    _isListen = _mTcpServer->listen(QHostAddress::LocalHost, MESSAGE_PORT);
    qDebug() << "listen : " << _isListen;
  }
  return _isListen;
}


void PostMessageSender::Do(ImpMessage message)
{
  if (_mSockets.empty())
  {
    return;
  }
  _sendData.push_back(message);
  qDebug() << "add message";
}


void PostMessageSender::doNewConnection()
{
  QTcpSocket* mSocket = _mTcpServer->nextPendingConnection();
  connect(mSocket, &QTcpSocket::readyRead, this, [=]()
  {
    while(mSocket->bytesAvailable()>0)
    {
      QByteArray array = mSocket->readAll();
      _request = static_cast<ExcelRequest>(array.at(0));
      QTimer::singleShot(0, this, &PostMessageSender::send);
    }
  });
  connect(mSocket, &QTcpSocket::disconnected, this, [=]()
  {
    _mSockets.remove(mSocket);
    mSocket->close();
    mSocket->deleteLater();
    qDebug() << "socket disconnect : " << mSocket;
  });

  _mSockets.push_back(mSocket);
  qDebug() << "add socket : " << mSocket;
}


void PostMessageSender::send()
{
  _waitingTimer->start();
  if (_mSockets.empty()
      || !isListening())
  {
    return;
  }

  QByteArray toSend;
  if (_sendData.empty())
  {
    toSend.push_back(createPost(ImpMessageCreator::EmptyMessage()));
  }
  else
  {
    for (ImpMessage message : _sendData)
    {
      toSend.push_back(createPost(message));
    }
    _sendData.clear();
  }

  for (QTcpSocket* socket : _mSockets)
  {
    if (socket->isValid()
        && socket->state() == QAbstractSocket::SocketState::ConnectedState)
    {
      socket->write(toSend);
    }
  }
}


QByteArray PostMessageSender::createPost(ImpMessage message)
{
  ImpMessageCreator creator(this);
  return creator.Do(message);
}


void PostMessageSender::writeTimeOut()
{
  if (_sendData.empty())
  {
    return;
  }
  _sendData.clear();
  qDebug() << "delete messages after waiting";
  _request = ExcelRequest::Empty;
}

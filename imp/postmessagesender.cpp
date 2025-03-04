#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QDebug>

#include "postmessagesender.h"

const int MESSAGE_PORT = 32789; // 0x8015
const int SEND_INTERVAL = 100;
const int WAIT_ANSWER = 10000;
const char ANSWER_PASSED = 0x01;
const char ANSWER_ERROR = 0x02;

PostMessageSender::PostMessageSender(QObject* parent)
  : QObject(parent)
  , _mTcpServer(new QTcpServer(this))
  , _senderTimer(new QTimer(this))
  , _waitingTimer(new QTimer(this))
  , _sendData(std::list<ImpMessage>())
  , _isListen(false)
  , _mSocket(nullptr)
  , _answer(ExcelAnswer::Passed)
{
  _senderTimer->setInterval(SEND_INTERVAL);
  connect
      (
        _senderTimer
        , &QTimer::timeout
        , this
        , &PostMessageSender::send
      );
  _waitingTimer->setInterval(WAIT_ANSWER);
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
  _sendData.push_back(message);
  qDebug() << "add message";
  if (!_senderTimer->isActive())
  {
    _senderTimer->start();
  }
}


void PostMessageSender::doNewConnection()
{
  if (_mSocket)
  {
    return;
  }
  _mSocket = _mTcpServer->nextPendingConnection();
  connect(_mSocket, &QTcpSocket::readyRead, this, &PostMessageSender::slotServerRead);
  connect(_mSocket, &QTcpSocket::disconnected, this, &PostMessageSender::slotClientDisconnected);
  qDebug() << "add socket";
}


void PostMessageSender::slotServerRead()
{
    while(_mSocket->bytesAvailable()>0)
    {
        QByteArray array = _mSocket->readAll();
        if (_answer == ExcelAnswer::Waiting)
        {
          if (array.contains(ANSWER_PASSED))
          {
            _answer = ExcelAnswer::Passed;
            _waitingTimer->stop();
            if (!_sendData.empty())
            {
              _sendData.pop_front();
              qDebug() << "delete message";
            }
          }
          else if (array.contains(ANSWER_ERROR))
          {
            _answer = ExcelAnswer::Error;
          }
        }
    }
}


void PostMessageSender::slotClientDisconnected()
{
    _mSocket->close();
    _mSocket->deleteLater();
    _mSocket = nullptr;
    qDebug() << "socket disconnect";
}


void PostMessageSender::send()
{
  if (_sendData.empty())
  {
    _senderTimer->stop();
    _waitingTimer->stop();
    return;
  }
  if (!_mSocket
      || !isListening())
  {
    return;
  }

  if (_answer == ExcelAnswer::Passed)
  {
    _answer = ExcelAnswer::Waiting;
    _waitingTimer->start();
    _mSocket->write(createPost(*_sendData.begin()));
  }

  if (_answer == ExcelAnswer::Error)
  {
    _answer = ExcelAnswer::Waiting;
    _mSocket->write(createPost(*_sendData.begin()));
  }
}


QByteArray PostMessageSender::createPost(ImpMessage message)
{
  ImpMessageCreator creator(this);
  return creator.Do(message);
}


void PostMessageSender::writeTimeOut()
{
  _sendData.pop_front();
  qDebug() << "delete message after waiting";
  _answer = ExcelAnswer::Passed;
  _waitingTimer->stop();
}

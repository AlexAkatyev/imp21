#include <QTimer>
#include <QDebug>

#include "postmessagesender.h"
#include <windows.h>

const int WM_IMP_INDICATOR_MESSAGE_ID = WM_APP + 0x15;
const int WM_IMP_METER_MESSAGE_ID = WM_IMP_INDICATOR_MESSAGE_ID + 10;
const int ROUNDING = 1000;
const int SEND_INTERVAL = 100;
const int DO_NOT_ACKNOWLEDGE = 1;
const int ATTEMPTS_COUNT = 2;

PostMessageSender::PostMessageSender(QObject* parent)
  : QObject(parent)
  , _senderTimer(new QTimer(this))
  , _sendData(std::list<sendData>())
{
  _senderTimer->setInterval(SEND_INTERVAL);
  connect
      (
        _senderTimer
        , &QTimer::timeout
        , this
        , &PostMessageSender::send
      );
}


PostMessageSender* PostMessageSender::Instance(QObject* parent)
{
  static PostMessageSender* instance = nullptr;
  if (instance == nullptr)
  {
    instance = new PostMessageSender(parent);
  }
  return instance;
}


void PostMessageSender::Do(DataSender sender, int id, float data)
{
  sendData d;
  d.sender = sender;
  d.id = id;
  d.data = data;
  _sendData.push_back(d);
  if (!_senderTimer->isActive())
  {
    _senderTimer->start();
  }
}


int PostMessageSender::getMesId(DataSender sender, int repeater)
{
  auto fixId = [](int baseCode, int repeater)
  {
    return baseCode + repeater - 1;
  };

  if (sender == DataSender::Meter)
  {
    return fixId(WM_IMP_METER_MESSAGE_ID, repeater);
  }
  return fixId(WM_IMP_INDICATOR_MESSAGE_ID, repeater);
}


void PostMessageSender::send()
{
  static int repeaterCount = 0;
  if (_sendData.empty())
  {
    _senderTimer->stop();
    return;
  }

  sendData d = *_sendData.begin();
  int i = d.data * ROUNDING;
  WPARAM wParam = d.id;
  HWND wndHndl = FindWindow(L"XLMAIN", 0); // Notepad  XLMAIN
  if (wndHndl == NULL)
  {
    wndHndl = HWND_BROADCAST;
    repeaterCount = DO_NOT_ACKNOWLEDGE;
  }
  else
  {
    if (repeaterCount == 0)
    {
      repeaterCount = ATTEMPTS_COUNT;
    }
  }

  PostMessage
      (
        wndHndl
        , getMesId(d.sender, repeaterCount)
        , wParam
        , (LPARAM)i
      );
  qDebug()
      << "PostMessage"
      << ((wndHndl == HWND_BROADCAST) ? "broadcast" : "xlmain")
      << QString::number(getMesId(d.sender, repeaterCount))
      << QString::number(wParam)
      << QString::number((LPARAM)i);

  --repeaterCount;
  if (repeaterCount == 0)
  {
    _sendData.pop_front();
  }
}

#include <QTimer>

#include "postmessagesender.h"
#include <windows.h>

const int WM_IMP_INDICATOR_MESSAGE_ID = WM_APP + 0x15;
const int WM_IMP_METER_MESSAGE_ID = WM_IMP_INDICATOR_MESSAGE_ID + 1;
const int ROUNDING = 1000;
const int SEND_INTERVAL = 45;

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


int PostMessageSender::getMesId(DataSender sender)
{
  if (sender == DataSender::Meter)
  {
    return WM_IMP_METER_MESSAGE_ID;
  }
  return WM_IMP_INDICATOR_MESSAGE_ID;
}


void PostMessageSender::send()
{
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
  }
  PostMessage
      (
        wndHndl
        , getMesId(d.sender)
        , wParam
        , (LPARAM)i
      );

  _sendData.pop_front();
}

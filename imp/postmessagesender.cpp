#include "postmessagesender.h"
#include <windows.h>

const int WM_IMP_INDICATOR_MESSAGE_ID = WM_APP + 0x15;
const int WM_IMP_METER_MESSAGE_ID = WM_IMP_INDICATOR_MESSAGE_ID + 1;
const int ROUNDING = 1000;

PostMessageSender::PostMessageSender()
{
}


PostMessageSender* PostMessageSender::Instance()
{
  static PostMessageSender* instance = nullptr;
  if (instance == nullptr)
  {
    instance = new PostMessageSender();
  }
  return instance;
}


void PostMessageSender::Do(DataSender sender, int id, float data)
{
  float m = data * ROUNDING;
  int i = m;
  WPARAM wParam = id;
  HWND wndHndl = FindWindow(L"XLMAIN", 0); // Notepad  XLMAIN
  if (wndHndl == NULL)
  {
    wndHndl = HWND_BROADCAST;
  }
  PostMessage
      (
        wndHndl
        , getMesId(sender)
        , wParam
        , (LPARAM)i
      );
}


int PostMessageSender::getMesId(DataSender sender)
{
  if (sender == DataSender::Meter)
  {
    return WM_IMP_METER_MESSAGE_ID;
  }
  return WM_IMP_INDICATOR_MESSAGE_ID;
}

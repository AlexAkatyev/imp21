
#include <QFile>
#include <QDateTime>
#include <QTextStream>

#include "logger.h"

Logger::Logger(QObject *parent)
  : QObject(parent)
  , _log(new QFile("imp.log", parent))
{
  _log->open(QIODevice::WriteOnly);
  WriteLnLog("Начало записи " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));
}


Logger* Logger::GetInstance(QObject *parent)
{
  static Logger* logger = nullptr;
  if (!logger && parent)
    logger = new Logger(parent);
  return logger;
}


void Logger::WriteLog(QString input)
{
  if(_log
     && !_log->isOpen())
    return;

  QTextStream out(_log);
  out.setCodec("Windows-1251");
  out << input;
  _log->flush();
}


void Logger::WriteLnLog(QString input)
{
  QString dateTime =QDateTime::currentDateTime().toString("HH:mm:zzz");
  WriteLog(dateTime + " | " + input + "\n");
}


void Logger::WriteBytes(QByteArray input)
{
  QString strOut;
  for (int i = 0; i < input.length(); ++i)
  {
    uchar d = input[i];
    strOut += "0x" + QString::number(d, 16);
    if ((i + 1) % 16)
    {
      strOut += " ";
    }
    else
    {
      strOut += "\n";
    }
  }
  WriteLnLog(strOut);
}


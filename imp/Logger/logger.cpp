
#include <QFile>
#include <QDateTime>
#include <QTextStream>

#include "logger.h"

Logger::Logger(QObject *parent)
  : QObject(parent)
  , _log(new QFile("imp21log.txt", parent))
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
  if(!_log->isOpen())
    return;

  QTextStream out(_log);
  out.setCodec("Windows-1251");
  out << input;
  _log->flush();
}


void Logger::WriteLnLog(QString input)
{
  WriteLog(input + "\n");
}


void Logger::WriteBytes(QByteArray input)
{
  _log->write(input);
  WriteLnLog();
}


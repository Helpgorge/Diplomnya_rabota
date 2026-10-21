#pragma once

#include <QFile>
#include <QMutex>
#include <QString>
#include <QtGlobal>

class Logger
{
  public:
    static void initialize();
    static void shutdown();

  private:
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message);

    static QFile logFile;
    static QMutex mutex;
};
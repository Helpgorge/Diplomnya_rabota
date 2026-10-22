#include "Logger.h"

#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QTextStream>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <cstdio>
#endif

QFile Logger::logFile;
QMutex Logger::mutex;

void Logger::initialize()
{
    const QString logDirectory = QCoreApplication::applicationDirPath() + "/logs";

    QDir().mkpath(logDirectory);

    const QString filename =
        logDirectory + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss-zzz") + ".log";

    logFile.setFileName(filename);

    if (!logFile.open(QIODevice::WriteOnly | QIODevice::NewOnly | QIODevice::Text))
    {
        fprintf(stderr, "Cannot create log file\n");
    }

    qInstallMessageHandler(Logger::messageHandler);
}

void Logger::shutdown()
{
    qInstallMessageHandler(nullptr);

    QMutexLocker locker(&mutex);
    if (logFile.isOpen()) logFile.close();
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QMutexLocker locker(&mutex);

    QString level;

    switch (type)
    {
        case QtDebugMsg:
            level = "DEBUG";
            break;
        case QtInfoMsg:
            level = "INFO";
            break;
        case QtWarningMsg:
            level = "WARNING";
            break;
        case QtCriticalMsg:
            level = "ERROR";
            break;
        case QtFatalMsg:
            level = "FATAL";
            break;
    }

    const QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    const QString category = QString::fromUtf8(context.category ? context.category : "default");

    const QString line = QString("%1 [%2] [%3] %4").arg(timestamp, level, category, message);

    if (logFile.isOpen())
    {
        QTextStream stream(&logFile);
        stream << line << '\n';
        stream.flush();
    }

#ifdef Q_OS_WIN
    OutputDebugStringW(reinterpret_cast<const wchar_t*>(line.utf16()));
    OutputDebugStringW(L"\n");
#else
    fprintf(stderr, "%s\n", line.toLocal8Bit().constData());
#endif

    if (type == QtFatalMsg) abort();
}
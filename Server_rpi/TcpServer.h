#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(QObject* parent = nullptr);

    bool start(quint16 port);
    void stop();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    QTcpServer* server = nullptr;
    QTcpSocket* client = nullptr;
};
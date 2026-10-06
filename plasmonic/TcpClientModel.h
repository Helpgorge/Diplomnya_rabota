#pragma once

#include <QObject>
#include <QTcpSocket>

class TcpClientModel : public QObject
{
    Q_OBJECT

public:
    explicit TcpClientModel(QObject* parent = nullptr);
    ~TcpClientModel();

    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();

    bool isConnected() const;

signals:
    void connected();
    void disconnected();

    void dataReceived(const QByteArray& data);

    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QTcpSocket* socket;
};
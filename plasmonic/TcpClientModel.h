#pragma once

#include <QObject>
#include <QTcpSocket>

class TcpClientModel : public QObject
{
    Q_OBJECT

public:
    explicit TcpClientModel(QObject* parent = nullptr);
    ~TcpClientModel();

    void connectToServer();
    void disconnectFromServer();
    bool isConnected() const;

signals:
    void connected();
    void disconnected(bool requestedByUser);
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QTcpSocket* socket;
    bool disconnectRequested = false;
    static constexpr const char* ServerIp = "192.168.50.1";
    static constexpr quint16 ServerPort = 5000;
};
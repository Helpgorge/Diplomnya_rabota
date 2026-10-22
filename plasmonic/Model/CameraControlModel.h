#pragma once

#include <QObject>
#include <QTcpSocket>

class CameraControlModel : public QObject
{
    Q_OBJECT

  public:
    explicit CameraControlModel(QObject* parent = nullptr);

    // Управление подлючением по TCP
    void connectToServer();
    void disconnectFromServer();
    bool isConnected() const;

    // Управляющие камерой, команды
    void setExposure(quint32 exposure);
    void setGain(float gain);

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
    enum class Command : quint8
    {
        SetExposure = 0x01,
        SetGain = 0x02
    };

    QTcpSocket* socket;
    bool disconnectRequested = false;
    static constexpr const char* ServerIp = "192.168.50.1";
    static constexpr quint16 ServerPort = 5000;
};
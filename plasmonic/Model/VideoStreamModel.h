#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>

class VideoStreamModel : public QObject
{
    Q_OBJECT

  public:
    explicit VideoStreamModel(QObject* parent = nullptr);

    void connectToServer();
    void disconnectFromServer();

  signals:
    void frameReceived(QByteArray& frame);
    void connectionError(const QString& message);
    void connected();

  private:
    void receiveFrames();
    void handleConnectionError();

    QString m_host = "192.168.50.1";
    quint16 m_port = 5000;
    QTcpSocket* m_socket = nullptr;

    static constexpr int FrameSize = 2592000;
    QByteArray m_receiveBuffer;
    QByteArray m_frame;
};
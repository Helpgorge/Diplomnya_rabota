#include "TcpClientModel.h"

TcpClientModel::TcpClientModel(QObject* parent): QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &TcpClientModel::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClientModel::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClientModel::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpClientModel::onErrorOccurred);
}

void TcpClientModel::connectToServer()
{
    disconnectRequested = false;
    socket->connectToHost(ServerIp, ServerPort);
}

void TcpClientModel::disconnectFromServer()
{
    disconnectRequested = true;
    socket->disconnectFromHost();
}

bool TcpClientModel::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

void TcpClientModel::onConnected()
{
    emit connected();
}

void TcpClientModel::onDisconnected()
{
    emit disconnected(disconnectRequested);
    disconnectRequested = false;
}

void TcpClientModel::onReadyRead()
{
    QByteArray data = socket->readAll();

    emit dataReceived(data);
}

void TcpClientModel::onErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    emit errorOccurred(socket->errorString());
}

void TcpClientModel::startStream()
{
    const char command = static_cast<char>(Command::StartStream);
    socket->write(&command, 1);
}

void TcpClientModel::stopStream()
{
    const char command = static_cast<char>(Command::StopStream);
    socket->write(&command, 1);
}

void TcpClientModel::setExposure(quint32 exposure)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream.setByteOrder(QDataStream::BigEndian);

    stream << static_cast<quint8>(Command::SetExposure);
    stream << exposure;

    socket->write(data);
}

void TcpClientModel::setGain(float gain)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << static_cast<quint8>(Command::SetGain);
    stream << gain;

    socket->write(data);
}
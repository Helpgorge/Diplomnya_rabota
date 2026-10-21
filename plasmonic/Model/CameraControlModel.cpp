#include "CameraControlModel.h"

CameraControlModel::CameraControlModel(QObject* parent) : QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &CameraControlModel::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &CameraControlModel::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &CameraControlModel::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &CameraControlModel::onErrorOccurred);
}

void CameraControlModel::connectToServer()
{
    disconnectRequested = false;
    socket->connectToHost(ServerIp, ServerPort);
}

void CameraControlModel::disconnectFromServer()
{
    disconnectRequested = true;
    socket->disconnectFromHost();
}

bool CameraControlModel::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

void CameraControlModel::onConnected()
{
    emit connected();
}

void CameraControlModel::onDisconnected()
{
    emit disconnected(disconnectRequested);
    disconnectRequested = false;
}

void CameraControlModel::onReadyRead()
{
    QByteArray data = socket->readAll();

    emit dataReceived(data);
}

void CameraControlModel::onErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    emit errorOccurred(socket->errorString());
}

void CameraControlModel::startStream()
{
    const char command = static_cast<char>(Command::StartStream);
    socket->write(&command, 1);
}

void CameraControlModel::stopStream()
{
    const char command = static_cast<char>(Command::StopStream);
    socket->write(&command, 1);
}

void CameraControlModel::setExposure(quint32 exposure)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream.setByteOrder(QDataStream::BigEndian);

    stream << static_cast<quint8>(Command::SetExposure);
    stream << exposure;

    socket->write(data);
}

void CameraControlModel::setGain(float gain)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << static_cast<quint8>(Command::SetGain);
    stream << gain;

    socket->write(data);
}
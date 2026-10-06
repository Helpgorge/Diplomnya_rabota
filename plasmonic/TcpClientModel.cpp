#include "TcpClientModel.h"

TcpClientModel::TcpClientModel(QObject* parent): QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &TcpClientModel::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClientModel::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClientModel::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpClientModel::onErrorOccurred);
}

TcpClientModel::~TcpClientModel()
{
}

void TcpClientModel::connectToServer(const QString& host, quint16 port)
{
    disconnectRequested = false;
    socket->connectToHost(host, port);
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
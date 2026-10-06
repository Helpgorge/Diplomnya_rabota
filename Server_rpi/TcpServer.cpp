#include "TcpServer.h"

#include <QHostAddress>
#include <QDebug>

TcpServer::TcpServer(QObject* parent)
    : QObject(parent)
{
    server = new QTcpServer(this);

    connect(
        server,
        &QTcpServer::newConnection,
        this,
        &TcpServer::onNewConnection
    );
}

bool TcpServer::start(quint16 port)
{
    if (!server->listen(QHostAddress::Any, port))
    {
        qCritical() << "Failed to start TCP server:"
            << server->errorString();

        return false;
    }

    qDebug() << "TCP server started.";
    qDebug() << "Port:" << server->serverPort();

    return true;
}

void TcpServer::stop()
{
    if (client)
        client->disconnectFromHost();

    server->close();

    qDebug() << "TCP server stopped.";
}

void TcpServer::onNewConnection()
{
    client = server->nextPendingConnection();

    qDebug() << "Client connected:"
        << client->peerAddress().toString()
        << "port:"
        << client->peerPort();

    connect(
        client,
        &QTcpSocket::readyRead,
        this,
        &TcpServer::onReadyRead
    );

    connect(
        client,
        &QTcpSocket::disconnected,
        this,
        &TcpServer::onClientDisconnected
    );

    client->write("SERVER_CONNECTED\n");
}

void TcpServer::onReadyRead()
{
    QByteArray data = client->readAll();

    qDebug() << "Received from client:" << data;

    // Temporary test response.
    client->write("OK\n");
}

void TcpServer::onClientDisconnected()
{
    qDebug() << "Client disconnected.";

    client->deleteLater();
    client = nullptr;
}
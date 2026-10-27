#include "videostreammodel.h"

VideoStreamModel::VideoStreamModel(QObject* parent) : QObject(parent)
{
}

void VideoStreamModel::connectToServer()
{
    if (!m_socket)
    {
        m_socket = new QTcpSocket(this);

        connect(m_socket, &QTcpSocket::readyRead, this, &VideoStreamModel::receiveFrames);
        connect(m_socket, &QTcpSocket::errorOccurred, this, &VideoStreamModel::handleConnectionError);
        connect(m_socket, &QTcpSocket::connected, this, &VideoStreamModel::connected);
    }

    if (m_socket->state() != QAbstractSocket::UnconnectedState) return;

    m_socket->connectToHost(m_host, m_port);
}

void VideoStreamModel::disconnectFromServer()
{
    if (!m_socket) return;

    m_socket->disconnectFromHost();
}

void VideoStreamModel::receiveFrames()
{
    if (!m_socket) return;
    m_receiveBuffer.append(m_socket->readAll());

    while (m_receiveBuffer.size() >= FrameSize)
    {
        m_frame = m_receiveBuffer.left(FrameSize);
        m_receiveBuffer.remove(0, FrameSize);

        emit frameReceived(m_frame);
    }
}

void VideoStreamModel::handleConnectionError()
{
    if (!m_socket) return;

    const QString errorMessage = m_socket->errorString();

    m_socket->abort();
    m_receiveBuffer.clear();
    m_frame.clear();

    emit connectionError(errorMessage);
}
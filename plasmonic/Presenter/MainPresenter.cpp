#include "MainPresenter.h"
#include "MainWindow.h"
#include "TcpClientModel.h"

#include <QThread>


MainPresenter::MainPresenter(MainWindow* view, TcpClientModel* tcpClient): view(view), tcpClient(tcpClient) 
{
    QObject::connect(tcpClient, &TcpClientModel::connected, this, &MainPresenter::onConnected);
    QObject::connect(tcpClient, &TcpClientModel::disconnected, this, &MainPresenter::onDisconnected);
    QObject::connect(tcpClient, &TcpClientModel::errorOccurred, this, &MainPresenter::onErrorOccurred);
    
    QObject::connect(view, &MainWindow::connectRequested, this, &MainPresenter::onConnectRequested);
    QObject::connect(view, &MainWindow::disconnectRequested, this, &MainPresenter::onDisconnectRequested);
}

void MainPresenter::onConnected()
{
    view->setConnectionStatus("Connected to server.");
    QThread::msleep(700);
    view->closeConnectionProgress();
    view->setConnected(true, false);
    testSendingCommand();
}

void MainPresenter::onDisconnected(bool requestedByUser)
{
    qDebug() << "Disconnected from server.";
    if (!requestedByUser)
    {
        view->showConnectionLost();
    }
    view->setConnected(false, false);
}

void MainPresenter::onErrorOccurred(const QString& error)
{
    view->setConnectionStatus("Connection failed: " + error);
    view->closeConnectionProgress();
    view->setConnected(false, false);
}

void MainPresenter::onConnectRequested()
{
    view->setConnected(true, true);
    view->showConnectionProgress();
    view->setConnectionStatus("Establishing TCP connection...");
    tcpClient->connectToServer();
}

void MainPresenter::onDisconnectRequested()
{
    view->setConnected(false, true);
    tcpClient->disconnectFromServer();
}

void MainPresenter::testSendingCommand()
{
    tcpClient->startStream();
    tcpClient->setExposure(100);
    tcpClient->setGain(1.5);
    tcpClient->stopStream();
}
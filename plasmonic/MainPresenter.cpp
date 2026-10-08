#include "MainPresenter.h"
#include "MainWindow.h"
#include "TcpClientModel.h"

#include <QThread>


MainPresenter::MainPresenter(MainWindow* view, TcpClientModel* tcpClient)
    : view(view), tcpClient(tcpClient)
{
    QObject::connect(view, &MainWindow::connectRequested, view, [this]()
        {
            onConnectRequested();
        });

    QObject::connect(view, &MainWindow::disconnectRequested, view, [this]()
        {
            onDisconnectRequested();
        });

    QObject::connect(tcpClient, &TcpClientModel::connected, view, [this]()
        {
            this->view->setConnectionStatus("Connected to server.");
            QThread::msleep(700);
            this->view->closeConnectionProgress();
            this->view->setConnected(true, false);
        });

    QObject::connect(tcpClient, &TcpClientModel::errorOccurred, view, [this](const QString& error)
        {
            this->view->setConnectionStatus("Connection failed: " + error);
            this->view->closeConnectionProgress();
            this->view->setConnected(false, false);
        });

    QObject::connect(tcpClient, &TcpClientModel::disconnected, view, [this](bool requestedByUser)
        {
            qDebug() << "Disconnected from server.";
            if (!requestedByUser)
            {
                this->view->showConnectionLost();
            }
            this->view->setConnected(false, false);
        });
}

MainPresenter::~MainPresenter()
{
}

void MainPresenter::onConnectRequested()
{
    view->setConnected(true, true);
    view->showConnectionProgress();
    this->view->setConnectionStatus("Establishing TCP connection...");
    this->tcpClient->connectToServer();
}

void MainPresenter::onDisconnectRequested()
{
    view->setConnected(false, true);
    tcpClient->disconnectFromServer();
}
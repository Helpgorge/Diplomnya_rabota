#pragma once

#include <QObject>

class MainWindow;
class TcpClientModel;

class MainPresenter : public QObject
{
public:
    explicit MainPresenter(MainWindow* view, TcpClientModel* tcpClient);

private:
    MainWindow* view;
    TcpClientModel* tcpClient;

    void onConnected();
    void onDisconnected(bool requestedByUser);
    void onErrorOccurred(const QString& error);
    void onConnectRequested();
    void onDisconnectRequested();

    void testSendingCommand();
};
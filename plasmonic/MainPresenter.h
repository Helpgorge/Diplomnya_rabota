#pragma once

class MainWindow;
class TcpClientModel;

class MainPresenter
{
public:
    explicit MainPresenter(MainWindow* view, TcpClientModel* tcpClient);
    ~MainPresenter();

private:
    MainWindow* view;
    TcpClientModel* tcpClient;

    void onConnectRequested();
    void onDisconnectRequested();
};
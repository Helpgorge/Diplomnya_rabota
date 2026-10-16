#pragma once

#include <QMainWindow>
#include <QProgressDialog>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    void showConnectionProgress();
    void setConnectionStatus(const QString& text);
    void closeConnectionProgress();
    void setConnected(bool connected, bool inProgress);
    void showConnectionLost();

signals:
    void connectRequested();
    void disconnectRequested();

private slots:
    void onConnectClicked();
    void onDisconnectClicked();

private:
    QProgressDialog* connectionDialog = nullptr;
    QAction* connectTcpAction = nullptr;
    QAction* disconnectTcpAction = nullptr;
};
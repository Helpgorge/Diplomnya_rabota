#pragma once

#include <QMainWindow>
#include <QProgressDialog>

class VideoWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);

    void setConnectionStatus(const QString& text);
    void setConnected(bool connected, bool inProgress);

    void showConnectionProgress();
    void showConnectionLost();
    void showVideoWindow();
    void showSubtractionWindow();

    void closeConnectionProgress();

  signals:
    void connectRequested();
    void disconnectRequested();

    void showVideoRequested();
    void showSubtractionRequested();

  private slots:
    void onConnectClicked();
    void onDisconnectClicked();

  private:
    QAction* connectTcpAction = nullptr;
    QAction* disconnectTcpAction = nullptr;
    QAction* showVideoAction = nullptr;
    QAction* showSubtractionAction = nullptr;

    QProgressDialog* connectionDialog = nullptr;
    VideoWindow* videoWindow = nullptr;
    VideoWindow* subtractionWindow = nullptr;
};
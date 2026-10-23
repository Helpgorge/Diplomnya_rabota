#pragma once

#include <QMainWindow>
#include <QProgressDialog>

class VideoWindow;
class ControlCameraWindow;

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
    void showControlCameraWindow();

    void closeConnectionProgress();

    VideoWindow* getVideoWindow() const;
    VideoWindow* getSubtractionWindow() const;

  signals:
    void connectRequested();
    void disconnectRequested();

    void showVideoRequested();
    void showSubtractionRequested();
    void showCameraControlRequested();

    void exposureChanged(quint32 exposure);
    void gainChanged(float gain);

  private slots:
    void onConnectClicked();
    void onDisconnectClicked();

  private:
    QAction* connectTcpAction = nullptr;
    QAction* disconnectTcpAction = nullptr;
    QAction* showVideoAction = nullptr;
    QAction* showSubtractionAction = nullptr;
    QAction* cameraAction = nullptr;

    QProgressDialog* connectionDialog = nullptr;
    VideoWindow* videoWindow = nullptr;
    VideoWindow* subtractionWindow = nullptr;
    ControlCameraWindow* controlCameraWindow = nullptr;
};
#pragma once

#include <QMainWindow>
#include <QProgressDialog>

class VideoWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);

    void setConnected(bool connected, bool inProgress);

    void showConnectionLost(const QString& message);
    void showVideoWindow();
    void showSubtractionWindow();

    VideoWindow* getVideoWindow() const;
    VideoWindow* getSubtractionWindow() const;

    void setStartRecordingEnabled(bool enabled);
    void setStopRecordingEnabled(bool enabled);
    void setSaveSelectedFrameEnabled(bool enabled);

  signals:
    void connectRequested();
    void disconnectRequested();

    void showVideoRequested();
    void showSubtractionRequested();

    void selectReferenceFrameRequested();

    void startRecordingRequested();
    void stopRecordingRequested();
    void saveSelectedFrameRequested();

  private:
    QAction* connectTcpAction = nullptr;
    QAction* disconnectTcpAction = nullptr;

    QAction* showVideoAction = nullptr;
    QAction* showSubtractionAction = nullptr;

    QAction* selectReferenceFrameAction = nullptr;

    QAction* startRecordingAction;
    QAction* stopRecordingAction;
    QAction* saveSelectedFrameAction;

    VideoWindow* videoWindow = nullptr;
    VideoWindow* subtractionWindow = nullptr;
};
#pragma once

#include <QObject>

// для тетста
#include <chrono>
#include <cstdint>

class MainWindow;
class CameraControlModel;
class VideoStreamModel;
class VideoPresenter;

class MainPresenter : public QObject
{
  public:
    explicit MainPresenter(MainWindow* view, CameraControlModel* сameraControlModel, VideoStreamModel* videoStreamModel,
                           VideoPresenter* videoPresenter);

  private slots:
    // TCP.
    void onConnected();
    void onDisconnected(bool requestedByUser);
    void onErrorOccurred(const QString& error);

    // Запросы пользователя.
    void onConnectRequested();
    void onDisconnectRequested();

    // GStreamer.
    void onFrameReceived(const QByteArray& frame);
    void onStreamError(const QString& error);
    void onStreamStopped();

    // Окна.
    void onShowVideoRequested();
    void onShowSubtractionRequested();
    void onShowCameraControlRequested();

  private:
    void disconnectAll();

    MainWindow* view = nullptr;
    CameraControlModel* сameraControlModel = nullptr;
    VideoStreamModel* videoStreamModel = nullptr;
    VideoPresenter* videoPresenter = nullptr;

    bool connecting = false;
    bool tcpConnected = false;
    bool streamReady = false;
    bool disconnecting = false;

    // Тест

    using Clock = std::chrono::steady_clock;

    Clock::time_point lastReport_ = Clock::now();

    uint64_t totalFrames_ = 0;
    uint64_t intervalFrames_ = 0;
    uint64_t intervalBytes_ = 0;

    void updateFrameStatistics(size_t bytes);
};
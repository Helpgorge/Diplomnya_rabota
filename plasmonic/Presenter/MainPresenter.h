#pragma once

#include <QObject>

class MainWindow;
class CameraControlModel;
class VideoStreamModel;

class MainPresenter : public QObject
{
  public:
    explicit MainPresenter(MainWindow* view, CameraControlModel* сameraControlModel,
                           VideoStreamModel* videoStreamModel);

  private slots:
    // TCP
    void onConnected();
    void onDisconnected(bool requestedByUser);
    void onErrorOccurred(const QString& error);

    // Управление камерой из окна
    void onGainChanged(float gain);
    void onExposureChanged(quint32 exposure);

    // Запросы пользователя
    void onConnectRequested();
    void onDisconnectRequested();

    // GStreamer
    void onFrameReceived(const QByteArray& frame);
    void onStreamError(const QString& error);
    void onStreamStopped();

    // Окна
    void onShowCameraControlRequested();

  private:
    void disconnectAll();

    MainWindow* view = nullptr;
    CameraControlModel* сameraControlModel = nullptr;
    VideoStreamModel* videoStreamModel = nullptr;

    bool connecting = false;
    bool tcpConnected = false;
    bool streamReady = false;
    bool disconnecting = false;
};
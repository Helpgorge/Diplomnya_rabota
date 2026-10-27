#pragma once

#include <QObject>

class MainWindow;
class VideoStreamModel;

class MainPresenter : public QObject
{
  public:
    explicit MainPresenter(MainWindow* view, VideoStreamModel* videoStreamModel);

  private slots:
    // Запросы пользователя
    void onConnectRequested();
    void onDisconnectRequested();

    // GStreamer
    void onFrameReceived(const QByteArray& frame);
    void onStreamError(const QString& error);

  private:
    void onConnected();
    MainWindow* view = nullptr;
    VideoStreamModel* videoStreamModel = nullptr;
};
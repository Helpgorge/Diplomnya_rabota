#pragma once

#include <QObject>
#include <QByteArray>

#include <thread>
#include <atomic>

#include <gst/gst.h>

class VideoStreamModel : public QObject
{
    Q_OBJECT

  public:
    explicit VideoStreamModel(QObject* parent = nullptr);
    ~VideoStreamModel() override = default;

    // Управление
    void startReceiving();
    void stopReceiving();

  signals:
    void frameReceived(const QByteArray& frame);
    void streamError(const QString& message);
    void streamStopped();

  private:
    std::thread receivingThread;
    std::atomic_bool receiving{false};

    GstElement* pipeline = nullptr;

    // Параметры GStream
    static constexpr const char* STREAM_HOST = "192.168.50.1";
    static constexpr int STREAM_PORT = 5001;
    static constexpr const char* APP_SINK_NAME = "framesink";
    static constexpr int MAX_BUFFERS = 1;
    static constexpr int SAMPLE_TIMEOUT_MS = 100;

    // Параметры кадров
    static constexpr int MAX_FRAME_WIDTH = 1920;
    static constexpr int MAX_FRAME_HEIGHT = 1080;
    static constexpr const char* RAW_CAPS_NAME = "application/x-raw10-frame";
    static constexpr const char* RAW_FORMAT = "SBGGR10_CSI2P";

    static constexpr int BUS_TIMEOUT_MS = 0;

    void receivingLoop();
    void processBusMessages();
};

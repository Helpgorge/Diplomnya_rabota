#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

#include <atomic>
#include <thread>

#include <gst/gst.h>

class VideoStreamModel : public QObject
{
    Q_OBJECT

  public:
    explicit VideoStreamModel(QObject* parent = nullptr);
    ~VideoStreamModel() override;

    void startReceiving();
    void stopReceiving();

    // Тест
    void updateStatistics(const QByteArray& frame);

  signals:
    void frameReceived(const QByteArray& frame);
    void streamError(const QString& error);
    void streamStopped();

  private:
    void receivingLoop();
    void processBusMessages(GstBus* bus);

    GstElement* pipeline = nullptr;

    std::thread receivingThread;
    std::atomic_bool receiving{false};

    static constexpr const char* STREAM_HOST = "192.168.50.1";
    static constexpr int STREAM_PORT = 5001;
    static constexpr const char* APP_SINK_NAME = "rawsink";

    static constexpr int MAX_BUFFERS = 4;
    static constexpr int SAMPLE_TIMEOUT_MS = 100;
};
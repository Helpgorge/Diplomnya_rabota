#include "VideoStreamModel.h"

#include <gst/app/gstappsink.h>

#include <QDebug>
#include <chrono>

VideoStreamModel::VideoStreamModel(QObject* parent) : QObject(parent)
{
}

VideoStreamModel::~VideoStreamModel()
{
    stopReceiving();
}

void VideoStreamModel::updateStatistics(const QByteArray& frame)
{
    using Clock = std::chrono::steady_clock;

    static auto lastReport = Clock::now();
    static uint64_t totalFrames = 0;
    static uint64_t intervalFrames = 0;
    static uint64_t intervalBytes = 0;

    totalFrames++;
    intervalFrames++;
    intervalBytes += frame.size();

    const auto now = Clock::now();
    const double elapsed = std::chrono::duration<double>(now - lastReport).count();

    if (elapsed < 1.0) return;

    qInfo() << "Received:" << totalFrames << "| FPS:" << intervalFrames / elapsed
            << "| Speed:" << intervalBytes * 8.0 / elapsed / 1000000.0 << "Mbps"
            << "| Bytes:" << frame.size();

    intervalFrames = 0;
    intervalBytes = 0;
    lastReport = now;
}

void VideoStreamModel::startReceiving()
{
    if (pipeline) return;

    // Инициализация GStreamer.
    gst_init(nullptr, nullptr);

    // Создание pipeline.
    const QString pipelineDescription =
        QString("tcpclientsrc host=%1 port=%2 ! gdpdepay ! appsink name=%3 sync=false max-buffers=%4 drop=true")
            .arg(STREAM_HOST)
            .arg(STREAM_PORT)
            .arg(APP_SINK_NAME)
            .arg(MAX_BUFFERS);

    GError* error = nullptr;
    pipeline = gst_parse_launch(pipelineDescription.toUtf8().constData(), &error);

    if (error)
    {
        const QString errorText = QString::fromUtf8(error->message);
        g_error_free(error);

        if (pipeline)
        {
            gst_object_unref(pipeline);
            pipeline = nullptr;
        }

        emit streamError(errorText);
        return;
    }

    if (!pipeline)
    {
        emit streamError(QStringLiteral("Не удалось создать GStreamer pipeline"));
        return;
    }

    // Запуск pipeline.
    const GstStateChangeReturn result = gst_element_set_state(pipeline, GST_STATE_PLAYING);

    if (result == GST_STATE_CHANGE_FAILURE)
    {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;

        emit streamError(QStringLiteral("Не удалось запустить GStreamer pipeline"));
        return;
    }

    // Запуск потока приёма кадров.
    receiving = true;
    receivingThread = std::thread(&VideoStreamModel::receivingLoop, this);
}

void VideoStreamModel::stopReceiving()
{
    receiving = false;

    if (receivingThread.joinable()) receivingThread.join();

    if (!pipeline) return;

    // Остановка и освобождение pipeline.
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    pipeline = nullptr;
}

void VideoStreamModel::receivingLoop()
{
    GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), APP_SINK_NAME);

    if (!sink)
    {
        receiving = false;
        emit streamError(QStringLiteral("Не удалось получить GStreamer appsink"));
        emit streamStopped();
        return;
    }

    GstAppSink* appSink = GST_APP_SINK(sink);
    GstBus* bus = gst_element_get_bus(pipeline);

    while (receiving)
    {
        // Проверка сообщений GStreamer без ожидания.
        processBusMessages(bus);

        if (!receiving) break;

        // Ожидание очередного кадра.
        GstSample* sample =
            gst_app_sink_try_pull_sample(appSink, static_cast<GstClockTime>(SAMPLE_TIMEOUT_MS) * GST_MSECOND);

        if (!sample)
        {
            if (gst_app_sink_is_eos(appSink)) break;
            continue;
        }

        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;

        // Копирование исходных байтов кадра.
        if (buffer && gst_buffer_map(buffer, &map, GST_MAP_READ))
        {
            QByteArray frame(reinterpret_cast<const char*>(map.data), static_cast<qsizetype>(map.size));

            gst_buffer_unmap(buffer, &map);

            if (receiving) emit frameReceived(frame);
            if (receiving) updateStatistics(frame);
        }

        gst_sample_unref(sample);
    }

    // Освобождение ресурсов.
    if (bus) gst_object_unref(bus);
    gst_object_unref(sink);

    receiving = false;
    emit streamStopped();
}

void VideoStreamModel::processBusMessages(GstBus* bus)
{
    if (!bus) return;

    const GstMessageType messageTypes = static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    while (receiving)
    {
        GstMessage* message = gst_bus_pop_filtered(bus, messageTypes);

        if (!message) break;

        switch (GST_MESSAGE_TYPE(message))
        {
            case GST_MESSAGE_ERROR:
            {
                GError* error = nullptr;
                gchar* debug = nullptr;

                gst_message_parse_error(message, &error, &debug);

                const QString errorText =
                    error ? QString::fromUtf8(error->message) : QStringLiteral("Неизвестная ошибка GStreamer");

                if (error) g_error_free(error);
                if (debug) g_free(debug);

                receiving = false;
                emit streamError(errorText);
                break;
            }

            case GST_MESSAGE_EOS:
            {
                receiving = false;
                break;
            }

            default:
                break;
        }

        gst_message_unref(message);
    }
}
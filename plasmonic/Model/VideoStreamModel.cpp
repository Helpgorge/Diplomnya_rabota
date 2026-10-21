#include "VideoStreamModel.h"

#include <cstring>

#include <QDebug>

#include <gst/app/gstappsink.h>

VideoStreamModel::VideoStreamModel(QObject* parent) : QObject(parent)
{
}

void VideoStreamModel::startReceiving()
{
    if (pipeline) return;

    // Инициализация GStreamer.
    gst_init(nullptr, nullptr);

    // Создание цепочки: TCP, распаковка GDP, получение кадров.
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
        qWarning() << "Ошибка GStreamer:" << error->message;
        g_error_free(error);

        if (pipeline)
        {
            gst_object_unref(pipeline);
            pipeline = nullptr;
        }

        return;
    }

    if (!pipeline)
    {
        qWarning() << "Не удалось создать GStreamer pipeline";
        return;
    }

    // Запуск pipeline (переход в состояние PLAYING).
    const GstStateChangeReturn result = gst_element_set_state(pipeline, GST_STATE_PLAYING);

    if (result == GST_STATE_CHANGE_FAILURE)
    {
        qWarning() << "Не удалось запустить GStreamer pipeline";

        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return;
    }

    // Запуск приёма кадров в отдельном потоке.
    receiving = true;
    receivingThread = std::thread(&VideoStreamModel::receivingLoop, this);
}

void VideoStreamModel::stopReceiving()
{
    // Остановка цикла и ожидание завершения потока.
    receiving = false;

    if (receivingThread.joinable()) receivingThread.join();

    if (!pipeline) return;

    // Остановка GStreamer и освобождение pipeline.
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    pipeline = nullptr;
}

void VideoStreamModel::receivingLoop()
{
    if (!pipeline) return;

    GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), APP_SINK_NAME);
    if (!sink) return;

    GstAppSink* appSink = GST_APP_SINK(sink);

    while (receiving)
    {
        processBusMessages();

        GstSample* sample =
            gst_app_sink_try_pull_sample(appSink, static_cast<GstClockTime>(SAMPLE_TIMEOUT_MS) * GST_MSECOND);

        if (!sample)
        {
            if (gst_app_sink_is_eos(appSink)) break;
            continue;
        }

        // Получение буфера и описания формата.
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstCaps* caps = gst_sample_get_caps(sample);

        if (!buffer || !caps || gst_caps_is_empty(caps) || gst_caps_is_any(caps))
        {
            qWarning() << "Отсутствуют данные кадра или описание формата";
            gst_sample_unref(sample);
            continue;
        }

        const GstStructure* structure = gst_caps_get_structure(caps, 0);

        int width = 0;
        int height = 0;
        int stride = 0;
        const char* format = gst_structure_get_string(structure, "format");

        // Проверка типа RAW10 и обязательных параметров.
        if (!gst_structure_has_name(structure, RAW_CAPS_NAME) || !format || strcmp(format, RAW_FORMAT) != 0 ||
            !gst_structure_get_int(structure, "width", &width) ||
            !gst_structure_get_int(structure, "height", &height) ||
            !gst_structure_get_int(structure, "stride", &stride))
        {
            qWarning() << "Неподдерживаемый формат или неполные метаданные RAW";
            gst_sample_unref(sample);
            continue;
        }

        // Проверка разрешения и минимального размера строки RAW10.
        const qint64 minimumStride = (static_cast<qint64>(width) * 10 + 7) / 8;

        if (width <= 0 || width > MAX_FRAME_WIDTH || height <= 0 || height > MAX_FRAME_HEIGHT || stride < minimumStride)
        {
            qWarning() << "Недопустимые параметры RAW-кадра:" << width << height << stride;
            gst_sample_unref(sample);
            continue;
        }

        // Размер кадра должен учитывать выравнивание строк.
        const quint64 expectedSize = static_cast<quint64>(stride) * height;

        if (gst_buffer_get_size(buffer) != expectedSize)
        {
            qWarning() << "Несоответствие размера RAW-кадра:" << gst_buffer_get_size(buffer)
                       << "ожидалось:" << expectedSize;
            gst_sample_unref(sample);
            continue;
        }

        GstMapInfo map;

        // Чтение исходных байтов без изменения значений пикселей.
        if (gst_buffer_map(buffer, &map, GST_MAP_READ))
        {
            QByteArray frame(reinterpret_cast<const char*>(map.data), static_cast<qsizetype>(map.size));

            gst_buffer_unmap(buffer, &map);

            if (receiving) emit frameReceived(frame);

            if (receiving)
            {
                qDebug().noquote() << "RAW-кадр:"
                                   << "Разрешение:" << width << "x" << height << "| Формат:" << format
                                   << "| Stride:" << stride << "| Размер:" << frame.size() << "байт";
            }
        }

        gst_sample_unref(sample);
    }

    gst_object_unref(sink);

    receiving = false;
    emit streamStopped();
}

void VideoStreamModel::processBusMessages()
{
    if (!pipeline) return;

    GstBus* bus = gst_element_get_bus(pipeline);
    if (!bus) return;

    GstMessage* message = nullptr;

    while ((message = gst_bus_timed_pop_filtered(bus, static_cast<GstClockTime>(BUS_TIMEOUT_MS) * GST_MSECOND,
                                                 static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS))) !=
           nullptr)
    {
        switch (GST_MESSAGE_TYPE(message))
        {
            case GST_MESSAGE_ERROR:
            {
                GError* error = nullptr;
                gchar* debug = nullptr;

                gst_message_parse_error(message, &error, &debug);

                const QString errorText =
                    error ? QString::fromUtf8(error->message) : QStringLiteral("Неизвестная ошибка GStreamer");

                qWarning() << "Ошибка видеопотока:" << errorText;
                emit streamError(errorText);

                if (error) g_error_free(error);
                if (debug) g_free(debug);

                receiving = false;
                break;
            }

            case GST_MESSAGE_EOS:
            {
                qDebug() << "Видеопоток завершён";
                receiving = false;
                break;
            }

            default:
                break;
        }

        gst_message_unref(message);

        if (!receiving) break;
    }

    gst_object_unref(bus);
}

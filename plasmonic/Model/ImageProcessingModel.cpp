#include "ImageProcessingModel.h"
#include <QtConcurrent>
#include <QDir>
#include <QUuid>
#include <QFile>
#include <opencv2/imgcodecs.hpp>
#include <vector>

ImageProcessingModel::ImageProcessingModel(QObject* parent) : QObject(parent)
{
}

void ImageProcessingModel::processFrame(const QByteArray& frame)
{
    constexpr int frameSize = 2592000;

    if (frame.size() != frameSize) return;
    if (recording_) recordFrame(frame);
    if (processingFuture_.isRunning()) return;

    processingFuture_ = QtConcurrent::run([this, frame]() { processFrameWorker(frame); });
}

cv::Mat ImageProcessingModel::unpackRawFrame(const QByteArray& frame)
{
    constexpr int width = 1920;
    constexpr int height = 1080;
    constexpr int stride = width * 5 / 4;

    if (frame.size() != stride * height) return {};

    cv::Mat raw(height, width, CV_16UC1);

    const auto* source = reinterpret_cast<const uint8_t*>(frame.constData());

    for (int y = 0; y < height; ++y)
    {
        const uint8_t* row = source + y * stride;
        uint16_t* pixels = raw.ptr<uint16_t>(y);

        for (int x = 0; x < width; x += 4)
        {
            const int offset = x * 5 / 4;
            const uint8_t low = row[offset + 4];

            for (int i = 0; i < 4; ++i)
            {
                pixels[x + i] = (static_cast<uint16_t>(row[offset + i]) << 2) | ((low >> (2 * i)) & 0x03);
            }
        }
    }

    return raw;
}

void ImageProcessingModel::processFrameWorker(const QByteArray& frame)
{
    const cv::Mat raw = unpackRawFrame(frame);

    if (raw.empty()) return;

    cv::Mat preview;
    raw.convertTo(preview, CV_8UC1, 0.25);

    emit frameReady(preview);

    if (referenceRequested_)
    {
        referenceFrame_ = raw.clone();
        referenceRequested_ = false;
    }

    if (referenceFrame_.empty()) referenceFrame_ = raw.clone();

    cv::Mat difference;
    cv::subtract(raw, referenceFrame_, difference);

    cv::Mat subtractionPreview;
    difference.convertTo(subtractionPreview, CV_8UC1, 0.25);

    emit subtractionReady(subtractionPreview);
}

void ImageProcessingModel::selectReferenceFrame()
{
    processingFuture_.waitForFinished();
    referenceRequested_ = true;
}

bool ImageProcessingModel::startRecording()
{
    if (recording_) return false;

    processingFuture_.waitForFinished();

    const QString directoryPath = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("recordings"));

    if (!QDir().mkpath(directoryPath))
    {
        emit recordingError(QStringLiteral("Не удалось создать папку: %1").arg(directoryPath));
        return false;
    }

    QDir directory(directoryPath);

    const QString sessionName = QUuid::createUuid().toString(QUuid::WithoutBraces);

    if (!directory.mkdir(sessionName))
    {
        emit recordingError(QStringLiteral("Не удалось создать папку записи"));
        return false;
    }

    recordingDirectory_ = directory.filePath(sessionName);
    recordingFrameIndex_ = 0;
    recording_ = true;

    return true;
}

void ImageProcessingModel::recordFrame(const QByteArray& frame)
{
    if (!recording_) return;

    const QString fileName = QStringLiteral("frame_%1.tiff").arg(recordingFrameIndex_ + 1, 6, 10, QLatin1Char('0'));

    const QString filePath = QDir(recordingDirectory_).filePath(fileName);

    try
    {
        const cv::Mat raw = unpackRawFrame(frame);

        if (raw.empty()) return;

        std::vector<uchar> encoded;

        const std::vector<int> parameters = {cv::IMWRITE_TIFF_COMPRESSION, 1};

        if (!cv::imencode(".tiff", raw, encoded, parameters))
        {
            recording_ = false;
            emit recordingError(QStringLiteral("Не удалось сформировать TIFF"));
            return;
        }

        QFile file(filePath);

        if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly))
        {
            recording_ = false;
            emit recordingError(file.errorString());
            return;
        }

        const qint64 size = static_cast<qint64>(encoded.size());

        const bool written = file.write(reinterpret_cast<const char*>(encoded.data()), size) == size;

        if (!written || !file.flush())
        {
            const QString message = file.errorString();

            file.close();
            file.remove();

            recording_ = false;
            emit recordingError(message);
            return;
        }

        ++recordingFrameIndex_;
    }
    catch (const cv::Exception& error)
    {
        recording_ = false;
        emit recordingError(QString::fromUtf8(error.what()));
    }
}

void ImageProcessingModel::stopRecording()
{
    processingFuture_.waitForFinished();
    recording_ = false;
}

void ImageProcessingModel::saveSelectedFrame()
{
    processingFuture_.waitForFinished();

    if (referenceFrame_.empty())
    {
        emit frameSavingError(QStringLiteral("Кадр не выбран"));
        return;
    }

    const QString directoryPath = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("snapshots"));

    if (!QDir().mkpath(directoryPath))
    {
        emit frameSavingError(QStringLiteral("Не удалось создать папку: %1").arg(directoryPath));
        return;
    }

    const QString fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + QStringLiteral(".tiff");

    const QString filePath = QDir(directoryPath).filePath(fileName);

    try
    {
        std::vector<uchar> encoded;

        const std::vector<int> parameters = {cv::IMWRITE_TIFF_COMPRESSION, 1};

        if (!cv::imencode(".tiff", referenceFrame_, encoded, parameters))
        {
            emit frameSavingError(QStringLiteral("Не удалось сформировать TIFF"));
            return;
        }

        QFile file(filePath);

        if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly))
        {
            emit frameSavingError(file.errorString());
            return;
        }

        const qint64 size = static_cast<qint64>(encoded.size());

        const bool written = file.write(reinterpret_cast<const char*>(encoded.data()), size) == size;

        if (!written || !file.flush())
        {
            const QString message = file.errorString();

            file.close();
            file.remove();

            emit frameSavingError(message);
        }
    }
    catch (const cv::Exception& error)
    {
        emit frameSavingError(QString::fromUtf8(error.what()));
    }
}

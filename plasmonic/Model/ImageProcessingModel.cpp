#include "ImageProcessingModel.h"

#include <QtConcurrent>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>

#include <cstdint>
#include <vector>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

ImageProcessingModel::ImageProcessingModel(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");

    processingPool_.setMaxThreadCount(1);
    originalRecordingPool_.setMaxThreadCount(1);
    subtractionRecordingPool_.setMaxThreadCount(1);

    snapshotDirectory_ =
        QDir(QCoreApplication::applicationDirPath())
            .filePath(QStringLiteral("snapshots"));

    createFolders({snapshotDirectory_});
}

ImageProcessingModel::~ImageProcessingModel()
{
    stopRecording();
}

void ImageProcessingModel::processFrame(const QByteArray &frame)
{
    if (frame.size() != 2592000 || processingFuture_.isRunning())
        return;

    const QString originalDirectory =
        recording_ ? originalRecordingDirectory_ : QString();

    const QString subtractionDirectory =
        recording_ ? subtractionRecordingDirectory_ : QString();

    processingFuture_ = QtConcurrent::run(
        &processingPool_,
        [this, frame, originalDirectory, subtractionDirectory]() {
            processFrameWorker(
                frame,
                originalDirectory,
                subtractionDirectory
            );
        }
    );
}

cv::Mat ImageProcessingModel::unpackRawFrame(const QByteArray &frame)
{
    constexpr int width = 1920;
    constexpr int height = 1080;
    constexpr int stride = width * 5 / 4;

    cv::Mat raw(height, width, CV_16UC1);

    const std::uint8_t *source =
        reinterpret_cast<const std::uint8_t *>(frame.constData());

    for (int y = 0; y < height; ++y) {
        const std::uint8_t *row = source + y * stride;
        std::uint16_t *pixels = raw.ptr<std::uint16_t>(y);

        for (int x = 0; x < width; x += 4) {
            const int offset = x * 5 / 4;
            const std::uint8_t low = row[offset + 4];

            for (int i = 0; i < 4; ++i) {
                pixels[x + i] =
                    (static_cast<std::uint16_t>(row[offset + i]) << 2)
                    | ((low >> (2 * i)) & 0x03);
            }
        }
    }

    return raw;
}

void ImageProcessingModel::processFrameWorker(
    const QByteArray &frame,
    const QString &originalDirectory,
    const QString &subtractionDirectory)
{
    const cv::Mat raw = unpackRawFrame(frame);

    if (referenceRequested_ || referenceFrame_.empty()) {
        referenceFrame_ = raw.clone();
        referenceRequested_ = false;
    }

    cv::Mat difference;
    cv::subtract(raw, referenceFrame_, difference);

    if (!originalDirectory.isEmpty()
        && !originalRecordingFuture_.isRunning()
        && !subtractionRecordingFuture_.isRunning()) {
        const QString fileName =
            QStringLiteral("frame_%1.tiff")
                .arg(++recordingFrameIndex_, 6, 10, QLatin1Char('0'));

        const QString originalPath =
            QDir(originalDirectory).filePath(fileName);

        const QString subtractionPath =
            QDir(subtractionDirectory).filePath(fileName);

        originalRecordingFuture_ = QtConcurrent::run(
            &originalRecordingPool_,
            [this, raw, originalPath]() {
                createTiffFile(raw, originalPath);
            }
        );

        subtractionRecordingFuture_ = QtConcurrent::run(
            &subtractionRecordingPool_,
            [this, difference, subtractionPath]() {
                createTiffFile(difference, subtractionPath);
            }
        );
    }

    cv::Mat preview;
    raw.convertTo(preview, CV_8UC1, 0.25);
    emit frameReady(preview);

    cv::Mat subtractionPreview;
    difference.convertTo(subtractionPreview, CV_8UC1, 0.25);
    emit subtractionReady(subtractionPreview);
}

void ImageProcessingModel::selectReferenceFrame()
{
    processingFuture_.waitForFinished();
    referenceRequested_ = true;
}

bool ImageProcessingModel::createFolders(const QStringList &directories)
{
    for (const QString &directory : directories) {
        if (!QDir().mkpath(directory))
            return false;
    }

    return true;
}

bool ImageProcessingModel::startRecording()
{
    processingFuture_.waitForFinished();

    const QString directoryPath =
        QDir(QCoreApplication::applicationDirPath()).filePath(
            QStringLiteral("recordings/")
            + QDateTime::currentDateTime().toString(
                QStringLiteral("yyyy-MM-dd_HH-mm-ss-zzz")
            )
        );

    originalRecordingDirectory_ =
        QDir(directoryPath).filePath(QStringLiteral("original"));

    subtractionRecordingDirectory_ =
        QDir(directoryPath).filePath(QStringLiteral("subtraction"));

    recordingFrameIndex_ = 0;

    recording_ = createFolders({
        originalRecordingDirectory_,
        subtractionRecordingDirectory_
    });

    return recording_;
}

void ImageProcessingModel::stopRecording()
{
    recording_ = false;

    processingFuture_.waitForFinished();
    originalRecordingFuture_.waitForFinished();
    subtractionRecordingFuture_.waitForFinished();
}

void ImageProcessingModel::createTiffFile(
    const cv::Mat &frame,
    const QString &filePath)
{
    QFile file(filePath);
    std::vector<uchar> encoded;

    if (!cv::imencode(
            ".tiff",
            frame,
            encoded,
            {cv::IMWRITE_TIFF_COMPRESSION, 1})
        || !file.open(QIODevice::WriteOnly | QIODevice::NewOnly)) {
        return;
    }

    const qint64 size = static_cast<qint64>(encoded.size());

    if (file.write(
            reinterpret_cast<const char *>(encoded.data()),
            size) != size
        || !file.flush()) {
        file.close();
        file.remove();
    }
}

void ImageProcessingModel::saveSelectedFrame()
{
    processingFuture_.waitForFinished();

    if (referenceFrame_.empty() || referenceRequested_)
        return;

    const QString fileName =
        QStringLiteral("frame_%1.tiff").arg(
            QDateTime::currentDateTime().toString(
                QStringLiteral("yyyy-MM-dd_HH-mm-ss-zzz")
            )
        );

    createTiffFile(
        referenceFrame_,
        QDir(snapshotDirectory_).filePath(fileName)
    );
}
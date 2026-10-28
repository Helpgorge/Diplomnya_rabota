#pragma once

#include <QObject>
#include <QByteArray>
#include <QFuture>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QThreadPool>

#include <opencv2/core.hpp>

Q_DECLARE_METATYPE(cv::Mat)

class ImageProcessingModel : public QObject
{
    Q_OBJECT

public:
    explicit ImageProcessingModel(QObject *parent = nullptr);
    ~ImageProcessingModel() override;

    void processFrame(const QByteArray &frame);
    void selectReferenceFrame();

    bool startRecording();
    void stopRecording();

    void saveSelectedFrame();

signals:
    void frameReady(const cv::Mat &frame);
    void subtractionReady(const cv::Mat &frame);

private:
    void processFrameWorker(
        const QByteArray &frame,
        const QString &originalDirectory,
        const QString &subtractionDirectory
    );

    cv::Mat unpackRawFrame(const QByteArray &frame);

    bool createFolders(const QStringList &directories);
    void createTiffFile(const cv::Mat &frame, const QString &filePath);

    QThreadPool processingPool_;
    QThreadPool originalRecordingPool_;
    QThreadPool subtractionRecordingPool_;

    QFuture<void> processingFuture_;
    QFuture<void> originalRecordingFuture_;
    QFuture<void> subtractionRecordingFuture_;

    cv::Mat referenceFrame_;
    bool referenceRequested_ = false;

    bool recording_ = false;
    quint64 recordingFrameIndex_ = 0;

    QString originalRecordingDirectory_;
    QString subtractionRecordingDirectory_;
    QString snapshotDirectory_;
};
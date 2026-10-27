#pragma once

#include <QObject>
#include <QByteArray>
#include <QFuture>
#include <QString>

#include <opencv2/core.hpp>

class ImageProcessingModel : public QObject
{
    Q_OBJECT

  public:
    explicit ImageProcessingModel(QObject* parent = nullptr);

    void processFrame(const QByteArray& frame);
    void selectReferenceFrame();

    bool startRecording();
    void stopRecording();

    void saveSelectedFrame();

  signals:
    void frameReady(const cv::Mat& frame);
    void subtractionReady(const cv::Mat& frame);
    void recordingError(const QString& message);
    void frameSavingError(const QString& message);

  private:
    void processFrameWorker(const QByteArray& frame);
    void recordFrame(const QByteArray& frame);
    cv::Mat unpackRawFrame(const QByteArray& frame);

    cv::Mat referenceFrame_;
    bool referenceRequested_ = false;

    QFuture<void> processingFuture_;

    bool recording_ = false;
    QString recordingDirectory_;
    quint64 recordingFrameIndex_ = 0;
};
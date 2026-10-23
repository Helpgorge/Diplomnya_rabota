#pragma once

#include <QObject>
#include <QByteArray>
#include <QFuture>

#include <opencv2/core.hpp>

class ImageProcessingModel : public QObject
{
    Q_OBJECT

  public:
    explicit ImageProcessingModel(QObject* parent = nullptr);

    void processFrame(const QByteArray& frame);
    void selectReferenceFrame();

  signals:
    void frameReady(const cv::Mat& frame);
    void subtractionReady(const cv::Mat& frame);

  private:
    cv::Mat referenceFrame_;
    bool referenceRequested_ = false;

    QFuture<void> processingFuture_;

    void processFrameWorker(const QByteArray& frame);
};

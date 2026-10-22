#pragma once

#include <QObject>
#include <QByteArray>

#include <opencv2/core.hpp>

class ImageProcessingModel : public QObject
{
    Q_OBJECT

  public:
    explicit ImageProcessingModel(QObject* parent = nullptr);

    void processFrame(const QByteArray& frame);

  Q_SIGNALS:
    void frameReady(const cv::Mat& frame);
};

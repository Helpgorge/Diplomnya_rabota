#pragma once

#include <QOpenGLWidget>
#include <QImage>

#include <opencv2/core.hpp>

class VideoWindow : public QOpenGLWidget
{
    Q_OBJECT

  public:
    explicit VideoWindow(QWidget* parent = nullptr);

    void showFrame(const cv::Mat& frame);

  protected:
    void paintGL() override;

  private:
    QImage image_;
};

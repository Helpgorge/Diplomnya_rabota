#include "VideoWindow.h"

#include <QPainter>

VideoWindow::VideoWindow(QWidget* parent) : QOpenGLWidget(parent)
{
}

void VideoWindow::showFrame(const cv::Mat& frame)
{
    QImage image(frame.data, frame.cols, frame.rows, static_cast<qsizetype>(frame.step), QImage::Format_Grayscale8);

    image_ = image.copy();
    update();
}

void VideoWindow::paintGL()
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    if (image_.isNull()) return;

    const QSize size = image_.size().scaled(this->size(), Qt::KeepAspectRatio);

    const QRect target((width() - size.width()) / 2, (height() - size.height()) / 2, size.width(), size.height());

    painter.drawImage(target, image_);
}

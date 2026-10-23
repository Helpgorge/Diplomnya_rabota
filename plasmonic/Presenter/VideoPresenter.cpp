#include "VideoPresenter.h"

#include "VideoStreamModel.h"
#include "ImageProcessingModel.h"
#include "VideoWindow.h"
#include "MainWindow.h"

VideoPresenter::VideoPresenter(MainWindow* view, VideoStreamModel* stream, ImageProcessingModel* processing,
                               QObject* parent)
    : view(view), processing_(processing), QObject(parent)
{
    connect(stream, &VideoStreamModel::frameReceived, processing_, &ImageProcessingModel::processFrame);

    QObject::connect(view, &MainWindow::showVideoRequested, this, &VideoPresenter::onShowVideoRequested);
    QObject::connect(view, &MainWindow::showSubtractionRequested, this, &VideoPresenter::onShowSubtractionRequested);

    QObject::connect(processing_, &ImageProcessingModel::frameReady, this, &VideoPresenter::onFrameReady);
    QObject::connect(processing_, &ImageProcessingModel::subtractionReady, this, &VideoPresenter::onSubtractionReady);
    QObject::connect(view, &MainWindow::selectReferenceFrameRequested, this,
                     &VideoPresenter::onSelectReferenceFrameRequested);
}

// Окна видео
void VideoPresenter::onShowVideoRequested()
{
    view->showVideoWindow();
}

void VideoPresenter::onShowSubtractionRequested()
{
    view->showSubtractionWindow();
}

void VideoPresenter::onFrameReady(const cv::Mat& frame)
{
    if (view->getVideoWindow())
    {
        view->getVideoWindow()->showFrame(frame);
    }
}

void VideoPresenter::onSubtractionReady(const cv::Mat& frame)
{
    if (view->getSubtractionWindow())
    {
        view->getSubtractionWindow()->showFrame(frame);
    }
}

// Выбор кадра
void VideoPresenter::onSelectReferenceFrameRequested()
{
    processing_->selectReferenceFrame();
}
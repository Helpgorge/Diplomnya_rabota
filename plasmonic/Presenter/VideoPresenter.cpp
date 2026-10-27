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

    connect(view, &MainWindow::showVideoRequested, this, &VideoPresenter::onShowVideoRequested);
    connect(view, &MainWindow::showSubtractionRequested, this, &VideoPresenter::onShowSubtractionRequested);

    connect(view, &MainWindow::startRecordingRequested, this, &VideoPresenter::onStartRecordingRequested);
    connect(view, &MainWindow::stopRecordingRequested, this, &VideoPresenter::onStopRecordingRequested);
    connect(view, &MainWindow::saveSelectedFrameRequested, this, &VideoPresenter::onSaveSelectedFrameRequested);

    connect(processing_, &ImageProcessingModel::frameReady, this, &VideoPresenter::onFrameReady);
    connect(processing_, &ImageProcessingModel::subtractionReady, this, &VideoPresenter::onSubtractionReady);
    connect(view, &MainWindow::selectReferenceFrameRequested, this, &VideoPresenter::onSelectReferenceFrameRequested);
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

// Выбор кадра и сохранение кадра
void VideoPresenter::onSelectReferenceFrameRequested()
{
    processing_->selectReferenceFrame();
}

void VideoPresenter::onSaveSelectedFrameRequested()
{
    processing_->saveSelectedFrame();
}

// Запись
void VideoPresenter::onStartRecordingRequested()
{
    processing_->startRecording();
    view->setStopRecordingEnabled(true);
    view->setStartRecordingEnabled(false);
}

void VideoPresenter::onStopRecordingRequested()
{
    processing_->stopRecording();
    view->setStopRecordingEnabled(false);
    view->setStartRecordingEnabled(true);
}

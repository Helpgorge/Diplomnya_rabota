#include "VideoPresenter.h"

#include "VideoStreamModel.h"
#include "ImageProcessingModel.h"
#include "VideoWindow.h"
#include "MainWindow.h"

VideoPresenter::VideoPresenter(VideoStreamModel* stream, ImageProcessingModel* processing, QObject* parent)
    : QObject(parent), processing_(processing)
{
    connect(stream, &VideoStreamModel::frameReceived, processing_, &ImageProcessingModel::processFrame);
}

void VideoPresenter::showVideoWindow(MainWindow* mainWindow)
{
    mainWindow->showVideoWindow();

    VideoWindow* window = mainWindow->getVideoWindow();

    connect(processing_, &ImageProcessingModel::frameReady, window, &VideoWindow::showFrame, Qt::UniqueConnection);
}

void VideoPresenter::showSubtractionWindow(MainWindow* mainWindow)
{
    mainWindow->showSubtractionWindow();

    // Подключение результата вычитания добавим позже.
}

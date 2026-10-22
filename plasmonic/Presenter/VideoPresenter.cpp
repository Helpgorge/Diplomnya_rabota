
#include "VideoPresenter.h"

#include "VideoStreamModel.h"
#include "ImageProcessingModel.h"
#include "VideoWindow.h"

VideoPresenter::VideoPresenter(VideoStreamModel* stream, ImageProcessingModel* processing, VideoWindow* window,
                               QObject* parent)
    : QObject(parent)
{
    connect(stream, &VideoStreamModel::frameReceived, processing, &ImageProcessingModel::processFrame);

    connect(processing, &ImageProcessingModel::frameReady, window, &VideoWindow::showFrame);
}

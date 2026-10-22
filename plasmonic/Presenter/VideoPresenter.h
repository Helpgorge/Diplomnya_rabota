#pragma once

#include <QObject>

class VideoStreamModel;
class ImageProcessingModel;
class VideoWindow;

class VideoPresenter : public QObject
{
    Q_OBJECT

  public:
    explicit VideoPresenter(VideoStreamModel* stream, ImageProcessingModel* processing, VideoWindow* window,
                            QObject* parent = nullptr);
};

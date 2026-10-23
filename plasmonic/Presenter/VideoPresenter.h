#pragma once

#include <QObject>

class VideoStreamModel;
class ImageProcessingModel;
class MainWindow;

class VideoPresenter : public QObject
{
    Q_OBJECT

  public:
    explicit VideoPresenter(VideoStreamModel* stream, ImageProcessingModel* processing, QObject* parent = nullptr);

    void showVideoWindow(MainWindow* mainWindow);
    void showSubtractionWindow(MainWindow* mainWindow);

  private:
    ImageProcessingModel* processing_;
};

#pragma once

#include <opencv2/core.hpp>

#include <QObject>

class MainWindow;
class VideoStreamModel;
class ImageProcessingModel;

class VideoPresenter : public QObject
{
    Q_OBJECT

  private slots:
    void onShowVideoRequested();
    void onShowSubtractionRequested();

  public:
    explicit VideoPresenter(MainWindow* view, VideoStreamModel* stream, ImageProcessingModel* processing,
                            QObject* parent = nullptr);

  private:
    void onFrameReady(const cv::Mat& frame);
    void onSubtractionReady(const cv::Mat& frame);
    void onSelectReferenceFrameRequested();

    void onStartRecordingRequested();
    void onStopRecordingRequested();
    void onSaveSelectedFrameRequested();

    ImageProcessingModel* processing_ = nullptr;
    MainWindow* view = nullptr;
};

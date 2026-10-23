#pragma once

#include <QWidget>
#include <QLabel>
#include <QAction>
#include <QKeyEvent>

#include <opencv2/core.hpp>

class VideoWindow : public QWidget
{
    Q_OBJECT

  public:
    explicit VideoWindow(QWidget* parent = nullptr);

    void showFrame(const cv::Mat& frame);

  protected:
    void keyPressEvent(QKeyEvent* event) override;

  private:
    void onScaleSelected(QAction* action);
    void enterFullscreen();
    void exitFullscreen();

    QLabel* imageLabel_;
    int currentScale_ = 50;
};

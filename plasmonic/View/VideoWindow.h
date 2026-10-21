#pragma once

#include <QWidget>

class VideoWindow : public QWidget
{
    Q_OBJECT

  public:
    explicit VideoWindow(QWidget* parent = nullptr);
    ~VideoWindow() override = default;

  private:
    void setupUi();
};

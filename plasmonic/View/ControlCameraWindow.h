#pragma once

#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>

class ControlCameraWindow : public QWidget
{
    Q_OBJECT

  public:
    explicit ControlCameraWindow(QWidget* parent = nullptr);

  signals:
    void exposureChanged(quint32 exposure);
    void gainChanged(float gain);

  private:
    void onGainSliderChanged(int value);
    void onGainSpinBoxChanged(double value);

    void onExposureFinished();
    void onGainFinished();

    QSlider* exposureSlider;
    QSpinBox* exposureSpinBox;

    QSlider* gainSlider;
    QDoubleSpinBox* gainSpinBox;
};

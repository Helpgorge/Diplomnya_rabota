#include "ControlCameraWindow.h"

#include <QFormLayout>

ControlCameraWindow::ControlCameraWindow(QWidget* parent) : QWidget(parent, Qt::Window)
{
    setWindowTitle("Управление камерой");
    resize(420, 180);

    exposureSlider = new QSlider(Qt::Horizontal, this);
    exposureSpinBox = new QSpinBox(this);

    exposureSlider->setRange(1, 66666);
    exposureSpinBox->setRange(1, 66666);
    exposureSpinBox->setSuffix(" мкс");

    exposureSlider->setValue(10000);
    exposureSpinBox->setValue(10000);

    gainSlider = new QSlider(Qt::Horizontal, this);
    gainSpinBox = new QDoubleSpinBox(this);

    gainSlider->setRange(10, 160);
    gainSpinBox->setRange(1.0, 16.0);
    gainSpinBox->setSingleStep(0.1);
    gainSpinBox->setDecimals(1);
    gainSpinBox->setSuffix(" ×");

    gainSlider->setValue(10);
    gainSpinBox->setValue(1.0);

    auto* layout = new QFormLayout(this);

    layout->addRow("Экспозиция:", exposureSlider);
    layout->addRow("Точное значение:", exposureSpinBox);
    layout->addRow("Усиление:", gainSlider);
    layout->addRow("Точное значение:", gainSpinBox);

    connect(exposureSlider, &QSlider::valueChanged, exposureSpinBox, &QSpinBox::setValue);
    connect(exposureSpinBox, &QSpinBox::valueChanged, exposureSlider, &QSlider::setValue);

    connect(gainSlider, &QSlider::valueChanged, this, &ControlCameraWindow::onGainSliderChanged);
    connect(gainSpinBox, &QDoubleSpinBox::valueChanged, this, &ControlCameraWindow::onGainSpinBoxChanged);

    connect(exposureSlider, &QSlider::sliderReleased, this, &ControlCameraWindow::onExposureFinished);
    connect(exposureSpinBox, &QSpinBox::editingFinished, this, &ControlCameraWindow::onExposureFinished);

    connect(gainSlider, &QSlider::sliderReleased, this, &ControlCameraWindow::onGainFinished);
    connect(gainSpinBox, &QDoubleSpinBox::editingFinished, this, &ControlCameraWindow::onGainFinished);
}

void ControlCameraWindow::onGainSliderChanged(int value)
{
    gainSpinBox->setValue(value / 10.0);
}

void ControlCameraWindow::onGainSpinBoxChanged(double value)
{
    gainSlider->setValue(static_cast<int>(value * 10));
}

void ControlCameraWindow::onExposureFinished()
{
    Q_EMIT exposureChanged(static_cast<quint32>(exposureSpinBox->value()));
}

void ControlCameraWindow::onGainFinished()
{
    Q_EMIT gainChanged(static_cast<float>(gainSpinBox->value()));
}

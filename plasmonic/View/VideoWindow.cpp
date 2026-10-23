#include "VideoWindow.h"

#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QActionGroup>
#include <QPixmap>
#include <QKeySequence>

VideoWindow::VideoWindow(QWidget* parent) : QWidget(parent)
{
    setWindowFlag(Qt::Window);
    setFocusPolicy(Qt::StrongFocus);

    imageLabel_ = new QLabel(this);
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setStyleSheet("background-color: black;");
    imageLabel_->setMinimumSize(480, 270);
    imageLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel_->setScaledContents(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(imageLabel_);

    auto* menuBar = new QMenuBar(this);
    auto* scaleMenu = menuBar->addMenu("Масштаб");
    auto* scaleGroup = new QActionGroup(this);

    scaleGroup->setExclusive(true);

    for (int scale : {100, 75, 50, 25})
    {
        QAction* action = scaleMenu->addAction(QString("%1%").arg(scale));

        action->setCheckable(true);
        action->setData(scale);
        scaleGroup->addAction(action);

        if (scale == currentScale_) action->setChecked(true);
    }

    QAction* fullscreenAction = scaleMenu->addAction("Полный экран");
    fullscreenAction->setShortcut(QKeySequence(Qt::Key_F12));

    connect(fullscreenAction, &QAction::triggered, this, &VideoWindow::enterFullscreen);

    connect(scaleGroup, &QActionGroup::triggered, this, &VideoWindow::onScaleSelected);

    layout->setMenuBar(menuBar);

    setFixedSize(960, 540 + menuBar->sizeHint().height());
}

void VideoWindow::showFrame(const cv::Mat& frame)
{
    QImage image(frame.data, frame.cols, frame.rows, static_cast<qsizetype>(frame.step), QImage::Format_Grayscale8);

    imageLabel_->setPixmap(QPixmap::fromImage(image.copy()));
}

void VideoWindow::onScaleSelected(QAction* action)
{
    currentScale_ = action->data().toInt();

    const int width = 1920 * currentScale_ / 100;
    const int height = 1080 * currentScale_ / 100;
    const int menuHeight = layout()->menuBar()->sizeHint().height();

    setFixedSize(width, height + menuHeight);
}

void VideoWindow::enterFullscreen()
{
    if (isFullScreen()) return;

    layout()->menuBar()->hide();

    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    showFullScreen();
    setFocus();
}

void VideoWindow::exitFullscreen()
{
    if (!isFullScreen()) return;

    showNormal();
    layout()->menuBar()->show();

    const int width = 1920 * currentScale_ / 100;
    const int height = 1080 * currentScale_ / 100;
    const int menuHeight = layout()->menuBar()->sizeHint().height();

    setFixedSize(width, height + menuHeight);
}

void VideoWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen())
    {
        exitFullscreen();
        return;
    }

    QWidget::keyPressEvent(event);
}

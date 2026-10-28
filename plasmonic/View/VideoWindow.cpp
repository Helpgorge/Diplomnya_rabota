#include "VideoWindow.h"

#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QKeySequence>
#include <QKeyEvent>

VideoWindow::VideoWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::Window);
    setFocusPolicy(Qt::StrongFocus);

    imageLabel_ = new QLabel(this);
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setStyleSheet("background-color: black;");
    imageLabel_->setSizePolicy(
        QSizePolicy::Ignored,
        QSizePolicy::Ignored
    );
    imageLabel_->setScaledContents(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(imageLabel_);

    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *scaleMenu = menuBar->addMenu("Масштаб");
    QActionGroup *scaleGroup = new QActionGroup(this);

    for (int scale : {100, 75, 50, 25}) {
        QAction *action =
            scaleMenu->addAction(QString("%1%").arg(scale));

        action->setData(scale);
        scaleGroup->addAction(action);
    }

    QAction *fullscreenAction =
        scaleMenu->addAction("Полный экран");

    fullscreenAction->setShortcut(QKeySequence(Qt::Key_F12));

    connect(
        fullscreenAction, &QAction::triggered,
        this, &VideoWindow::enterFullscreen
    );

    connect(
        scaleGroup, &QActionGroup::triggered,
        this, &VideoWindow::onScaleSelected
    );

    layout->setMenuBar(menuBar);

    resize(
        1920 * currentScale_ / 100,
        1080 * currentScale_ / 100 + menuBar->sizeHint().height()
    );
}

void VideoWindow::showFrame(const cv::Mat &frame)
{
    QImage image(
        frame.data,
        frame.cols,
        frame.rows,
        static_cast<qsizetype>(frame.step),
        QImage::Format_Grayscale8
    );

    imageLabel_->setPixmap(QPixmap::fromImage(image.copy()));
}

void VideoWindow::onScaleSelected(QAction *action)
{
    currentScale_ = action->data().toInt();

    const int width = 1920 * currentScale_ / 100;
    const int height = 1080 * currentScale_ / 100;
    const int menuHeight = layout()->menuBar()->sizeHint().height();

    resize(width, height + menuHeight);
}

void VideoWindow::enterFullscreen()
{
    layout()->menuBar()->hide();

    showFullScreen();
    setFocus();
}

void VideoWindow::exitFullscreen()
{
    showNormal();
    layout()->menuBar()->show();
}

void VideoWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        exitFullscreen();
        return;
    }

    QWidget::keyPressEvent(event);
}
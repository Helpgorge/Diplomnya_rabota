#include "VideoWindow.h"

VideoWindow::VideoWindow(QWidget* parent) : QWidget(parent, Qt::Window)
{
    setupUi();
}

void VideoWindow::setupUi()
{
    setWindowTitle("Видеопоток");
    resize(800, 600);
}

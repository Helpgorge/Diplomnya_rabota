#include "MainWindow.h"
#include "VideoWindow.h"
#include "ControlCameraWindow.h"

#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QProgressDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    // Начальные параметры окна
    setWindowTitle("Plasmonic Microscope");
    resize(1280, 720);

    // Центральная область окна
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Верхняя строка меню
    QMenu* networkMenu = menuBar()->addMenu("Сеть");
    QMenu* recordingMenu = menuBar()->addMenu("Запись");
    QMenu* processingMenu = menuBar()->addMenu("Обработка");
    QMenu* windowsMenu = menuBar()->addMenu("Окна");

    connectTcpAction = networkMenu->addAction("Установить соединение");
    disconnectTcpAction = networkMenu->addAction("Разорвать соединение");
    selectReferenceFrameAction = processingMenu->addAction("Выбрать опорный кадр");

    showVideoAction = windowsMenu->addAction("Исходный видеопоток");
    showSubtractionAction = windowsMenu->addAction("Вычитание опорного кадра");

    cameraAction = processingMenu->addAction("Управление камерой");

    setConnected(false, false);

    connect(connectTcpAction, &QAction::triggered, this, &MainWindow::onConnectClicked);
    connect(disconnectTcpAction, &QAction::triggered, this, &MainWindow::onDisconnectClicked);

    connect(showVideoAction, &QAction::triggered, this, &MainWindow::showVideoRequested);
    connect(showSubtractionAction, &QAction::triggered, this, &MainWindow::showSubtractionRequested);
    connect(cameraAction, &QAction::triggered, this, &MainWindow::showCameraControlRequested);

    connect(selectReferenceFrameAction, &QAction::triggered, this, &MainWindow::selectReferenceFrameRequested);

    // Окна видеопотоков
    videoWindow = new VideoWindow(this);
    videoWindow->setWindowFlag(Qt::Window);
    videoWindow->setWindowTitle("Исходный видеопоток");

    subtractionWindow = new VideoWindow(this);
    subtractionWindow->setWindowFlag(Qt::Window);
    subtractionWindow->setWindowTitle("Вычитание опорного кадра");

    // Окно управление камерой
    controlCameraWindow = new ControlCameraWindow(this);
}

// Управление подключением к камере
// Прогресс подключения
void MainWindow::showConnectionProgress()
{
    connectionDialog = new QProgressDialog(this);

    connectionDialog->setWindowTitle("Connection");
    connectionDialog->setLabelText("Establishing TCP connection...");
    connectionDialog->setRange(0, 0);
    connectionDialog->setCancelButton(nullptr);
    connectionDialog->setMinimumDuration(0);
    connectionDialog->setAutoClose(false);
    connectionDialog->show();
}

void MainWindow::setConnectionStatus(const QString& text)
{
    if (connectionDialog)
    {
        connectionDialog->setLabelText(text);
    }
}

void MainWindow::closeConnectionProgress()
{
    if (!connectionDialog) return;

    connectionDialog->close();
    connectionDialog->deleteLater();
    connectionDialog = nullptr;
}

void MainWindow::showConnectionLost()
{
    QMessageBox::warning(this, "Соединение", "Соединение с сервером было разорвано.");
}

// Кнопки подклоючитсья и отключиться
void MainWindow::onConnectClicked()
{
    emit connectRequested();
}

void MainWindow::onDisconnectClicked()
{
    emit disconnectRequested();
}

// Кликабельность кнопок подклоючитсья и отключиться
void MainWindow::setConnected(bool connected, bool inProgress)
{
    if (inProgress)
    {
        connectTcpAction->setEnabled(false);
        disconnectTcpAction->setEnabled(false);
    }
    else
    {
        connectTcpAction->setEnabled(!connected);
        disconnectTcpAction->setEnabled(connected);
    }
}

// Окна с видео
void MainWindow::showVideoWindow()
{
    videoWindow->show();
    videoWindow->raise();
    videoWindow->activateWindow();
}

void MainWindow::showSubtractionWindow()
{
    subtractionWindow->show();
    subtractionWindow->raise();
    subtractionWindow->activateWindow();
}

VideoWindow* MainWindow::getVideoWindow() const
{
    return videoWindow;
}

VideoWindow* MainWindow::getSubtractionWindow() const
{
    return subtractionWindow;
}

// Окна с параметрами камеры
void MainWindow::showControlCameraWindow()
{
    controlCameraWindow->show();
    controlCameraWindow->raise();
    controlCameraWindow->activateWindow();
}

ControlCameraWindow* MainWindow::getControlCameraWindow() const
{
    return controlCameraWindow;
}
#include "MainWindow.h"
#include "VideoWindow.h"

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

    startRecordingAction = recordingMenu->addAction("Начать запись");
    stopRecordingAction = recordingMenu->addAction("Остановить запись");
    startRecordingAction->setEnabled(false);
    stopRecordingAction->setEnabled(false);

    recordingMenu->addSeparator();
    saveSelectedFrameAction = recordingMenu->addAction("Сохранить выбранный кадр");
    saveSelectedFrameAction->setEnabled(false);

    setConnected(false, false);

    // Сигналы на подключение и отключение по TCP
    connect(connectTcpAction, &QAction::triggered, this, &MainWindow::connectRequested);
    connect(disconnectTcpAction, &QAction::triggered, this, &MainWindow::disconnectRequested);

    // Сигналы на отображение окон
    connect(showVideoAction, &QAction::triggered, this, &MainWindow::showVideoRequested);
    connect(showSubtractionAction, &QAction::triggered, this, &MainWindow::showSubtractionRequested);

    // Сигнал на выбор кадра
    connect(selectReferenceFrameAction, &QAction::triggered, this, &MainWindow::selectReferenceFrameRequested);

    // Сигналы на запросы записи
    connect(startRecordingAction, &QAction::triggered, this, &MainWindow::startRecordingRequested);
    connect(stopRecordingAction, &QAction::triggered, this, &MainWindow::stopRecordingRequested);

    // Сохранение кадра
    connect(saveSelectedFrameAction, &QAction::triggered, this, &MainWindow::saveSelectedFrameRequested);

    // Окна с видео
    videoWindow = new VideoWindow(this);
    videoWindow->setWindowFlag(Qt::Window);
    videoWindow->setWindowTitle("Исходный видеопоток");

    subtractionWindow = new VideoWindow(this);
    subtractionWindow->setWindowFlag(Qt::Window);
    subtractionWindow->setWindowTitle("Вычитание опорного кадра");
}

// Управление подключением к камере
void MainWindow::showConnectionLost(const QString& message)
{
    QMessageBox::warning(this, tr("Соединение"), tr("Соединение с сервером было разорвано.\nПричина: %1").arg(message));
}

// Управление кнопками
// Управление кнопоками подклоючитсья и отключиться
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

// Управление кнопками записи
void MainWindow::setStartRecordingEnabled(bool enabled)
{
    startRecordingAction->setEnabled(enabled);
}

void MainWindow::setStopRecordingEnabled(bool enabled)
{
    stopRecordingAction->setEnabled(enabled);
}

void MainWindow::setSaveSelectedFrameEnabled(bool enabled)
{
    saveSelectedFrameAction->setEnabled(enabled);
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

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

    showVideoAction = windowsMenu->addAction("Исходный видеопоток");
    showSubtractionAction = windowsMenu->addAction("Вычитание опорного кадра");

    setConnected(false, false);

    connect(connectTcpAction, &QAction::triggered, this, &MainWindow::onConnectClicked);
    connect(disconnectTcpAction, &QAction::triggered, this, &MainWindow::onDisconnectClicked);

    connect(showVideoAction, &QAction::triggered, this, &MainWindow::showVideoRequested);
    connect(showSubtractionAction, &QAction::triggered, this, &MainWindow::showSubtractionRequested);
}

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
    qDebug() << "Connection progress dialog show.";
}

void MainWindow::setConnectionStatus(const QString& text)
{
    if (connectionDialog)
    {
        qDebug() << "Set status:" << text;
        connectionDialog->setLabelText(text);
    }
}

void MainWindow::closeConnectionProgress()
{
    if (!connectionDialog) return;

    connectionDialog->close();
    connectionDialog->deleteLater();
    connectionDialog = nullptr;
    qDebug() << "Connection progress dialog closed.";
}

void MainWindow::onConnectClicked()
{
    emit connectRequested();
}

void MainWindow::onDisconnectClicked()
{
    emit disconnectRequested();
}

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

void MainWindow::showConnectionLost()
{
    QMessageBox::warning(this, "Соединение", "Соединение с сервером было разорвано.");
}

void MainWindow::showVideoWindow()
{
    if (!videoWindow)
    {
        videoWindow = new VideoWindow(this);
        videoWindow->setWindowTitle("Исходный видеопоток");
    }

    videoWindow->show();
    videoWindow->raise();
    videoWindow->activateWindow();
}

void MainWindow::showSubtractionWindow()
{
    if (!subtractionWindow)
    {
        subtractionWindow = new VideoWindow(this);
        subtractionWindow->setWindowTitle("Вычитание опорного кадра");
    }

    subtractionWindow->show();
    subtractionWindow->raise();
    subtractionWindow->activateWindow();
}

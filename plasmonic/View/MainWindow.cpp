#include "MainWindow.h"

#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QProgressDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
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
    
    connectTcpAction = networkMenu->addAction("Установить соединение");
    disconnectTcpAction = networkMenu->addAction("Разорвать соединение");
    setConnected(false, false);
    

    connect(connectTcpAction, &QAction::triggered, this, &MainWindow::onConnectClicked);
    connect(disconnectTcpAction, &QAction::triggered, this, &MainWindow::onDisconnectClicked);

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
    QMessageBox::warning(
        this,
        "Соединение",
        "Соединение с сервером было разорвано."
    );
}

#include "MainWindow.h"
#include "settings_view.h"

#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QProgressDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
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
    menuBar()->addSeparator();
    QAction* opensettingsActions = menuBar()->addAction("Настройки");
    
    QAction* connectTcpAction = networkMenu->addAction("Установить соединение");
    QAction* disconnectTcpAction = networkMenu->addAction("Разорвать соединение");
    

    connect(
        connectTcpAction,
        &QAction::triggered,
        this,
        &MainWindow::onConnectClicked
    );

    connect(
        opensettingsActions,
        &QAction::triggered,
        this,
        &MainWindow::onSettingsClicked
    );

}

MainWindow::~MainWindow()
{
}

void MainWindow::showConnectionProgress()
{
    connectionDialog = new QProgressDialog(this);

    connectionDialog->setWindowTitle("Подключение");
    connectionDialog->setLabelText("Установка соединения по TCP...");
    connectionDialog->setRange(0, 0);
    connectionDialog->setCancelButton(nullptr);
    connectionDialog->setMinimumDuration(0);
    connectionDialog->setAutoClose(false);

    qDebug() << "Connection progress dialog show.";

    connectionDialog->show();
}

void MainWindow::setConnectionStatus(const QString& text)
{
    qDebug() << "Set tatus:" << text;
    if (connectionDialog)
        connectionDialog->setLabelText(text);
}

void MainWindow::closeConnectionProgress()
{
    if (!connectionDialog)
        return;

    connectionDialog->close();
    connectionDialog->deleteLater();
    connectionDialog = nullptr;
	qDebug() << "Connection progress dialog closed.";
}

void MainWindow::onConnectClicked()
{
    emit connectRequested();
}

void MainWindow::onSettingsClicked()
{
    SettingsView settings(this);
    settings.exec();
}

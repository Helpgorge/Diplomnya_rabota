#include "MainWindow.h"
#include "settings_view.h"

#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QProgressDialog>
#include <QTimer>

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

void MainWindow::onConnectClickedTest()
{
    QProgressDialog* dialog = new QProgressDialog(this);

    dialog->setWindowTitle("Подключение");
    dialog->setLabelText("Установка соединения по TCP...");
    dialog->setRange(0, 0);
    dialog->setCancelButton(nullptr);
    dialog->setMinimumDuration(0);
    dialog->setAutoClose(false);

    dialog->show();

    // Через 1 секунду
    QTimer::singleShot(1000, dialog, [dialog]()
        {
            dialog->setLabelText("Установлено соединение с сервером.");
        });

    // Через 2 секунды
    QTimer::singleShot(2000, dialog, [dialog]()
        {
            dialog->setLabelText("Отправка данных...");
        });

    // Через 3 секунды
    QTimer::singleShot(3000, dialog, [dialog]()
        {
            dialog->setLabelText("Подключено.");
        });

    // Через 4 секунды закрываем окно
    QTimer::singleShot(4000, dialog, [dialog]()
        {
            dialog->close();
            dialog->deleteLater();
        });
}

void MainWindow::onSettingsClicked()
{
    SettingsView settings(this);
    settings.exec();
}

void MainWindow::onConnectClicked(){}
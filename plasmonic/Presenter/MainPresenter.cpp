#include "MainPresenter.h"
#include "MainWindow.h"
#include "CameraControlModel.h"
#include "VideoStreamModel.h"

#include <QDebug>

MainPresenter::MainPresenter(MainWindow* view, CameraControlModel* сameraControlModel,
                             VideoStreamModel* videoStreamModel)
    : view(view), сameraControlModel(сameraControlModel), videoStreamModel(videoStreamModel)
{
    // Сигналы управляющего TCP-соединения.
    QObject::connect(сameraControlModel, &CameraControlModel::connected, this, &MainPresenter::onConnected);
    QObject::connect(сameraControlModel, &CameraControlModel::disconnected, this, &MainPresenter::onDisconnected);
    QObject::connect(сameraControlModel, &CameraControlModel::errorOccurred, this, &MainPresenter::onErrorOccurred);

    // Запросы подключения и отключения.
    QObject::connect(view, &MainWindow::connectRequested, this, &MainPresenter::onConnectRequested);
    QObject::connect(view, &MainWindow::disconnectRequested, this, &MainPresenter::onDisconnectRequested);

    // Сигналы GStreamer.
    QObject::connect(videoStreamModel, &VideoStreamModel::frameReceived, this, &MainPresenter::onFrameReceived);
    QObject::connect(videoStreamModel, &VideoStreamModel::streamError, this, &MainPresenter::onStreamError);
    QObject::connect(videoStreamModel, &VideoStreamModel::streamStopped, this, &MainPresenter::onStreamStopped);

    // Запросы отображения окон.
    QObject::connect(view, &MainWindow::showVideoRequested, this, &MainPresenter::onShowVideoRequested);
    QObject::connect(view, &MainWindow::showSubtractionRequested, this, &MainPresenter::onShowSubtractionRequested);
}

// Реакции на нажатие кнопок из вкладки "Сеть"

void MainPresenter::onConnectRequested()
{
    if (connecting || tcpConnected || disconnecting) return;

    connecting = true;
    streamReady = false;

    qInfo() << "Подключение к Raspberry Pi";

    view->setConnected(true, true);
    view->showConnectionProgress();
    view->setConnectionStatus("Establishing TCP connection...");

    // Подключаем управляющий канал (5000).
    сameraControlModel->connectToServer();
}

void MainPresenter::onDisconnectRequested()
{
    qInfo() << "Отключение от Raspberry Pi";

    disconnectAll();
}

// Взаимодействие с CameraControlModel

void MainPresenter::onConnected()
{
    if (disconnecting || !connecting) return;

    tcpConnected = true;

    qInfo() << "TCP: соединение установлено, порт 5000";

    view->setConnectionStatus("Connecting GStreamer...");

    // Запускаем приём RAW после установления TCP-соединения.
    videoStreamModel->startReceiving();
}

void MainPresenter::onDisconnected(bool requestedByUser)
{
    qInfo() << "TCP: соединение разорвано";

    tcpConnected = false;

    if (disconnecting) return;

    const bool connectionLost = !requestedByUser && !connecting;

    disconnectAll();

    if (connectionLost) view->showConnectionLost();
}

void MainPresenter::onErrorOccurred(const QString& error)
{
    if (disconnecting) return;

    qWarning() << "TCP:" << error;

    view->setConnectionStatus("Connection failed: " + error);

    disconnectAll();
}

// Взаимодействие с VideoStreamModel

void MainPresenter::onFrameReceived(const QByteArray& frame)
{
    if (disconnecting || !connecting || !tcpConnected || streamReady || frame.isEmpty()) return;

    streamReady = true;
    connecting = false;

    qInfo() << "GStreamer: первый RAW-кадр получен, размер:" << frame.size() << "байт";

    view->setConnectionStatus("Connected to server.");
    view->closeConnectionProgress();
    view->setConnected(true, false);
}

void MainPresenter::onStreamError(const QString& error)
{
    if (disconnecting || (!connecting && !tcpConnected)) return;

    qWarning() << "GStreamer:" << error;

    const bool connectionLost = streamReady;

    view->setConnectionStatus("GStreamer error: " + error);

    disconnectAll();

    if (connectionLost) view->showConnectionLost();
}

void MainPresenter::onStreamStopped()
{
    qInfo() << "GStreamer: приём видеопотока завершён";

    if (disconnecting || (!connecting && !tcpConnected)) return;

    const bool connectionLost = streamReady;

    qWarning() << "GStreamer: неожиданная остановка видеопотока";

    disconnectAll();

    if (connectionLost) view->showConnectionLost();
}

// Общие функции

void MainPresenter::disconnectAll()
{
    if (disconnecting) return;

    disconnecting = true;

    qInfo() << "Отключение TCP и GStreamer";

    // Сначала запрещаем обработку событий текущего подключения.
    connecting = false;
    tcpConnected = false;
    streamReady = false;

    // Останавливаем видеопоток (5001).
    videoStreamModel->stopReceiving();

    // Отключаем управляющий канал (5000).
    сameraControlModel->disconnectFromServer();

    view->closeConnectionProgress();
    view->setConnected(false, false);

    disconnecting = false;
}

// Управление окнами

void MainPresenter::onShowVideoRequested()
{
    view->showVideoWindow();

    qDebug() << "Запрошено окно исходного видеопотока";
}

void MainPresenter::onShowSubtractionRequested()
{
    view->showSubtractionWindow();

    qDebug() << "Запрошено окно вычитания опорного кадра";
}

// Тест

void MainPresenter::updateFrameStatistics(size_t bytes)
{
    totalFrames_++;
    intervalFrames_++;
    intervalBytes_ += bytes;

    const auto now = Clock::now();
    const double elapsed = std::chrono::duration<double>(now - lastReport_).count();

    if (elapsed < 1.0) return;

    const double fps = intervalFrames_ / elapsed;
    const double mbps = intervalBytes_ * 8.0 / elapsed / 1000000.0;

    qInfo() << "Received:" << totalFrames_ << "| FPS:" << fps << "| Speed:" << mbps << "Mbps"
            << "| Bytes:" << bytes;

    intervalFrames_ = 0;
    intervalBytes_ = 0;
    lastReport_ = now;
}

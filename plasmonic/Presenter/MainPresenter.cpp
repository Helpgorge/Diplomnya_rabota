#include "MainPresenter.h"
#include "MainWindow.h"
#include "VideoStreamModel.h"

#include <QDebug>

MainPresenter::MainPresenter(MainWindow* view, VideoStreamModel* videoStreamModel)
    : view(view), videoStreamModel(videoStreamModel)
{
    // Запросы подключения и отключения
    QObject::connect(view, &MainWindow::connectRequested, this, &MainPresenter::onConnectRequested);
    QObject::connect(view, &MainWindow::disconnectRequested, this, &MainPresenter::onDisconnectRequested);

    // Сигналы VideoStreamModel
    QObject::connect(videoStreamModel, &VideoStreamModel::connectionError, this, &MainPresenter::onStreamError);
    QObject::connect(videoStreamModel, &VideoStreamModel::connected, this, &MainPresenter::onConnected);
}

// Реакции на нажатие кнопок из вкладки "Сеть"

void MainPresenter::onConnectRequested()
{
    qInfo() << "TCP: Подключение к Raspberry Pi";

    view->setConnected(true, true);
    videoStreamModel->connectToServer();
}

void MainPresenter::onDisconnectRequested()
{
    qInfo() << "TCP: Отключение от Raspberry Pi";
    videoStreamModel->disconnectFromServer();
    view->setConnected(false, false);
    view->setStartRecordingEnabled(false);
    view->setStopRecordingEnabled(false);
    view->setSaveSelectedFrameEnabled(false);
    view->setSelectFrameEnabled(false);
}

// Взаимодействие с VideoStreamModel

void MainPresenter::onConnected()
{
    qInfo() << "TCP: Подключено";
    view->setConnected(true, false);
    view->setStartRecordingEnabled(true);
    view->setStopRecordingEnabled(false);
    view->setSaveSelectedFrameEnabled(true);
    view->setSelectFrameEnabled(true);
}

void MainPresenter::onStreamError(const QString& error)
{
    qWarning() << "TCP:" << error;
    view->showConnectionLost(error);
    view->setConnected(false, false);
    view->setStartRecordingEnabled(false);
    view->setStopRecordingEnabled(false);
    view->setSaveSelectedFrameEnabled(false);
    view->setSelectFrameEnabled(false);
}

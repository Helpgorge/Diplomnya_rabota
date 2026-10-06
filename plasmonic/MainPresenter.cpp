#include "MainPresenter.h"
#include "MainWindow.h"

MainPresenter::MainPresenter(MainWindow* view)
    : view(view)
{
    QObject::connect(
        view,
        &MainWindow::connectRequested,
        view,
        [this]()
        {
            onConnectRequested();
        }
    );
}

MainPresenter::~MainPresenter()
{
}

void MainPresenter::onConnectRequested()
{
    bool connected = true;
    view->showConnectionProgress();

    view->setConnectionStatus(
        "Установлено соединение с сервером."
    );

    view->setConnectionStatus(
        "Отправка данных..."
    );

	view->closeConnectionProgress();
}
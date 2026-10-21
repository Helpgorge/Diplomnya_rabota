#include <QApplication>

#include "MainWindow.h"
#include "MainPresenter.h"
#include "CameraControlModel.h"
#include "VideoStreamModel.h"
#include "Logger.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Логи
    Logger::initialize();

    int result = 0;

    {
        // Model
        CameraControlModel сameraControlModel;
        VideoStreamModel videoStreamModel;

        // View
        MainWindow window;

        // Presenter
        MainPresenter mainPresenter(&window, &сameraControlModel, &videoStreamModel);

        window.show();

        result = app.exec();
    }

    Logger::shutdown();

    return result;
}
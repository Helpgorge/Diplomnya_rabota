#include <QApplication>

#include "MainWindow.h"
#include "MainPresenter.h"
#include "VideoPresenter.h"

#include "CameraControlModel.h"
#include "VideoStreamModel.h"
#include "ImageProcessingModel.h"

#include "Logger.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Logger::initialize();

    int result = 0;

    {
        // Model
        CameraControlModel cameraControlModel;
        VideoStreamModel videoStreamModel;
        ImageProcessingModel imageProcessingModel;

        // View
        MainWindow window;

        // Presenter
        MainPresenter mainPresenter(&window, &cameraControlModel, &videoStreamModel);

        VideoPresenter videoPresenter(&window, &videoStreamModel, &imageProcessingModel);

        window.show();

        result = app.exec();
    }

    Logger::shutdown();

    return result;
}

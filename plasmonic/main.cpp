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
        VideoPresenter videoPresenter(&videoStreamModel, &imageProcessingModel);

        MainPresenter mainPresenter(&window, &cameraControlModel, &videoStreamModel, &videoPresenter);

        window.show();

        result = app.exec();
    }

    Logger::shutdown();

    return result;
}

#include <QApplication>

#include "MainWindow.h"
// #include "MainPresenter.h"
// #include "TcpClient.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Model
    // TcpClient tcpClient;

    // View
    MainWindow window;

    // Presenter
    // MainPresenter presenter(&window, &tcpClient);

    window.show();

    return app.exec();
}
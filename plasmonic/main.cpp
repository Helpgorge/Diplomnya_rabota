#include <QApplication>

#include "MainWindow.h"
#include "MainPresenter.h"
#include "TcpClientModel.h"


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Model
    TcpClientModel tcpClient;

    // View
    MainWindow window;

    // Presenter
    MainPresenter presenter(&window, &tcpClient);

    window.show();

    return app.exec();
}
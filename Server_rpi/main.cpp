#include <QCoreApplication>
#include <QDebug>

#include "TcpServer.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    TcpServer server;

    if (!server.start(5000))
        return 1;

    return app.exec();
}
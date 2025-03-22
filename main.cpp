#include <QApplication>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    if (!window.connectToGreeter()) {
        qCritical() << "Falha ao conectar ao LightDM";
        return 1;
    }

    window.show();
    return app.exec();
}

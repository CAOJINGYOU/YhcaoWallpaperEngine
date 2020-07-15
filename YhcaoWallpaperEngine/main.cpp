#include "mainwindow.h"
#include "logger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Logger::initLog();

    MainWindow w;
    w.show();
    return a.exec();
}

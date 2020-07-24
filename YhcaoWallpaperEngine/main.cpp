#include "mainwindow.h"
#include "logger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator tsor;           //创建翻译器
    tsor.load("vc_zh_cn.qm");    //加载语言包
    a.installTranslator(&tsor); //安装翻译器

    Logger::initLog();

    MainWindow w;
    w.show();
    return a.exec();
}

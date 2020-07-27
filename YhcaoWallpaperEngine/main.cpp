#include "mainwindow.h"
#include "logger.h"
#include <QApplication>
//#include <QFileInfo>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Logger::initLog();
	//QString str = QCoreApplication::applicationDirPath() + QDir::separator() +"yhcaowallpaperengine_zh.qm";
	//qInfo()<<str;
	//QFileInfo fileInfo(str);
	//if(fileInfo.exists())
	//{
	//    QTranslator tsor;           //创建翻译器
	//    qInfo() <<  tsor.load("yhcaowallpaperengine_zh.qm", a.applicationDirPath());    //加载语言包
	//    qInfo() << a.installTranslator(&tsor); //安装翻译器
	//    qInfo() << ("找到语言文件");
	//}
	//else
	//{
	//    qInfo() << ("未找到语言文件");
	//}

	MainWindow w;
	w.show();
	return a.exec();
}

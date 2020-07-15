#include "logger.h"

#include <QApplication>
#include<QMutex>
#include<QFile>
#include<QTextStream>
#include<QDir>
#include <QDateTime>

#include "configjson.h"

#define LOG_PATH     qApp->applicationDirPath()+"/log"

static int g_mLogType = 0;

namespace Logger
{

void MessageOutPut(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if( type < g_mLogType )
    {
        return;
    }

    static QMutex mutex;
    mutex.lock();
//    QString text;
//    switch(type)
//    {
//    case QtDebugMsg:
//       text = QString("Debug:");
//       break;

//    case QtWarningMsg:
//       text = QString("Warning:");
//       break;

//    case QtCriticalMsg:
//       text = QString("Critical:");
//       break;

//    case QtFatalMsg:
//       text = QString("Fatal:");
//       break;
//    default:
//       break;
//    }
    //日志写到文件
    QDateTime dt = QDateTime::currentDateTime();
    //QString current_date_time = dt.toString("yyyy-MM-dd hh:mm:ss");
    //QString message = QString("%1 %2%3").arg(current_date_time).arg(text).arg(msg);

    QString formatMsg = qFormatLogMessage( type, context, msg );

    QFile file(LOG_PATH+dt.toString("/yyyy-MM-dd.log"));
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    //text_stream << message << endl;
    text_stream << formatMsg << endl;
    file.flush();
    file.close();
    mutex.unlock();
}

void initLog()
{
//#ifndef _DEBUG

    g_mLogType = CONFIG_JSON->ReadInt("logtype",2);
    CONFIG_JSON->Write("logtype",g_mLogType);

    QDir dir(LOG_PATH);
    if (!dir.exists())
    {
        dir.mkpath(dir.absolutePath());
    }

    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

    qInstallMessageHandler(MessageOutPut);
//#endif
}
}

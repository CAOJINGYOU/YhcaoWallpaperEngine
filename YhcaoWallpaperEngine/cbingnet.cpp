#include "cbingnet.h"

QString g_strUrl = "https://www.bing.com/HPImageArchive.aspx?format=js&idx=0&n=1&mkt=zh-cn";

//"https://www.bing.com/HPImageArchive.aspx?format=js&idx=0&n=1&mkt=en-US"

CBingNet::CBingNet(QObject *parent) : QObject(parent)
{
    connect(&manager,&QNetworkAccessManager::finished,this,&CBingNet::downloadFinished);
}

void CBingNet::execute()
{
    QUrl url(g_strUrl);

    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    currentReplys.append(reply);
}

void CBingNet::doGetJson(QNetworkReply *reply)
{
    QByteArray byteArray = reply->readAll();

    QJsonParseError jsonError;
    QJsonDocument json = QJsonDocument::fromJson(byteArray,&jsonError);

    if (!json.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if(json.isObject())
        {
            QJsonObject obj =json.object();
            if(obj.contains("images"))
            {
                QJsonValue value = obj.value("images");
                if(value.isArray())
                {
                    QJsonArray jsonArray = value.toArray();
                    QJsonValue imageValue = jsonArray.at(0);
                    if(imageValue.isObject())
                    {
                        QString image = imageValue["url"].toString();
                        QString baseUrl = "https://www.bing.com";
                        baseUrl.append(image);
                        QUrl url(baseUrl);
                        QNetworkRequest request(url);
                        QNetworkReply *replyImage = manager.get(request);
                        currentReplys.append(replyImage);
                    }
                }
            }
        }
    }
}

void CBingNet::downloadFinished(QNetworkReply *reply)
{
    if(reply->url() == QUrl(g_strUrl))
    {
        doGetJson(reply);
    }
    else
    {
        saveImages(reply);
    }

    currentReplys.removeAll(reply);
    reply->deleteLater();
}

void CBingNet::saveImages(QNetworkReply *reply)
{
    QString fileName = QStandardPaths:: writableLocation(QStandardPaths::PicturesLocation)+QDir::separator()+"Bing"+QDir::separator();
    QDir dir(fileName);
    if(!dir.exists())
    {
        dir.mkpath(fileName);
    }
    QDate date = QDate::currentDate();
    fileName.append(date.toString("yyyy-MM-dd")).append(".jpg");
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }
    file.write(reply->readAll());
    file.close();

    emit ViewImage(fileName);
}

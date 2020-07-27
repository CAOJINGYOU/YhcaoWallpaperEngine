#include "cunsplashnet.h"
#include <QApplication>
#include <QDesktopWidget>

CUnsplashNet::CUnsplashNet(QObject* parent) : QObject(parent)
{
	connect(&manager, &QNetworkAccessManager::finished, this, &CUnsplashNet::downloadFinished);
}

void CUnsplashNet::execute(QString strUrl)
{
	QDesktopWidget* desktop = QApplication::desktop();

	int height = desktop->height();
	int width = desktop->width();

	QUrl url;

	if (strUrl.contains(".unsplash."))
	{
		url.setUrl(strUrl + "/" + QString("%1x%2").arg(width).arg(height));
	}
	else if (strUrl.contains("picsum.photos"))
	{
		url.setUrl(strUrl + "/" + QString("%1/%2").arg(width).arg(height));
	}
	else
	{
		url.setUrl(strUrl);
	}

	QNetworkRequest request(url);
	QNetworkReply* reply = manager.get(request);
	currentReplys.append(reply);
}

void CUnsplashNet::downloadFinished(QNetworkReply* reply)
{
	QNetworkReply::NetworkError error = reply->error();
	if (error != QNetworkReply::NoError)
	{
		//<https://www.jianshu.com/p/4799efe85e84>
		qCritical() << "URL:" << reply->url().toString() << "\tNetworkError:" << reply->errorString() << "\tErrorId:" << error;
	}
	else
	{
		const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		if (!redirectionTarget.isNull()) {//如果网址跳转重新请求

			const QUrl redirectedUrl = reply->url().resolved(redirectionTarget.toUrl());
			//qDebug()<<"redirectedUrl:"<<redirectedUrl.url();
			QNetworkRequest request(redirectedUrl);
			QNetworkReply* reply1 = manager.get(request);
			currentReplys.append(reply1);
			//return;
		}
		else
		{
			saveImages(reply);
		}
	}

	currentReplys.removeAll(reply);
	reply->deleteLater();
}

void CUnsplashNet::saveImages(QNetworkReply* reply)
{
	QString strFolder;
	if (reply->url().toString().contains(".unsplash."))
	{
		strFolder = "Unsplash";
	}
	else if (reply->url().toString().contains("picsum.photos"))
	{
		strFolder = "Picsum";
	}
	else
	{
		strFolder = "YhcaoWallpaperEngine";
	}

	QString fileName = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QDir::separator() + strFolder + QDir::separator();
	QDir dir(fileName);
	if (!dir.exists())
	{
		dir.mkpath(fileName);
	}
	QDateTime date = QDateTime::currentDateTime();
	fileName.append(date.toString("yyyy-MM-dd hhmmss")).append(".jpg");
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		return;
	}
	file.write(reply->readAll());
	file.close();

	emit ViewImage(fileName);
}

﻿#include "configjson.h"
#include <QGlobalStatic>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <QStandardPaths>
#include <QDir>

Q_GLOBAL_STATIC(ConfigJson, configJson)

ConfigJson::ConfigJson()
	:m_fileName("config.json")
{
	ReadJson();
}

ConfigJson::~ConfigJson()
{
	WriteJson();
}

ConfigJson* ConfigJson::instance()
{
	return configJson;
}

void ConfigJson::IntilJson()
{
	m_cache.insert("selecttype", "SingleImage");

	QJsonObject jsonObjMonochrome;
	jsonObjMonochrome.insert("red", 0);
	jsonObjMonochrome.insert("green", 170);
	jsonObjMonochrome.insert("blue", 127);
	jsonObjMonochrome.insert("alpha", 200);
	jsonObjMonochrome.insert("time", 0);
	jsonObjMonochrome.insert("lcd", 0);
	m_cache.insert("Monochrome", jsonObjMonochrome);

	QJsonObject jsonObjSingleImage;
	jsonObjSingleImage.insert("imagepath", "Yasuo.jpg");
	m_cache.insert("SingleImage", jsonObjSingleImage);

	QJsonObject jsonObjMultipleImage;
	jsonObjMultipleImage.insert("imagepath", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
	jsonObjMultipleImage.insert("time", 60);
	m_cache.insert("MultipleImage", jsonObjMultipleImage);

	QJsonObject jsonObjBing;
	jsonObjBing.insert("url", "https://uploadbeta.com/api/pictures/random/?key=BingEverydayWallpaperPicture");
	jsonObjBing.insert("time", 0);
	m_cache.insert("Bing", jsonObjBing);

	QJsonObject jsonObjUnsplash;
	jsonObjUnsplash.insert("time", 60);
	jsonObjUnsplash.insert("selectindex", 0);
	QJsonArray jsonUrl;
	jsonUrl.append("https://source.unsplash.com/random");
	jsonUrl.append("https://picsum.photos");
	jsonUrl.append("http://www.dmoe.cc/random.php");
	jsonUrl.append("https://img.paulzzh.tech/touhou/random");
	jsonObjUnsplash.insert("url", jsonUrl);
	m_cache.insert("Unsplash", jsonObjUnsplash);

	QJsonObject jsonObjWeb;
	jsonObjWeb.insert("url", "https://tianqi.qq.com/");//http://tianqi.sogou.com/pc/weather,https://acg.yanwz.cn/wallpaper/api.php
	jsonObjWeb.insert("time", 60);
	QJsonArray jsonUrlWeb;
	jsonUrlWeb.append("https://tianqi.qq.com/");
	jsonUrlWeb.append("http://tianqi.sogou.com/pc/weather");
	jsonUrlWeb.append("https://acg.yanwz.cn/wallpaper/api.php");
	jsonObjWeb.insert("otherurl", jsonUrlWeb);
	m_cache.insert("Web", jsonObjWeb);

	QJsonObject jsonObjVideo;
	jsonObjVideo.insert("videopath", "");
	jsonObjVideo.insert("volume", 100);
	m_cache.insert("Video", jsonObjVideo);

	QJsonObject jsonObOther;
	jsonObOther.insert("lcd", 0);
	jsonObOther.insert("autostart", 0);
	m_cache.insert("Other", jsonObOther);
}

bool ConfigJson::ReadJson()
{
	QFile file(m_fileName);

	if (!file.exists())
	{
		IntilJson();
		WriteJson();
	}

	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QByteArray allData = file.readAll();
	file.close();

	QJsonParseError jsonError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(allData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		return false;
	}

	QJsonObject root = jsonDoc.object();
	m_cache = root.toVariantMap();

	return true;
}
void ConfigJson::WriteJson()
{
	QJsonObject root = QJsonObject::fromVariantMap(m_cache);
	QJsonDocument jsonDoc(root);
	QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);
	QFile file(m_fileName);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(data);
		file.close();
	}
}

void ConfigJson::Write(const QString& key, const QVariant& value)
{
	m_cache.insert(key, value);
}

QString ConfigJson::ReadString(const QString& key, const QString& defaultValue)
{
	if (m_cache.contains(key))
	{
		return m_cache.value(key).toString();
	}

	return defaultValue;
}

QJsonArray ConfigJson::ReadArray(const QString& key)
{
	if (m_cache.contains(key))
	{
		return m_cache.value(key).toJsonArray();
	}

	return QJsonArray();
}

bool ConfigJson::ReadBool(const QString& key, bool defaultValue)
{
	if (m_cache.contains(key))
	{
		return m_cache.value(key).toBool();
	}

	return defaultValue;
}

int ConfigJson::ReadInt(const QString& key, int defaultValue)
{
	if (m_cache.contains(key))
	{
		return m_cache.value(key).toInt();
	}

	return defaultValue;
}

QString ConfigJson::GetSelectType()
{
	return m_cache.value("selecttype").toString();
}

void ConfigJson::SetSelectType(QString selectType)
{
	m_cache.insert("selecttype", selectType);
}

QString ConfigJson::GetSingleImage()
{
	return m_cache.value("SingleImage").toJsonObject().value("imagepath").toString();
}

void ConfigJson::SetSingleImage(QString imagepath)
{
	QVariantMap val = m_cache["SingleImage"].toMap();
	val["imagepath"] = imagepath;
	m_cache["SingleImage"] = val;
}

QString ConfigJson::GetMultipleImage()
{
	return m_cache.value("MultipleImage").toJsonObject().value("imagepath").toString();
}

void ConfigJson::SetMultipleImage(QString imagepath)
{
	QVariantMap val = m_cache["MultipleImage"].toMap();
	val["imagepath"] = imagepath;
	m_cache["MultipleImage"] = val;
}

int ConfigJson::GetMultipleImageTime()
{
	return m_cache.value("MultipleImage").toJsonObject().value("time").toInt();
}

void ConfigJson::SetMultipleImage(int time)
{
	QVariantMap val = m_cache["MultipleImage"].toMap();
	val["time"] = time;
	m_cache["MultipleImage"] = val;
}

int ConfigJson::GetUnsplashTime()
{
	return m_cache.value("Unsplash").toJsonObject().value("time").toInt();
}

void ConfigJson::SetUnsplashTime(int time)
{
	QVariantMap val = m_cache["Unsplash"].toMap();
	val["time"] = time;
	m_cache["Unsplash"] = val;
}

QJsonArray ConfigJson::GetUnsplashUrl()
{
	return m_cache.value("Unsplash").toJsonObject()["url"].toArray();
}

int ConfigJson::GetUnsplashSelectIndex()
{
	return m_cache.value("Unsplash").toJsonObject().value("selectindex").toInt();
}

void ConfigJson::SetUnsplashSelectIndex(int index)
{
	QVariantMap val = m_cache["Unsplash"].toMap();
	val["selectindex"] = index;
	m_cache["Unsplash"] = val;
}

QColor ConfigJson::GetMonochromeColor()
{
	QJsonObject jsonObj = m_cache.value("Monochrome").toJsonObject();
	QColor qColor(jsonObj.value("red").toInt(), jsonObj.value("green").toInt(), jsonObj.value("blue").toInt(), jsonObj.value("alpha").toInt());
	return qColor;
}

void ConfigJson::SetMonochromeColor(QColor qColor)
{
	QVariantMap val = m_cache["Monochrome"].toMap();
	val["red"] = qColor.red();
	val["green"] = qColor.green();
	val["blue"] = qColor.blue();
	val["alpha"] = qColor.alpha();
	m_cache["Monochrome"] = val;
}

int ConfigJson::GetMonochromeTime()
{
	return m_cache.value("Monochrome").toJsonObject().value("time").toInt();
}

void ConfigJson::SetMonochromeTime(int time)
{
	QVariantMap val = m_cache["Monochrome"].toMap();
	val["time"] = time;
	m_cache["Monochrome"] = val;
}

bool ConfigJson::GetOtherLCD()
{
	return m_cache.value("Other").toJsonObject().value("lcd").toInt() == 1;
}

void ConfigJson::SetOtherLCD(bool bLcd)
{
	QVariantMap val = m_cache["Other"].toMap();
	val["lcd"] = (bLcd ? 1 : 0);
	m_cache["Other"] = val;
}

bool ConfigJson::GetOtherAutoStart()
{
	return m_cache.value("Other").toJsonObject().value("autostart").toInt() == 1;
}

void ConfigJson::SetOtherAutoStart(bool bAutoStart)
{
	QVariantMap val = m_cache["Other"].toMap();
	val["autostart"] = (bAutoStart ? 1 : 0);
	m_cache["Other"] = val;
}

QString ConfigJson::GetVideoPath()
{
	return m_cache.value("Video").toJsonObject().value("videopath").toString();
}

void ConfigJson::SetVideoPath(QString videoPath)
{
	QVariantMap val = m_cache["Video"].toMap();
	val["videopath"] = videoPath;
	m_cache["Video"] = val;
}

int ConfigJson::GetVideoVolume()
{
	return m_cache.value("Video").toJsonObject().value("volume").toInt();
}

void ConfigJson::SetVideoVolume(int volume)
{
	QVariantMap val = m_cache["Video"].toMap();
	val["volume"] = volume;
	m_cache["Video"] = val;
}

QString ConfigJson::GetWebUrl()
{
	return m_cache.value("Web").toJsonObject().value("url").toString();
}

void ConfigJson::SetWebUrl(QString webUrl)
{
	QVariantMap val = m_cache["Web"].toMap();
	val["url"] = webUrl;
	m_cache["Web"] = val;
}

int ConfigJson::GetWebTime()
{
	return m_cache.value("Web").toJsonObject().value("time").toInt();
}

void ConfigJson::SetWebTime(int time)
{
	QVariantMap val = m_cache["Web"].toMap();
	val["time"] = time;
	m_cache["Web"] = val;
}

QString ConfigJson::GetBingUrl()
{
	return m_cache.value("Bing").toJsonObject().value("url").toString();
}

void ConfigJson::SetBingUrl(QString strUrl)
{
	QVariantMap val = m_cache["Bing"].toMap();
	val["url"] = strUrl;
	m_cache["Bing"] = val;
}

int ConfigJson::GetBingTime()
{
	return m_cache.value("Bing").toJsonObject().value("time").toInt();
}

void ConfigJson::SetBingTime(int time)
{
	QVariantMap val = m_cache["Bing"].toMap();
	val["time"] = time;
	m_cache["Bing"] = val;
}

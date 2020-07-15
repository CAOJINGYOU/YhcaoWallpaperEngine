#include "configjson.h"
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

ConfigJson *ConfigJson::instance()
{
    return configJson;
}

void ConfigJson::IntilJson()
{
    m_cache.insert("selecttype","SingleImage");

    QJsonObject jsonObjSingleImage;
    jsonObjSingleImage.insert("imagepath", "Yasuo.jpg");
    m_cache.insert("SingleImage",jsonObjSingleImage);

    QJsonObject jsonObjMultipleImage;
    jsonObjMultipleImage.insert("imagepath", QStandardPaths:: writableLocation(QStandardPaths::PicturesLocation));
    jsonObjMultipleImage.insert("time", 60);
    jsonObjMultipleImage.insert("selectindex", 0);
    m_cache.insert("MultipleImage",jsonObjMultipleImage);

    m_cache.insert("Bing",QJsonObject());

    QJsonObject jsonObjUnsplash;
    jsonObjUnsplash.insert("time", 60);
    QJsonArray jsonUrl;
    jsonUrl.append("https://source.unsplash.com/random");
    jsonUrl.append("https://source.unsplash.com/user/yhcao");
    jsonUrl.append("https://source.unsplash.com/user/yhcao/likes");
    jsonUrl.append("https://source.unsplash.com/weekly?water");
    jsonObjUnsplash.insert("url", jsonUrl);
    m_cache.insert("Unsplash",jsonObjUnsplash);

    QJsonObject jsonObjWeb;
    jsonObjWeb.insert("url", "");
    m_cache.insert("Web",jsonObjWeb);

    QJsonObject jsonObjVideo;
    jsonObjVideo.insert("videopath", "");
    m_cache.insert("Video",jsonObjVideo);
}

bool ConfigJson::ReadJson()
{
    QFile file(m_fileName);

    if(!file.exists())
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

void ConfigJson::Write(const QString &key, const QVariant &value)
{
    m_cache.insert(key, value);
}

QString ConfigJson::ReadString(const QString &key, const QString &defaultValue)
{
    if (m_cache.contains(key))
    {
        return m_cache.value(key).toString();
    }

    return defaultValue;
}

QJsonArray ConfigJson::ReadArray(const QString &key)
{
    if (m_cache.contains(key))
    {
        return m_cache.value(key).toJsonArray();
    }

    return QJsonArray();
}

bool ConfigJson::ReadBool(const QString &key, bool defaultValue)
{
    if (m_cache.contains(key))
    {
        return m_cache.value(key).toBool();
    }

    return defaultValue;
}

int ConfigJson::ReadInt(const QString &key, int defaultValue)
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
    m_cache.value("SingleImage").toJsonObject()["imagepath"] = imagepath;
}

QString ConfigJson::GetMultipleImage()
{
    return m_cache.value("MultipleImage").toJsonObject().value("imagepath").toString();
}

void ConfigJson::SetMultipleImage(QString imagepath)
{
    m_cache.value("MultipleImage").toJsonObject()["imagepath"] = imagepath;
}

int ConfigJson::GetMultipleImageTime()
{
    return m_cache.value("MultipleImage").toJsonObject().value("time").toInt();
}

void ConfigJson::SetMultipleImage(int time)
{
    m_cache.value("MultipleImage").toJsonObject()["time"] = time;
}

int ConfigJson::GetUnsplashTime()
{
    return m_cache.value("Unsplash").toJsonObject().value("time").toInt();
}

void ConfigJson::SetUnsplashTime(int time)
{
    m_cache.value("Unsplash").toJsonObject()["time"] = time;
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
    m_cache.value("Unsplash").toJsonObject()["selectindex"] = index;
}

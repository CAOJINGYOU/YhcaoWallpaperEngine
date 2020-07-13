#ifndef CONFIGJSON_H
#define CONFIGJSON_H

#include <QVariantMap>
#include <QString>

class ConfigJson
{
public:
    ConfigJson();
    ~ConfigJson();
public:
    static ConfigJson *instance();

    void Write(const QString &key, const QVariant& value);

    QString ReadString(const QString &key,const QString &defaultValue = "");
    QJsonArray ReadArray(const QString &key);
    bool ReadBool(const QString &key, bool defaultValue = false);
    int ReadInt(const QString &key, int defaultValue = 0);

    bool ReadJson();
    void WriteJson();
    // 初始化内容
    void IntilJson();

    QJsonObject CreatTableJson(QString code,QString name,int order);

public:
    // 单图
    QString GetSingleImage();
    void SetSingleImage(QString imagepath);

    // 多图
    QString GetMultipleImage();
    void SetMultipleImage(QString imagepath);

    int GetMultipleImageTime();
    void SetMultipleImage(int time);

private:
    QVariantMap m_cache;
    QString m_fileName;

};

// 宏定义来获取配置
#define CONFIG_JSON  ConfigJson::instance()

#endif // CONFIGJSON_H

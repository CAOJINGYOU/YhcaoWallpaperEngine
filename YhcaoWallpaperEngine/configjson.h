#ifndef CONFIGJSON_H
#define CONFIGJSON_H

#include <QVariantMap>
#include <QString>
#include <QColor>

class ConfigJson
{
public:
	ConfigJson();
	~ConfigJson();
public:
	static ConfigJson* instance();


	void Write(const QString& key, const QVariant& value);

	QString ReadString(const QString& key, const QString& defaultValue = "");
	QJsonArray ReadArray(const QString& key);
	bool ReadBool(const QString& key, bool defaultValue = false);
	int ReadInt(const QString& key, int defaultValue = 0);

private:

	bool ReadJson();
	void WriteJson();
	// 初始化内容
	void IntilJson();

public:
	QString GetSelectType();
	void SetSelectType(QString selectType);

	// 单图
	QString GetSingleImage();
	void SetSingleImage(QString imagepath);

	// 多图
	QString GetMultipleImage();
	void SetMultipleImage(QString imagepath);

	int GetMultipleImageTime();
	void SetMultipleImage(int time);

	int GetUnsplashTime();
	void SetUnsplashTime(int time);
	QJsonArray GetUnsplashUrl();
	int GetUnsplashSelectIndex();
	void SetUnsplashSelectIndex(int index);

	QColor GetMonochromeColor();
	void SetMonochromeColor(QColor qColor);

	int GetMonochromeTime();
	void SetMonochromeTime(int time);

	bool GetOtherLCD();
	void SetOtherLCD(bool bLcd);

	bool GetOtherAutoStart();
	void SetOtherAutoStart(bool bAutoStart);

	QString GetVideoPath();
	void SetVideoPath(QString videoPath);
	int GetVideoVolume();
	void SetVideoVolume(int volume);

	QString GetWebUrl();
	void SetWebUrl(QString webUrl);
	int GetWebTime();
	void SetWebTime(int time);
private:
	QVariantMap m_cache;
	QString m_fileName;

};

// 宏定义来获取配置
#define CONFIG_JSON  ConfigJson::instance()

#endif // CONFIGJSON_H

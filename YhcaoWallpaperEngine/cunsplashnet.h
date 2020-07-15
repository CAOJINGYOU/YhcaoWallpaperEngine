#ifndef CUNSPLASHNET_H
#define CUNSPLASHNET_H

#include <QObject>
#include <QtNetwork>

class CUnsplashNet : public QObject
{
    Q_OBJECT
public:
    explicit CUnsplashNet(QObject *parent = nullptr);

private:
    QNetworkAccessManager manager;
    QVector<QNetworkReply *> currentReplys;

public:
    void execute(QString strUrl);
signals:

    void ViewImage(QString fileName);

public slots:
    void downloadFinished(QNetworkReply *reply);
    void saveImages(QNetworkReply *reply);
};

#endif // CUNSPLASHNET_H

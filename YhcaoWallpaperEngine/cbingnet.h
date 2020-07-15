#ifndef CBINGNET_H
#define CBINGNET_H

#include <QObject>
#include <QtNetwork>

class CBingNet : public QObject
{
    Q_OBJECT
public:
    explicit CBingNet(QObject *parent = nullptr);

private:
    QNetworkAccessManager manager;
    QVector<QNetworkReply *> currentReplys;

public:
    void execute();
signals:

    void ViewImage(QString fileName);

public slots:
    void doGetJson(QNetworkReply *reply);
    void downloadFinished(QNetworkReply *reply);
    void saveImages(QNetworkReply *reply);
};

#endif // CBINGNET_H

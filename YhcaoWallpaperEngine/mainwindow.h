﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QButtonGroup>
#include "cbingnet.h"
#include "cunsplashnet.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLCDNumber>
#include <QMovie>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaPlaylist>

#ifdef __MINGW32__
#include <ActiveQt/QAxWidget>
#else
#include <QWebEngineView>
#endif


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private:
    QSystemTrayIcon *m_SysTrayIcon;

    QAction *m_ShowWindow;
    QAction *m_ExitAppAction;
    QMenu *m_Menu;

    bool m_bClose;

    WId m_nViewId;

    QWidget *m_view;

    QMap<QString,int> m_mapType;
    QVector<QString> m_vectorType;

    QButtonGroup *m_bgGroup;

    QStringList m_filePathList;

    QTimer *m_pTimer;

    CBingNet *m_pBingNet;

    CUnsplashNet *m_pUnsplashNet;

    QString m_imagefile;

    //LCD
    QVBoxLayout *m_layoutVLCD;
    QLCDNumber *m_lcdNumber;

    QTimer *m_pTimerLCD;

    //GIF
    QVBoxLayout *m_layoutVGIF;
    QMovie *m_movieGIF;
    QLabel *m_labelGIF;

    //Video
    QVBoxLayout *m_layoutVVideo;
    QMediaPlayer *m_mediaPlayerVideo;
    QVideoWidget *m_videoWidgetVideo;
    QMediaPlaylist *m_mediaPlaylistVideo;

    //Web
    QVBoxLayout* m_layoutVWeb;
#ifdef __MINGW32__
    QAxWidget *m_webWidget;
#else
    QWebEngineView *m_webEngineView;
#endif


private slots:
    void on_showMainAction();
    void on_exitAppAction();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

    void on_pushButtonSingleImage_clicked();

    void on_lineEditSingleImage_textChanged(const QString &arg1);


    void on_bgGroup_toggled(int id, bool status);

    void on_pushButtonMultipleImage_clicked();

    void on_lineEditMultipleImage_textChanged(const QString &arg1);

    void on_handleTimeout();

    void on_spinBoxMultipleImage_valueChanged(int arg1);

    void on_spinBoxUnsplash_valueChanged(int arg1);

    void on_BingSetViewImage(QString path);
    void on_UnsplashSetViewImage(QString path);

    void on_comboBoxUnsplash_currentIndexChanged(int index);

    void on_pushButtonMonochrome_clicked();

    void on_spinBoxMonochrome_valueChanged(int arg1);

    void on_handleTimeLCDout();

    void on_checkBoxLCD_stateChanged(int arg1);

    void on_pushButtonVideo_clicked();

    void on_lineEditVideo_textChanged(const QString &arg1);

    void on_spinBoxVideo_valueChanged(int arg1);

    void on_pushButtonWeb_clicked();

    void on_lineEditWeb_textChanged(const QString &arg1);

    void on_spinBoxWeb_valueChanged(int arg1);

    void on_checkBoxAutoStart_stateChanged(int arg1);

    void on_spinBoxBing_valueChanged(int arg1);

private:
    void closeEvent(QCloseEvent *event);
    void resizeEvent ( QResizeEvent * event );
    // 初始化托盘
    void InitSysTrayIcon();
    void releseMain();
    void Init();

    void SetViewImage(QString path);

    void BingNetExecute();
    void SetMainBGImage();
    void ScanMultipleImage(const QString &arg1);
    void SetLabelColor();
    void SetViewColor();
    void SetLCD(bool checked);
    void SetGif(bool bchecked,int height, int width);
    void SetVideo(bool bDisplay, QString arg1);
    void SetWeb(bool bDisplay, QString arg1);
};
#endif // MAINWINDOW_H

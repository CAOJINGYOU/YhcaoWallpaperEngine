#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include "utils.h"
#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <configjson.h>
#include "fileutils.h"
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QTimer>
#include <QDate>
#include <QColorDialog>
#include <QPainter>

enum SELECTTYPE
{
    MONOCHROME,
    SINGLEIMAGE,
    MULTIPLEIMAGE,
    BING,
    UNSPLASH,
    WEB,
    VIDEO,
    TYPEMAX
};

void MainWindow::InitSysTrayIcon()
{
    m_SysTrayIcon = new QSystemTrayIcon(this);
    QIcon icon = QIcon(":/icon/icon.png");
    m_SysTrayIcon->setIcon(icon);
    m_SysTrayIcon->setToolTip("Wallpaper");
    connect(m_SysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

    m_ShowWindow = new QAction(QIcon("://icon/ShowWindow.png"),"显示程序",this);
    connect(m_ShowWindow,SIGNAL(triggered()),SLOT(on_showMainAction()));
    m_ExitAppAction = new QAction(QIcon(":/icon/ExitApp.png"),"退出程序",this);
    connect(m_ExitAppAction,SIGNAL(triggered()),this,SLOT(on_exitAppAction()));

    m_Menu = new QMenu(this);
    m_Menu->addAction(m_ShowWindow);
    m_Menu->addAction(m_ExitAppAction);
    //m_Menu->setStyleSheet("QMenu{color:black;border-bottom:1px solid #ccc;}");
    m_SysTrayIcon->setContextMenu(m_Menu);

    m_SysTrayIcon->show();
}

void MainWindow::Init()
{
    int nIndex = 0;

    m_mapType.insert("Monochrome",nIndex++);
    m_mapType.insert("SingleImage",nIndex++);
    m_mapType.insert("MultipleImage",nIndex++);
    m_mapType.insert("Bing",nIndex++);
    m_mapType.insert("Unsplash",nIndex++);
    m_mapType.insert("Web",nIndex++);
    m_mapType.insert("Video",nIndex++);

    m_vectorType.append("Monochrome");
    m_vectorType.append("SingleImage");
    m_vectorType.append("MultipleImage");
    m_vectorType.append("Bing");
    m_vectorType.append("Unsplash");
    m_vectorType.append("Web");
    m_vectorType.append("Video");

    m_bgGroup = new QButtonGroup( this );

    m_bgGroup->addButton(ui->radioButtonMonochrome,m_mapType["Monochrome"]);
    m_bgGroup->addButton(ui->radioButtonSingleImage,m_mapType["SingleImage"]);
    m_bgGroup->addButton(ui->radioButtonMultipleImage,m_mapType["MultipleImage"]);
    m_bgGroup->addButton(ui->radioButtonBing,m_mapType["Bing"]);
    m_bgGroup->addButton(ui->radioButtonUnsplash,m_mapType["Unsplash"]);
    m_bgGroup->addButton(ui->radioButtonWeb,m_mapType["Web"]);
    m_bgGroup->addButton(ui->radioButtonVideo,m_mapType["Video"]);

    connect(m_bgGroup,SIGNAL(buttonToggled(int,bool)),this,SLOT(on_bgGroup_toggled(int,bool)));

    QString strSelectType = CONFIG_JSON->GetSelectType();

    switch (m_mapType[strSelectType]) {
    case MONOCHROME:
    {
        ui->radioButtonMonochrome->setChecked(true);
        break;
    }
    case SINGLEIMAGE:
    {
        ui->radioButtonSingleImage->setChecked(true);
        break;
    }
    case MULTIPLEIMAGE:
    {
        ui->radioButtonMultipleImage->setChecked(true);
        break;
    }
    case BING:
    {
        ui->radioButtonBing->setChecked(true);
        break;
    }
    case UNSPLASH:
    {
        ui->radioButtonUnsplash->setChecked(true);
        break;
    }
    case WEB:
    {
        ui->radioButtonWeb->setChecked(true);
        break;
    }
    case VIDEO:
    {
        ui->radioButtonVideo->setChecked(true);
        break;
    }
    default:
        break;
    }

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_bClose(false)
    , m_imagefile("Yasuo.jpg")
    , m_layoutVLCD(nullptr)
    , m_lcdNumber(nullptr)
    , m_layoutVGIF(nullptr)
    , m_movieGIF(nullptr)
    , m_labelGIF(nullptr)
    , m_layoutVVideo(nullptr)
    , m_mediaPlayerVideo(nullptr)
    , m_videoWidgetVideo(nullptr)
    , m_mediaPlaylistVideo(nullptr)
{
    ui->setupUi(this);

    InitSysTrayIcon();

    m_view = new QWidget();

    m_nViewId = m_view->winId();

    Utils::InitProgman();
    Utils::SetParentWorkerW((HWND)m_nViewId);

    m_view->showFullScreen();

    QDesktopWidget *desktop = QApplication::desktop();

    int height = desktop->height()/5;
    int width = desktop->width()/5;

    qInfo()<<"w:"<<width<<"\th:"<<height;
    this->resize(width,height);

    m_view->move(QPoint(0,0));
    m_view->resize(QSize(width,height));

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(on_handleTimeout()));

    m_pBingNet = new CBingNet();
    connect(m_pBingNet,SIGNAL(ViewImage(QString)),this,SLOT(on_BingSetViewImage(QString)));

    m_pUnsplashNet = new CUnsplashNet();
    connect(m_pUnsplashNet,SIGNAL(ViewImage(QString)),this,SLOT(on_UnsplashSetViewImage(QString)));

    SetViewImage(CONFIG_JSON->GetSingleImage());

    m_pTimerLCD  = new QTimer(this);
    m_pTimerLCD->setInterval(1000);

    connect(m_pTimerLCD, SIGNAL(timeout()), this, SLOT(on_handleTimeLCDout()));

    Init();

    this->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    releseMain();
}

void MainWindow::on_showMainAction()
{
    this->show();
    m_bClose = false;
}

void MainWindow::on_exitAppAction()
{
    m_bClose = true;
    this->close();
}

void MainWindow::releseMain()
{
    delete m_view;
    delete m_Menu;
    delete m_SysTrayIcon;
    delete m_bgGroup;
    delete m_pBingNet;
    delete m_pUnsplashNet;
    delete m_pTimer;

    //delete m_label;
    //delete m_layoutV;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    if(m_bClose)
    {
        event->accept();
        releseMain();
        exit(0);
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
    if(m_bgGroup->checkedId()==MONOCHROME)
    {
        SetLabelColor();
    }
    else
    {
        SetMainBGImage();
    }
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        this->show();
        break;
    case QSystemTrayIcon::DoubleClick:
        this->show();
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButtonSingleImage_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"选择图片",QStandardPaths:: writableLocation(QStandardPaths::PicturesLocation),"Images (*.bmp *.gif *.mng *.jpg *.jpeg *.png)");
    if(path.isEmpty())
    {
        return;
    }
    CONFIG_JSON->SetSingleImage(path);
    ui->lineEditSingleImage->setText(path);
}

void MainWindow::SetMainBGImage()
{
    QPixmap pixmap = QPixmap(m_imagefile).scaled(this->size(),
                                           Qt::IgnoreAspectRatio,
                                           Qt::SmoothTransformation);
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette(palette);
}

void MainWindow::SetGif(bool bchecked,int height, int width)
{
    if(bchecked)
    {
        if(m_layoutVGIF==nullptr)
        {
            m_layoutVGIF = new QVBoxLayout;
        }

        if(m_labelGIF==nullptr)
        {
            m_labelGIF = new QLabel;
        }

        if(m_movieGIF==nullptr)
        {
            m_movieGIF = new QMovie(m_imagefile);
        }

        m_movieGIF->setScaledSize(QSize(width,height));
        m_labelGIF->setMovie(m_movieGIF);

        m_layoutVGIF->addWidget(m_labelGIF);
        m_layoutVGIF->setMargin(0);
        m_view->setLayout(m_layoutVGIF);
        m_movieGIF->start();

    }
    else
    {
        if(m_layoutVGIF!=nullptr)
        {
            delete m_layoutVGIF;
            m_layoutVGIF = nullptr;

            if(m_layoutVLCD!=nullptr)
            {
                m_view->setLayout(m_layoutVLCD);
            }
            else
            {
                m_view->setLayout(nullptr);
            }
        }

        if(m_labelGIF!=nullptr)
        {
            delete m_labelGIF;
            m_labelGIF = nullptr;
        }

        if(m_movieGIF!=nullptr)
        {
            delete m_movieGIF;
            m_movieGIF = nullptr;
        }

    }
}

void MainWindow::SetViewImage(QString path)
{
    if(path.isEmpty())
    {
        return;
    }
    QFileInfo fileInfo(path);
    if(!fileInfo.exists())
    {
        return;
    }

    m_imagefile = path;
    qInfo()<<"使用图片:"<<path;

    QDesktopWidget *desktop = QApplication::desktop();

    int height = desktop->height();
    int width = desktop->width();

    m_view->move(QPoint(0,0));
    m_view->resize(QSize(width,height));

    if(m_imagefile.endsWith(".gif")||m_imagefile.endsWith(".mng"))
    {
        SetGif(false,height, width);
        SetGif(true,height, width);
    }
    else
    {
        //    QImage image;
        //    image.load(QDir::toNativeSeparators(strFile));

        //    QFile::remove("d:\\ViewImage.png");
        //    bool b =image.save("d:\\ViewImage.png","PNG");

        QPalette palette;

        //QPixmap pixmap = QPixmap::fromImage(image);

        QPixmap pixmap(m_imagefile);

        palette.setBrush(this->backgroundRole(),
                                QBrush(pixmap.scaled(width, height,
                                Qt::IgnoreAspectRatio,
                                Qt::SmoothTransformation)));
        //m_view->hide();
        m_view->setPalette(palette);
        //this->setPalette(palette);

        //SendMessage((HWND)m_nViewId,WM_PAINT,NULL,NULL);
        //UpdateWindow((HWND)m_nViewId);
        //RedrawWindow((HWND)m_nViewId);
    }

    m_view->show();

    SetMainBGImage();
}

void MainWindow::on_lineEditSingleImage_textChanged(const QString &arg1)
{
    if(m_bgGroup->checkedId()!=SINGLEIMAGE)
    {
        return;
    }
    CONFIG_JSON->SetSingleImage(arg1);
    SetViewImage(arg1);
}

void MainWindow::BingNetExecute()
{
    QString fileName = QStandardPaths:: writableLocation(QStandardPaths::PicturesLocation)+QDir::separator()+"Bing"+QDir::separator();
    QDate date = QDate::currentDate();
    fileName.append(date.toString("yyyy-MM-dd")).append(".jpg");
    QFileInfo fileInfo(fileName);
    if(fileInfo.exists())
    {
        SetViewImage(fileName);
    }
    else
    {
        m_pBingNet->execute();
    }
}

void MainWindow::on_bgGroup_toggled(int id, bool status)
{
    if(status)
    {
        SetLCD(false);
        SetGif(false,0, 0);
        SetVideo(false, "");

        switch (id) {
        case MONOCHROME:
        {
            SetLabelColor();
            SetViewColor();

            if(m_pTimer->isActive())
            {
                m_pTimer->stop();
            }

            ui->spinBoxMonochrome->setValue(CONFIG_JSON->GetMonochromeTime());

            if(CONFIG_JSON->GetMonochromeTime()>0)
            {
                m_pTimer->start(1000*60*CONFIG_JSON->GetMonochromeTime());
            }
            break;
        }
        case SINGLEIMAGE:
        {
            if(CONFIG_JSON->GetSingleImage().compare(ui->lineEditSingleImage->text())==0)
            {
                SetViewImage(CONFIG_JSON->GetSingleImage());
            }
            else
            {
                ui->lineEditSingleImage->setText(CONFIG_JSON->GetSingleImage());
            }

            break;
        }
        case MULTIPLEIMAGE:
        {
            if(CONFIG_JSON->GetMultipleImage().compare(ui->lineEditMultipleImage->text())==0)
            {
                if(m_filePathList.size()==0)
                {
                    ScanMultipleImage(CONFIG_JSON->GetMultipleImage());
                }

                if(m_filePathList.size()>0)
                {
                    SetViewImage(m_filePathList.at(QRandomGenerator::global()->bounded(m_filePathList.size())));
                }
            }
            else
            {
                ui->lineEditMultipleImage->setText(CONFIG_JSON->GetMultipleImage());
            }

            ui->spinBoxMultipleImage->setValue(CONFIG_JSON->GetMultipleImageTime());

            if(m_pTimer->isActive())
            {
                m_pTimer->stop();
            }
            m_pTimer->start(1000*60*CONFIG_JSON->GetMultipleImageTime());

            break;
        }
        case BING:
        {
            BingNetExecute();
            if(m_pTimer->isActive())
            {
                m_pTimer->stop();
            }
            m_pTimer->start(1000*60*60);
            break;
        }
        case UNSPLASH:
        {
            if(ui->comboBoxUnsplash->count()==0)
            {
                QJsonArray arrUrl = CONFIG_JSON->GetUnsplashUrl();
                for (int i=0;i<arrUrl.count();++i) {
                    ui->comboBoxUnsplash->addItem(arrUrl[i].toString());
                }
                ui->comboBoxUnsplash->setCurrentIndex(CONFIG_JSON->GetUnsplashSelectIndex());
            }
            else
            {
                m_pUnsplashNet->execute(ui->comboBoxUnsplash->currentText());
            }

            if(m_pTimer->isActive())
            {
                m_pTimer->stop();
            }
            m_pTimer->start(1000*60*CONFIG_JSON->GetUnsplashTime());

            break;
        }
        case WEB:
        {

            break;
        }
        case VIDEO:
        {
            if(CONFIG_JSON->GetVideoPath().compare(ui->lineEditVideo->text())==0)
            {
                SetVideo(true, CONFIG_JSON->GetVideoPath());
            }
            else
            {
                ui->lineEditVideo->setText(CONFIG_JSON->GetVideoPath());
            }

            ui->spinBoxVideo->setValue(CONFIG_JSON->GetVideoVolume());
            break;
        }
        default:
            break;
        }

        if(id<TYPEMAX)
        {
            CONFIG_JSON->SetSelectType(m_vectorType[id]);
        }

        if(id<WEB)
        {
            if(ui->checkBoxLCD->isChecked()==CONFIG_JSON->GetOtherLCD())
            {
                SetLCD(CONFIG_JSON->GetOtherLCD());
            }
            else
            {
                ui->checkBoxLCD->setChecked(CONFIG_JSON->GetOtherLCD());
            }
        }
    }
}

void MainWindow::on_pushButtonMultipleImage_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"选择图片路径",QStandardPaths:: writableLocation(QStandardPaths::PicturesLocation));
    if(path.isEmpty())
    {
        return;
    }
    CONFIG_JSON->SetMultipleImage(path);
    ui->lineEditMultipleImage->setText(path);
}

void MainWindow::ScanMultipleImage(const QString &arg1)
{
    QStringList filters;
    filters.append("*.bmp");
    filters.append("*.gif");
    filters.append("*.mng");
    filters.append("*.jpg");
    filters.append("*.jpeg");
    filters.append("*.png");

    QDir fromDir = arg1;
    m_filePathList.clear();
    CFileUtils::ScanData(fromDir,filters,m_filePathList);
}

void MainWindow::on_lineEditMultipleImage_textChanged(const QString &arg1)
{
    if(m_bgGroup->checkedId()!=MULTIPLEIMAGE)
    {
        return;
    }

    if(m_filePathList.size()==0)
    {
        ScanMultipleImage(arg1);
    }

    if(m_filePathList.size()>0)
    {
        SetViewImage(m_filePathList.at(QRandomGenerator::global()->bounded(m_filePathList.size())));
    }
}

void MainWindow::on_handleTimeout()
{
    if(m_bgGroup->checkedId()==MONOCHROME)
    {
        QColor qColor(QRandomGenerator::global()->bounded(255),QRandomGenerator::global()->bounded(255),QRandomGenerator::global()->bounded(255),QRandomGenerator::global()->bounded(255));

        CONFIG_JSON->SetMonochromeColor(qColor);

        SetLabelColor();
        SetViewColor();
    }
    else if(m_bgGroup->checkedId()==MULTIPLEIMAGE)
    {
        if(m_filePathList.size()>0)
        {
            SetViewImage(m_filePathList.at(QRandomGenerator::global()->bounded(m_filePathList.size())));
        }
    }
    else if(m_bgGroup->checkedId()==BING)
    {
        BingNetExecute();
    }
    else if(m_bgGroup->checkedId()==UNSPLASH)
    {
        m_pUnsplashNet->execute(ui->comboBoxUnsplash->currentText());
    }
}

void MainWindow::on_spinBoxMultipleImage_valueChanged(int arg1)
{
    CONFIG_JSON->SetMultipleImage(arg1);

    if(m_bgGroup->checkedId()==MULTIPLEIMAGE)
    {
        if(m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
        m_pTimer->start(1000*60*CONFIG_JSON->GetMultipleImageTime());
    }
}

void MainWindow::on_spinBoxUnsplash_valueChanged(int arg1)
{
    CONFIG_JSON->SetUnsplashTime(arg1);

    if(m_bgGroup->checkedId()==UNSPLASH)
    {
        if(m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
        m_pTimer->start(1000*60*CONFIG_JSON->GetUnsplashTime());
    }
}

void MainWindow::on_BingSetViewImage(QString path)
{
    SetViewImage(path);
}

void MainWindow::on_UnsplashSetViewImage(QString path)
{
    SetViewImage(path);
}

void MainWindow::on_comboBoxUnsplash_currentIndexChanged(int index)
{
    CONFIG_JSON->SetUnsplashSelectIndex(index);
    if(m_bgGroup->checkedId()==UNSPLASH)
    {
        m_pUnsplashNet->execute(ui->comboBoxUnsplash->currentText());

        if(m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
        m_pTimer->start(1000*60*CONFIG_JSON->GetUnsplashTime());
    }
}

void MainWindow::SetLabelColor()
{
    ui->labelMonochrome->clear();
    QPalette palette;
    QColor qColor = CONFIG_JSON->GetMonochromeColor();
    palette.setColor(QPalette::Background, qColor);
    ui->labelMonochrome->setAutoFillBackground(true);
    ui->labelMonochrome->setPalette(palette);

    QPalette palette1;
    palette1.setColor(QPalette::WindowText,QColor(255-qColor.red(),255-qColor.green(),255-qColor.blue()));
    ui->labelMonochrome->setPalette(palette1);

    ui->labelMonochrome->setText(QString("rgba(%1,%2,%3,%4)").arg(qColor.red()).arg(qColor.green()).arg(qColor.blue()).arg(qColor.alpha()));

    this->setPalette(palette);
}

void MainWindow::SetViewColor()
{
    QPalette palette;
    palette.setColor(QPalette::Background, CONFIG_JSON->GetMonochromeColor());

    m_view->setPalette(palette);

}

void MainWindow::on_pushButtonMonochrome_clicked()
{
    QColor color = QColorDialog::getColor(CONFIG_JSON->GetMonochromeColor(), this,"选择单色",QColorDialog::ShowAlphaChannel);
    if(color.isValid())
    {
        CONFIG_JSON->SetMonochromeColor(color);

        SetLabelColor();

        if(m_bgGroup->checkedId()==MONOCHROME)
        {
            SetViewColor();
        }
    }
}

void MainWindow::on_spinBoxMonochrome_valueChanged(int arg1)
{
    int nTime = CONFIG_JSON->GetMonochromeTime();
    CONFIG_JSON->SetMonochromeTime(arg1);

    if(m_bgGroup->checkedId()<WEB)
    {
        if(nTime==0&&arg1>0)
        {
            QColor qColor(QRandomGenerator::global()->bounded(255),QRandomGenerator::global()->bounded(255),QRandomGenerator::global()->bounded(255),QRandomGenerator::global()->bounded(255));

            CONFIG_JSON->SetMonochromeColor(qColor);

            SetLabelColor();
            SetViewColor();
        }

        if(m_pTimer->isActive())
        {
            m_pTimer->stop();
        }
        if(arg1>0)
        {
            m_pTimer->start(1000*60*CONFIG_JSON->GetMonochromeTime());
        }
    }
}

void MainWindow::SetLCD(bool checked)
{
    if(checked)
    {
        if(m_layoutVLCD==nullptr)
        {
            m_layoutVLCD = new QVBoxLayout;
        }

        if(m_lcdNumber==nullptr)
        {
            m_lcdNumber = new QLCDNumber;
        }

        // 设置能显示的位数
        m_lcdNumber->setDigitCount(19);
        // 设置显示的模式为十进制
        m_lcdNumber->setMode(QLCDNumber::Dec);
        // 设置显示外观
        m_lcdNumber->setSegmentStyle(QLCDNumber::Outline);//Outline, Filled, Flat

        m_lcdNumber->setFrameStyle(QFrame::NoFrame | QFrame::Raised);

        m_layoutVLCD->addWidget(m_lcdNumber);
        m_layoutVLCD->setMargin(0);

        if(m_labelGIF!=nullptr)
        {
            m_labelGIF->setLayout(m_layoutVLCD);
        }
        else
        {
            m_view->setLayout(m_layoutVLCD);
        }

        m_pTimerLCD->start();
    }
    else
    {
        if(m_pTimerLCD->isActive())
        {
            m_pTimerLCD->stop();
        }

        if(m_lcdNumber!=nullptr)
        {
            m_layoutVLCD->removeWidget(m_lcdNumber);

            delete m_lcdNumber;
            m_lcdNumber = nullptr;
        }

        if(m_layoutVLCD!=nullptr)
        {
            if(m_labelGIF!=nullptr)
            {
                m_labelGIF->setLayout(nullptr);
            }
            else
            {
                m_view->setLayout(nullptr);
            }

            delete m_layoutVLCD;
            m_layoutVLCD = nullptr;
        }
    }
}

void MainWindow::on_handleTimeLCDout()
{
    if(m_bgGroup->checkedId()<WEB)
    {
        if(m_lcdNumber!=nullptr)
        {
            m_lcdNumber->display(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        }
    }
    else
    {
        if(m_pTimerLCD->isActive())
        {
            m_pTimerLCD->stop();
        }

        SetLCD(false);
    }
}

void MainWindow::on_checkBoxLCD_stateChanged(int arg1)
{
    CONFIG_JSON->SetOtherLCD(arg1 == Qt::Checked);

    if(m_bgGroup->checkedId()<WEB)
    {
        SetLCD(arg1 == Qt::Checked);
    }
}

void MainWindow::on_pushButtonVideo_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"选择视频",QStandardPaths:: writableLocation(QStandardPaths::MoviesLocation),"Video (*.mp4)");
    if(path.isEmpty())
    {
        return;
    }
    CONFIG_JSON->SetVideoPath(path);
    ui->lineEditVideo->setText(path);
}

void MainWindow::SetVideo(bool bDisplay, QString arg1)
{
    if(bDisplay)
    {
        if(m_layoutVVideo==nullptr)
        {
            m_layoutVVideo = new QVBoxLayout;
            m_layoutVVideo->setMargin(0);
        }
        if(m_mediaPlayerVideo==nullptr)
        {
            m_mediaPlayerVideo = new QMediaPlayer;
        }
        if(m_videoWidgetVideo==nullptr)
        {
            m_videoWidgetVideo = new QVideoWidget;
        }
        if(m_mediaPlaylistVideo==nullptr)
        {
            m_mediaPlaylistVideo = new QMediaPlaylist;
        }

        m_videoWidgetVideo->setAspectRatioMode(Qt::IgnoreAspectRatio);
        m_videoWidgetVideo->setFullScreen(true);

        m_layoutVVideo->addWidget(m_videoWidgetVideo);

        m_mediaPlaylistVideo->clear();
        m_mediaPlaylistVideo->addMedia(QUrl::fromLocalFile(arg1));
        m_mediaPlaylistVideo->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        m_mediaPlayerVideo->setPlaylist(m_mediaPlaylistVideo);

        m_mediaPlayerVideo->setVideoOutput(m_videoWidgetVideo);

        m_view->setLayout(m_layoutVVideo);

        m_mediaPlayerVideo->setVolume(CONFIG_JSON->GetVideoVolume());

        m_mediaPlayerVideo->play();
    }
    else
    {
        if(m_layoutVVideo!=nullptr)
        {
            delete m_layoutVVideo;
            m_layoutVVideo = nullptr;

            m_view->setLayout(nullptr);
        }
        if(m_mediaPlayerVideo!=nullptr)
        {
            delete m_mediaPlayerVideo;
            m_mediaPlayerVideo = nullptr;
        }
        if(m_videoWidgetVideo!=nullptr)
        {
            delete m_videoWidgetVideo;
            m_videoWidgetVideo = nullptr;
        }
        if(m_mediaPlaylistVideo!=nullptr)
        {
            delete m_mediaPlaylistVideo;
            m_mediaPlaylistVideo = nullptr;
        }
    }
}

void MainWindow::on_lineEditVideo_textChanged(const QString &arg1)
{
    QFileInfo fileInfo(arg1);
    if(!fileInfo.exists())
    {
        return;
    }

    CONFIG_JSON->SetVideoPath(arg1);

    if(m_bgGroup->checkedId()==VIDEO)
    {
        SetVideo(false, arg1);
        SetVideo(true, arg1);
    }
}

void MainWindow::on_spinBoxVideo_valueChanged(int arg1)
{
    CONFIG_JSON->SetVideoVolume(arg1);

    if(m_bgGroup->checkedId()==VIDEO)
    {
        if(m_mediaPlayerVideo!=nullptr)
        {
            m_mediaPlayerVideo->setVolume(CONFIG_JSON->GetVideoVolume());
        }
    }
}

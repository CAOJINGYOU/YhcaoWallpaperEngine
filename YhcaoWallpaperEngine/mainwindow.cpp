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
    QString path = QFileDialog::getOpenFileName(this,"选择图片",QStandardPaths:: writableLocation(QStandardPaths::PicturesLocation),"Images (*.bmp *.gif *.jpg *.jpeg *.png)");
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

//    QImage image;
//    image.load(QDir::toNativeSeparators(strFile));

//    QFile::remove("d:\\ViewImage.png");
//    bool b =image.save("d:\\ViewImage.png","PNG");

    QPalette palette;

    //QPixmap pixmap = QPixmap::fromImage(image);

    QPixmap pixmap(path);

    palette.setBrush(this->backgroundRole(),
                            QBrush(pixmap.scaled(width, height,
                            Qt::IgnoreAspectRatio,
                            Qt::SmoothTransformation)));
    //m_view->hide();
    m_view->setPalette(palette);
    //this->setPalette(palette);
    m_view->show();
    //SendMessage((HWND)m_nViewId,WM_PAINT,NULL,NULL);
    //UpdateWindow((HWND)m_nViewId);
    //RedrawWindow((HWND)m_nViewId);

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
        switch (id) {
        case MONOCHROME:
        {
            SetLabelColor();
            SetViewColor();

            if(m_pTimer->isActive())
            {
                m_pTimer->stop();
            }

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

            break;
        }
        default:
            break;
        }

        if(id<TYPEMAX)
        {
            CONFIG_JSON->SetSelectType(m_vectorType[id]);
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

    if(m_bgGroup->checkedId()==MONOCHROME)
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

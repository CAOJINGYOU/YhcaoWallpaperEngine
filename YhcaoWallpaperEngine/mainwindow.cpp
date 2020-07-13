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

enum SELECTTYPE
{
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
    m_mapType.insert("SingleImage",nIndex++);
    m_mapType.insert("MultipleImage",nIndex++);
    m_mapType.insert("Bing",nIndex++);
    m_mapType.insert("Unsplash",nIndex++);
    m_mapType.insert("Web",nIndex++);
    m_mapType.insert("Video",nIndex++);

    m_vectorType.append("SingleImage");
    m_vectorType.append("MultipleImage");
    m_vectorType.append("Bing");
    m_vectorType.append("Unsplash");
    m_vectorType.append("Web");
    m_vectorType.append("Video");

    m_bgGroup = new QButtonGroup( this );

    m_bgGroup->addButton(ui->radioButtonSingleImage,m_mapType["SingleImage"]);
    m_bgGroup->addButton(ui->radioButtonMultipleImage,m_mapType["MultipleImage"]);
    m_bgGroup->addButton(ui->radioButtonBing,m_mapType["Bing"]);
    m_bgGroup->addButton(ui->radioButtonUnsplash,m_mapType["Unsplash"]);
    m_bgGroup->addButton(ui->radioButtonWeb,m_mapType["Web"]);
    m_bgGroup->addButton(ui->radioButtonVideo,m_mapType["Video"]);

    connect(m_bgGroup,SIGNAL(buttonToggled(int,bool)),this,SLOT(on_bgGroup_toggled(int,bool)));

    CONFIG_JSON->ReadJson();
    QString strSelectType;
    strSelectType = CONFIG_JSON->ReadString("selecttype");

    switch (m_mapType[strSelectType]) {
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
{
    ui->setupUi(this);


    InitSysTrayIcon();

    m_view = new QWidget();

    m_nViewId = m_view->winId();

    Utils::InitProgman();
    Utils::SetParentWorkerW((HWND)m_nViewId);

    m_view->showFullScreen();

    QDesktopWidget *desktop = QApplication::desktop();

    int height = desktop->height();
    int width = desktop->width();

    m_view->move(QPoint(0,0));
    m_view->resize(QSize(width,height));

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
//todo::某些大图无法显示
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

    //QString strFile = QCoreApplication::applicationDirPath()+QDir::separator()+"ViewImage."+fileInfo.suffix();
//    QString strFile = "d:\\ViewImage."+fileInfo.suffix();
//    QFileInfo fileInfoViewImage(QDir::toNativeSeparators(strFile));
//    if(fileInfoViewImage.exists())
//    {
//        QFile::remove(QDir::toNativeSeparators(strFile));
//    }

    //QFile::copy(QDir::toNativeSeparators(path), QDir::toNativeSeparators(strFile));

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

    QPixmap pixmap1(path);

    palette.setBrush(this->backgroundRole(),
                            QBrush(pixmap1.scaled(width, height,
                            Qt::IgnoreAspectRatio,
                            Qt::SmoothTransformation)));
    //m_view->hide();
    m_view->setPalette(palette);
    //this->setPalette(palette);
    m_view->show();
    //SendMessage((HWND)m_nViewId,WM_PAINT,NULL,NULL);
    //UpdateWindow((HWND)m_nViewId);
    //RedrawWindow((HWND)m_nViewId);
}

void MainWindow::on_lineEditSingleImage_textChanged(const QString &arg1)
{
    if(m_bgGroup->checkedId()!=SINGLEIMAGE)
    {
        return;
    }

    SetViewImage(arg1);
}

void MainWindow::on_bgGroup_toggled(int id, bool status)
{
    if(status)
    {
        switch (id) {
        case SINGLEIMAGE:
        {
            ui->lineEditSingleImage->setText(CONFIG_JSON->GetSingleImage());
            break;
        }
        case MULTIPLEIMAGE:
        {
            QString str = CONFIG_JSON->GetMultipleImage();
            ui->lineEditMultipleImage->setText(str);
            ui->spinBoxMultipleImage->setValue(CONFIG_JSON->GetMultipleImageTime());
            break;
        }
        case BING:
        {

            break;
        }
        case UNSPLASH:
        {

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

void MainWindow::on_lineEditMultipleImage_textChanged(const QString &arg1)
{
    if(m_bgGroup->checkedId()!=MULTIPLEIMAGE)
    {
        return;
    }

    QStringList filters;
    filters.append("*.bmp");
    filters.append("*.gif");
    filters.append("*.jpg");
    filters.append("*.jpeg");
    filters.append("*.png");

    QDir fromDir = arg1;
    m_filePathList.clear();
    CFileUtils::ScanData(fromDir,filters,m_filePathList);

    if(m_filePathList.size()>0)
    {
        SetViewImage(m_filePathList.at(QRandomGenerator::global()->bounded(m_filePathList.size())));
    }
}

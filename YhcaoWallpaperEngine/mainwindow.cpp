#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include "utils.h"
#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <configjson.h>

extern HWND _WORKERWSHELLDLLTEST;

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
    case 0:
    {
        ui->radioButtonSingleImage->setChecked(true);
        //ui->lineEditSingleImage->setText("Yasuo.jpg");
        break;
    }
    case 1:
    {

        break;
    }
    case 2:
    {

        break;
    }
    case 3:
    {

        break;
    }
    case 4:
    {

        break;
    }
    case 5:
    {

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

    m_view = new QWidget /*QWebEngineView*/();
    //view->setAutoFillBackground(true);
//    QImage image;
//    QPalette palette;
//    image.load("E:\\图片\\Aatrox_Splash_0.jpg"); // 指定图片所在位置及图片名
//    palette.setBrush(this->backgroundRole(),QBrush(image));
//    m_view->setPalette(palette);
    //view->setStyleSheet("background-image:url(:/bmp/1257253475842.jpg)");

//    QDesktopWidget *desktop = QApplication::desktop();
//    m_view->move(QPoint(0,0));
//    int height = desktop->height();
//    int width = desktop->width();
//    m_view->resize(QSize(width,height));

//    QPalette bgpal = palette();
//    bgpal.setColor (QPalette::Background, QColor (0, 0 , 0, 255));
//    //bgpal.setColor (QPalette::Background, Qt::transparent);
//    bgpal.setColor (QPalette::Foreground, QColor (255,255,255,255));
//    m_view->setPalette (bgpal);

    m_nViewId = m_view->winId();

    Utils::InitProgman();
    Utils::SetParentWorkerW((HWND)m_nViewId);

    m_view->showFullScreen();

    Init();


    //m_view->show();
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
    QString path = QFileDialog::getOpenFileName(this);
    ui->lineEditSingleImage->setText(path);
}

void MainWindow::on_lineEditSingleImage_textChanged(const QString &arg1)
{
    QString path = ui->lineEditSingleImage->text();
    path = arg1;
    QDesktopWidget *desktop = QApplication::desktop();

    //QRect size = desktop->screenGeometry(0);
    int height = desktop->height();
    int width = desktop->width();

    m_view->move(QPoint(0,0));
    //int height = desktop->height();
    //int width = desktop->width();
    m_view->resize(QSize(width,height));

    QImage image;
    image.load(path);
    QPalette palette;
    QPixmap pixmap = QPixmap::fromImage(image);
    palette.setBrush(this->backgroundRole(),
                            QBrush(pixmap.scaled(width, height,
                            Qt::IgnoreAspectRatio,
                            Qt::SmoothTransformation)));
    m_view->setPalette(palette);
    //this->setPalette(palette);
    m_view->show();
}

void MainWindow::on_bgGroup_toggled(int id, bool status)
{

    if(status)
    {
        QString str = m_vectorType[id];
        QString ss = str;

        switch (id) {
        case 0:
        {
            //ui->radioButtonSingleImage->setChecked(true);
            ui->lineEditSingleImage->setText("Yasuo.jpg");
            break;
        }
        case 1:
        {

            break;
        }
        case 2:
        {

            break;
        }
        case 3:
        {

            break;
        }
        case 4:
        {

            break;
        }
        case 5:
        {

            break;
        }
        default:
            break;
        }
    }
}

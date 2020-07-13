#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QButtonGroup>

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

private slots:
    void on_showMainAction();
    void on_exitAppAction();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

    void on_pushButtonSingleImage_clicked();

    void on_lineEditSingleImage_textChanged(const QString &arg1);


    void on_bgGroup_toggled(int id, bool status);

    void on_pushButtonMultipleImage_clicked();

    void on_lineEditMultipleImage_textChanged(const QString &arg1);

private:
    void closeEvent(QCloseEvent *event);
    // 初始化托盘
    void InitSysTrayIcon();
    void releseMain();
    void Init();

    void SetViewImage(QString path);

};
#endif // MAINWINDOW_H

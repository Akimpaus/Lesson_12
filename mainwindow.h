#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <filedownloader.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bttnFind_clicked();

public slots:
    void on_fd_1();
    void on_fd_2();
    void on_fd_3();

private:

private:
    Ui::MainWindow *ui;
    FileDownloader* m_fd_1;
    FileDownloader* m_fd_2;
    FileDownloader* m_fd_3;
};
#endif // MAINWINDOW_H

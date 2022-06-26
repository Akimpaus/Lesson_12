#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtNetwork>
#include "filedownloader.h"
#include <functional>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_fd_1(new FileDownloader(this)),
    m_fd_2(new FileDownloader(this)),
    m_fd_3(new FileDownloader(this))
{
    ui->setupUi(this);

    connect(m_fd_1, SIGNAL(downloaded()), this, SLOT(on_fd_1()));
    connect(m_fd_2, SIGNAL(downloaded()), this, SLOT(on_fd_2()));
    connect(m_fd_3, SIGNAL(downloaded()), this, SLOT(on_fd_3()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void htmlGet(const QUrl &url, const std::function<void(const QString&)> &func)
{
   QScopedPointer<QNetworkAccessManager> manager(new QNetworkAccessManager);
   QNetworkReply *response = manager->get(QNetworkRequest(QUrl(url)));

   QObject::connect
   (
    response, &QNetworkReply::finished, [response, func]
       {
          response->deleteLater();
          response->manager()->deleteLater();

          if (response->error() != QNetworkReply::NoError)
            return;

          auto const contentType = response->header(QNetworkRequest::ContentTypeHeader).toString();
          static QRegularExpression re("charset=([!-~]+)");
          auto const match = re.match(contentType);

          if (!match.hasMatch() || 0 != match.captured(1).compare("utf-8", Qt::CaseInsensitive))
          {
             qWarning() << "Content charsets other than utf-8 are not implemented yet:" << contentType;
             return;
          }

          auto const html = QString::fromUtf8(response->readAll());

          func(html);
       }
   ) && manager.take();
}

QString substr(const QString& str, const size_t left, const size_t right)
{
    const size_t len = str.length();
    return str.mid(left, len - left - right);
}

void MainWindow::on_bttnFind_clicked()
{
    QString text = ui->lineEdit->text();
    if(text.isEmpty())
        return;
    constexpr size_t number_of_images = 3;
    htmlGet(QUrl("https://yandex.ru/images/search?text=" + text), [this](const QString& body)
        {
            QRegularExpression reg("(http|ftp|https):\\/\\/([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])");
            auto it = reg.globalMatch(body);
            size_t index = 0;
            while(it.hasNext() && index < number_of_images)
            {
                auto match = it.next();
                const QString url = match.captured(0);
                QString ex = url.right(4);
                if(ex == ".jpg" || ex == ".png")
                {
                    qDebug() << url;

                    switch (index)
                    {
                    case 0:
                        m_fd_1->setUrl(url);
                        break;
                    case 1:
                        m_fd_2->setUrl(url);
                        break;
                    case 2:
                        m_fd_3->setUrl(url);
                        break;
                    default:
                        break;

                    }

                    index++;
                }
            }
        }
    );
}

void MainWindow::on_fd_1()
{
    QPixmap pixmap;
    pixmap.loadFromData(m_fd_1->downloadedData());
    ui->label_1->setPixmap(pixmap);
    qDebug() << "1";
}

void MainWindow::on_fd_2()
{
    QPixmap pixmap;
    pixmap.loadFromData(m_fd_2->downloadedData());
    ui->label_2->setPixmap(pixmap);
    qDebug() << "2";
}

void MainWindow::on_fd_3()
{
    QPixmap pixmap;
    pixmap.loadFromData(m_fd_3->downloadedData());
    ui->label_3->setPixmap(pixmap);
    qDebug() << "3";
}

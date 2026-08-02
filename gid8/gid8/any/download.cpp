#include <QtCore>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


#include <cstdio>

#include "download.h"
#include "mystd.h"

static int max_connect = 100;

DownloadManager::DownloadManager()
{
    n_download = 0;

    connect(&manager, &QNetworkAccessManager::finished,
            this, &DownloadManager::downloadFinished);
}

void DownloadManager::doDownload(const QUrl &url, const QString &file_name)
{
    if (files.contains(file_name)) {
        return;
    }

/*

    if (currentDownloads.size() > max_connect) {

        qDebug() << "Засовываю в стек " << file_name;
        
        stackL.push(QPair<QUrl, QString>(url, file_name));
        return;
    }
*/

    qDebug() << "Начинаю скачивать " << file_name;

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
//    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    
//QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
//sslConfig.setProtocol(QSsl::TlsV1_2);  // Или QSsl::TlsV1_2OrLater
//request.setSslConfiguration(sslConfig);    
    
    
    QNetworkReply *reply = manager.get(request);

    files.insert(file_name);


#if QT_CONFIG(ssl)
    connect(reply, &QNetworkReply::sslErrors,
            this, &DownloadManager::sslErrors);
#endif

//    currentDownloads.append(reply);
    currentDownloads[reply] = file_name;
}

/*

QString DownloadManager::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}
*/

void DownloadManager::init()
{
    n_download = 0;
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    mkdir_for_file(filename);

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    n_download ++;

    return true;
}

bool DownloadManager::isHttpRedirect(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

/*

void DownloadManager::execute()
{
    QStringList args = QCoreApplication::instance()->arguments();
    args.takeFirst();           // skip the first argument, which is the program's name
    if (args.isEmpty()) {
        printf("Qt Download example - downloads all URLs in parallel\n"
               "Usage: download url1 [url2... urlN]\n"
               "\n"
               "Downloads the URLs passed in the command-line to the local directory\n"
               "If the target file already exists, a .0, .1, .2, etc. is appended to\n"
               "differentiate.\n");
        QCoreApplication::instance()->quit();
        return;
    }

    for (const QString &arg : qAsConst(args)) {
        QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());
        doDownload(url);
    }
}
*/


void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{
    QString filename = "";
    auto it = currentDownloads.find(reply);
    if (it != currentDownloads.end()) {
        filename = it.value();
        erase(filename);
    }
    
    QUrl url = reply->url();
    if (reply->error()) {
        qDebug() << "Ошибка " << reply->error() << " "<< reply->errorString() << "" << filename;
/*
        qDebug() << reply->error() << reply->errorString();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << reply->rawHeaderList();
        qDebug() << reply->readAll();
*/

        /*
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
                */
    } else {
        if (isHttpRedirect(reply)) {
            fputs("Request was redirected.\n", stderr);
        } 
        else {
            int bs = reply->size();
            if (bs > 0) {
                if (saveToDisk(filename, reply)) {
//                    qDebug() << "Пишу в файл " << filename;
                }
            }
        }
    }

    currentDownloads.remove(reply);
    reply->deleteLater();

/*
    if (currentDownloads.size() < max_connect) {
        if (!stackL.empty()) {
            auto p = stackL.pop();
            qDebug() << "Читаю из стека ";

            doDownload(p.first, p.second);
        }
    }
*/


    if (currentDownloads.isEmpty()) {
        // all downloads finished
//        QCoreApplication::instance()->quit();
    }
}

/*

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    DownloadManager manager;
    QTimer::singleShot(0, &manager, SLOT(execute()));

    app.exec();
}

*/

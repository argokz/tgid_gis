#include <QtCore>
#include <QtNetwork>

#include <cstdio>

#include "download2.h"

void mkdir_for_file(const QString & fn);

bool fun_error_std(const QString & txt)
{
    return true;
}



DownloadManager2::DownloadManager2()
{
    n_download = 0;

    connect(&manager, &QNetworkAccessManager::finished,
            this, &DownloadManager2::downloadFinished);
}

void DownloadManager2::doDownload(const QUrl &url, const fun_download & fun, const fun_error & fun2)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
    QNetworkReply *reply = manager.get(request);

#if QT_CONFIG(ssl)
    connect(reply, &QNetworkReply::sslErrors,
            this, &DownloadManager2::sslErrors);
#endif


//    currentDownloads.append(reply);
    currentDownloads[reply] = fun;

    currentErrors[reply] = *fun2;

}

void DownloadManager2::init()
{
    n_download = 0;
}

bool DownloadManager2::isHttpRedirect(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void DownloadManager2::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

//#include <iostream>

void DownloadManager2::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
//        fprintf(stderr, "Download of %s failed: %s\n",
//                url.toEncoded().constData(),
//                qPrintable(reply->errorString()));
//        QString ss = QString(QObject::tr("Download of %1 failed: %2\n")).arg(url.toEncoded().constData()).arg(qPrintable(reply->errorString()));

        auto it = currentErrors.find(reply);
        if (it != currentErrors.end()) {
            fun_error fun = it.value();
            fun(reply->errorString());
        }

//        std::cout << ss.toStdString();
    } else {
        if (isHttpRedirect(reply)) {
            fputs("Request was redirected.\n", stderr);
        } else {
            auto it = currentDownloads.find(reply);

            if (it != currentDownloads.end()) {
                fun_download fun = it.value();
                QByteArray ar = reply->readAll();
                QString str = QString::fromUtf8(ar.data());

                fun(str);
            }
        }
    }

    currentDownloads.remove(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty()) {
        // all downloads finished
//        QCoreApplication::instance()->quit();
    }
}

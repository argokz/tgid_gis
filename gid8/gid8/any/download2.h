#pragma once

#include <QtCore>
#include <QtNetwork>


QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

typedef bool (*fun_download)(const QString & txt);
typedef bool (*fun_error)(const QString & error);

bool fun_error_std(const QString & txt);



class DownloadManager2: public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;
//    QList<QNetworkReply *> currentDownloads;
    QMap<QNetworkReply *, fun_download > currentDownloads;
    QMap<QNetworkReply *, fun_error > currentErrors;

public:
    DownloadManager2();
    void doDownload(const QUrl &url, const fun_download & fun, const fun_error & fun2 = fun_error_std);
    static QString saveFileName(const QUrl &url);
    static bool isHttpRedirect(QNetworkReply *reply);

    void init();

    int n_download;

public slots:
//    void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
};


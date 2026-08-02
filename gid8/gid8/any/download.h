#pragma once

#include <QtCore>
#include <QtNetwork>


QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

class DownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;
//    QList<QNetworkReply *> currentDownloads;
    QMap<QNetworkReply *, QString> currentDownloads;
    QStack<QPair<QUrl, QString> > stackL;
    QSet<QString> files;

public:
    DownloadManager();
    void doDownload(const QUrl &url, const QString &file_name);
    static QString saveFileName(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    static bool isHttpRedirect(QNetworkReply *reply);

    bool contains(const QString & fn) { return files.contains(fn);};
    void erase(const QString & fn) {
        auto it = files.find(fn);
        if (it != files.end()) {
            files.erase(it);
        }
    };

    void init();

    int n_download;

public slots:
//    void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);

};


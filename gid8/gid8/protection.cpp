#include <QtGui>
#include <QtWidgets>
#include <any/download2.h>

#include "mainwindow.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

static int PROT_LEFT = 0;

DownloadManager2 download_manager1;
DownloadManager2 download_manager2;

const char *keys[] = {
#include "keys.h"
};

#define N80 80


QString rand_text16(int n)
{
    QString txt = "";

    for (int i = 0; i < n; i++) {
        uint c = rand() % 0xFF;
        QString s = QString("%1").arg(c, 2, 16, QLatin1Char('0'));
        txt += s;
    }

    return txt;
}


const int N1 = 5;
const int N2 = 5;

QString key25_encode(unsigned int key0, const QString & key25)
{
    key0 &= 0xFF;

    bool ok;
    QString a1 = rand_text16(N1);
    QString a2 = rand_text16(N2);

    int nkey = sizeof(keys) / sizeof(keys[0]);
    unsigned int key = key0 % nkey;

    QString s;

    int l = key25.length();

    QString key2 = "";

    s = QString("%1").arg((uint)key0, 2, 16, QLatin1Char('0'));


    key2 += s;

    for (int k = 0, i = 0; k < l; k += 2, i++) {
        QString s = key25.mid(k, 2);
        unsigned int c = s.toUInt(&ok, 16);
        c ^= (keys[key][i]+256)%256;
        s = QString("%1").arg((uint)c, 2, 16, QLatin1Char('0'));
        key2 += s;
    }

    key2 = a1 + key2 + a2;

    return key2;
}

QString key25_decode(const QString & key25_0)
{
    QString key25 = key25_0;
    
    key25 = key25.mid(N1 * 2, key25.length() - (N1 + N2) * 2);

    bool ok;
    int nkey = sizeof(keys) / sizeof(keys[0]);

    int l = key25.length();
    QString s = key25.mid(0, 2);
    unsigned int key = s.toUInt(&ok, 16);
    key = key % nkey;

    QString txt = "";

    for (int k = 2, i = 0; k < l; k += 2, i++) {
        QString s = key25.mid(k, 2);
        unsigned int c = s.toUInt(&ok, 16);

        c ^= (keys[key][i] + 256) % 256;
        s = QString("%1").arg((uint)c, 2, 16, QLatin1Char('0'));
        txt += s;
    }
    return txt;
}




#if 0

void encode(int n, unsigned int *R1, unsigned int *L1)
{
    n = n;

    for (int r = 0; r < 10; r++) {
        for (int i = 0; i < n; i++) {
            unsigned int r1 = R1[i]; 
            unsigned int l1 = L1[i]; 

            unsigned int f1 = (l1 + 0x2345) % 0xFFFF;
            
            R1[i] = l1;
            L1[i] = r1 ^ f1;
        }
    }
}

QString encode(const QString & text)
{
    int len = text.length();
    int l = (len+1)/2;

    if (l < 64) l = 64;

    vector<unsigned int> v(l*2, 0);

    for (int i = 0; i < len; i++) {
        v[i] = text.data()[i].unicode();
    }


    encode(l, &v[0], &v[l]);

    QString s = "";

    for (int i = 0; i < l*2; i++) {
        s += QString("%1").arg(v[i], 4, 16);
    }

    return s;
}


QString decode(const QString & text)
{
    int len = text.length()/4;
    int l = (len+1)/2;

    vector<unsigned int> v(len, 0);


    QTextStream ts(&text, QIODeviceBase::Text|QIODeviceBase::ReadOnly);

    int i = 0;

    while (!ts.atEnd()) {
        QString qq = ts.read(4);
        v[i] = qq.toInt(NULL, 16);
        i ++;
    }

    encode(l, &v[l], &v[0]);

    text = "";

    for (int i = 0; i < len; i++) {
        if (v[i] != 0) {
            text += QChar(v[i]);
        }
    }

    return text;
}

#endif

//QString encode(const QString & text, unsigned int key)

vector<unsigned short> encode(const QString & text, unsigned int key)
{
    int nkey = sizeof(keys)/sizeof(keys[0]);
    key = key % nkey;

    int l = text.length();
    
    vector<unsigned short> v(l, 0);

    for (int i = 0; i < l; i++) {
        unsigned short vv = text.data()[i].unicode() ^ keys[key][i%N80];
        v[i] = vv;
    }

    return v;
}

QString decode(vector<unsigned short> &vv, unsigned int key)
{
    int nkey = sizeof(keys)/sizeof(keys[0]);
    key = key%nkey;

    QString text = "";

    int i = 0;

    for (auto v : vv)  {
        if (v != 0) {
            v = v ^  keys[key][i%N80];
            v = v%0xFFFF;
            QChar c(v);
            text += c;
        }
        i ++;
    }
    return text;
}

void exit_app() {
//    QMessageBox::warning(nullptr, "", "Обратитесь к производителю");
    QApplication::quit();
}

bool check_password();

#define HALF_SHIFT 16
#define LOW_16_MASK 0xFFFF

unsigned int swap_int(unsigned int num)
{
    return (num >> HALF_SHIFT) ^ ((num & LOW_16_MASK) << HALF_SHIFT);
}

unsigned int F1(unsigned int num, unsigned int key)
{
    num ^= key;
    num *= num;
    return swap_int(num);
}

unsigned int F2(unsigned int num, unsigned int key)
{
    num = swap_int(num);
    num = int(sqrt(num)+0.1);
    num ^= key;
    return num;
}


bool check_protection_add(const QString & txt)
{
    QMessageBox::warning(nullptr, "", "Добавил");
    
    return false;
}

void save_settings(const QString & city, unsigned int left)
{
    QSettings settings;

    unsigned int key = rand() & 0xFFFF;
    unsigned int left2 = F1(left, key);

    settings.setValue("prot_n1", left2);
    settings.setValue("prot_n2", key);

    vector<unsigned short> v = encode(city, key);

    QByteArray ba((const char *)v.data(), v.size()*sizeof(short));
    settings.setValue("prot_city", ba.toHex());
}

#include <config.h>

void ask_protection(bool force, QString &city, bool & yes, bool & add)
{
    QSettings settings;

    yes = false;
    add = false;

    unsigned int left = settings.value("prot_n1", 0).toUInt();
    unsigned int key = settings.value("prot_n2", 0).toUInt();

//    city = settings.value("prot_city", "").toString();
    QByteArray ba1 = settings.value("prot_city").toByteArray();
    QByteArray ba =  QByteArray::fromHex(ba1);
    unsigned short *d1 = (unsigned short *)ba.begin();
    unsigned short *d2 = (unsigned short *)ba.end();
    vector<unsigned short> v(d1, d2);
    city =  decode(v, key);

//    QMessageBox::warning(nullptr, QString("%1").arg(key), city);

    if (key == 0) {
        key = rand() & 0xFFFF;
    }

    left = F2(left, key);

    if (city == "" || left == 0 || left > PROT_LEFT || force) {
        bool ok;

        city = QInputDialog::getText(nullptr, QObject::tr("Кто вы?"), QObject::tr("Кто вы?"), QLineEdit::Normal, "", &ok);
        if (!ok) {
            exit_app();
            return;
        }

        if (!check_password()) {
            exit_app();
            return;
        }

        QString _url = QString("%1?q=%2").arg(get_ini()->protection_url, city);

        QUrl url = QUrl::fromPercentEncoding(QUrl::toPercentEncoding(_url));
        download_manager2.doDownload(url, check_protection_add);

        left = PROT_LEFT;
        add = true;
    }

    left -= 1;
    save_settings(city, left);

    yes = true;

}

static QString city = "";


bool check_protection_reply(const QString & txt)
{
    QJsonParseError error;

    QJsonDocument doc = QJsonDocument::fromJson(txt.toUtf8(), &error);
    QJsonObject json = doc.object();

    QJsonValue v = json["isValid"];
    bool ok = v.toBool();

    if (ok) {
        save_settings(city, PROT_LEFT);
    }
    else {
        QString city;
        bool add, yes;
        ask_protection(true, city, yes, add);
    }           

    return true;
}

QString check_protection_reply3(const QString & txt)
{
    QJsonParseError error;

    QJsonDocument doc = QJsonDocument::fromJson(txt.toUtf8(), &error);
    QJsonObject json = doc.object();

    QJsonValue v = json["isValid"];

    if (!v.isBool()) return "";

    bool ret = v.toBool();
    if (ret) return "true";
    return "false";

}




/*

bool MainWindow::check_protection()
{
    return true;

//    QString city;
    bool add, yes;

    ask_protection(false, city, yes, add);

    if (yes) {
        QString _url = QString("https://lan.avto-glass.kz/check.php?q=%1").arg(city);

        QUrl url = QUrl::fromPercentEncoding(QUrl::toPercentEncoding(_url));
        download_manager1.doDownload(url, check_protection_reply);
    }
    if (add) {
        QString _url = QString("https://lan.avto-glass.kz/check.php?q=%1").arg(city);

        QUrl url = QUrl::fromPercentEncoding(QUrl::toPercentEncoding(_url));

        download_manager2.doDownload(url, check_protection_add);
    }

    return true;
}

*/

void write_key(const QString & key25, unsigned int left)
{
    unsigned int key = rand() & 0xFFFF;
    left = F1(left, key);

    QSettings settings;

    settings.setValue("prot_city", key25_encode(key+1, key25_encode(key, key25)));
    settings.setValue("prot_n1", left);
    settings.setValue("prot_n2", key);
}


QString read_key25()
{
    QSettings settings;

    QString key25 = settings.value("prot_city").toString();

    unsigned int left = settings.value("prot_n1", 0).toUInt();
    unsigned int key = settings.value("prot_n2", 0).toUInt();

    key25 = key25_decode(key25);
    key25 = key25_decode(key25);

    left = F2(left, key);

    if (left == 0 || left > PROT_LEFT) {
        return "";
    }

    write_key(key25, left - 1);

    return key25;
}

QString ask_key25(const QString & message = QObject::tr("Введите ключ активации"))
{
    QString key25 = "";
    bool ok;

    key25 = QInputDialog::getText(nullptr, "", message, QLineEdit::Normal, "", &ok);

    if (!ok) {
        return "";
    }

    return key25;
}

extern MainWindow *main_window;

/*
QString readHtml(const QString & url)
{
    QNetworkAccessManager manager;

    QNetworkRequest netRequest((QUrl(url)));
    QNetworkReply *netReply = manager.get(netRequest);

    QEventLoop loop;
    main_window->connect(netReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString out = netReply->readAll();


    return out;
}

*/

QString readHtml(const QString &url)
{
    QNetworkAccessManager manager;
    QNetworkRequest netRequest((QUrl(url)));
    QNetworkReply *netReply = manager.get(netRequest);

    QEventLoop loop;
    QObject::connect(netReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString out;
    if (netReply->error() == QNetworkReply::NoError) {
        out = QString::fromUtf8(netReply->readAll());
    } else {
        qWarning() << "Network error:" << netReply->errorString();
    }

    netReply->deleteLater();
    return out;
}


QString protocolToString(QSsl::SslProtocol p)
{
    switch (p) {
    case QSsl::TlsV1_2:            return "TLS 1.2";
    case QSsl::TlsV1_3:            return "TLS 1.3";
    case QSsl::TlsV1_2OrLater:     return "TLS 1.2 or later";
    case QSsl::AnyProtocol:        return "Any (not recommended)";
    case QSsl::SecureProtocols:    return "SecureProtocols (TLS 1.2+)";
    default:                       return "Unknown";
    }
}

void printSslInfo()
{
    qDebug() << "Supports SSL:" << QSslSocket::supportsSsl();
    qDebug() << "Build SSL:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "Runtime SSL:" << QSslSocket::sslLibraryVersionString();

    auto protocols = QSslSocket::supportedProtocols();
    for (auto p : protocols) {
        qDebug() << "Protocol:" << protocolToString(p);
    }
}

QString ask_server(const QString & key25)
{
    unsigned int key = rand() & 0xFFFF;

    QString key2 = key25_encode(key, key25);

    QString url = QString("%1?q=%2").arg(get_ini()->protection_url, key2);

//    qDebug() << url;

    QString s = readHtml(url);

//    qDebug() << s;

//    s = "dgedrfgyhrteyrthy";

    s = check_protection_reply3(s);

//    qDebug() << s;

qDebug() << QSslSocket::supportsSsl();
qDebug() << QSslSocket::sslLibraryBuildVersionString();
qDebug() << QSslSocket::sslLibraryVersionString();
printSslInfo();

    //    AfxMessageBox(s);
    return s;
}


#include <config.h>

bool MainWindow::check_protection()
{
#if !USE_ACTIVATION
    qInfo() << "Activation check is disabled for this build";
    return true;
#else
    PROT_LEFT = get_ini()->PROT_LEFT;

    QString key25 = read_key25();
    QString out = "";

    bool new_key = false;

    if (key25 == "") {
        key25 = ask_key25();
        new_key = true;
        if (key25 == "") {
            exit_app();
            return false;
        }
    }

    while (true) {
        out = ask_server(key25);
        if (out == "true") break;
//        QMessageBox::warning(this, "", tr("Неверный ключ активации!\nОбратитесь к производителю"));
        key25 = ask_key25(tr("Неверный ключ активации! Поопробуйте еще раз"));
        if (key25 == "") {
            exit_app();
            return false;
        }
    }

    if (out == "true") {
        write_key(key25, PROT_LEFT);
    }
    return true;




#if 0
    //    QString city;
    bool add, yes;

    ask_protection(false, city, yes, add);



    if (yes) {
        QString _url = QString("https://lan.avto-glass.kz/check.php?q=%1").arg(city);

        QUrl url = QUrl::fromPercentEncoding(QUrl::toPercentEncoding(_url));
        download_manager1.doDownload(url, check_protection_reply);
    }
    if (add) {
        QString _url = QString("https://lan.avto-glass.kz/check.php?q=%1").arg(city);

        QUrl url = QUrl::fromPercentEncoding(QUrl::toPercentEncoding(_url));

        download_manager2.doDownload(url, check_protection_add);
    }

#endif
    return true;
#endif
}

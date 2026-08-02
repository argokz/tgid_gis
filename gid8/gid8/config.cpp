#include <QCoreApplication>
#include <QCryptographicHash>
#include <QtGui>
#include <QtWidgets>

//#include <ui2/OpenServer.h>

#include <config.h>

static InitConfig ini;
QString argpath();


QByteArray simpleEncryptDecrypt(const QByteArray &data, const QByteArray &key);

void encrypt_file(const QString & filename, const QString & filename2, const QByteArray &key)
{
    // Считываем файл и шифруем
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QByteArray encryptedData = simpleEncryptDecrypt(data, key);

        QFile file2(filename2);
        // Записываем зашифрованный файл
        if (file2.open(QIODevice::WriteOnly)) {
            file2.write(encryptedData);
            file2.close();
        }
    }
}


InitConfig * get_ini()
{
    return &ini;
}

void saveConnectStr(ConnectStr &cs);


bool init_config()
{
    QByteArray key = QCryptographicHash::hash("my_secret_key", QCryptographicHash::Sha256);

    QString fn = QString("%1kls/config.ini").arg(argpath());

    // Без локального config.ini приложение должно оставаться рабочим:
    // показываем прежний диалог подключения с безопасными значениями.
    ini.cs.rdbms = 1;
    ini.cs.host = qEnvironmentVariable("TGID_HOST", "localhost");
    ini.cs.port = qEnvironmentVariableIntValue("TGID_PORT");
    if (ini.cs.port <= 0) ini.cs.port = 5432;
    ini.cs.user = qEnvironmentVariable("TGID_USER", "postgres");
    ini.cs.password = qEnvironmentVariable("PGPASSWORD", "");
    ini.cs.baza = qEnvironmentVariable("TGID_DB", "tgid_gis");
    ini.cs.geo = ini.cs.baza;
    ini.ok = true;
    ini.enable_rdbms = true;
    ini.enable_host = true;
    ini.enable_port = true;
    ini.enable_user = true;
    ini.enable_password = true;
    ini.enable_baza = true;
    ini.enable_geo = true;

    bool enable = true;

    if (QFile::exists(fn)) {
        QString fn2 = fn;

        fn2 = QString("%1/tgid.tmp").arg(QDir::tempPath());
        encrypt_file(fn, fn2, key);

        QSettings settings(fn2, QSettings::IniFormat);
        ini.cs.rdbms =    settings.value("cs/rdbms"    , 0).toInt();
        ini.cs.host =     settings.value("cs/host"    , "").toString();
        ini.cs.port =     settings.value("cs/port"    , 0).toInt();
        ini.cs.user =     settings.value("cs/user"    , "").toString();
        ini.cs.password = settings.value("cs/password", "").toString();
        ini.cs.baza =     settings.value("cs/baza"    , "").toString();
        ini.cs.geo =      settings.value("cs/geo"     , "").toString();
        ini.cs.gid9 =     settings.value("cs/gid9"    , "").toString();
        enable =          settings.value("cs/enable"  , false).toBool();

        ini.ok =          settings.value("cs/ok",       false).toBool();

        ini.enable_rdbms = settings.value("cs/enable_rdbms",    false).toBool();
        ini.enable_host = settings.value("cs/enable_host",    false).toBool();
        ini.enable_port = settings.value("cs/enable_port",    false).toBool();
        ini.enable_user = settings.value("cs/enable_user",    false).toBool();
        ini.enable_password = settings.value("cs/enable_password",    false).toBool();
        ini.enable_baza = settings.value("cs/enable_baza",    false).toBool();
        ini.enable_geo = settings.value("cs/enable_geo",    false).toBool();
        ini.enable_newbaza = settings.value("cs/enable_newbaza",    false).toBool();
        ini.enable_gid9 = settings.value("cs/enable_gid9",    false).toBool();
        ini.enable_alseco = settings.value("cs/enable_alseco",    false).toBool();
        ini.enable_tu = settings.value("cs/enable_tu",    false).toBool();

        ini.PROT_LEFT = settings.value("cs/PROT_LEFT",    5).toInt();

        ini.protection = settings.value("cs/protection",    false).toBool();
        ini.protection_url = settings.value("cs/protection_url", "").toString();
        ini.elevation_url = settings.value("cs/elevation_url", "").toString();

        QFile::remove(fn2);

        {
            QSettings settings;

            if (!ini.enable_rdbms) settings.setValue("cs/rdbms", ini.cs.rdbms);

            if (!ini.enable_host) settings.setValue("cs/host", ini.cs.host);
            if (!ini.enable_port) settings.setValue("cs/port", ini.cs.port);
            if (!ini.enable_user) settings.setValue("cs/user", ini.cs.user);
            if (!ini.enable_password) settings.setValue("cs/password", ini.cs.password);
            if (!ini.enable_baza) settings.setValue("cs/baza", ini.cs.baza);
            if (!ini.enable_geo) settings.setValue("cs/geo", ini.cs.geo);
            if (!ini.enable_baza) settings.setValue("cs/gid9", ini.cs.gid9);
        }
    }
    return ini.ok;
}


#include "qregularexpression.h"
#include <QCoreApplication>
#include <QLocale>
#include <QStandardPaths>
#include <QDir>

#include <mystd.h>

#ifdef __APPLE__
static QString _argpath = "/Users/dmitriy/QtProject/ReleaseGid8/";
#else
static QString _argpath = "C:/vc_projects/gid6/gidr/==";
#endif

void mkdir_for_file(const QString & fn)
{
    int n = fn.lastIndexOf("\\");
    if (n == -1) {
        n = fn.lastIndexOf("/");
    }

    if (n >= 0) {
        QString dir = fn.left(n+1);
        if (!QFile::exists(dir)) {
            QDir().mkpath(dir);
        }
    }
}

void set_argpath(const QString & path)
{
    _argpath = path;
    if (_argpath.last(1) != "/") _argpath += "/";
}

QString argpath() 
{
    return _argpath;
}

QString argpath_2()
{
//    QString localAppDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
//    return localAppDataPath;

    QString path1 = QDir::homePath() + "/Application Data/Sirius/tgid/";
    QString path2 = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return path2;
}

QString argpath_local()
{
//    return QDir::homePath() + "/Application Data/Sirius/tgid/";
    QString localDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/";
    return localDataLocation;
}


QString argpath_0()
{
    QString dir = QCoreApplication::applicationDirPath();
    return dir;
}


QString GetTitle(const QString & path) 
{
    int i;

    QString p = path;

    i = p.lastIndexOf(QRegularExpression("[\\/]"));

    if (i != -1) {
        p = p.left(i);
    }
  
    i = p.indexOf('.');
    if (i != -1) {
        p = p.left(i);
    }

    return p;
}


QString readFileUTF8(const QString & fn, FilePlace fp)
{
    QString fnp = argpath() + QString(fn);

    if (fp == fp_users) {
        fnp = argpath_2() + QString(fn);
    }

    if (fp == fp_absolute) {
        fnp = fn;
    }

    QFile file(fnp);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QString q = file.readAll();

    return q;
}

QString readQSS(const QString & fn, FilePlace fp)
{
    QString fnp = argpath() + QString(fn);

    if (fp == fp_users) {
        fnp = argpath_2() + QString(fn);
    }

    QFile file(fnp);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QString style = file.readAll();

    QFileInfo fileInfo(fnp);
    QString path = fileInfo.absolutePath(); 


    style.replace("url(:/", QString("url(%1/").arg(path));
    style.replace("url(/", QString("url(%1/").arg(path));

    return style;
}


QString readFile(const QString & fn, FilePlace fp)
{
    QString fnp = argpath() + QString(fn);

    if (fp == fp_users) {
        fnp = argpath_2() + QString(fn);
    }

    if (fp == fp_absolute) {
        fnp = fn;
    }
    
    QFile file(fnp);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

//    QTextStream in(&file);
//    in.setEncoding(QStringConverter::System);

    QString cp2utf(const char * s);


//    QString q = file.readAll();
    QString q = cp2utf(file.readAll().data());

    return q;
}

bool is_POSTGRESQL();
bool is_SQLITE();

QString nameQ(const QString & fn)
{
    QString fnp = argpath() + fn;

    if (is_POSTGRESQL()) {
        if (QFile::exists(argpath() + "postgresql/" + fn)) {
            fnp = argpath() + "postgresql/" + fn;
        }
    }
    if (is_SQLITE()) {
        if (QFile::exists(argpath() + "postgresql/" + fn)) {
            fnp = argpath() + "postgresql/" + fn;
        }
    }



    return fnp;
}


QString readQ(const QString & fn)
{
    QString fnp = nameQ(fn);
    
    qDebug() << "read file " << fnp;

    QFile file(fnp);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QString cp2utf(const char * s);
    QString q = cp2utf(file.readAll().data());

    return q;
}


QString readQUTF8(const QString & fn)
{
    QString fnp = nameQ(fn);
    QFile file(fnp);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return ""; // Файл не открыт, возвращаем пустую строку
    }

    QString q = QString::fromUtf8(file.readAll()); // Корректно обрабатываем UTF-8 данные
    return q;
}

QString rstrip(const QString& str);
QString cp2utf(const char * s);


QString readLineCode(QFile &file, const QString & codepage)
{
    if (codepage == "cp1251") {
        return rstrip(cp2utf(file.readLine().data()));
    }
    else {
        return rstrip(file.readLine());
    }
}

static QString m_baza_gid9 = "demo9";

static bool m_is_gid9 = true;

bool is_gid9()
{
    return m_is_gid9;
}

void set_gid9(bool yes)
{
    m_is_gid9 = yes;
}




QString get_baza_gid9()
{
    QSettings settings;
    m_baza_gid9 = settings.value("cs/gid9", m_baza_gid9).toString();

    return m_baza_gid9;
}

void set_baza_gid9(const QString & gid9)
{
    QSettings settings;
    settings.setValue("cs/gid9", gid9);

    m_baza_gid9 = gid9;
}


bool isFragmentList(const QString & par)
{
    static const QRegularExpression re(
        QStringLiteral("^\\s*-?\\d+(\\s*,\\s*-?\\d+)*\\s*$"));

    return re.match(par).hasMatch();
}
    

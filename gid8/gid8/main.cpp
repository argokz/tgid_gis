#include "mainwindow.h"

#include <QApplication>
#include <QElapsedTimer>
#include <QLocale>
#include <QTranslator>
#include <QFileDialog>
#include <gidview/colorgtd.h>
#include <config.h>
#include <db/db.h>

bool init_config();

bool init_path(QWidget *widget)
{
    QSettings settings;
    QString f = "/sql3/us.sql";
    QString dir = QCoreApplication::applicationDirPath();

    if (!QFile::exists(dir+f)) {
        dir = settings.value("path", dir).toString();

        while (!QFile::exists(dir+f)) {
            dir = QFileDialog::getExistingDirectory(0, QObject::tr("Выберите каталог"), dir, QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
            if (dir == "") {
                QMessageBox::warning(nullptr, "", "Программа не может работать. Обратитесь за разъяснениями к разработчику");
                return false;
            }
        }
        settings.setValue("path", dir);
    }
    set_argpath(dir);

    return true;
}

bool hasp();

#if USE_ITWIN
//#include "version.h"
#else
//#include "version_tgid.h"
#endif

#include "version0.h"


const QString log_file_name = "tgid_qt.log";

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg1)
{
    QString msg = msg1;

    // Путь, создание каталога и поток — ОДИН раз на весь запуск.
    //
    // Раньше на каждое сообщение заново вызывались argpath_2()
    // (QStandardPaths), mkdir_for_file() и создавался QTextStream, а
    // Qt::endl сбрасывал буфер на диск. За один запуск программа пишет
    // около 4000 строк — это столько же системных вызовов mkdir и
    // столько же сбросов, причём профиль лежит на системном диске.
    // Сам вывод при этом почти весь служебный: подписи действий вида
    // "|Текст| |Текст|", по три строки на команду меню.
    static QFile *logFile = nullptr;
    static QTextStream *out_ptr = nullptr;
    if (!logFile) {
        const QString fn = QString("%1/%2").arg(argpath_2(), log_file_name);
        mkdir_for_file(fn);
        logFile = new QFile(fn);
        logFile->open(QIODevice::WriteOnly | QIODevice::Append);
        out_ptr = new QTextStream(logFile);
    }
    if (!logFile->isOpen()) return;

    QTextStream &out = *out_ptr;

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    if (msg.contains("\\n")) {
        msg.replace("\\n", "\n");
        msg = msg.mid(1, msg.length()-2);
        msg = "\n---------------------------------------\n" + msg + "\n---------------------------------------";
    }

    msg.replace("\\\"", "\"");

//    msg = qFormatLogMessage(type, context, msg1);

    switch (type) {
    case QtDebugMsg:
        out << timestamp << " DEBUG: " << msg << '\n';
        break;
    case QtInfoMsg:
        out << timestamp << " INFO: " << msg << '\n';
        break;
    case QtWarningMsg:
        out << timestamp << " WARNING: " << msg << '\n';
        break;
    case QtCriticalMsg:
        out << timestamp << " CRITICAL: " << msg << '\n';
        break;
    case QtFatalMsg:
        out << timestamp << " FATAL: " << msg << '\n';
        out.flush();
        abort();
    }

    // Сброс на диск — только для того, что нельзя потерять при падении.
    // Отладочные и информационные строки уходят пачкой, когда буфер
    // заполнится или при закрытии файла.
    if (type == QtWarningMsg || type == QtCriticalMsg) {
        out.flush();
    }
}


void init_pics();

void open_b4();
void open_b5();
void initTableRusName(const QString & database);
void initColumnRusName(const QString & database);
void initLookup(const QString & database);
void initLookup2(const QString & database);
void initRenamedTables();
void initRenamedColumns();
void initInclude();

void initLinePoint();

void init_onlygeo();
void init_line_node();
void init_onlyline_list_file();

void set_system_coord(double _False_Easting, double _False_Northing, double _Central_Meridian, double _Latitude_Of_Origin, double _Scale_Factor, double _Angle, double _dx, double _dy);

bool init_gis_convert(int epsg, const std::string & proj4);
QString getFileDate();

int runDatabaseSmokeTest()
{
    const ConnectStr cs = get_ini()->cs;
    QSqlDatabase db;
    if (!connectSQL0(cs.rdbms, cs.host, cs.port, cs.baza,
                     cs.user, cs.password, db)) {
        qCritical() << "DB_SMOKE: connection failed" << db.lastError().text();
        return 2;
    }

    QSqlQuery query(db);
    if (!query.exec(
            "SELECT current_setting('search_path'), "
            "(SELECT count(*) FROM net.v_nodes), "
            "(SELECT count(*) FROM fragments)")) {
        qCritical() << "DB_SMOKE: query failed" << query.lastError().text();
        return 3;
    }
    if (!query.next()) {
        qCritical() << "DB_SMOKE: query returned no row";
        return 4;
    }

    qInfo() << "DB_SMOKE: OK"
            << "search_path=" << query.value(0).toString()
            << "nodes=" << query.value(1).toLongLong()
            << "fragments=" << query.value(2).toLongLong();
    return 0;
}


int main(int argc, char *argv[])
{

//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


//    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    srand( (unsigned)time( NULL ) );

    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

    set_system_coord(0, 0, 0, 0, 1, 0, 0, 0);

    void init_tok_table();
    init_tok_table();

    m_colors.load(0);

    QApplication a(argc, argv);
    QApplication::instance()->setAttribute(Qt::AA_DontShowIconsInMenus, true);

/*
    QSettings settings(QString("Sirius"), QString("Gidr 8.0"), nullptr);
    if (!init_path(nullptr)) return 0;
*/

#if 0
    QString fn = QString("%1/%2").arg(argpath_2(), log_file_name);

    QFile file(fn);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&file);
        out << "---------------------------------------------------\n";
        file.close();
    }

    qDebug() << "Начали!";

    qInstallMessageHandler(customMessageHandler);

#endif


    QLocale curLocale(QLocale("ru_RU"));
    QLocale::setDefault(curLocale);

    QCoreApplication::setOrganizationDomain(VER_COMPANY_DOMAIN);
    QCoreApplication::setOrganizationName(VER_COMPANY_STR);

    QCoreApplication::setApplicationName(VER_FILEDESCRIPTION_STR);
//    QCoreApplication::setApplicationName(ITWIN_TEPLO);
    qDebug() << VER_FILEDESCRIPTION_STR;
    


    QString date = getFileDate();

//    QCoreApplication::setApplicationVersion(QString("%1.%2").arg(VER_PRODUCT_VERSION_STR, date));
    QCoreApplication::setApplicationVersion(VER_PRODUCT_VERSION_STR);

    QGuiApplication::setWindowIcon(QIcon(QString(":/images/%1").arg(GIDR_ICO)));

    if (!init_path(nullptr)) return 0;

#if 1

    QString fn = QString("%1/%2").arg(argpath_2(), log_file_name);

//    QMessageBox::warning(nullptr, "", fn);

    QFile file(fn);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&file);
        out << "---------------------------------------------------\n";
        file.close();
    }

    qDebug() << "Начали!";

#if USE_HASP
    qDebug() << "USE_HASP";
#endif

#if USE_ITWIN
    qDebug() << "USE_ITWIN";
#endif

#if USE_GID9
    qDebug() <<"USE_GID9";
#endif

//    qDebug() << "USE_ITWIN " << USE_ITWIN;
//    qDebug() << "USE_GID9 " << USE_GID9;
//    qDebug() << "USE_HASP " << USE_HASP;


    qInstallMessageHandler(customMessageHandler);

#endif

    init_config();
    if (a.arguments().contains("--db-smoke")) {
        return runDatabaseSmokeTest();
    }

    // Замер каждого шага запуска.
    //
    // По журналу между проверкой активации и подключением к БД уходило
    // 12 секунд — до единого запроса, то есть это чистая загрузка
    // справочников с диска. Какой именно шаг столько стоит, из журнала
    // было не видно: все они молчали. Без этого замера «ускорение
    // запуска» свелось бы к угадыванию.
    QElapsedTimer _t_all, _t_step;
    _t_all.start();
    _t_step.start();
#define TGID_INIT_STEP(call)                                            \
    do {                                                                \
        _t_step.restart();                                              \
        call;                                                           \
        const qint64 _ms = _t_step.elapsed();                           \
        if (_ms >= 50)                                                  \
            qInfo() << "запуск:" << #call << _ms << "мс";               \
    } while (0)

    TGID_INIT_STEP(init_pics());
    TGID_INIT_STEP(open_b4());
    TGID_INIT_STEP(open_b5());

    TGID_INIT_STEP(initInclude());

    TGID_INIT_STEP(initRenamedTables());
    TGID_INIT_STEP(initRenamedColumns());
    TGID_INIT_STEP(initLinePoint());

    TGID_INIT_STEP(initColumnRusName("sprav"));

    TGID_INIT_STEP(initTableRusName("gid"));
    TGID_INIT_STEP(initColumnRusName("gid"));
    TGID_INIT_STEP(initLookup("gid"));
    TGID_INIT_STEP(initLookup("gid8"));
    TGID_INIT_STEP(initLookup2("gid"));

    TGID_INIT_STEP(initTableRusName("OpenStreetMap"));
    TGID_INIT_STEP(initColumnRusName("OpenStreetMap"));
    TGID_INIT_STEP(initLookup("OpenStreetMap"));
    TGID_INIT_STEP(initLookup2("OpenStreetMap"));

    TGID_INIT_STEP(init_onlygeo());
    TGID_INIT_STEP(init_line_node());

    TGID_INIT_STEP(init_onlyline_list_file());

    qInfo() << "запуск: справочники всего" << _t_all.elapsed() << "мс";
#undef TGID_INIT_STEP


//    std::string pr_almaty = "+proj=tmerc +lat_0=0 +lon_0=76.9166666666667 +k=1 +x_0=-3021 +y_0=-4791536 +ellps=krass +towgs84=24,-121,-76,0,0,0,0 +units=m +no_defs +type=crs";

//+proj=tmerc +ellps=bessel +towgs84=316.151,78.924,589.65,-1.57273,2.69209,2.34693,8.4507 +units=m +lon_0=37.5 +lat_0=55.66666666667 +k_0=1 +x_0=0 +y_0=0    

//    std::string pr_moscow = "+proj=tmerc +lat_0=55.666666667 +lon_0=37.5 +k=1 +x_0=109 +y_0=23 +a=6377397 +rf=299.15 +units=m +no_defs";
    std::string pr_moscow = "+proj=tmerc +lat_0=55.666666667 +lon_0=37.5 +k=1 +x_0=-109 +y_0=-23 +a=6377397 +rf=299.15 +units=m +no_defs";
//    std::string pr_moscow = "+proj=tmerc +ellps=bessel +towgs84=316.151,78.924,589.65,-1.57273,2.69209,2.34693,8.4507 +units=m +lon_0=37.5 +lat_0=55.66666666667 +k_0=1 +x_0=0 +y_0=0";

//    4326  - wgs84 degree
//    3857  - google
//    3395  - yandex
    init_gis_convert(4326, pr_moscow);


    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

//    if translator.load(QLocale("ru"), "qtbase", "_", QApplication.libraryPaths()[0] + "/translations"):
//        app.installTranslator(translator)

    for (const QString &locale : uiLanguages) {
        if (translator.load("qt_" + locale, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
             a.installTranslator(&translator);
             break;
        }
    }

    /*
    for (const QString &locale : uiLanguages) {
        const QString baseName = "gid8_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    */

#if USE_HASP
    if (!hasp()) {
        return 0;
    }
#endif

//    QSettings settings(QString("Sirius"), QString("Gidr 8.0"), nullptr);
//    if (!init_path(nullptr)) return 0;

    MainWindow w;
    w.showMaximized();
    return a.exec();
}

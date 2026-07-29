// Проверка связки «Qt + драйвер QPSQL + новые запросы» без графического
// интерфейса.
//
// Повторяет то, что делает приложение при открытии схемы:
//   1. читает sql3/us.sql и sql3/ut.sql в CP1251, как это делает
//      readQ -> cp2utf (gid8/mystd.cpp:189);
//   2. выбирает вариант postgresql/<файл>, если он есть, — так же,
//      как nameQ (gid8/mystd.cpp);
//   3. дописывает фильтр фрагмента конкатенацией, как read_nodes.cpp:243
//      и read_lines.cpp:212;
//   4. выполняет запрос через QSqlQuery и читает поля по именам.
//
// Смысл проверки: psycopg2 уже показал, что запросы верны. Здесь
// проверяется именно то, чего он проверить не мог, — что их так же
// прочитает и выполнит сам Qt.

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QStringDecoder>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>
#include <QElapsedTimer>

static QTextStream out(stdout);

// Таблица CP1251 для байтов 0x80..0xFF.
//
// QStringDecoder("windows-1251") в Qt 6 без модуля Qt5Compat недоступен
// и молча возвращает пустую строку — на этом проверка сначала и упала.
// Само приложение не затронуто: у него своя функция cp2utf.
static const ushort kCp1251[128] = {
    0x0402,0x0403,0x201A,0x0453,0x201E,0x2026,0x2020,0x2021,
    0x20AC,0x2030,0x0409,0x2039,0x040A,0x040C,0x040B,0x040F,
    0x0452,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
    0x0000,0x2122,0x0459,0x203A,0x045A,0x045C,0x045B,0x045F,
    0x00A0,0x040E,0x045E,0x0408,0x00A4,0x0490,0x00A6,0x00A7,
    0x0401,0x00A9,0x0404,0x00AB,0x00AC,0x00AD,0x00AE,0x0407,
    0x00B0,0x00B1,0x0406,0x0456,0x0491,0x00B5,0x00B6,0x00B7,
    0x0451,0x2116,0x0454,0x00BB,0x0458,0x0405,0x0455,0x0457,
    0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
    0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
    0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
    0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
    0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
    0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
    0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
    0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F
};

static QString readCp1251(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray raw = f.readAll();
    QString s;
    s.reserve(raw.size());
    for (unsigned char c : raw)
        s.append(c < 0x80 ? QChar(c) : QChar(kCp1251[c - 0x80]));
    return s;
}

// Аналог nameQ: для PostgreSQL предпочитается вариант из postgresql/
static QString resolve(const QString &root, const QString &rel)
{
    const QString pg = root + "/postgresql/" + rel;
    return QFile::exists(pg) ? pg : root + "/" + rel;
}

static bool check(QSqlDatabase &db, const QString &root, const QString &rel,
                  const QString &alias, const QString &frag,
                  const QStringList &mustHave)
{
    const QString path = resolve(root, rel);
    const QString sql = readCp1251(path);
    if (sql.isEmpty()) {
        out << "ПРОВАЛ: не прочитан " << path << Qt::endl;
        return false;
    }

    const QString q = sql + QString(" AND %1.fileID IN (%2)").arg(alias, frag);

    QElapsedTimer t;
    t.start();
    QSqlQuery query(db);
    query.setForwardOnly(true);
    if (!query.exec(q)) {
        out << "ПРОВАЛ: " << path << "\n  " << query.lastError().text() << Qt::endl;
        return false;
    }

    int n = 0;
    QStringList missing;
    while (query.next()) {
        if (n == 0) {
            const QSqlRecord rec = query.record();
            for (const QString &c : mustHave)
                if (rec.indexOf(c) < 0)
                    missing << c;
        }
        ++n;
    }

    out << QString("%1: %2 строк за %3 мс")
               .arg(rel).arg(n).arg(t.elapsed()) << Qt::endl;

    if (!missing.isEmpty()) {
        out << "  ПРОВАЛ: нет колонок: " << missing.join(", ") << Qt::endl;
        return false;
    }
    if (n == 0) {
        out << "  ПРОВАЛ: пусто" << Qt::endl;
        return false;
    }
    out << "  колонки на месте: " << mustHave.join(", ") << Qt::endl;
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser p;
    QCommandLineOption oRoot("root", "каталог gidr", "path");
    QCommandLineOption oDb("db", "база", "name", "tgid_gis");
    QCommandLineOption oHost("host", "сервер", "host", "localhost");
    QCommandLineOption oPort("port", "порт", "port", "5440");
    QCommandLineOption oUser("user", "пользователь", "user", "postgres");
    QCommandLineOption oPass("password", "пароль", "pass", "");
    QCommandLineOption oFrag("fragment", "фрагмент", "id", "80");
    p.addOptions({oRoot, oDb, oHost, oPort, oUser, oPass, oFrag});
    p.addHelpOption();
    p.process(app);

    out << "драйверы Qt SQL: " << QSqlDatabase::drivers().join(", ") << Qt::endl;
    if (!QSqlDatabase::drivers().contains("QPSQL")) {
        out << "ПРОВАЛ: драйвер QPSQL недоступен" << Qt::endl;
        return 1;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(p.value(oHost));
    db.setPort(p.value(oPort).toInt());
    db.setUserName(p.value(oUser));
    db.setPassword(p.value(oPass));
    db.setDatabaseName(p.value(oDb));

    if (!db.open()) {
        out << "ПРОВАЛ: подключение: " << db.lastError().text() << Qt::endl;
        return 1;
    }
    out << "подключение к " << p.value(oDb) << ": OK" << Qt::endl;

    const QString root = p.value(oRoot);
    const QString frag = p.value(oFrag);

    bool ok = true;
    ok &= check(db, root, "sql3/us.sql", "n", frag,
                {"id", "fileid", "x", "y", "type_txt", "id2"});
    ok &= check(db, root, "sql3/ut.sql", "n1", frag,
                {"id", "fileid", "nodeid1", "nodeid2", "coords", "type_txt"});

    db.close();
    out << (ok ? "ВСЁ ПРОШЛО" : "ЕСТЬ ПРОВАЛЫ") << Qt::endl;
    return ok ? 0 : 1;
}

// Отчёты ТУ: свод по годам и журнал регистрации (Excel через QXlsx).
// Порт содержимого gid6 OnTuSvod / OnTuZhurnal без HTML/print_doc стека.

#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QTemporaryDir>

#include "db/db.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"

QString transl(const QString &s);

using namespace QXlsx;

namespace {

QVariant cell(const QSqlQuery &q, const QString &rus)
{
    const QString col = transl(rus);
    const int idx = q.record().indexOf(col);
    if (idx < 0) return QVariant();
    return q.value(idx);
}

QString cellS(const QSqlQuery &q, const QString &rus)
{
    return cell(q, rus).toString().trimmed();
}

double cellD(const QSqlQuery &q, const QString &rus)
{
    return cell(q, rus).toDouble();
}

QString joinDate(const QString &a, const QString &b)
{
    if (a.isEmpty()) return b;
    if (b.isEmpty()) return a;
    return a + " " + b;
}

QString readSostCode(const QSqlQuery &q)
{
    const QVariant v = cell(q, "Состояние договора");
    if (!v.isValid() || v.isNull()) return "Д";
    bool ok = false;
    const int id = v.toInt(&ok);
    if (ok && id > 0) {
        switch (id) {
        case 1: return "Д";
        case 2: return "А";
        case 3: return "М";
        case 4: return "З";
        default: break;
        }
    }
    const QString s = v.toString().trimmed().toUpper();
    if (s == "З" || s == "Z" || s.startsWith("ЗАВЕРШ")) return "З";
    if (s == "А" || s == "A" || s.startsWith("АННУЛ")) return "А";
    if (s == "М" || s == "M" || s.contains("МОНИТОР")) return "М";
    return s.isEmpty() ? "Д" : s.left(1);
}

QString formatSost(const QSqlQuery &q)
{
    const QString code = readSostCode(q);
    if (code == "З") {
        return QObject::tr("Завершен ") + cellS(q, "Дата выдачи акта допуска");
    }
    if (code == "А") {
        return QObject::tr("Аннулирован ") + cellS(q, "Дата аннулирования");
    }
    if (code == "М") {
        return QObject::tr("Аннулирован мониторингу ") + cellS(q, "Дата аннулирования");
    }
    return QString();
}

Format headerFmt()
{
    Format f;
    f.setFontBold(true);
    f.setHorizontalAlignment(Format::AlignHCenter);
    f.setVerticalAlignment(Format::AlignVCenter);
    f.setTextWrap(true);
    return f;
}

Format fillFmt(const QString &hex)
{
    Format f;
    f.setFillPattern(Format::PatternSolid);
    f.setPatternForegroundColor(QColor(hex));
    return f;
}

QString safeSheetName(QString name)
{
    name.replace(QRegularExpression(R"([\\/?*\[\]])"), "_");
    name = name.trimmed();
    if (name.isEmpty()) name = "_";
    if (name.size() > 31) name = name.left(31);
    return name;
}

QString lastAmendment(const QSqlQuery &q)
{
    for (int i = 7; i >= 1; --i) {
        const QString s0 = cellS(q, QString("Изменения продления %1").arg(i));
        const QString s1 = cellS(q, QString("Дата изменения продления %1").arg(i));
        if (!s0.isEmpty() || !s1.isEmpty()) {
            return joinDate(s0, s1);
        }
    }
    return QString();
}

double coefGvs()
{
    QSettings settings;
    bool ok = false;
    double c = settings.value("coef24", 2.4).toDouble(&ok);
    if (!ok || c <= 0) c = 2.4;
    return c;
}

struct Totals {
    double d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0;
    double d6 = 0, d7 = 0, d8 = 0, d9 = 0, d10 = 0;
    Totals &operator+=(const Totals &o)
    {
        d1 += o.d1; d2 += o.d2; d3 += o.d3; d4 += o.d4; d5 += o.d5;
        d6 += o.d6; d7 += o.d7; d8 += o.d8; d9 += o.d9; d10 += o.d10;
        return *this;
    }
};

bool openReport(Document &xlsx, const QString &baseName)
{
    const QString dir = QDir::temp().filePath("tgid_tu_reports");
    QDir().mkpath(dir);
    const QString path = QDir(dir).filePath(
        QString("%1_%2.xlsx")
            .arg(baseName, QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")));
    if (!xlsx.saveAs(path)) {
        QMessageBox::warning(nullptr, "", QObject::tr("Не удалось сохранить отчёт:\n%1").arg(path));
        return false;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    return true;
}

} // namespace


void print_tu_svod(QSqlDatabase &db)
{
    const QString q =
        "SELECT * FROM tehnicheskie_usloviya "
        "ORDER BY data_vydachi_tu NULLS LAST, nomer_tu";

    QSqlQuery query(db);
    query.setForwardOnly(true);
    if (!query_exec(db, query, q)) {
        QMessageBox::warning(nullptr, "", QObject::tr("Ошибка чтения технических условий"));
        return;
    }

    Document xlsx;
    const Format hdr = headerFmt();
    Format green = fillFmt("#EBF1DE");
    Format orange = fillFmt("#FDE9D9");
    Format peach = fillFmt("#FCD5B4");

    const QStringList headers = {
        QObject::tr("№"),
        QObject::tr("Номер / дата ТУ"),
        QObject::tr("Организация"),
        QObject::tr("Объект"),
        QObject::tr("Адрес"),
        QObject::tr("Нагрузка, Гкал/ч"),
        QObject::tr("Прирост"),
        QObject::tr("Доп. мероприятия"),
        QObject::tr("Состояние"),
        QObject::tr("Изменение/продление"),
        QObject::tr("Согл. ТС"),
        QObject::tr("Согл. ОВ"),
        QObject::tr("Согл. ТП"),
        QObject::tr("Исполнение мероприятий"),
        QObject::tr("Стадия строительства"),
        QObject::tr("Акт допуска"),
        QObject::tr("Нагрузка по акту"),
        QObject::tr("Договор"),
    };

    QMap<int, int> yearRow; // year -> next data row
    QMap<int, QString> yearSheet;

    auto ensureSheet = [&](int year) {
        if (!yearSheet.contains(year)) {
            QString title = year < 1900 ? QObject::tr("Без даты") : QString::number(year);
            title = safeSheetName(title);
            if (yearSheet.isEmpty()) {
                xlsx.renameSheet("Sheet1", title);
            } else {
                xlsx.addSheet(title);
            }
            yearSheet[year] = title;
            yearRow[year] = 2;
            xlsx.selectSheet(title);
            for (int c = 0; c < headers.size(); ++c) {
                xlsx.write(1, c + 1, headers[c], hdr);
            }
        } else {
            xlsx.selectSheet(yearSheet[year]);
        }
    };

    while (query.next()) {
        const QDate d = cell(query, "Дата выдачи ТУ").toDate();
        int year = d.isValid() ? d.year() : 0;
        ensureSheet(year);
        int row = yearRow[year]++;

        const QString code = readSostCode(query);
        Format rowFmt;
        if (code == "З") rowFmt = green;
        else if (code == "А") rowFmt = orange;
        else if (code == "М") rowFmt = peach;

        auto write = [&](int col, const QVariant &v) {
            if (rowFmt.fillPattern() != Format::PatternNone)
                xlsx.write(row, col, v, rowFmt);
            else
                xlsx.write(row, col, v);
        };

        int c = 1;
        write(c++, row - 1);
        write(c++, joinDate(cellS(query, "Номер ТУ"), cellS(query, "Дата выдачи ТУ")));
        write(c++, cellS(query, "Наименование организации, запрашивающей ТУ"));
        write(c++, cellS(query, "Наименование объекта"));
        write(c++, cellS(query, "Адрес объекта"));
        write(c++, cellD(query, "Тепловые потоки, Гкал/ч"));
        write(c++, cellD(query, "Прирост нагрузки"));
        write(c++, cellS(query, "Дополнительные технические мероприятия"));
        write(c++, formatSost(query));
        write(c++, lastAmendment(query));
        write(c++, joinDate(cellS(query, "Номер согласования ТС"), cellS(query, "Дата согласования ТС")));
        write(c++, joinDate(cellS(query, "Номер согласования ОВ"), cellS(query, "Дата согласования ОВ")));
        write(c++, joinDate(cellS(query, "Номер согласования ТП"), cellS(query, "Дата согласования ТП")));
        write(c++, cellS(query, "Исполнение доп техн и энерг мероприятий в рамках ТУ"));
        write(c++, cellS(query, "Стадия строительства объектов"));
        write(c++, joinDate(cellS(query, "Номер выдачи акта допуска"), cellS(query, "Дата выдачи акта допуска")));
        write(c++, cellD(query, "Тепловая нагрузка по акту допуска (проекту) Гкал/ч"));
        write(c++, joinDate(cellS(query, "Номер договора"), cellS(query, "Дата договора")));
    }

    if (yearSheet.isEmpty()) {
        QMessageBox::information(nullptr, "", QObject::tr("Нет данных технических условий"));
        return;
    }

    // легенда на первом листе
    auto it0 = yearSheet.constBegin();
    xlsx.selectSheet(it0.value());
    const int legendRow = yearRow.value(it0.key()) + 2;
    xlsx.write(legendRow, 1, "", green);
    xlsx.write(legendRow, 2, QObject::tr("объекты, получившие акты допуска"));
    xlsx.write(legendRow + 1, 1, "", orange);
    xlsx.write(legendRow + 1, 2, QObject::tr("объекты, по которым ТУ аннулированы"));
    xlsx.write(legendRow + 2, 1, "", peach);
    xlsx.write(legendRow + 2, 2, QObject::tr("аннулировано по результатам мониторинга"));

    openReport(xlsx, "tu_svod");
}


void print_tu_zhurnal(QSqlDatabase &db, int year)
{
    const double k = coefGvs();

    QString q;
    if (year > 0) {
        q = QString(
                "SELECT * FROM tehnicheskie_usloviya "
                "WHERE EXTRACT(YEAR FROM data_vydachi_tu)=%1 "
                "ORDER BY rayon_ekspluatatsii NULLS LAST, data_vydachi_tu NULLS LAST, nomer_tu")
                .arg(year);
    } else {
        q = "SELECT * FROM tehnicheskie_usloviya "
            "WHERE data_vydachi_tu IS NULL "
            "ORDER BY rayon_ekspluatatsii NULLS LAST, nomer_tu";
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    if (!query_exec(db, query, q)) {
        QMessageBox::warning(nullptr, "", QObject::tr("Ошибка чтения технических условий"));
        return;
    }

    Document xlsx;
    const Format hdr = headerFmt();
    Format green = fillFmt("#EBF1DE");
    Format orange = fillFmt("#FDE9D9");
    Format peach = fillFmt("#FCD5B4");

    const QStringList headers = {
        QObject::tr("№"),
        QObject::tr("Номер / дата ТУ"),
        QObject::tr("Организация"),
        QObject::tr("Объект"),
        QObject::tr("Адрес"),
        QObject::tr("Q, Гкал/ч"),
        QObject::tr("в т.ч. отопление"),
        QObject::tr("в т.ч. вентиляция"),
        QObject::tr("в т.ч. ГВС макс"),
        QObject::tr("ГВС ср"),
        QObject::tr("Прирост"),
        QObject::tr("пр. отопление"),
        QObject::tr("пр. вентиляция"),
        QObject::tr("пр. ГВС макс"),
        QObject::tr("пр. ГВС ср"),
        QObject::tr("Камера"),
        QObject::tr("Доп. мероприятия"),
        QObject::tr("Срок действия ТУ"),
        QObject::tr("Состояние"),
    };

    struct SheetState {
        QString name;
        int row = 2;
        Totals all, ann, zav;
    };
    QMap<QString, SheetState> sheets;
    bool first = true;

    auto ensure = [&](QString raion) -> SheetState & {
        if (raion.isEmpty()) raion = QObject::tr("Не задан");
        if (!sheets.contains(raion)) {
            const QString title = safeSheetName(raion);
            if (first) {
                xlsx.renameSheet("Sheet1", title);
                first = false;
            } else {
                xlsx.addSheet(title);
            }
            SheetState st;
            st.name = title;
            sheets.insert(raion, st);
            xlsx.selectSheet(title);
            for (int c = 0; c < headers.size(); ++c) {
                xlsx.write(1, c + 1, headers[c], hdr);
            }
        } else {
            xlsx.selectSheet(sheets[raion].name);
        }
        return sheets[raion];
    };

    int nRows = 0;
    while (query.next()) {
        ++nRows;
        SheetState &st = ensure(cellS(query, "Район эксплуатации"));
        const int row = st.row++;

        const QString code = readSostCode(query);
        Format rowFmt;
        if (code == "З") rowFmt = green;
        else if (code == "А") rowFmt = orange;
        else if (code == "М") rowFmt = peach;

        auto write = [&](int col, const QVariant &v) {
            if (rowFmt.fillPattern() != Format::PatternNone)
                xlsx.write(row, col, v, rowFmt);
            else
                xlsx.write(row, col, v);
        };

        Totals t;
        t.d1 = cellD(query, "Тепловые потоки, Гкал/ч");
        t.d2 = cellD(query, "В том числе отопление");
        t.d3 = cellD(query, "В том числе вентиляция");
        t.d4 = cellD(query, "В том числе ГВС макс");
        t.d5 = t.d4 / k;
        t.d6 = cellD(query, "Прирост нагрузки");
        t.d7 = cellD(query, "В том числе прирост отопление");
        t.d8 = cellD(query, "В том числе прирост вентиляция");
        t.d9 = cellD(query, "В том числе прирост ГВС макс");
        t.d10 = t.d9 / k;
        st.all += t;
        if (code == "З") st.zav += t;
        if (code == "А" || code == "М") st.ann += t;

        int c = 1;
        write(c++, row - 1);
        write(c++, joinDate(cellS(query, "Номер ТУ"), cellS(query, "Дата выдачи ТУ")));
        write(c++, cellS(query, "Наименование организации, запрашивающей ТУ"));
        write(c++, cellS(query, "Наименование объекта"));
        write(c++, cellS(query, "Адрес объекта"));
        write(c++, t.d1);
        write(c++, t.d2);
        write(c++, t.d3);
        write(c++, t.d4);
        write(c++, t.d5);
        write(c++, t.d6);
        write(c++, t.d7);
        write(c++, t.d8);
        write(c++, t.d9);
        write(c++, t.d10);
        write(c++, cellS(query, "Камера"));
        write(c++, cellS(query, "Дополнительные технические мероприятия"));
        write(c++, cellS(query, "Срок действия ТУ"));
        write(c++, formatSost(query));
    }

    if (nRows == 0) {
        QMessageBox::information(nullptr, "", QObject::tr("Нет ТУ за выбранный период"));
        return;
    }

    auto writeTotals = [&](int &row, const QString &name, const Totals &t) {
        Format bold;
        bold.setFontBold(true);
        int c = 1;
        xlsx.write(row, c++, QVariant(), bold);
        xlsx.write(row, c++, QVariant(), bold);
        xlsx.write(row, c++, name, bold);
        xlsx.write(row, c++, QVariant(), bold);
        xlsx.write(row, c++, QVariant(), bold);
        xlsx.write(row, c++, t.d1, bold);
        xlsx.write(row, c++, t.d2, bold);
        xlsx.write(row, c++, t.d3, bold);
        xlsx.write(row, c++, t.d4, bold);
        xlsx.write(row, c++, t.d5, bold);
        xlsx.write(row, c++, t.d6, bold);
        xlsx.write(row, c++, t.d7, bold);
        xlsx.write(row, c++, t.d8, bold);
        xlsx.write(row, c++, t.d9, bold);
        xlsx.write(row, c++, t.d10, bold);
        ++row;
    };

    for (auto it = sheets.begin(); it != sheets.end(); ++it) {
        SheetState &st = it.value();
        xlsx.selectSheet(st.name);
        int row = st.row + 1;
        writeTotals(row, QObject::tr("ИТОГО:"), st.all);
        writeTotals(row, QObject::tr("Аннулировано"), st.ann);
        writeTotals(row, QObject::tr("Подключено"), st.zav);
        ++row;
        Format bold;
        bold.setFontBold(true);
        xlsx.write(row, 3, QObject::tr("Итого прирост, в т.ч:"), bold);
        xlsx.write(row, 11, st.all.d6, bold);
        ++row;
        xlsx.write(row, 3, "ОВ");
        xlsx.write(row, 11, st.all.d7 + st.all.d8);
        ++row;
        xlsx.write(row, 3, QObject::tr("ГВС, макс"));
        xlsx.write(row, 11, st.all.d9);
        ++row;
        xlsx.write(row, 3, QObject::tr("ГВС, ср"));
        xlsx.write(row, 11, st.all.d10);
    }

    const QString base = year > 0
        ? QString("tu_zhurnal_%1").arg(year)
        : QString("tu_zhurnal_nodate");
    openReport(xlsx, base);
}

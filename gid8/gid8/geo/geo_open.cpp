#include <QApplication>
#include <QtGui>
#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QProgressDialog>
#include <QInputDialog>
#include <set>
#include <mystd.h>
#include <win.h>


#include "db/db.h"
#include "gidview/colorgtd.h"
#include "mainwindow.h"

#include <geo/geofile.h>
#include <geo/Klassif.h>
#include <cxema/coordlis.h>

int readWKB(const char *buf, int len, CCoordList &cl);

int m_n_list = 0;

double m_frmx1 = LONG_MAX;
double m_frmy1 = LONG_MAX;
double m_frmx2 = LONG_MIN;
double m_frmy2 = LONG_MIN;

long sizeOfQ(QSqlDatabase &db, const QString & q);

long getGeoColor(int color_id)
{
    if (color_id == 1) {
        return 0xFF0000; // RED
    }
    else if (color_id == 2) {
        return 0x0000FF; // BLUE
    }
    else if (color_id == 3) {
        return 0x00FF00; // GREEN
    }
    else if (color_id == 4) {
        return 0x000000; // BLACK
    }

    else if (color_id == 1000) {
        return 16777092; //  Здание ТУ
    }
    else if (color_id == 57) {
        return 16764159; //  Дом с нагрузкой
    }
    else if (color_id == 56) {
        return 10210815; //  Дом без нагрузки
    }



    else if (color_id == 1001) {
        return 0x00FFFF; //  Здание ТУ (Завершенный)
        //        return 0x00C4C4; //  Здание ТУ (Завершенный)
        //        return 0xC4C400; //  Здание ТУ (Завершенный)
    }
    else if (color_id >= 2000 && color_id < 2030) {
        int c = color_id-2000;
        if ((c & (1 << (4-1))) && !(c & (1 << (1-1)))) {
            return 0x00FFFF; //  Здание ТУ (Завершенный)
            //            return 0xC4C400; //  Здание ТУ (Завершенный) Темноголубой
        }
        return 16777092; //  Здание ТУ
    }


    return color_id;
}

bool readWKB(QVariant &var, CCoordList &cl)
{
    if (var.isNull()) return false;

    QByteArray sa = var.toByteArray();
    long r1 = 0, r2 = sa.length();
    const char *pb = sa.data();
    readWKB(pb, r2 - r1, cl);
    return true;
}


#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

#if 0
// Детерминированная функция: по индексу n возвращает яркий различимый цвет в формате #RRGGBB
//std::string getDistinctColor(int n) {

unsigned int getDistinctColor(int n) {
    // Сдвигаем на золотое сечение для максимальной равномерности по цветовому кругу
    const double golden_ratio_conjugate = 0.6180339887498948;
    double hue = std::fmod(n * golden_ratio_conjugate, 1.0); // 0.0 .. 1.0

    // Делаем цвета яркими: высокая насыщенность и значение
    double saturation = 0.85;  // можно варьировать: 0.7–1.0
    double value      = 0.95;  // почти максимальная яркость

    // HSV → RGB
    int h = static_cast<int>(hue * 360);
    double c = value * saturation;
    double x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
    double m = value - c;

    double r, g, b;
    if (h < 60)      { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else              { r = c; g = 0; b = x; }

    int R = static_cast<int>((r + m) * 255);
    int G = static_cast<int>((g + m) * 255);
    int B = static_cast<int>((b + m) * 255);


    return (static_cast<uint32_t>(r * 255) << 16) |
           (static_cast<uint32_t>(g * 255) <<  8) |
           (static_cast<uint32_t>(b * 255)      );
    

/*
    std::stringstream ss;
    ss << '#' << std::hex << std::uppercase
       << std::setfill('0') << std::setw(2) << R
       << std::setfill('0') << std::setw(2) << G
       << std::setfill('0') << std::setw(2) << B;
    return ss.str();
*/
}

#endif



#include <cstdint>

// Возвращает цвет в формате 0xRRGGBB
// n          — индекс цвета (0, 1, 2, ...), всегда один и тот же n → один и тот же оттенок
// brightness — яркость от 0.0 до 1.0 (рекомендую 0.4–1.0)
// saturation — насыщенность от 0.0 до 1.0 (по умолчанию 0.85 — яркие цвета)

/*

Рекомендации по яркости:

Яркость
Когда использовать
0.9–1.0 Основные цвета, выделение
0.6–0.8 Обычные элементы, метки
0.3–0.5 Фон, неактивные, подсказки
0.8–0.95 + saturation 0.4 Пастельная палитра
*/


uint32_t getDistinctColor(uint32_t n,
                                    double brightness = 0.95,
                                    double saturation = 0.85) {
    // Защита от некорректных значений
    if (brightness < 0.0) brightness = 0.0;
    if (brightness > 1.0) brightness = 1.0;
    if (saturation < 0.0) saturation = 0.0;
    if (saturation > 1.0) saturation = 1.0;

    // Золотое сечение — равномерно распределяет оттенки по цветовому кругу
    const double golden = 0.6180339887498948;
    double hue = n * golden;
    hue -= static_cast<uint64_t>(hue);  // hue ∈ [0, 1)

    double v = brightness;
    double s = saturation;

    int i = static_cast<int>(hue * 6);
    double f = hue * 6 - i;
    double p = v * (1.0 - s);
    double q = v * (1.0 - f * s);
    double t = v * (1.0 - (1.0 - f) * s);

    double r, g, b;
    switch (i % 6) {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    default: r = v, g = p, b = q; break;
    }

    return (static_cast<uint32_t>(r * 255 + 0.5) << 16) |
           (static_cast<uint32_t>(g * 255 + 0.5) <<  8) |
           (static_cast<uint32_t>(b * 255 + 0.5)      );
}

/*
// Пример использования
int main() {
    std::cout << "Первые 15 цветов:\n";
    for (int i = 0; i < 15; ++i) {
        std::cout << i << " → " << getDistinctColor(i) << "\n";
    }
    return 0;
}
*/

bool readMySql_Shape_table1(QSqlDatabase & db, const QString & q0, QProgressDialog *percent = nullptr, int p1 = 0, int p2 = 100)
{
    double x1, x2, y1, y2;

    long count0 = sizeOfQ(db, q0);

    int n_loc1 = 0, n_loc2 = 0, n_loc3 = 0;

    if (percent) {
        percent->setLabelText(QString("Начали"));
        QApplication::processEvents();
    }


    QString old_name = "Неправильное название таблицы";
    Klassif* kls = nullptr;

//    str.Format(_TR("Открывается %1, подождите пожалуйста..."), ado->m_schema);

    int n10 = max(10, (int)(count0/300));

    QSqlQuery query(db);

//    if (percent) {
//        percent->reset();
//    }


    int n_color = 0;


    query.setForwardOnly(true);
    if (query_exec(db, query, q0, true)) {
        qDebug() << q0;
        int n = 0;

        while (query.next()) {
            if (percent && count0) {
                if (n % n10 == 0) {
                    percent->setValue(n * 100. / count0 );
                    QApplication::processEvents();
                }
            }


            QString name = query.value("name").toString();

            if (name != old_name) {
                kls = m_kl_list.findKlN(name);

                if (percent) {
                    percent->setLabelText(QString("Открывается %1").arg(name));
                    QApplication::processEvents();
                }

                if (!kls) {
                    Klassif kl;
                    kl.m_db = &db;
                    kl.nazv = name;
                    kl.nazv_alias = name;
                    kl.loc = LOC2;

                    m_kl_list[kl.nazv.toLower()] = kl;
                }
                
                
                if (kls) {
                    kls->m_db = &db;
                    old_name = name;
                }
            }

            if (!kls) continue;

            long count = (long) kls->geo4.size();

            long id = query.value("id").toInt();

            QVariant var = query.value("__wkb");

            CCoordList cl;
            readWKB(var, cl);

            if (cl.loc == 1) {
                n_loc1++;
            }
            else if (cl.loc == 2) {
                n_loc2++;
            }
            else if (cl.loc == 3) {
                n_loc3++;
            }

            cl.po = query.value("__po").toInt();

            QString label = query.value("__label").toString();

            long label_color = query.value("__label_color").toInt();
            long label_color2 = query.value("__label_color2").toInt();
            long label_color3 = query.value("__label_color3").toInt();

            QDateTime label_date = query.value("__label_date").toDateTime();

            cl.angle = query.value("__label_angle").toDouble();
            cl.fileID = query.value("__label_fileID").toInt();

            if (label_color != -1) {
                cl.is_color = true;
                cl.color = getGeoColor(label_color);
            }


            if (label_color2 != -1) {
                cl.is_color2 = true;
                cl.color2 = getGeoColor(label_color2);
            }

            if (label_color3 != -1) {
                cl.is_color = true;
                cl.color = getGeoColor(label_color3);
            }

            if (name == "adm_border.exp_raion") {
                cl.is_color = true;
                cl.color = getDistinctColor(n_color, 0.95, 0.4);
                cl.color2 = getDistinctColor(n_color);
                n_color += 1;
            }


            if (!label_date.isNull()) {
                cl.is_date = true;
                cl.date = label_date;
            }

            CFRect rect = cl.GetFRect();

            if (cl.size() != 0) {
                x1 = rect.left;
                y1 = rect.top;
                x2 = rect.right;
                y2 = rect.bottom;

                if (kls->loc == LOC6) {
                    CFPoint p = cl.first();
                    x1 = x2 = p.x;
                    y1 = y2 = p.y;
                }

                if (x2 > m_frmx2 || y2 > m_frmy2 || x1 < m_frmx1 || y1 < m_frmy1) {
                    int qq;
                    qq = 1;
                    rect = cl.GetFRect();
                    //                readWKB(var, cl);
                }

//                if (ado->m_schema != getPsAdoName()) {
                if (x1 < m_frmx1 && fabs(x1) < 1e10) m_frmx1 = x1;
                if (y1 < m_frmy1 && fabs(y1) < 1e10) m_frmy1 = y1;

                if (x2 > m_frmx2 && fabs(x2) < 1e10) m_frmx2 = x2;
                if (y2 > m_frmy2 && fabs(y2) < 1e10) m_frmy2 = y2;
//                }

                CGeoObject * g = new CGeoObject;

                g->nom = id;
                g->nom2 = id;
                g->mark = 0;
                g->rect = CFRect(x1, y1, x2, y2);

                g->text00000 = label;
                g->n_label = kls->n_label;

                g->cl = cl;
                g->is_read = true;

                if (kls->cnt < count) {
                    kls->geo4[kls->cnt] = g;
                }
                else {
                    kls->geo4.push_back(g);
                }
                kls->cnt++;
            }
            n++;
        }
    }

    percent->setValue(100);
    QApplication::processEvents();

    return true;
}

bool readMySql_Shape(QSqlDatabase &db, QProgressDialog *percent)
{
    if (m_n_list == 0) {
        m_frmx1 = LONG_MAX;
        m_frmy1 = LONG_MAX;
        m_frmx2 = LONG_MIN;
        m_frmy2 = LONG_MIN;
    }

    QString str = "";

//  str.Format(_TR("Открывается %1, подождите пожалуйста..."), ado->m_schema);

    if (is_SQLITE()) {
        return false;
    }
    
    std::set<QString> set_tab;

    QString q = "SELECT T.name FROM sys.objects AS T JOIN sys.columns AS C ON T.object_id = C.object_id JOIN sys.types AS P ON C.user_type_id = P.user_type_id WHERE  T.type_desc = 'USER_TABLE' AND C.name = 'shape'";

    QSqlQuery query(db);
    query.setForwardOnly(true);

q = 

"SELECT\n"
"    s.name AS SchemaName,\n"
"    t.name AS TableName,\n"
"    c.name AS ColumnName,\n"
"    c2.name AS ColumnName\n"

"FROM sys.schemas s\n"
"JOIN sys.tables t ON s.schema_id = t.schema_id\n"
"JOIN sys.columns c ON t.object_id = c.object_id\n"
"JOIN sys.types ty ON c.user_type_id = ty.user_type_id AND ty.name = 'geometry'\n"


"JOIN sys.tables t2 ON t2.object_id=t.object_id\n"
"JOIN sys.columns c2 ON t2.object_id = c2.object_id\n"
"JOIN sys.identity_columns ic2 ON c2.object_id = ic2.object_id AND c2.column_id = ic2.column_id\n";



    if (is_POSTGRESQL()) {
q = 

"SELECT \n"
"    n.nspname AS schema_name,\n"
"    c.relname AS table_name,\n"
"    a.attname AS column_name,\n"
"    a2.attname AS column_name2\n"
"FROM \n"
"    pg_class c\n"
"JOIN \n"
"    pg_namespace n ON n.oid = c.relnamespace\n"
"JOIN \n"
"    pg_attribute a ON a.attrelid = c.oid\n"
"JOIN \n"
"    pg_type t ON a.atttypid = t.oid\n"
"JOIN \n"
"    pg_class c2 on c.oid=c2.oid\n"
"JOIN \n"
"    pg_namespace n2 ON n2.oid = c2.relnamespace\n"
"JOIN \n"
"    pg_attribute a2 ON a2.attrelid = c2.oid\n"
"JOIN \n"
"    pg_attrdef d2 ON d2.adrelid = c2.oid AND d2.adnum = a2.attnum\n"

"WHERE \n"
"    a2.atttypid = 'int4'::regtype \n"
"    AND pg_get_expr(d2.adbin, d2.adrelid) LIKE 'nextval(%::regclass)'\n"
"    AND c2.relkind = 'r'  -- only regular tables    \n"
"AND\n"
"    t.typname = 'geometry'\n"
"    AND c.relkind = 'r'  -- only regular tables\n"

//"ORDER BY \n"
//"    n.nspname,\n"
//"    c.relname\n"
;

    }

    query.setForwardOnly(true);
    if (query_exec(db, query, q, true)) {
        while (query.next()) {
            QString s = query.value(0).toString().toLower();
            QString tn = query.value(1).toString().toLower();

            tn = full_name(s, tn);

            set_tab.insert(tn);

            QString shape = query.value(2).toString().toLower();
            QString id = query.value(3).toString().toLower();
            Klassif *kls = m_kl_list.findKlN(tn);
            if (kls) {
                kls->shape = shape;
                kls->id = id;
            }
        }
    }

    q = "";

    for (Klassif &kl: m_kl_list) {
        QString wkb_str;

        if (is_POSTGRESQL()) {
            wkb_str = QString("ST_AsBinary(%1)").arg(kl.shape);
        }
        else {
            wkb_str = QString("%1.STAsBinary()").arg(kl.shape);
        }

//    if (ado_m_type_of_net_TYPE_OF_NET_MYSQL) {
//        wkb_str = "ST_AsWKB(shape)";
//    }

//    if (kl.m_ado == ado) {
        QString s = kl.nazv.toLower();
        auto it = set_tab.find(s);
        if (it == set_tab.end()) continue;

        if (!kl.ins || kl.open ) continue;

        if (q != "") q += " UNION ALL\n";
        QString qq;
        QString label = " '' AS __label";
        QString label_color = " -1 AS __label_color";
        QString label_color2 = " -1 AS __label_color2";
        QString label_color3 = " -1 AS __label_color3";
        QString label_date = " CAST(NULL as DATE) AS __label_date";
        QString label_stateID = " CAST(NULL as int) AS __stateID";
        QString label_angle = " CAST(NULL as float) AS __label_angle";
        QString label_fileID = " CAST(NULL as int) AS __label_fileID";

        if (kl.label && kl.codlabel != "") {
            if (column_in_table(db, kl.nazv, kl.codlabel))  {
//                label = QString(" %1 AS __label ").arg(kl.codlabel);
//                label = QString(" TRY_CONVERT(NVARCHAR(MAX), %1) AS __label").arg(kl.codlabel);
                if (is_POSTGRESQL()) {
                    label = QString(" CAST(\"%1\" AS TEXT) AS __label").arg(kl.codlabel);
                }
                if (is_MSSQL()) {
                    label = QString(" CAST([%1] AS NVARCHAR) AS __label").arg(kl.codlabel);
                }
            }
        }

        if (kl.label_color && kl.codlabel_color != "") {
            if (column_in_table(db, kl.nazv, kl.codlabel_color))  {
                label_color = QString(" %1 AS __label_color ").arg(kl.codlabel_color);
            }
        }

        if (kl.label_color2 && kl.codlabel_color2 != "") {
            if (column_in_table(db, kl.nazv, kl.codlabel_color2))  {
                label_color2 = QString(" %1 AS __label_color2 ").arg(kl.codlabel_color2);
            }
        }

        if (kl.label_color3 && kl.codlabel_color3 != "") {
            if (column_in_table(db, kl.nazv, kl.codlabel_color3))  {
                label_color3 = QString(" IIF( %1 IS NULL OR %1=0, 56, 57 ) AS __label_color3 ").arg(kl.codlabel_color3);
            }
        }



        
        if (kl.label_date && kl.codlabel_date != "") {
            if (column_in_table(db, kl.nazv, kl.codlabel_date))  {
                label_date = QString(" %1 AS __label_date ").arg(kl.codlabel_date);
            }
        }

        if (kl.label_angle && kl.codlabel_angle != "") {
            if (column_in_table(db, kl.nazv, kl.codlabel_angle))  {
                label_angle = QString(" %1 AS __label_angle ").arg(kl.codlabel_angle);
            }
        }

        if (kl.label_fileID && kl.codlabel_fileID != "") {
            if (column_in_table(db, kl.nazv, kl.codlabel_fileID))  {
                label_fileID = QString(" %1 AS __label_fileID ").arg(kl.codlabel_fileID);
            }
        }






/*
        if (kl.label_stateID && kl.codlabel_stateID != "") {
            if (column_in_table(db, kl.nazv, kl.codlabel_stateID))  {
               QString q1 = QString(", %1 AS __stateID FROM ")_.arg(kl.codlabel_stateID);
               q.Replace(" FROM ", q1);
               is_label_stateID = true;
            }
        }
*/

        QString dop = "NULL";

//        if (kl.nazv == "indikator_korrozii") {
//            dop = "truboprovod";
//        }

        QString tnb;


/*
        if (kl.nazv == "defect") {
            tnb = "(select * from defect WHERE NOT id IN (SELECT defectID FROM defectsForDiag)) _defect";
        }
        else {
*/
            tnb = tbl_sql(kl.nazv);
//        }

        if (kl.nazv == "zdaniya_tu") {
//            QString tnb.Format("(SELECT z.*, tu.sostoyanie_dogovora FROM %s z LEFT JOIN (SELECT id,sostoyanie_dogovora,zdanie FROM tehnicheskie_usloviya WHERE sostoyanie_dogovora=4) tu ON tu.zdanie=z.id) _%s", kl->nazv, kl->nazv);

tnb = R"(

(SELECT z.*, t.code FROM zdaniya_tu z
LEFT JOIN (
SELECT
zdanie, 
SUM(POWER(2, sostoyanie_dogovora-1)) AS code
FROM (
SELECT DISTINCT sostoyanie_dogovora, zdanie FROM tehnicheskie_usloviya WHERE sostoyanie_dogovora IS NOT NULL AND sostoyanie_dogovora > 0
) tu
WHERE zdanie IS NOT NULL AND zdanie <> 0
GROUP BY zdanie
) t ON t.zdanie=z.id ) _zdaniya_tu

)";

                    label_color = " IIF (code IS NULL OR code = 0, 0, code)+2000  AS __label_color";
                }





        qq = QString("SELECT %1 AS id,%2,%3,%4,%5,%6,'%7' AS name,%8 AS __wkb, %9 AS __po, %10, %11 FROM %12\n")
            .arg(kl.id, label, label_color, label_color2, label_color3, label_date, kl.nazv, wkb_str, dop, label_angle, label_fileID, tnb);

        q += qq;

        Klassif* kls = m_kl_list.findKlN(kl.nazv);
        if (kls) {
            kls->open = 1;
        }
//    }
    }

    if (q == "") {
        return true;
    }

    QString q2;

    for (auto & kl : m_kl_list) {
        kl.cnt = -1;
    }

    q2 = QString("SELECT name, COUNT(*) AS cnt FROM (%1) _QQ GROUP BY name").arg(q);

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q2);

    query.setForwardOnly(true);
    if (query_exec(db, query, q2, true)) {
        while (query.next()) {
            QString name = query.value("name").toString();
            long cnt = query.value("cnt").toInt();

            Klassif* kls = m_kl_list.findKlN(name);
            if (kls) {
                kls->cnt = 0;
                kls->geo4.resize(cnt);
                for (int i = 0; i < cnt; i++) {
                    kls->geo4[i] = NULL;
                }
            }
        }
    }

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);

    readMySql_Shape_table1(db, q, percent);

    for (auto &kl : m_kl_list) {
        if (kl.cnt >= 0) {
            kl.geo4.resize(kl.cnt);
        }
    }

    return true;
}

QString readstr1(const QString & s, QString & s1)
{
    QString s2 = "";
    static QRegularExpression re("^\\s*([^,]*),(.+)\\s*$");
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        s1 = match.captured(1);
        s2 = match.captured(2);
    }
    return s2;
}

QString readstr(const QString & s, QString & s1)
{
    QString s2 = "";
    static QRegularExpression re("^\\s*\"([^\"]*)\"\\s*,\\s*(.+)\\s*$");
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        s1 = match.captured(1);
        s2 = match.captured(2);
    }
    return s2;
}

QString readint(const QString & s, int &n1)
{
    QString s2 = "";
    static QRegularExpression re("^\\s*(-?[0-9]+)\\s*,\\s*(.+)\\s*$");
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        n1 = match.captured(1).toInt();
        s2 = match.captured(2);
    }
    return s2;
}

QString readint(const QString & s, unsigned char &n1)
{
    QString s2 = "";
    static QRegularExpression re("^\\s*([0-9]+)\\s*,\\s*(.+)\\s*$");
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        n1 = match.captured(1).toInt();
        s2 = match.captured(2);
    }
    return s2;
}

QString readint(const QString & s, unsigned long &n1)
{
    QString s2 = "";
    static QRegularExpression re("^\\s*([0-9]+)\\s*,\\s*(.+)\\s*$");
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        n1 = match.captured(1).toInt();
        s2 = match.captured(2);
    }
    return s2;
}

unsigned long rgb_bgr(unsigned long col);

bool open_klassif(const QString & klfn, const QString & name)
{
    QFile file(klfn);

//    *,3,1,16711680,16777215,  1,32512,  0,"corrosionIndicators","","",0,Arial,-13,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,"",0,-1,0,0

    int m_nMaxPrio = 0;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);
            QString ss;

            s = readstr1(s, ss);
            if (ss == "") break;

            Klassif kl;

            kl.baza = name;

            s = readint(s, kl.loc);              // 1
            s = readint(s, kl.prio);             // 2

//    m_nMaxPrio = max(m_nMaxPrio, kl.prio);

            s = readint(s, kl.coli); kl.coli = rgb_bgr(kl.coli);  // 3
            s = readint(s, kl.colb); kl.colb = rgb_bgr(kl.colb);  // 4
            s = readint(s, kl.typ);                               // 5
            s = readint(s, kl.col3); kl.col3 = rgb_bgr(kl.col3);  // 6
            s = readint(s, kl.punct);                             // 7
            s = readstr(s, ss);                                   // 8
            kl.nazv = ss;
            kl.nazv_alias = ss;
            s = readstr(s, ss);                                   // -
            s = readstr(s, ss);                                   // 9
            kl.codlabel = ss;
            s = readint(s, kl.label);                             // 10

            if (kl.nazv == "zdaniya_tu") {
                int qqq;
                qqq = 0;
            }


            kl.lf.lfFaceName = "";

            if (s != "") {
                s = readstr1(s, kl.lf.lfFaceName);                // 11
                s = readint(s, kl.lf.lfHeight);                   // 12
                s = readint(s, kl.lf.lfWidth);                    // 13
                s = readint(s, kl.lf.lfEscapement);               // 14
                s = readint(s, kl.lf.lfOrientation);              // 15
                s = readint(s, kl.lf.lfWeight);                   // 16
                s = readint(s, kl.lf.lfItalic);                   // 17
                s = readint(s, kl.lf.lfUnderline);                // 18
                s = readint(s, kl.lf.lfStrikeOut);                // 19
                s = readint(s, kl.lf.lfCharSet);                  // 20
                s = readint(s, kl.lf.lfOutPrecision);             // 21
                s = readint(s, kl.lf.lfClipPrecision);            // 22
                s = readint(s, kl.lf.lfQuality);                  // 23
                s = readint(s, kl.lf.lfPitchAndFamily);           // 24
                s = readint(s, kl.color); kl.color = rgb_bgr(kl.color);  // 25
            }
            kl.is_edit = 1;
            if (s != "") {
                s = readint(s, kl.is_edit);                       // 26
            }

            s = readint(s, kl.ins);                               // 27
            s = readint(s, kl.ins2);                              // 28
            s = readint(s, kl.is_potr);                           // 29
            s = readint(s, kl.ramka);                             // 30

            kl.codlabel_color = "";
            if (s != "") s = readstr(s, ss);                      // 31
            kl.codlabel_color = ss;

            kl.label_color = 0;
            if (s != "") s = readint(s, kl.label_color);

            kl.n_label_color = 0;
            if (s != "") s = readint(s, kl.n_label_color);

            kl.n_label = 0;
            if (s != "") s = readint(s, kl.n_label);

            kl.no_mas = 0;
            if (s != "") s = readint(s, kl.no_mas);

            if (kl.typ == 12 && kl.loc == LOC3) kl.typ = 0;

            if (kl.nazv == "remont" || kl.nazv == "defect") {
                kl.label_date = 1;
                kl.codlabel_date = "data_osmotra";
            }

            kl.label_color2 = 0;
            
            if (kl.nazv == "shurfy") {
                kl.label_color = 1;
                kl.label_color2 = 1;
                kl.codlabel_color = "sostoyanie_shurfaID";
                kl.codlabel_color2 = "naznachenie_vskrID";
            }
            if (kl.nazv == "defect") {
                kl.label_color = 1;
                kl.label_color2 = 1;
                kl.codlabel_color = "stateID";
                kl.codlabel_color2 = "remontTypeID";
            }
            kl.label_stateID = 0;
            if (kl.nazv == "zapornaya_armatura") {
                kl.label_color = 1;
                kl.codlabel_color = "stateID";
                kl.label_stateID = 1;
                kl.codlabel_stateID = "stateID";
            }

            if (kl.nazv == "zdaniya_2") {
                kl.label_color3 = 1;
                kl.codlabel_color3 = "otop";
            }


            Klassif *kls = m_kl_list.findKlN(kl.nazv);
            if (kls) {
// Тут добавлять если что
                kl.m_db = kls->m_db;
                if (kls->loc_from_baza) {
                    kl.loc_from_baza = kls->loc_from_baza;
                    kl.loc = kls->loc;
                    kl.codlabel_angle = kls->codlabel_angle;
                    kl.label_angle = kls->label_angle;
                }

                m_kl_list[kl.nazv.toLower()] = kl;
            }
        }
    }
    return true;
}

// Функция для преобразования HSV в RGB
unsigned int hsvToRgb(double h, double s, double v)
{
    double c = v * s;
    double x = c * (1 - std::fabs(fmod(h / 60.0, 2) - 1));
    double m = v - c;
    double r1, g1, b1;

    if (0 <= h && h < 60) {
        r1 = c; g1 = x; b1 = 0;
    } else if (60 <= h && h < 120) {
        r1 = x; g1 = c; b1 = 0;
    } else if (120 <= h && h < 180) {
        r1 = 0; g1 = c; b1 = x;
    } else if (180 <= h && h < 240) {
        r1 = 0; g1 = x; b1 = c;
    } else if (240 <= h && h < 300) {
        r1 = x; g1 = 0; b1 = c;
    } else {
        r1 = c; g1 = 0; b1 = x;
    }

    unsigned int r = static_cast<unsigned int>((r1 + m) * 255);
    unsigned int g = static_cast<unsigned int>((g1 + m) * 255);
    unsigned int b = static_cast<unsigned int>((b1 + m) * 255);

    return (r << 16) | (g << 8) | b;
}


#if 0
unsigned int rand_color(double brightness, double saturation) 
{
/*
    unsigned int remainingBrightness = brightness * 3;
    unsigned int red = rand() % (remainingBrightness / 3 + 1);
    remainingBrightness -= red;
    unsigned int green = rand() % (remainingBrightness / 2 + 1);
    remainingBrightness -= green;
    unsigned int blue = remainingBrightness;

    return (red << 16) | (green << 8) | blue;
*/

    unsigned int red, green, blue;

    double hue = static_cast<double>(rand() % 360);  // Генерация случайного оттенка (Hue) от 0 до 359

}
#endif

void init_lf(MyLOGFONT &lf)
{
    lf.lfHeight = -20; // Высота шрифта в логических единицах. Отрицательное значение для указания высоты в пикселях.
    lf.lfWidth = 0; // Автоматический выбор ширины
    lf.lfEscapement = 0; // Угол поворота текста
    lf.lfOrientation = 0; // Угол ориентации текста
    lf.lfWeight = FW_NORMAL; // Обычный шрифт
    lf.lfItalic = FALSE; // Обычный, не курсивный шрифт
    lf.lfUnderline = FALSE; // Без подчеркивания
    lf.lfStrikeOut = FALSE; // Без зачеркивания
    lf.lfCharSet = DEFAULT_CHARSET; // Набор символов по умолчанию
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS; // Точность вывода по умолчанию
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS; // Точность отсечения по умолчанию
    lf.lfQuality = DEFAULT_QUALITY; // Качество шрифта по умолчанию
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; // Шаг и семейство шрифта по умолчанию
//    lstrcpy(lf.lfFaceName, TEXT("Arial")); // Имя семейства шрифта            
    lf.lfFaceName = "Arial"; // Имя семейства шрифта            
}



bool GeoFile::open_geo(GidWidget *view, const QString & name, QSqlDatabase &db, QProgressDialog *percent)
{
    QString q = "SELECT s.name AS TABLE_SCHEMA, t.name AS TABLE_NAME, c.name AS COLUMN_NAME, '' as type, '' as srid, '' as angle, '' as fileID FROM sys.columns c JOIN sys.tables t ON t.object_id=c.object_id JOIN sys.schemas s ON s.schema_id=t.schema_id WHERE c.user_type_id=129";
/*
SELECT

c.*,

s.name AS TABLE_SCHEMA, t.name AS TABLE_NAME, c.name AS COLUMN_NAME, c2.name as angle
FROM sys.columns c 
JOIN sys.tables t ON t.object_id=c.object_id 
JOIN sys.schemas s ON s.schema_id=t.schema_id 
LEFT JOIN sys.columns c2 on c2.object_id=c.object_id and c2.name='id'

WHERE c.user_type_id=129;
*/




    if (is_POSTGRESQL()) {
        q = "select i.table_schema, i.table_name, i.column_name FROM information_schema.columns i WHERE i.udt_name = 'geometry'";
    q = 
    "select i.table_schema, i.table_name, i.column_name,f.type,f.srid \n"
    "FROM information_schema.columns i\n"
    "join geometry_columns AS f\n"
             "ON (i.table_schema = f.f_table_schema and i.table_name = f.f_table_name )\n"
    "WHERE i.udt_name = 'geometry'";

    q =
        "select i.table_schema, i.table_name, i.column_name,f1.type,f1.srid , f2.column_name as angle , f3.column_name as fileID \n"
        "FROM information_schema.columns i\n"
        "join geometry_columns AS f1 ON (i.table_schema = f1.f_table_schema and i.table_name = f1.f_table_name )\n"
        "left join information_schema.columns AS f2 ON (i.table_schema = f2.table_schema and i.table_name = f2.table_name and f2.column_name='angle')\n"
        "left join information_schema.columns AS f3 ON (i.table_schema = f3.table_schema and i.table_name = f3.table_name and f3.column_name='fileid')\n"
        "WHERE i.udt_name = 'geometry';\n";
    }

    if (is_SQLITE()) {
        q = 
"SELECT 'dbo', m.name as table_name, p.name as column_name\n"
"FROM sqlite_master m\n"
"left outer join pragma_table_info((m.name)) p\n"
"     on m.name <> p.name\n"
//"order by table_name, column_name\n"
;
    }

    Klassif *kls1;

    QSqlQuery query(db);

    query.setForwardOnly(true);
    query_exec(db, query, q, true);
    std::list<QString> list_tables;

    bool special = false;

    while (query.next()) {
        QString table_schema = query.value("table_schema").toString();
        QString table_name = query.value("table_name").toString();
        QString type = query.value("type").toString();
        QString srid = query.value("srid").toString();
        QString angle = query.value("angle").toString();
        QString fileID = query.value("fileID").toString();

        table_name = full_name(table_schema, table_name);

        QString ss = table_name.toLower();

        if (ss != "nodes" && ss != "linesobj") {
            bool yes = true;

            list_tables.push_back(table_name);

            Klassif kl;
            kl.nazv = table_name;
            kl.nazv_alias = table_name;
            kl.loc = 2;
            kl.typ = 1;
            kl.srid = srid;

            double hue = static_cast<double>(rand() % 360);  // Генерация случайного оттенка (Hue) от 0 до 359

            kl.color = hsvToRgb(hue, 0.99, 0.9);
            kl.coli = hsvToRgb(hue, 0.99, 0.9);
            kl.colb = hsvToRgb(hue, 0.2, 0.9);

            init_lf(kl.lf);

//            kl.coli = rand_color(255, 0.5);
//            kl.colb = rand_color(255, 0.5);

            if (fileID != "") {
                kl.label_fileID = 1;
                kl.codlabel_fileID = "fileID";
            }

            kl.loc_from_baza = true;
            
            if (type == "POINT") {
                kl.typ = 67;
                kl.loc = LOC1;
                if (angle == "angle") {
                    kl.loc = LOC1;
                    kl.label_angle = 1;
                    kl.codlabel_angle = "angle";
                }
            }
            else if (type == "LINESTRING" || type == "MULTILINESTRING") kl.loc = 2;
            else if (type == "POLYGON" || type == "MULTIPOLYGON") kl.loc = 3;
            else
                kl.loc_from_baza = false;

            kl.prio = 5;
            kl.m_db = &db;

            kl.baza = name;

            m_kl_list[table_name.toLower()] = kl;

            kls1 = m_kl_list.findKlN("building3");
            if (kls1)  {
                if (kls1->typ == 204) {
                    int ww;
                    ww = 1;
                }
            }
        }
    }



    QString klfn;

//    klfn = QString("%1kls/gid.kls2").arg(argpath(), name);
//    open_klassif(klfn, name);

    klfn = QString("%1/kls/OpenStreetMap.kls2").arg(argpath());
    open_klassif(klfn, name);


    klfn = QString("%1/kls/%2.kls2").arg(argpath(), name);
    open_klassif(klfn, name);

    klfn = QString("%1/%2.kls2").arg(argpath_2(), name);
    open_klassif(klfn, name);

    readMySql_Shape(db, percent);

    void initTableRusName(const QString & database);
    void initColumnRusName(const QString & database);
    void initLookup(const QString & database);

    initTableRusName(name);
    initColumnRusName(name);
    initLookup(name);

    kls1 = m_kl_list.findKlN("building3");
    if (kls1)  {
        int ww;
        ww = 1;
    }


    main_window->m_left_geo->init(view, name);
    main_window->m_addr->init(view, name);

    return true;
}

bool delObject(QSqlDatabase &db, const QString & tn, int id);

bool GeoFile::delGeoObj(const QString & tn, int id)
{
    bool ret = false;

    Klassif* kls = m_kl_list.findKlN(tn);
    if (kls) {
        int n = kls->GetNom2(id);
        if (n > 0) {
            CGeoObject* geo = kls->getGeoObjectById(id);
            if (geo) {
                if (delObject(*kls->m_db, tn, id)) {
                    delete geo;
                }
                kls->geo4.erase(kls->geo4.begin() + (n - 1));
            }

            ret = true;
        }
        else {
/*
            if (tn == EL_LINE) {
                AfxMessageBox("Невозможно удалить линию электропередач, пока существуют привязанные к ней объекты", MB_OK|MB_ICONINFORMATION);
            }
            else if (tn == EL_PRIEM) {
                if (kls->m_ado->getError().Find(EL_LINE) != -1) {
                    AfxMessageBox("Невозможно удалить приемник электроснабжения, пока существует привязанная к нему линия электропередач", MB_OK|MB_ICONINFORMATION);
                }
                else {
                    AfxMessageBox("Невозможно удалить приемник электроснабжения, пока существуют привязанные к нему объекты", MB_OK|MB_ICONINFORMATION);
                }
            }
            else if (tn == EL_IST) {
                if (kls->m_ado->getError().Find(EL_LINE) != -1) {
                    AfxMessageBox("Невозможно удалить источник электроснабжения, пока существует привязанная к нему линия электропередач", MB_OK|MB_ICONINFORMATION);
                }
                else {
                    AfxMessageBox("Невозможно удалить приемник электроснабжения, пока существуют привязанные к нему объекты", MB_OK|MB_ICONINFORMATION);
                }
            }
            else if (tn == "indikator_korrozii") {
                if (kls->m_ado->getError().Find("indikator_korrozii_po_godam") != -1) {
                    AfxMessageBox("Невозможно удалить Индикатор коррозии, пока существуют записи в Журнале истории", MB_OK|MB_ICONINFORMATION);
                }
            }
            else {
                AfxMessageBox(kls->m_ado->getError(), MB_OK|MB_ICONINFORMATION);
            }
*/
        }
    }

    return ret;
}

bool isPoint(int loc);

bool isElPoint(const QString & tn)
{
    return false;
}

bool isElEnd(const QString & tn)
{
    return false;
}

#if 1
void get_dop_s(const std::map<QString, QVariant>& m_map_geo_value, QString& dop_p1, QString& dop_p2, const QString & codlabel, QString& label)
{
    dop_p1 = "";
    dop_p2 = "";

    for (auto it : m_map_geo_value) {
        dop_p1 += ",";
        dop_p2 += ",";
        dop_p1 += it.first;
        dop_p2 += "'";

//        QString s = CCrack::strVARIANT2(it.second);
        QString s = it.second.toString();

        dop_p2 += s;
        dop_p2 += "'";

        if (codlabel == it.first) {
            label = it.second.toString();
        }
    }
}

#endif

long color_open_close(int stateID)
{
    if (stateID == 1) { // Открыта
        return 0x000000; // BLACK
    }
    if (stateID == 4) { // Частично открыта
        return 0x0000FF; // BLUE
    }
//    return LIGHTRED;  // Закрыта
    return 0xFF7F7F; // LIGHTRED
}

int get_colorID2();
int get_colorID1();

int GeoFile::createObj2(GidWidget* view, const QString & tn, CCoordList& cl, std::map<QString, QVariant>& m_map_geo_value1)
{
    QString label = "";
    QString dop_p1 = "", dop_p2 = "";
    int id_opr = 0;
    QString q, q2;
    CNode2* node = NULL;
    CLINE2* line = NULL;
    q2 = "";

    int po = 0;

    std::map<QString, QVariant> m_map_geo_value = m_map_geo_value1;

    Klassif* kls = m_kl_list.findKlN(tn);
//    if (kls && kls->m_ado) {
    if (kls) {

        if (isPoint(kls->loc) && !isElPoint(tn) && !isElEnd(tn)) {
            CFPoint pt = cl.first();

//            id_opr = view->m_id_opr;
//            view->findNodeLine(pt, &node, &line, D5);

            node = view->getCxema()->m_graph->find( 0, pt, D5);
            line = view->getCxema()->m_graph->findLine( 0, pt, D5);

            if (line) {
                if (!column_in_table(*kls->m_db, kls->nazv, "lineID")) {
                    line = NULL;
                }
            }
            if (node) {
                if (!column_in_table(*kls->m_db, kls->nazv, "nodeID")) {
                    node = NULL;
                }
            }
        }

#if 0
        if (tn == "defect") {
            if (view->m_id_opr > 0) {
                m_map_geo_value["opresID"] = (long)view->m_id_opr;
                m_map_geo_value["remontTypeID"] = 2L;
            }
            else if (view->m_id_osmotr2 > 0) {
                m_map_geo_value["osmotrID"] = (long)view->m_id_osmotr2;
                m_map_geo_value["remontTypeID"] = 6L;
            }
            else {
                auto it = m_map_geo_value.find("remontTypeID");
                if (it == m_map_geo_value.end()) {
                    m_map_geo_value["remontTypeID"] = 1L;
                }
            }
            m_map_geo_value["stateID"] = 1L;
        }
#endif

//        QString str = cl.getWKT(kls->loc, 0);
//        if (node) {
//            q = QString("INSERT INTO %1 (nodeID, shape) VALUES (%2, %3)").arg(tn).arg(node->id).arg(str);
//        }
#if 0
        else if (line && (kls->m_ado->findMySQLColumns(tn, "priznak_truboprovoda") || strcmp("indikator_korrozii", tn) == 0)) {
            int nomP = bline(line)->line.nomP;
            int nomO = bline(line)->line.nomO;

            CNode2* n1 = where(line);
            CNode2* n2 = other(line);

            double dl1 = 0, dl2 = 0;

            CNode2* nn1 = n1->isKam() ? n1 : find_next11(n1->node.m_graph, n1, n2, n1, dl1);
            CNode2* nn2 = n2->isKam() ? n2 : find_next11(n2->node.m_graph, n2, n1, n2, dl2);

            CFPoint p = cl.first();

            CString kod0 = "", uzel0 = "";
            double dl1_w = rasst(line, p);
            double dl2_w = getLengthLine(line, false) - dl1_w;

            dl1 += dl1_w;
            dl2 += dl2_w;

            double dl;

            if (dl1 < dl2) {
                dl = dl1;
                if (nn1) {
                    kod0 = nn1->getKod();
                    uzel0 = nn1->node.name;
                }
            }
            else {
                dl = dl2;
                if (nn2) {
                    kod0 = nn2->getKod();
                    uzel0 = nn2->node.name;
                }
            }

            if (strcmp("indikator_korrozii", tn) == 0) {
                m_map_geo_value["kod_rs_blizhayshey_kamery"] = kod0;
                m_map_geo_value["blizhayshaya_kamera"] = uzel0;

                CString dls;
                dls.Format("%g", dl);

                m_map_geo_value["rasstoyanie_do_kamery__m"] = dls;
            }

            get_dop_s(m_map_geo_value, dop_p1, dop_p2, kls->codlabel, label);


            if ((nomP != nomO && nomP != -1 && nomO != -1) || (nomP == nomO)) {
                CMMenu menu(AfxGetMainWnd(), IDD_MENU);
                menu.lst.Add(_TR("Подающий"));
                menu.lst.Add(_TR("Обратный"));
                if ((nomP == nomO) && strcmp("indikator_korrozii", tn) != 0) {
                    menu.lst.Add(_TR("Общий"));
                    menu.lst.Add(_TR("Подающий + Обратный"));
                }

                CString priznakTruboprovoda = "priznak_truboprovoda";
                if (strcmp("indikator_korrozii", tn) == 0)
                    priznakTruboprovoda = "truboprovod";
                if (menu.DoModal() == IDOK) {

                    switch (menu.m_Index) {
                    case 0: q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomP, str, 2, dop_p2);
                        po = 2;
                        break;
                    case 1: q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomO, str, 3, dop_p2);
                        po = 3;
                        break;
                    case 2: q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomO, str, 1, dop_p2);
                        po = 1;
                        break;
                    case 3: q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomP, str, 2, dop_p2);
                        po = 0;
                        q2.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomO, str, 3, dop_p2);
                        break;
                    }
                }
                else {
                    return false;
                }
            }
            else
            {
                int id = bline(line)->line.nomP > 0 ? bline(line)->line.nomP : bline(line)->line.nomO;
                int type = bline(line)->line.nomP > 0 ? 2 : 3;

                CString priznakTruboprovoda = "priznak_truboprovoda";
                if (strcmp("indikator_korrozii", tn) == 0)
                    priznakTruboprovoda = "truboprovod";

                q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, id, str, type, dop_p2);
            }

            /*else if (line) {
                int id = bline(line)->line.nomP > 0 ? bline(line)->line.nomP : bline(line)->line.nomO;
                q.Format("INSERT INTO %s (lineID, shape) VALUES (%d, %s)", tn, id, str);
            }*/
        }
#endif
        else {
//            get_dop_s(m_map_geo_value, dop_p1, dop_p2, kls->codlabel, label);
//            q = QString("INSERT INTO %1 (shape%2) VALUES (%3%4)").arg(tn, dop_p1, str, dop_p2);
        }

/*
        auto executeQuery = [&](QString q, const QString & tn)-> int
        {
            long idv = ado_executeInsert(kls->m_ado, q, &affected);
            if (idv) {
                if (kls->codlabel_color != "") {
                    cl.is_color = true;
                    cl.color = getGeoColor(1);
                    if (tn == "indikator_korrozii") {
                        cl.color = getGeoColor(4);
                    }
                    cl.po = po;
                }

                if (kls->codlabel_color2 != "") {
                    cl.is_color2 = true;
                    cl.color2 = getGeoColor(1);
                    cl.po = po;

                    int get_colorID2();
                    int get_colorID1();

                    cl.color = getGeoColor(get_colorID1());
                    cl.color2 = getGeoColor(get_colorID2());
                }

                if (kls->label_stateID) {
                    QString val;
                    cl.stateID = 1;
                    cl.color = color_open_close(cl.stateID);
                }

                CGeoObject* g = new CGeoObject(idv, cl);
                if (!g) return 0;
                g->text00000 = label;

                kls->geo4.push_back(g);

                return kls->geo4.size();
            }
            else
                return 0;
        };

        int size = executeQuery(q, tn);
        if (q2 != "") {
            executeQuery(q2, tn);
        }
        return size;
*/

        QString wkt = cl.getWKT(kls->loc, 0);

        m_map_geo_value[kls->shape] = wkt;

        int idv = insertIntoDatabase(*kls->m_db, kls->nazv, m_map_geo_value, kls->shape);

        if (idv >= 0) {
            get_dop_s(m_map_geo_value, dop_p1, dop_p2, kls->codlabel, label);

            if (kls->codlabel_color != "") {
                cl.is_color = true;
                cl.color = getGeoColor(1);
                if (tn == "indikator_korrozii") {
                    cl.color = getGeoColor(4);
                }
                cl.po = po;
            }

            if (kls->codlabel_color2 != "") {
                cl.is_color2 = true;
                cl.color2 = getGeoColor(1);
                cl.po = po;

//                cl.color = getGeoColor(get_colorID1());
//                cl.color2 = getGeoColor(get_colorID2());
            }

            if (kls->label_stateID) {
                QString val;
                cl.stateID = 1;
                cl.color = color_open_close(cl.stateID);
            }

            CGeoObject* g = new CGeoObject(idv, cl);
            if (!g) return 0;
            g->text00000 = label;

            kls->geo4.push_back(g);

            return (int) kls->geo4.size();
        }
    }
    return 0;
}


bool GeoFile::IsMark(const Klassif* kls, int n)
{
    return kls->geo4[n - 1]->mark;
}


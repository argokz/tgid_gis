#include <QApplication>
#include <QtGui>
#include <QInputDialog>

#include <mainwindow.h>
#include <gidview/GidWidget.h>

#include "TextFilterDialog.h"

#include "db/db.h"
#include <geo/Klassif.h>

static QString TechUsl = "Технические условия";


QString transl(const QString & s);


int decode_dom2(const char *s)
{
    for (int i = 0; s[i]; i++) {
        if ('0' <= s[i] && s[i] <= '9') {
            return atoi(&s[i]);
        }
    }
    return 0;
}

struct less_dom 
{
    using result_type = bool;
    using first_argument_type = const QString&;
    using second_argument_type = const QString&;
    result_type operator() (const QString arg1, const QString & arg2) const
    {
        int n1 = decode_dom2(arg1.toStdString().c_str());
        int n2 = decode_dom2(arg2.toStdString().c_str());

        if (n1 < n2) return true;
        if (n1 > n2) return false;

        return arg1 < arg2;
    }
};


static std::map<QString, int, less_dom> map_dom;
static std::map<QString, QString> map_str1;
static std::map<QString, int> map_nazv;
static set<QString> set_str;

QString cyr(const QString& input);


bool inStr(const QString &s1, std::set<QString> &setp)
{
    QString s = s1.toLower().trimmed();

    for (const QString &p1 : setp) {
        QString p = p1.toLower().trimmed();
        QString pp = cyr(p);

        if (s.indexOf(p) < 0 && s.indexOf(pp) < 0) {
            return false;
        }
    }
    return true;
}


void initList(std::set<QString> &setp, QListWidget *listWidget)
{
    {

        for (const auto& [key, value] : map_nazv) {
            QString s = key;
            if (inStr(s, setp)) {
                QListWidgetItem *item = new QListWidgetItem(s, listWidget);
                item->setData(Qt::UserRole, value);
                listWidget->addItem(item);

                if (listWidget->count() > 200) break;
            }
        }
    }

    if (listWidget->count() == 0 && setp.size() == 1) {

        QString s, line = *setp.begin();
        line = line.trimmed();

        std::set<QString> setp2;
        int pos = 0;

        while (pos != -1) {
            pos = line.indexOf(" ");
            if (pos != -1) {
                setp2.insert(line.left(pos));
                line = line.mid(pos + 1);
            }
        }
        setp2.insert(line);

        if (setp2.size() > 1) initList(setp2, listWidget);
    }
}

QString m_street;


void drawDom(QListWidget *listWidget, const QString &street)
{
    listWidget->clear();

    Klassif *kls = m_kl_list.findKlN(transl("Здания ТУ"));
    if (!kls) {
        return;
    }


//    CAdoFile *ado = kls->m_ado;

    QString q, street2;

    std::map<QString, QString>::const_iterator it = map_str1.find(street);
    if (it != map_str1.end()) {
        street2 = it->second;
    }

    map_dom.clear();

    q = QString("select distinct %1, ID from %2 WHERE %3='%4'")
        .arg(transl("Дом"), transl("Здания ТУ"), transl("Улица"), street2);

    QSqlQuery query(*kls->m_db);

    if (query_exec(*kls->m_db, query, q)) {
        while (query.next()) {

            QString s1 = query.value(0).toString();
            int id = query.value(1).toInt();

            QString s = QString("%1 %2").arg(street, s1);

            map_dom[s] = id;
        }
    }

    {
        for (const auto& [key, value] : map_dom) {
//            listWidget->addItem(key);

            QListWidgetItem *item = new QListWidgetItem(key, listWidget);
            item->setData(Qt::UserRole, value);
            listWidget->addItem(item);
            if (listWidget->count() > 200) break;
        }
    }
}


//bool tu_find(const QString &p)
//{
//}



bool init_find(const QString &p1, QListWidget *listWidget)
{
    if (p1 == "") return false;

    QString p = p1.toLower();

    QString pp = cyr(p);

    for (const auto& key : set_str) {
        QString s = key.toLower();

        if (s.indexOf(p) == 0 || s.indexOf(pp) == 0) {
            listWidget->addItem(key);
            if (listWidget->count() > 200) break;
        }
        if (s == p) {
            m_street = key;
            drawDom(listWidget, m_street);
            return false;
        }
    }

    std::set<QString> setp;
    setp.insert(p1);

    initList(setp, listWidget);

    return true;
}


bool init_tu_find(QSqlDatabase &db)
{
    Klassif *kls = m_kl_list.findKlN(transl("Здания ТУ"));
    if (!kls) {
        return true;
    }

    map_str1.clear();
    map_nazv.clear();


    QSqlQuery query(db);

    QString q = QString("select distinct name, name_2 from %1").arg(transl("Улицы"));

    if (query_exec(db, query, q)) {
        while (query.next()) {
            QString s1 = query.value(0).toString();
            QString s2 = query.value(1).toString();

            map_str1[s1] = s2;
            set_str.insert(s1);
        }
    }

    QString col1 = transl("Здание");
    QString col2 = transl("Наименование организации, запрашивающей ТУ");
    QString col3 = transl("Наименование объекта");
    QString col4 = transl("Номер ТУ");


//    q = QString("select %2, %3, %4, %5 from %1 where naimenovanie_obekta ilike '%парус%'")
    q = QString("select %2, %3, %4, %5 from %1")
        .arg(transl(TechUsl))
        .arg(col1, col2, col3, col4)
        ;

    if (query_exec(db, query, q)) {
        while (query.next()) {
            //        long id = ado->read_long("ID");
            int id = query.value(col1).toInt();
            QString s1 = query.value(col2).toString();
            QString s2 = query.value(col3).toString();
            QString s3 = query.value(col4).toString();

            s1 += " организация";
            s2 += " объект";

            //        if (id != 0) {
            if (s1 != "") map_nazv[s1] = id;
            if (s2 != "") map_nazv[s2] = id;
            if (s3 != "") map_nazv[s3] = id;
            //        }

        }
    }

//    m_ctrl_text.SetFocus();

    return false;
}



void GidWidget::onTuFind() // ТУ, Договора
{
    init_tu_find(m_cxema.m_db);

    TextFilterDialog dialog;

    if (dialog.exec() == QDialog::Accepted) {
        int id = dialog.getResult();
        if (id > 0) {
            moveGeo(transl("Здания ТУ"), id);
        }
    }
}




#include "tu/TuQDialog.h"

#include "tu_p.h"

//void view_db2(DbWindow *view, const QString & title, QWidget *parent);
#include <table/MultiHeaderTable.h>

void view_my_table(MultiHeaderTable *view, const QString & title, QWidget *parent);
MultiHeaderTable* getMyTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title);


void GidWidget::onTuSost() // ТУ состояние
{
    TuQDialog dlg;

    dlg.addItemToList(3, 0, "Действующие",    true);
    dlg.addItemToList(3, 1, "Завершенные",    true);
    dlg.addItemToList(3, 2, "Аннулированные", true);

//    QString q = QString("SELECT DISTINCT YEAR(%1) FROM %2 ORDER BY YEAR(%1)").arg(transl("Дата выдачи ТУ"), transl(TechUsl));


    QSqlQuery query(m_cxema.m_db);


    QString q = QString("SELECT DISTINCT DATE_PART('year', %1)::int FROM %2 ORDER BY DATE_PART('year', %1)::int NULLS FIRST").arg(transl("Дата выдачи ТУ"), transl(TechUsl));

    if (query_exec(m_cxema.m_db, query, q)) {
        while (query.next()) {
            int y = query.value(0).toInt();
            QString s = QString("%1").arg(y);
            if (y == 0) {
                s = "Не заданный";
            }
            dlg.addItemToList(2, y, s, true);
        }
    }


    q = QString("SELECT DISTINCT CASE WHEN %1 IS NULL OR %1 = '' THEN '' ELSE %1 end as %1 FROM %2 ORDER BY %1 NULLS FIRST").arg(transl("Район эксплуатации"), transl(TechUsl));

    int yy;
    int y = 1;

    if (query_exec(m_cxema.m_db, query, q)) {
        while (query.next()) {
            QString s = query.value(0).toString();
            yy = y;

            if (s == "") {
                s = "Не заданный";
                yy = 0;
            }

            dlg.addItemToList(1, yy, s, true);
            y += 1;
            yy = y;

        }
    }

    q = QString("SELECT DISTINCT CASE WHEN %1 IS NULL OR %1 = '' THEN '' ELSE %1 end as %1 FROM %2 ORDER BY %1 NULLS FIRST").arg(transl("Источник"), transl(TechUsl));

    y = 1;

    if (query_exec(m_cxema.m_db, query, q)) {
        while (query.next()) {
            QString s = query.value(0).toString();

            if (s == "") {
                s = "Не заданный";
                yy = 0;
            }

            dlg.addItemToList(0, yy, s, true);
            y += 1;
            yy = y;
        }
    }
    
    if (dlg.exec() == QDialog::Accepted) {
        QString q = QString("SELECT %1\nFROM %2 t\nLEFT JOIN stateTu st ON st.id=t.sostoyanie_dogovora\nWHERE %3").arg(ppp, transl(TechUsl), dlg.getQ());
        QString qs = QString("SELECT %1\nFROM %2\nWHERE %3").arg(ppp2, transl(TechUsl), dlg.getQ());

        QString qq = QString("\n%1\nUNION ALL\n%2\n").arg(q, qs);

        QString tn = transl(TechUsl);


        MultiHeaderTable* table = getMyTableView(getCxema()->m_db, tn, qq, TechUsl);
        if (table) {
            table->setGidWidget(this);
            table->setDop("1");
            table->setEdit(false);
            table->checkCustomHeader();
            view_my_table(table, TechUsl, this);
        }
        else {
            QMessageBox::warning(nullptr, "", QString("Нет данных в таблице %1").arg(tn));
        }



        //        RunTableTU(kls->m_ado->m_schema, transl(TechUsl), qq, 0);
//        RunTableTU(kls->m_ado->m_schema, TechUsl, qq, 0);

    }
}

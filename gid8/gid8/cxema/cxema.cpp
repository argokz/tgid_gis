#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QFile>
#include <QMessageBox>

#include "mainwindow.h"

#include <mystd.h>

#include <iostream>
#include "cxema/Graph.h"
#include <db/db.h>

void setCurrentExternalCode(int fileID, int kod);

bool readTableMap(QSqlDatabase &db, const QString & q, const QString & name_id, const QString & name_name, std::map<int, QString>& map1)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int id = query.value(name_id).toInt();
            QString name = query.value(name_name).toString();
            map1[id] = name;
        }
        return true;
    }
    return false;
}

bool read_q(QSqlDatabase &db)
{
    QFile file(argpath()+QString("sql3/us.sql"));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

    QString q = file.readAll();

    q.replace(QString("$user_gid$"), QString("Администратор"));

//    std::cout << q.toStdString() << std::endl;

    QSqlQuery query(db);
    query.setForwardOnly(true);

    long nn = sizeOfQ(db, q);
    std::cout << "nn = " << nn << "                           \n";

//    return false;

    query_exec(db, query, q);

    int i = 0;

    while (query.next()) {
          QString str = query.value(0).toString();
//          doSomething(country);

          //           std::cout << str.toStdString() << std::endl;

          if (i %100 == 0) {
           std::cout << i*100./nn  << "%            " << str.toStdString() << "                           \r";
          }
          i++;
      }
    return true;
}

#include <db/db.h>
#include <cxema/cxema1.h>


bool CCxema::connectSQL(int rdbms, const QString & host, int port, const QString & baza, const QString & user, const QString & password)
{
    m_connect.rdbms    =  rdbms;      
    m_connect.host     =  host;      
    m_connect.baza     =  baza;      
    m_connect.user     =  user;      
    m_connect.password =  password;  
    m_connect.port     =  port;

    return connectSQL0(rdbms, host, port, baza, user, password, m_db);
}

bool CCxema::connectSQL2(int rdbms, const QString & host, int port, const QString & baza, const QString & user, const QString & password)
{
    m_connect.rdbms    =  rdbms;      
    m_connect.host     =  host;      
    m_connect.baza     =  baza;      
    m_connect.user     =  user;      
    m_connect.password =  password;  
    m_connect.port     =  port;

    return connectSQL0(rdbms, host, port, baza, user, password, m_db_2);
}



bool CCxema::connectSqlite(const QString & baza)
{
    return connectSqlite0(baza, m_db);
}



bool CCxema::openZN()
{
    QString q = "SELECT id, nodeID, pressFlow, pressRet, fragment_resultID FROM setPressNodes";

    QSqlQuery query(m_db);
    query.setForwardOnly(true);

    if (query_exec(m_db, query, q)) {
        while (query.next()) {

            int id = query.value("id").toInt();
            int nodeID = query.value("nodeID").toInt();


//            double h_p = query.value("pressFlow").toDouble();
//            double h_o = query.value("pressRet").toDouble();

            int fileID2 = query.value("fragment_resultID").toInt();


            CNode2* n = m_graph->find(nodeID);

            if (n) {
                n->node.nZN = id;
                if (fileID2 > 0) {
                    m_map_fileID_mag[n->node.fileID] = fileID2;
                }
            }
        }
    }

    return true;
}

bool CCxema::openUP()
{
    QString q = "SELECT id, nodeID FROM refillNodes";

    QSqlQuery query(m_db);
    query.setForwardOnly(true);

    if (query_exec(m_db, query, q)) {
        while (query.next()) {

            int id = query.value("id").toInt();
            int nodeID = query.value("nodeID").toInt();


//            double h_p = query.value("pressFlow").toDouble();
//            double h_o = query.value("pressRet").toDouble();

//            int fileID2 = query.value("fragment_resultID").toInt();


            CNode2* n = m_graph->find(nodeID);

            if (n) {
                n->node.nUP = id;
//                if (fileID2 > 0) {
//                    m_map_fileID_mag[n->node.fileID] = fileID2;
//                }
            }
        }
    }

    return true;
}


bool CCxema::open_heating_seasons()
{
    QString q_ps =
        "SELECT \n"
        "date_part('year',  hs1.date2) as y,\n"

        "hs1.date1 AS d0,\n"
        "hs1.date2 as d1,\n"

        "case \n"
        "    when hs2.date2 is null \n"
        "    then hs1.date2 - interval '1 year' \n"
        "    else hs2.date2 \n"
        "end as d2\n"

        "FROM heating_seasons hs1\n"
        "LEFT JOIN heating_seasons hs2 ON date_part('year',  hs2.date1)  = date_part('year',  hs1.date1)-1\n";



    QString q_ms = 
        "SELECT \n"
        "year(hs1.date2) as y,\n"
        "hs1.date1 AS d0,\n"
        "hs1.date2 as d1,\n"
        "case \n"
        "    when hs2.date2 is null \n"
        "    then DATEADD(year, -1, hs1.date2) \n"
        "    else hs2.date2 \n"
        "end as d2\n"
        "FROM heating_seasons hs1\n"
        "LEFT JOIN heating_seasons hs2 ON year(hs2.date1)  = year(hs1.date1)-1\n";


    QSqlQuery query(m_db);
    query.setForwardOnly(true);

    QString q = q_ms;

    if (is_POSTGRESQL()) {
        q = q_ps;
    }
    else {
        q = q_ms;
    }

    Heating_seasons hs;

    if (query_exec(m_db, query, q)) {
        while (query.next()) {
            hs.y = query.value("y").toInt();
            hs.d0 = query.value("d0").toDate();
            hs.d1 = query.value("d1").toDate();
            hs.d2 = query.value("d2").toDate();
            map_Heating_seasons[hs.y] = hs;
        }
    }
    return true;
}

Heating_seasons CCxema::get_Heating_seasons(int y)
{
    Heating_seasons h;
    h.y = -1;

    auto it = map_Heating_seasons.find(y);
    if (it != map_Heating_seasons.end()) {
        return it->second;
    }
    return h;
}



bool CCxema::openFragments(const QString & par)
{
    QSqlQuery query(m_db);
    query.setForwardOnly(true);

    QString q = QString("Select * FROM fragments WHERE removed = 0 AND id IN (%1)").arg(par) ;

    if (query_exec(m_db, query, q)) {
        while (query.next()) {
            Fragment fr;

            fr.m_id = query.value("id").toInt();
            fr.m_name = query.value("name").toString();
            fr.m_name_sys = query.value("name_sys").toString();
            fr.m_nasel_point = query.value("nasel_point").toString();
            fr.m_sezon = query.value("sezon").toString();
            fr.m_year = query.value("year").toString();
            fr.m_t_or = query.value("t_or").toDouble();
            fr.m_t_vr = query.value("t_vr").toDouble();
            fr.m_t_vnew = query.value("t_vnew").toDouble();
            fr.m_tx = query.value("tx").toDouble();
            fr.m_tx_leto = query.value("tx_leto").toDouble();
            fr.m_tip_Qgvs = query.value("tip_Qgvs").toString();
            //  `begin_year` DAT`E DEFAULT NULL COMMENT 'Дата начала отопительного периода',
            //  `end_year` DATE DEFAULT NULL COMMENT 'Дата окончания отопительного периода',

            fr.m_tn_god = query.value("tn_god").toDouble();
            fr.m_tg_god = query.value("tg_god").toDouble();
            fr.m_tn_god_leto = query.value("tn_god_leto").toDouble();
            fr.m_tn_god_leto = query.value("tn_god_leto").toDouble();
            fr.m_tg_god_leto = query.value("tg_god_leto").toDouble();


            fr.m_a = query.value("a").toDouble();
            //  `year_audit` DATE DEFAULT NULL COMMENT 'Отопительный сезон аудитной проверки',
            fr.m_tvn_1 = query.value("tvn_1").toDouble();
            fr.m_tvn_2 = query.value("tvn_2").toDouble();
            fr.m_tvn_3 = query.value("tvn_3").toDouble();
            fr.m_tvn_4 = query.value("tvn_4").toDouble();
            fr.m_tvn_5 = query.value("tvn_5").toDouble();

            fr.m_phone_manager = query.value("phone_manager").toString();

            if (fr.m_name.indexOf("Магистрал") == 0) {
        //        m_AstanaMag = fr.m_id;
        //        m_AstanaMagSet.insert(fr.m_id);
            }
            
            map_fragments[fr.m_id] = fr;
        }
    }
    return true;
}

#include <gid_data/heatSystem.h>

void set_system_coord(double _False_Easting, double _False_Northing, double _Central_Meridian, double _Latitude_Of_Origin, double _Scale_Factor, double _Angle, double _dx, double _dy);


bool CCxema::openheatSystem()
{
    C_heatSystem d;

    if (!getCT(m_db, d)) return false;

    m_city = d.nasel_point;

//    QString name = d.nasel_point;
    if (d.False_Easting == 0) d.False_Easting = d.Central_Meridian * 10000;

    set_system_coord(d.False_Easting, d.False_Northing, d.Central_Meridian, d.Latitude_Of_Origin, d.Scale_Factor, d.Angle, d.dx, d.dy);
 // m_Tn_ot = d.t_or;
 // m_city = d.name;

    return true;
}

bool CCxema::openIst()
{
    QSqlQuery query(m_db);
    query.setForwardOnly(true);

    QString q = "Select * FROM externalCodes WHERE removed = 0";

//    bool first = true;
    if (query_exec(m_db, query, q)) {
        while (query.next()) {
            int ist_id = query.value("heatSourceID").toInt();
            int id = query.value("id").toInt();
            map_ist[id] = ist_id;

            RasCx cx;

            cx.id = query.value("id").toInt();
            cx.name = query.value("name").toString();

            cx.belongMagistral = query.value("belongMagistral").toInt(); // Принадлежность магистрали
            cx.objectID = query.value("objectID").toInt(); // Объект РС, ссылка на Вспомогательную таблицу B10_Виды объектов расчетной схемы',
            cx.heatSourceID = query.value("heatSourceID").toInt(); // Код источника тепла , ссылка на Источник

            cx.fileID = query.value("fileID").toInt();

            map_rs[id] = cx;

            //            if (first) {
            setCurrentExternalCode(cx.fileID, cx.id);
//            first = false;
        }
    }
    return true;
}

bool CCxema::read_text(const QString &par, QProgressDialog *percent)
{
    QSqlQuery query(m_db);
    query.setForwardOnly(true);



    QString q = QString("Select * FROM texts WHERE removed = 0 and fileID in (%1)").arg(par);

    set_text.clear();

    if (query_exec(m_db, query, q)) {
        while (query.next()) {
            CText t;

            t.n = query.value("id").toInt();
            t.fileID = query.value("fileID").toInt();
            t.internalNodeID = query.value("internalNodeID").toInt();
            t.text = query.value("label_text").toString();
            t.coord.x = query.value("x").toDouble();
            t.coord.y = query.value("y").toDouble();

            set_text.push_back(t);
        }
    }
    return true;
}


RasCx *CCxema::getRasCx(int id)
{
    auto it = map_rs.find(id);
    if (it != map_rs.end()) {
        return &it->second;
    }
    return nullptr;
}

void reset_max_changed_object(QSqlDatabase &db);


bool CCxema::open_cxema_2(const QString & par, QProgressDialog *percent)
{
    m_graph->remove_fragments(par.toInt());

//    readTableMap(m_db, "SELECT id,name FROM externalCodes WHERE removed = 0", "id", "name", m_graph->map_kod);

    read_nodes(par, percent, true);
    read_lines(par, percent, true);

    openZN();
    openUP();

    reset_max_changed_object(m_db);


    return true;
}


bool CCxema::open_cxema(GidWidget *view, const QString & par, QProgressDialog *percent)
{
    void initDefault();
    initDefault();

    open_heating_seasons();

    openheatSystem();

    openFragments(par);
    openIst();

    readTableMap(m_db, "SELECT id,name FROM externalCodes WHERE removed = 0", "id", "name", m_graph->map_kod);

///    std::cout << "nodes!" << std::endl;
    read_nodes(par, percent, true);
//    std::cout << "lines!" << std::endl;
    read_lines(par, percent, false);
//    std::cout << "Ok!" << std::endl;

    read_text(par, nullptr);


    if (!openZN()) return false;
    if (!openUP()) return false;
//    if (!openUP(ado, "refillNodes")) return false;
//    if (!openVP(ado, "WDOdevices")) return false;

    main_window->m_left_gid->init(view, this);

    m_rect = graph()->getBorder(0);

    m_par = par;

    reset_max_changed_object(m_db);

    return true;
}

bool CCxema::print()
{
    for(auto p : m_graph->map_node) {
        CNode2 *n = p.second;
        if (n->node.internalNodeID == 0) {
//            std::cout << n->getName().toStdString() << " ";
        }
    }

    return true;
}


double getLengtn(CLINE2 *ll, bool ismap)
{
    double l1, l2, len;
    CLine2 *l = bline(ll);
    CNode2 *n1 = where(ll);
    CNode2 *n2 = other(ll);

    if (l->line.m_NP.size() == 0) {
        len = LengthG(n1->node.coord, n2->node.coord, ismap);
    }
    else {
        CFPoint c1 = l->line.m_NP.first();
        CFPoint c2 = l->line.m_NP.last();

        len = l->line.m_NP.Length();
        l1 = LengthG(n1->node.coord, c1, ismap);
        l2 = LengthG(n2->node.coord, c2, ismap);
        len = len+l1+l2;
    }

    return len;
}



bool CCxema::setLength(CLINE2 *ll, bool ismap, bool ask)
{
    CLine2 *l = bline(ll);
    CNode2 *n1 = where(ll);
    CNode2 *n2 = other(ll);

    if (l->line.typ != TIP_UT) return true;

    double l1, l2, len;

    if (l->line.m_NP.size() == 0) {
        len = LengthG(n1->node.coord, n2->node.coord, ismap);
    }
    else {
        CFPoint c1 = l->line.m_NP.first();
        CFPoint c2 = l->line.m_NP.last();

        len = l->line.m_NP.Length();
        l1 = LengthG(n1->node.coord, c1, ismap);
        l2 = LengthG(n2->node.coord, c2, ismap);
        len = len+l1+l2;
    }

    QString q = QString("UPDATE %1 SET pipeSectLength=%2 WHERE id=%3 OR id=%4")
            .arg(l->getTableMySQL())
            .arg(len, 0, 'f', 2)
            .arg(l->line.idP2).arg(l->line.idO2);

    if (execQ(m_db, q)) {
        l->line.pod.dl = l->line.obr.dl = len;

        if (ask) {
            QMessageBox::warning(0, "", "Выполнено!");
        }
    }

    return true;
}

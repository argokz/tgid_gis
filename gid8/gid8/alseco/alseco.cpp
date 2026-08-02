#include <QtGui>

#include <QtSql>
#include <QtWidgets>
#include <QVariant>
//#include <QtPrinter>
#include <cmath>
#include <QRegularExpression>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QMessageBox>

#include <QInputDialog>


#include <db/db.h>
#include "std.h"

#include <property/PropertyDial.h>


#include "mainwindow.h"
#include <gidview/GidWidget.h>



int sgn(double d)
{
    return d == 0 ? 0 : 1;
}

int sgn(int d)
{
    return d == 0 ? 0 : 1;
}


#include "pr.h"

//static map <QString, Nomer> map_idrp;


void read_nagr(int id, double &otop, double &vent, double &gvs)
{
    otop = vent = gvs = 0.;

    Klassif* kls = m_kl_list.findKlN("zdaniya_tu");
    if (!kls) return;

    QString q = QString(
      "SELECT \n"
      "sum(tu.v_tom_chisle_otoplenie) AS otop,\n"
      "sum(tu.v_tom_chisle_ventilyatsiya) AS vent,\n"
      "sum(tu.v_tom_chisle_gvs_maks) AS gvs\n"
      "FROM tehnicheskie_usloviya tu WHERE tu.zdanie=%1 AND tu.sostoyanie_dogovora=4\n").arg(id);

    std::vector<QVariant> vv;

    bool ret = readTableValues(*kls->m_db, q, 3, vv);

    if (ret) {
        otop = vv[0].toDouble();
        vent = vv[1].toDouble();
        gvs = vv[2].toDouble();
    }

    return;
}


/*
void GidWidget::createAlseco()
{
}
*/

void GidWidget::addAlseco1()
{
}

void GidWidget::addAlseco2()
{
}



void GidWidget::onNagrAlmatyAdd1() // Добавить Объект (МЖД) АЛСЕКО
{
/*
    if (m_ng <= 0) return;

    double otop = 0, vent = 0, gvs = 0;
    read_nagr(m_ng, otop, vent, gvs);

*/

//    QMessageBox::information(this, "", "2");
}

void GidWidget::onNagrAlmatyAdd2() // Добавить Объект (Прочие) АЛСЕКО
{
/*
    if (m_ng <= 0) return;

    double otop = 0, vent = 0, gvs = 0;
    read_nagr(m_ng, otop, vent, gvs);
*/


//    QMessageBox::information(this, "", "3");
}



QString get_nom_cxema(int gvpr, int gvsm, int gvps, int gvpw, int rez, int gvop, int gvoo, int otopln, int otoplz, int ventil, int elevat)
{

    for (auto & [key, val] : map_idrp) {
        int n_otoplz = val[0];
        int n_otopln = val[1];
        int n_ventil = val[2];
        int n_gvop = val[4];
        int n_gvoo = val[5];
        int n_gvpr = val[6];
        int n_gvsm = val[7];
        int n_gvps = val[8];
        int n_gvpw = val[9];
        int n_rez = val[10];
        int n_elevat = val[11];

        if (sgn(n_otoplz) != sgn(otoplz)) continue;
        if (sgn(n_otopln) != sgn(otopln)) continue;
        if (sgn(n_ventil) != sgn(ventil)) continue;
        if (sgn(n_gvop) != sgn(gvop)) continue;
        if (sgn(n_gvoo) != sgn(gvoo)) continue;
        if (sgn(n_gvpr) != sgn(gvpr)) continue;
        if (sgn(n_gvsm) != sgn(gvsm)) continue;
        if (sgn(n_gvps) != sgn(gvps)) continue;
        if (sgn(n_gvpw) != sgn(gvpw)) continue;

        int a14 = sgn(n_elevat);

        if (otoplz) if (sgn(elevat) == a14) continue;

        if (gvop != 0. || gvoo != 0. || gvsm != 0. || gvps != 0. || gvpw != 0. || gvpr != 0.) {
            if (sgn(n_rez) != sgn(rez)) continue;
            //QQ(rez);
        }

        return key.c_str();
    }

    return QString("");
}



bool readAlseco(QSqlDatabase &db, QString qq, double &otop_z_el, double &otop_z_be, double &otop_n, double &gvs_op, double &gvs_oo, double &gvs_pr, double &gvs_sm, double &gvs_ps, double &gvs_pw, double &vent)
{
    QString q;

    q = QString(
        "SELECT \n"
        "sum(otop * IIF(otop_cxema = 1, 1, 0)) AS otop_z_el, \n"
        "sum(otop * IIF(otop_cxema = 2, 1, 0)) AS otop_z_be, \n"
        "sum(otop * IIF(otop_cxema = 3, 1, 0)) AS otop_n, \n"

        "sum(gvs * IIF(gvs_cxema = 1, 1, 0)) AS gvs_op, \n"
        "sum(gvs * IIF(gvs_cxema = 2, 1, 0)) AS gvs_oo, \n"

        "sum(gvs * IIF(gvs_cxema = 3, 1, 0)) AS gvs_pr, \n"
        "sum(gvs * IIF(gvs_cxema = 4, 1, 0)) AS gvs_sm, \n"
        "sum(gvs * IIF(gvs_cxema = 5, 1, 0)) AS gvs_ps, \n"
        "sum(gvs * IIF(gvs_cxema = 6, 1, 0)) AS gvs_pw, \n"

        "sum(vent) AS vent, sum(par) AS par, sum(nagr) AS nagr FROM zdaniya_2 WHERE id IN (%1)").arg(qq);

    std::vector<QVariant> vv;

    bool ret = readTableValues(db, q, 11, vv);

    otop_z_el = vv[0].toDouble();
    otop_z_be = vv[1].toDouble();
    otop_n = vv[2].toDouble();
    gvs_op = vv[3].toDouble();
    gvs_oo = vv[4].toDouble();
    gvs_pr = vv[5].toDouble();
    gvs_sm = vv[6].toDouble();
    gvs_ps = vv[7].toDouble();
    gvs_pw = vv[8].toDouble();
    vent = vv[9].toDouble();
    double par = vv[10].toDouble();

    return ret;
}


QString alseco_text(double otop_z_el, double otop_z_be, double otop_n, double gvs_op, double gvs_oo, double gvs_pr, double gvs_sm, double gvs_ps, double gvs_pw, double vent)
{
    QString ss, str = "";
    
    if (otop_z_el > 0) { ss = QString("отопление, зависимое элеваторное: %1,\n").arg(otop_z_el); str += ss; }
    if (otop_z_be > 0) { ss = QString("отопление, зависимое безэлеваторное: %1,\n").arg(otop_z_be); str += ss; }
    if (otop_n > 0) { ss = QString("отопление, независимое: %1,\n").arg(otop_n); str += ss; }
    if (gvs_op > 0) { ss = QString("гвс, открытое из подачи: %1,\n").arg(gvs_op); str += ss; }
    if (gvs_oo > 0) { ss = QString("гвс, открытое из обратки: %1,\n").arg(gvs_oo); str += ss; }

    if (gvs_pr > 0) { ss = QString("гвс, паралельное: %1,\n").arg(gvs_pr); str += ss; }
    if (gvs_sm > 0) { ss = QString("гвс, смешанное: %1,\n").arg(gvs_sm); str += ss; }
    if (gvs_ps > 0) { ss = QString("гвс, последовательное: %1,\n").arg(gvs_ps); str += ss; }
    if (gvs_pw > 0) { ss = QString("гвс, предвключенное: %1,\n").arg(gvs_pw); str += ss; }

    if (vent > 0) { ss = QString("вентиляция: %1,\n").arg(vent); str += ss; }

    return str;
}


QString getPar(const std::list<int>& v);
QString getPar(const std::list<int>& v);


bool saveAlseco(PropertyDial *prop, QString m_prop_table, QString m_prop_field, QString name, double otop_z_el, double otop_z_be, double otop_n, double gvs_op, double gvs_oo, double gvs_pr, double gvs_sm, double gvs_ps, double gvs_pw, double vent)
{
    if (m_prop_table == "generalizedConsumers") {
        prop->setValue1("calcHLdep", otop_z_el+  otop_z_be);
        prop->setValue1("calcHLindep", otop_n);
        prop->setValue1("connectionSchemeID", otop_z_el > 0 ? 2 : 1);

        if (name != "") {
            prop->setValue1("name", name);
        }
        prop->setValue1("avgHLGVSopenSysFlow", gvs_op);
        prop->setValue1("calcHLventil", vent);
    }
    else {

        if (otop_z_el + otop_z_be > 0 && otop_n > 0) {
            QMessageBox::warning(0, "", "Выбранные дома имеют разные схемы включения СО.\nСохранение договорных нагрузок невозможно");
            return false;
        }

        if (name != "") {
            prop->setValue1("name", name);
        }
        QString get_nom_cxema(int gvpr, int gvsm, int gvps, int gvpw, int rez, int gvop, int gvoo, int otopln, int otoplz, int ventil, int elevat);

        double rez = 0;

        QString schemeNum = get_nom_cxema(gvs_pr > 0, gvs_sm > 0, gvs_ps > 0, gvs_pw > 0, rez > 0, gvs_op > 0, gvs_oo > 0, otop_n > 0, (otop_z_el + otop_z_be) > 0, vent > 0, otop_z_el <= 0);

        prop->setValue1("schemeNum", schemeNum);
        prop->setPR(schemeNum);

        prop->setValue1("calcHLdep", otop_z_el + otop_z_be);
        prop->setValue1("calcHLindep", otop_n);

        prop->setValue1("avgHLGVSopenFlow", gvs_op);
        prop->setValue1("avgHLGVSopenRet", gvs_oo);

        prop->setValue1("avgHLGVScloseParall", gvs_pr);
        prop->setValue1("avgHLGVScloseMix", gvs_sm);
        prop->setValue1("avgHLGVScloseConseq", gvs_ps);
        prop->setValue1("avgHLGVSclosePreON", gvs_pw);

        prop->setValue1("calcHLventil", vent);
    }

    return true;
}

QString buildValuesFromCsv(const QString &csvContent, const QChar delimiter = ';');


void GidWidget::alseco(PropertyDial *prop, const QString & table, const QString &  column, const QString &  kod, const QString &  name)
{
    Klassif* kls = m_kl_list.findKlN("zdaniya_2");
    if (!kls || !kls->ins) return;

    list<int> list_id;
    m_geo.NMark(kls, list_id);

    if (list_id.size() == 0) {
        QMessageBox::warning(this, "", "Выберите здание с нагрузками");
        return;
    }


    QString id_list = getPar(list_id);

    double otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent;

    if (!readAlseco(*kls->m_db, id_list, otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent)) {
        return;
    }

    QString str = QString("Ввести нагрузки из выбранных зданий?\n"
               "Всего выбрано %1 зданий\n"
                          "Нагрузка, Гкал/ч:\n").arg(list_id.size());

    str += alseco_text(otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent);

    if (QMessageBox::question(this, "", str) == QMessageBox::Yes) {
        saveAlseco(prop, table, column, "", otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent);

        QString q;

        q = QString("UPDATE zdaniya_2 SET potrebitel=NULL WHERE potrebitel='%1 %2'").arg(kod, name);

        bool ret1 = query_exec(*kls->m_db, q);

//        kls->m_ado->Execute(q);

        q = QString("UPDATE zdaniya_2 SET potrebitel='%1 %2' WHERE id IN (%3)").arg(kod, name, id_list);
        bool ret2 = query_exec(*kls->m_db, q);
    }
}



QString getAlsecoTxt(const QString &mkr2, const QString &str2, const QString &dom2, double otop, double vent, double gvs, double par)
{
    double q = (otop+gvs+vent+par)/1e6;

    QString txt = "";

    if (mkr2 != "") {
        if (txt != "") txt += " ";
        txt += mkr2;
    }
    if (str2 != "") {
        if (txt != "") txt += " ";
        txt += str2;
    }
    if (dom2 != "") {
        if (txt != "") txt += " ";
        txt += dom2;
    }

    if (otop) {
        if (txt != "") txt += "\r\n";
        txt += QString("Qот=%1").arg(otop/1e6);
    }
    if (gvs) {
        if (txt != "") txt += "\r\n";
        txt += QString("Qгвс=%1").arg(gvs/1e6);
    }

    if (vent) {
        if (txt != "") txt += "\r\n";
        txt += QString("Qвент=%1").arg(vent/1e6);
    }
    if (par) {
        if (txt != "") txt += "\r\n";
        txt += QString("Qпар=%1").arg(par/1e6);
    }

    if (q) {
        if (txt != "") txt += "\r\n";
        txt += QString("Qсум=%1").arg(q);
    }

    return txt;
}
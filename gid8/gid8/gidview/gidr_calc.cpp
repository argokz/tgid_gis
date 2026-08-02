#include <QtGui>
#include <QtSql>
#include <QFileDialog>

#include <mainwindow.h>
#include <gidview/GidWidget.h>

#include <db/db.h>

#include "ui/Param1Dialog.h"
#include "ui/Param2Dialog.h"

#include <QInputDialog>

//const QString sety4 = "sety4.exe";

//const QString sety4 = "D:/Qt/gid8/python/sety/sety/ww.py";
const QString sety4 = "";

QString init_sety4()
{
    QSettings settings;

    QString path = argpath()+"sety/ww.pyc";
    if (!QFile::exists(path)) {
        QString dir = QCoreApplication::applicationDirPath();
#if _WIN32
        path = dir+"/../python/sety/sety/ww.py";
#else
        path = dir+"/../../../python/sety/sety/ww.py";
#endif
    }

    /*

    QMessageBox::warning(this, tr(""), "Выберите активный фрагмент");

    if (!QFile::exists(path)) {
        path = settings.value("config/sety4", sety4).toString();

        while (path == "" || !QFile::exists(path)) {
            path = QFileDialog::getOpenFileName(nullptr,
                QObject::tr("Выберите программу для расчета"), 
                QString(), 
                QObject::tr("Программы (*.exe *.py *.pyc)"));

            if (path.isEmpty()) {
                return path;
            }
            settings.setValue("config/sety4", path);
        }
    }
*/
    return path;
}

QString python_exe();

QString get_sety4()
{
    QString path = init_sety4();
    
#if _WIN32
    return QString("%1 \"%2\"").arg(python_exe(), path);
#else
//    return QString("%1 '%2'").arg(python_exe(), path);
    return QString("%1 %2").arg(python_exe(), path);
#endif
}

enum TYPE_OF_NET {
  TYPE_OF_NET_MYSQL = 0,
  TYPE_OF_NET_MSSQL = 1,
};


// Фактический / Аварийный

QString getDoIt(CParam2Dialog& param, CCxema* cxema, int fileID, const QString & m_user)
//QString getDoIt(CParam1Dialog & param, CConnect cn, int fileID)
{
    QString str, par = "", qq;

    QString rdbms = "MsSql";

    if (is_POSTGRESQL()) {
        rdbms = "postgreSQL";
    }

    QString outName = QString("%1/out.txt").arg(QDir::tempPath());
    CConnect cn = cxema->m_connect;

    QString sopr = QString(" -sopr %1 -roP %2 -roO %3 ")
        .arg(param.m_sopr)
        .arg(param.m_roP)
        .arg(param.m_roO)
        ;

    if (param.m_ro_temp) {
        sopr += " -ro_temp ";
    }


    QFile::remove(outName);

    str = QString("%1 "
        " -type_of_net %2 -server \"%3\" -database \"%4\" -user \"%5\" -port %6 -fileID %7 "
        " %8 -Tn %9  -GWS %10 -GWS2 %11 %12"
        " -rdbms %13"
        " -out_file \"%14\""
        )
        .arg(get_sety4()).arg(TYPE_OF_NET_MSSQL)

        .arg(cn.host).arg(cn.baza).arg(cn.user).arg(cn.port).arg(fileID)
        .arg(par, param.m_strTn).arg(1).arg(1).arg(sopr)
        .arg(rdbms)
        .arg(outName);
            //        .arg(par, param.m_strTn).arg(param.m_nGWS).arg(param.m_nGWS2);

    if (cn.password != "") {
        str += QString(" -password \"%1\"").arg(cn.password);
    }




    /*
    if (param.m_name != "") {
        str += " -name \"";
        str += param.m_name;
        param.m_name.replace("\"", "'");
        str += "\"";
    }
*/

    if (param.m_bAvtomat) str += " -avtomat_yes";
    if (param.m_bChar_sety) str += " -char_sety";

    if (param.m_copy_calc) {
        QString gid9 = get_baza_gid9();
        if (gid9 != "") {
            str += QString(" -copy_calc -database2 \"%1\" ").arg(gid9);
        }
    }

    if (param.m_bVeter) {
        str += " -veter";
    }

    if (param.m_radioDetaliz == 1) {
        str += " -a";
    }
//    int m_ur = AfxGetApp()->GetProfileInt(szSection, "ur", 1);

//    bool m_ur = false;

//    if (!m_ur) {
//        //        str += " -no_teplovyd";
//        //        str += " -no_teplopoter";
//        str += " -tg";
//    }

//    if (param.m_mag_fn != "") {
//        qq.Format(" -mn \"%s\"", param.m_mag_fn);
//        str += qq;
//   }

/*
    bool m_reread_sety = false;
    if (param.m_bSave_po) {
        m_reread_sety = true;
        str += " -save_po";
    }
*/
    if (param.m_bLeto) {
        str += " -leto";
    }

    if (param.m_bSaveLeto) {
        str += " -save_po";
    }

    if (param.m_bMag_fragment) {
        str += " -mag_fragment";
    }

    if (!param.m_bKv) {
        str += " -no_kv";
    }

    qq = QString(" -user_gid \"%1\"").arg(m_user);
    str += qq;


//    qq = QString(" -user_gid \"%1\"").arg(cn.user_gid);
//    str += qq;


//    map <QString, double>::const_iterator it = param.map_t_ist.begin();

//    for (; it != param.map_t_ist.end(); ++it) {
//      QString s;
//        s = QString(" -Tist %1 %2").arg(it->first).arg(it->second);
//        str += s;
//    }

    str += " -color";

    return str;
}

QString getDoItDr(CParam1Dialog& param, CCxema* cxema, int fileID, const QString & m_user)
{
    QString s, str, par = "", qq;

    QString outName = QString("%1/out.txt").arg(QDir::tempPath());
    CConnect cn = cxema->m_connect;

    QFile::remove(outName);

    bool is_dross = true;

    if (is_dross && param.m_bDross) par += "-dross_yes ";
    if (param.m_bAvtomat) par += "-avtomat_yes ";

    double Tn = param.m_strTn.toDouble();
    int kol_iter = param.m_str_kol_iter.toInt();


//    if (is_mag) {
//        Tn = cxema->m_Tn_ot;
//    }

//    int param.m_kol_iter = 20;

    QString rdbms = "MsSql";

    if (is_POSTGRESQL()) {
        rdbms = "postgreSQL";
    }

#if 0

    str = QString("\"%1\" "
        " -type_of_net %2 -server \"%3\" -database \"%4\" -user \"%5\" -port %6 -fileID %7 "
        " %8 -iter %9 -dross -Tn %10 -tp_metod %11 -trtp %12 -rdbms %13")
        .arg(get_sety4())
        .arg(TYPE_OF_NET_MSSQL)
        .arg(cn.host).arg(cn.baza).arg(cn.user).arg(cn.port).arg(fileID)
        .arg(par).arg(param.m_kol_iter).arg(Tn).arg(param.m_cmbMetod).arg(param.m_cmbTrtp)
        .arg(rdbms);


#else

    QString sopr = QString(" -sopr %1 -roP %2 -roO %3 ")
        .arg(param.m_sopr)
        .arg(param.m_roP)
        .arg(param.m_roO)
        ;

    if (param.m_ro_temp) {
        sopr += " -ro_temp ";
    }

    str = QString("%1 "
                  " -type_of_net %2 -server \"%3\" -database \"%4\" -user \"%5\" -port %6 -fileID %7 "
                  " %8 -iter %9 -dross -Tn %10 -tp_metod %11 -trtp %12 %13 -rdbms %14"
                  " -out_file \"%15\""
                  )
              .arg(get_sety4())
              .arg(TYPE_OF_NET_MSSQL)
              .arg(cn.host).arg(cn.baza).arg(cn.user).arg(cn.port).arg(fileID)
              .arg(par).arg(kol_iter).arg(Tn).arg(param.m_cmbMetod).arg(param.m_cmbTrtp)
              .arg(sopr)
              .arg(rdbms)
              .arg(outName);

#endif


    if (cn.password != "") {
        str += QString(" -password \"%1\"").arg(cn.password);
    }

#if 0
    if (is_dross && !is_mag) {
        if (param.m_sav_off) {
            str += " -soff";
        }
        if (param.m_sav_on) {
            str += " -son";
        }
    }

    if (param.m_mag_fn != "") {
        qq = QString(" -mn \"%1\"").arg(param.m_mag_fn);
        str += qq;
    }
#endif

    if (param.m_bZn0) {
        str += " -zulu_zn0";
    }
    if (param.m_bUtechki) {
        str += " -zulu_utechki";
    }

    if (param.m_bChar_sety) str += " -char_sety";

    if (!param.m_bTeplovyd) {
        str += " -no_teplovyd";
    }

    if (param.m_bUf_calc) {
        str += " -uf_calc";
    }

    bool is_mag = false;

    if (param.m_radioUr == 1) {
        str += " -tg";

        if (!param.m_bTeplopoter) {
            str += " -no_teplopoter";
        }
        if (is_dross && !is_mag) {
            if (param.m_bUf_new) {
//                if (AfxMessageBox(tr("Сохранить коэффициенты смешения?"), MB_YESNO) == IDYES) {
//                    str += " -save_uf_new";
//                }
            }

        }
    }

    char szDT[256];

    time_t ltime;
    time(&ltime);
    struct tm* newtime = gmtime(&ltime);
    strftime(szDT, 128, "\"%Y-%m-%d %H:%M:%S\"", newtime);
    str += " -time ";
    str += szDT;

    if (param.m_strName != "") {
        str += " -name \"";
        str += param.m_strName;
        param.m_strName.replace("\"", "'");
        str += "\"";
    }

//    if (param.m_leto) {
//        str += " -leto";
//    }

    if (param.m_bMag_fragment) {
        str += " -mag_fragment";
    }

    if (param.m_copy_calc) {
        QString gid9 = get_baza_gid9();
        if (gid9 != "") {
            str += QString(" -copy_calc -database2 \"%1\" ").arg(gid9);
        }
    }


    if (param.m_bVeter) {
        str += " -veter";
    }
#if 1
    bool m_reread_sety = false;
    if (is_dross && !is_mag) {
        if (param.m_bSave_po) {
            m_reread_sety = true;
            str += " -save_po";
        }
    }

#endif

    if (!param.m_bKv) {
        str += " -no_kv";
    }

#if 0
    if (param.m_nagr) {
        s.Format(" -nagr %d", param.m_nagr);
        str += s;
    }

#endif

//    s = QString(" -user_gid \"%1\"").arg(cn.baza);
    s = QString(" -user_gid \"%1\"").arg(m_user);
    str += s;

#if 0
    map <CString, double>::const_iterator it = param.map_t_ist.begin();

    for (; it != param.map_t_ist.end(); ++it) {
        s.Format(" -Tist %s %g", it->first, it->second);
        str += s;
    }
#endif
//    if (is_mag) {
//        str += " -no_out";
//    }

    str += " -color";

    return str;
}

QString getDoItListDr(QWidget *win, CCxema *cxema, set<int> & set_fileID, const QString & m_user)
{
    CParam1Dialog dial(win);

/*
    bool m_bTeplopoter;
    bool m_bUf_calc;
    bool m_bTeplovyd;
    bool m_bVeter;
    bool m_bDross;
    bool m_bAvtomat;
    bool m_bUf_new;
    bool m_bChar_sety;
    bool m_bZn0;
    bool m_bUtechki;
    bool m_bMag_fragment;
    bool m_bSave_po;
    bool m_bKv;
    bool m_bPlan;
 */

    QSettings settings;

    QString date = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");

    dial.m_sopr = settings.value("param/sopr", 0).toInt();
    dial.m_roP = settings.value("param/roP", 0.975).toDouble();
    dial.m_roO = settings.value("param/roO", 0.975).toDouble();
    dial.m_ro_temp = settings.value("param/ro_temp", true).toBool();

    dial.m_strName = QString("Расчет планового режима %1").arg(date);

    dial.m_strTn = settings.value("param/Tn", -32).toString();
    dial.m_str_kol_iter = settings.value("param/kol_iter", 20).toString();

    dial.m_bTeplopoter = settings.value("param/Teplopoter", true).toBool();
    dial.m_bUf_calc = settings.value("param/Uf_calc", false).toBool();
    dial.m_bTeplovyd = settings.value("param/Teplovyd", true).toBool();
    dial.m_bVeter = settings.value("param/Veter", false).toBool();
    dial.m_bDross = settings.value("param/Dross", false).toBool();
    dial.m_bDross = false;
    dial.m_bAvtomat = settings.value("param/Avtomat", false).toBool();
    dial.m_bUf_new = settings.value("param/Uf_new", false).toBool();
    dial.m_bChar_sety = settings.value("param/Char_sety", false).toBool();
    dial.m_bZn0 = settings.value("param/Zn0", false).toBool();
    dial.m_bUtechki = settings.value("param/Utechki", false).toBool();
    dial.m_bMag_fragment = settings.value("param/Mag_fragment", false).toBool();
    dial.m_bSave_po = settings.value("param/Save_po", false).toBool();
    dial.m_bSave_po = false;

    dial.m_bKv = settings.value("param/Kv", true).toBool();
    dial.m_bPlan = settings.value("param/Plan", true).toBool();

    dial.m_radioUr = settings.value("param/Ur", 0).toInt();
    dial.m_radioDop_nagr = settings.value("param/Dop_nagr", 0).toInt();

    dial.m_cmbTrtp = settings.value("param/Trtp", 0).toInt();

    dial.ui.combo_Trtp->addItem(QObject::tr("расчетная tн отопл"), 0);
    dial.ui.combo_Trtp->addItem(QObject::tr("среднесезонная tн от.периода"), 1);
    dial.ui.combo_Trtp->addItem(QObject::tr("текущая tн"), 2);

    dial.m_cmbMetod = settings.value("param/Metod", 0).toInt();

//    dial.ui.combo_Metod->addItem(QObject::tr("настройка"), 0);
    dial.ui.combo_Metod->addItem(QObject::tr("нормы"), 1);
//    dial.ui.combo_Metod->addItem(QObject::tr("изоляция"), 2);

    dial.ui.check_Avtomat->setVisible(false);
    dial.ui.check_Veter->setVisible(false);
    dial.ui.check_Zn0->setVisible(false);

    dial.m_copy_calc = false;

#if !USE_GID9
    dial.ui.check_copy_calc->setVisible(false);
#else
    if (!is_gid9()) {
        dial.ui.check_copy_calc->setVisible(false);
    }
#endif



    dial.write();

    int ret = dial.exec();

    dial.read();

    settings.setValue("param/sopr",  dial.m_sopr);
    settings.setValue("param/roP",  dial.m_roP);
    settings.setValue("param/roO",  dial.m_roO);
    settings.setValue("param/ro_temp",  dial.m_ro_temp);

    settings.setValue("param/Tn",  dial.m_strTn.toDouble());
    settings.setValue("param/kol_iter",  dial.m_str_kol_iter.toInt());
    
    settings.setValue("param/Teplopoter",  dial.m_bTeplopoter);
    settings.setValue("param/Uf_calc",  dial.m_bUf_calc);
    settings.setValue("param/Teplovyd",  dial.m_bTeplovyd);
    settings.setValue("param/Veter",  dial.m_bVeter);
    settings.setValue("param/Dross",  dial.m_bDross);
    settings.setValue("param/Avtomat",  dial.m_bAvtomat);
    settings.setValue("param/Uf_new",  dial.m_bUf_new);
    settings.setValue("param/Char_sety",  dial.m_bChar_sety);
    settings.setValue("param/Zn0",  dial.m_bZn0);
    settings.setValue("param/Utechki",  dial.m_bUtechki);
    settings.setValue("param/Mag_fragment",  dial.m_bMag_fragment);
    settings.setValue("param/Save_po",  dial.m_bSave_po);
    settings.setValue("param/Kv",  dial.m_bKv);
    settings.setValue("param/Plan",  dial.m_bPlan);

    settings.setValue("param/Ur",  dial.m_radioUr);
    settings.setValue("param/Dop_nagr",  dial.m_radioDop_nagr);

    settings.setValue("param/Trtp",  dial.m_cmbTrtp);
    settings.setValue("param/Metod",  dial.m_cmbMetod);


    if (ret == QDialog::Accepted) {
        QString str = "";

        for (auto fileID: set_fileID) {
            QString s = getDoItDr(dial, cxema, fileID, QString("%1").arg(m_user));
            if (str != "") str += "\n";
            str += s;
        }
        return str;
    }
    return "";
}


QString getDoItList(QWidget *win, CCxema *cxema, set<int> & set_fileID, const QString & m_user)
{
    CParam2Dialog dial(win);

/*
    bool m_bTeplopoter;
    bool m_bUf_calc;
    bool m_bTeplovyd;
    bool m_bVeter;
    bool m_bDross;
    bool m_bAvtomat;
    bool m_bUf_new;
    bool m_bChar_sety;
    bool m_bZn0;
    bool m_bUtechki;
    bool m_bMag_fragment;
    bool m_bSave_po;
    bool m_bKv;
    bool m_bPlan;
 */

    QSettings settings;

    dial.m_sopr = settings.value("param/sopr", 0).toInt();
    dial.m_roP = settings.value("param/roP", 0.975).toDouble();
    dial.m_roO = settings.value("param/roO", 0.975).toDouble();
    dial.m_ro_temp = settings.value("param/ro_temp", true).toBool();


    QString date = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");

    dial.m_strName = QString("Расчет фактического режима %1").arg(date);

    dial.m_strTn = settings.value("param/Tn", -32).toString();
    dial.m_str_kol_iter = settings.value("param/kol_iter", 20).toString();

//    dial.m_bTeplopoter = settings.value("param/Teplopoter", true).toBool();
//    dial.m_bUf_calc = settings.value("param/Uf_calc", false).toBool();
//    dial.m_bTeplovyd = settings.value("param/Teplovyd", true).toBool();
    dial.m_bVeter = settings.value("param/Veter", false).toBool();
//    dial.m_bDross = settings.value("param/Dross", false).toBool();
    dial.m_bAvtomat = settings.value("param/Avtomat", false).toBool();
//    dial.m_bUf_new = settings.value("param/Uf_new", false).toBool();
    dial.m_bChar_sety = settings.value("param/Char_sety", false).toBool();
//    dial.m_bZn0 = settings.value("param/Zn0", false).toBool();
//    dial.m_bUtechki = settings.value("param/Utechki", false).toBool();
    dial.m_bMag_fragment = settings.value("param/Mag_fragment", false).toBool();
    dial.m_bSave_po = settings.value("param/Save_po", false).toBool();
    dial.m_bSave_po = false;
    dial.m_bKv = settings.value("param/Kv", true).toBool();
//    dial.m_bPlan = settings.value("param/Plan", true).toBool();

//    dial.m_bLeto =  settings.value("param/Leto", false).toBool();
    dial.m_bLeto = false;
//    dial.m_bSaveLeto =  settings.value("param/SaveLeto", false).toBool();
    dial.m_bSaveLeto = false;


    dial.m_cmbGws = settings.value("param/Gws", 0).toInt();
    dial.m_cmbGws2 = settings.value("param/Gws2", 0).toInt();

    dial.m_radioDetaliz = settings.value("param/Detaliz", 1).toInt();


    dial.ui.combo_Gws->addItem(QObject::tr("расчетная"), 0);
    dial.ui.combo_Gws->addItem(QObject::tr("среднесезонная"), 1);
    dial.ui.combo_Gws->addItem(QObject::tr("текущая"), 2);

    dial.ui.combo_Gws2->addItem(QObject::tr("расчетная"), 0);
    dial.ui.combo_Gws2->addItem(QObject::tr("среднесезонная"), 1);
    dial.ui.combo_Gws2->addItem(QObject::tr("текущая"), 2);

//    dial.ui.check_Avtomat->setVisible(false);
    
//    dial.m_radioUr = settings.value("param/Ur", 0).toInt();
//    dial.m_radioDop_nagr = settings.value("param/Dop_nagr", 0).toInt();

    dial.ui.check_Avtomat->setVisible(false);
//    dial.ui.check_Leto->setVisible(false);
    dial.ui.check_Save_po->setVisible(false);
    dial.ui.check_Veter->setVisible(false);

    dial.m_copy_calc = false;

#if !USE_GID9
    dial.ui.check_copy_calc->setVisible(false);
#else
    if (!is_gid9()) {
        dial.ui.check_copy_calc->setVisible(false);
    }
#endif



    dial.write();

    int ret = dial.exec();

    dial.read();

        settings.setValue("param/sopr",  dial.m_sopr);
        settings.setValue("param/roP",  dial.m_roP);
        settings.setValue("param/roO",  dial.m_roO);
        settings.setValue("param/ro_temp",  dial.m_ro_temp);

        settings.setValue("param/Tn",  dial.m_strTn.toDouble());
        settings.setValue("param/kol_iter",  dial.m_str_kol_iter.toInt());
//        settings.setValue("param/Teplopoter",  dial.m_bTeplopoter);
//        settings.setValue("param/Uf_calc",  dial.m_bUf_calc);
//        settings.setValue("param/Teplovyd",  dial.m_bTeplovyd);
        settings.setValue("param/Veter",  dial.m_bVeter);
//        settings.setValue("param/Dross",  dial.m_bDross);
        settings.setValue("param/Avtomat",  dial.m_bAvtomat);
//        settings.setValue("param/Uf_new",  dial.m_bUf_new);
        settings.setValue("param/Char_sety",  dial.m_bChar_sety);
//        settings.setValue("param/Zn0",  dial.m_bZn0);
//        settings.setValue("param/Utechki",  dial.m_bUtechki);
        settings.setValue("param/Mag_fragment",  dial.m_bMag_fragment);
        settings.setValue("param/Save_po",  dial.m_bSave_po);
        settings.setValue("param/Kv",  dial.m_bKv);

        settings.setValue("param/Leto",  dial.m_bLeto);
        settings.setValue("param/SaveLeto",  dial.m_bSaveLeto);

        settings.setValue("param/Gws",  dial.m_cmbGws);
        settings.setValue("param/Gws2",  dial.m_cmbGws2);


        settings.setValue("param/Detaliz",  dial.m_radioDetaliz);


//        settings.setValue("param/Plan",  dial.m_bPlan);

//       settings.setValue("param/Ur",  dial.m_radioUr);
//        settings.setValue("param/Dop_nagr",  dial.m_radioDop_nagr);


    if (ret == QDialog::Accepted) {

        QString str = "";

        for (auto fileID: set_fileID) {
            QString s = getDoIt(dial, cxema, fileID, QString("%1").arg(m_user));
            if (str != "") str += "\n";
            str += s;
        }
        return str;
    }
    return "";
}



#include <dialog/ChMenuDial.h>


QString menuFragmentR(QWidget *parent, QSqlDatabase &db, const QString & par, const QString & par1)
{
//    QString q = "SELECT id, name FROM fragments WHERE removed = 0 ORDER BY CASE WHEN Name like'%[0-9]%' THEN Replicate('0', 100 - Len(Name)) + Name ELSE Name END, name";
    QString q = QString("SELECT id, name FROM fragments WHERE removed = 0 AND id IN (%1) ORDER BY name").arg(par);

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query_exec(db, query, q);

    ChMenuDial dlg(parent, QObject::tr("Выбрать фрагменты сети для расчета"));

//    dlg.addButton(QObject::tr("Новый фрагмент"), parent, SLOT(new_fragment()));

    QStringList par_list = par1.split(",");

    while (query.next()) {
        QString name = query.value("name").toString();
        int id = query.value("id").toInt();
        bool on = par_list.indexOf(QString("%1").arg(id)) >= 0;
        dlg.Add(name, id, false);
    }

    if (!dlg.exec()) return "";

    QString par2 = "";

    for (auto & it : dlg.out) {
        if (par2 != "") par2 += ",";
        par2 += QString("%1").arg(it.toString());
    }

    return par2;
}


void run_bat(QWidget *wnd, const QString & str)
{
    QString tmp_bat = QString("%1/tgis_run.bat").arg(QDir::tempPath());

    QFile file(tmp_bat);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << "@echo off\n";
        out << str;
        file.close();
        main_window->start(tmp_bat, wnd, SLOT(onFinished(int, QProcess::ExitStatus)));
    }
    else {
        QMessageBox::warning(wnd, "", QObject::tr("Не могу записать файл"));
    }
}


void GidWidget::onDoItList() // Плановый по списку...
{                            
    if (!m_isOpen) return;

    endDraw();
    
    QString par = menuFragmentR(this, m_cxema.m_db, m_cxema.m_par, "");

    if (par == "") {
        return;
    }

    QStringList par_list = par.split(",");

    m_set_fileID.clear();

    for (auto s : par_list) {
        m_set_fileID.insert(s.toInt());
    }

    QString str = getDoItList(this, &m_cxema, m_set_fileID, QString("%1").arg(m_strUser));

    if (str != "") {
        run_bat(this, str);
    }
}



void GidWidget::onDoItListDr() // Плановый по списку...
{                            
    if (!m_isOpen) return;

    endDraw();
    
    QString par = menuFragmentR(this, m_cxema.m_db, m_cxema.m_par, "");

    if (par == "") {
        return;
    }

    QStringList par_list = par.split(",");

    m_set_fileID.clear();

    for (auto s : par_list) {
        m_set_fileID.insert(s.toInt());
    }

    QString str = getDoItListDr(this, &m_cxema, m_set_fileID, QString("%1").arg(m_strUser));

    if (str != "") {
        run_bat(this, str);
    }
}
    

void GidWidget::onDoItDr() // Плановый...
{
    if (!m_isOpen) return;

    endDraw();

    if (m_fileID == -1) {
        onMainCxema();

        if (m_fileID == -1) {
//            QMessageBox::warning(this, tr(""), "Выберите активный фрагмент");
            return;
        }
    }

    m_set_fileID = {m_fileID};

    QString str = getDoItListDr(this, &m_cxema, m_set_fileID, QString("%1").arg(m_strUser));
    if (str != "") {
//        bool ok;
//        QInputDialog::getMultiLineText(nullptr, "title", "label", str, &ok);

//        if (ok) {
            qDebug() << str;
            main_window->start(str, this, SLOT(onFinished(int, QProcess::ExitStatus)));
//        }
    }
}

void GidWidget::onDoIt() // Фактический...
{
    if (!m_isOpen) return;

    endDraw();

    if (m_fileID == -1) {
        onMainCxema();

//        QMessageBox::warning(this, tr(""), "Выберите активный фрагмент");
        if (m_fileID == -1) {
            return;
        }
    }

    m_set_fileID = {m_fileID};

    QString str = getDoItList(this, &m_cxema, m_set_fileID, QString("%1").arg(m_strUser));
    if (str != "") {
//        bool ok;
//        QInputDialog::getMultiLineText(nullptr, "title", "label", str, &ok);

//        if (ok) {
            qDebug() << str;
            main_window->start(str, this, SLOT(onFinished(int, QProcess::ExitStatus)));
//        }
    }

#if 0
    if (!m_isOpen) return;

    endDraw();

    if (m_fileID == -1) {
        QMessageBox::warning(this, tr(""), "Выберите активный фрагмент");
        return;
    }

    CParam2Dialog dial(this);

    if (dial.exec() == QDialog::Accepted) {
    }
#endif
}

QString getPar(std::set<int>& v);
void create_b5_new(QSqlDatabase &db, CNode2 *nP, CCxema* cxema);

void GidWidget::reopen(set<int> & set_fileID)
{
    int m_parent_id_old = m_parent_id;

//    if (nc && nc->node.typ == TIP_PR) return false;

    KamObr(false);

    qDebug() << "открываем фрагменты ";

    auto p = m_cxema.m_graph->map_node.begin();

    while (p != m_cxema.m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (set_fileID.find(n->node.fileID) != set_fileID.end()) {
            n->node.nomgP = -1;
            n->node.nomgO = -1;
            for (CLINE2* l = n->lines; l; l = nextl(l)) {
                bline(l)->line.nomgP = -1;
                bline(l)->line.nomgO = -1;
                bline(l)->line.pod.q = 0;
                bline(l)->line.obr.q = 0;
            }
        }
        p++;
    }


    QString par = getPar(set_fileID);

    qDebug() << par;


    m_cxema.m_graph->clean_out(set_fileID);

    qDebug() << "Очистили";


    QProgressDialog progress("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();


    m_cxema.open_cxema_2(par, &progress);

    qDebug() << "Закончили";


    if (m_parent_id_old > 0) {
        CNode2 *nc = m_cxema.m_graph->find(m_parent_id_old);

        if (nc && nc->node.typ == TIP_PR) {
            create_b5_new(m_cxema.m_db, nc, &m_cxema);
//            m_parent_id = m_parent_id_old;
            Kam(m_parent_id_old);
        }
        else {
            Kam(m_parent_id_old);
        }
    }

    repaint();
}


void GidWidget::onFinished(int exitCode, QProcess::ExitStatus exitStatus )
{
    if (exitStatus == QProcess::NormalExit) {
//        m_cxema.open_cxema(this, QString("%1").arg(m_fileID), nullptr);

        QString outName = QString("%1/out.txt").arg(QDir::tempPath());

        qDebug() << "outName " << outName;

        QString q_name = readFile(outName, fp_absolute).trimmed();

        qDebug() << "q_name " << q_name;


        if (q_name != "") {
            QString q = readFile(q_name, fp_absolute);
            if (q != "") {
                query_exec(m_cxema.m_db, q, true);
            }
        }

//        m_set_fileID
//        m_set_fileID.insert(s.toInt());

        if (exitCode == 0) {
            reopen(m_set_fileID);
        }
        
        if (exitCode == 0) {
//            QMessageBox::information(this, "", QString(tr("Расчет закончен")).arg(exitCode));
            QMessageBox::information(this, "", QString(tr("Расчет закончен")));
        }
        else {
            QMessageBox::information(this, "", QString(tr("Расчет закончен\nОбнаружены ошибки.\nСмотрите файл протокола")));
        }
    }
    else {
        QMessageBox::information(this, "", QString(tr("При проведении расчета произошел сбой")));
    }

}


#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <map>

#include "PropertyDial.h"
#include "qmessagebox.h"
#include <db/db.h>
#include <mystd.h>

#include <QMessageBox>

//#include "variantdelegate.h"

#include "prop_fun.h"

typedef QVariant (*Fun)(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);

std::map<
    QString, 
    FunIndex
    > map_fun = {
  {"average3",              FUN_average3},
  {"climat1",               FUN_climat1},
  {"color",                 FUN_color},
  {"diameter_std",          FUN_diameter_std},
  {"diameter_std2",         FUN_diameter_std2},
  {"Date",                  FUN_Date},
  {"Dir",                   FUN_Dir},
  {"File",                  FUN_File},
  {"FileView",              FUN_FileView},
  {"heatTestsCoeff",        FUN_heatTestsCoeff},
  {"Hidden",                FUN_Hidden},
  {"hydroRes",              FUN_hydroRes},
  {"ind_result",            FUN_ind_result},
  {"ind_save",              FUN_ind_save},
  {"length",                FUN_length},
  {"Math",                  FUN_Math},
  {"ms_rs",                 FUN_ms_rs},
  {"NagrSum",               FUN_NagrSum},
  {"nagruz_ot",             FUN_nagruz_ot},
  {"nagruz_vt",             FUN_nagruz_vt},
  {"nodeName",              FUN_nodeName},
  {"nodes",                 FUN_nodes},
  {"nodes_pr",              FUN_nodes_pr},
  {"node_pick",             FUN_node_pick},
  {"node_pick1",            FUN_node_pick1},
  {"node_pick2",            FUN_node_pick2},
  {"NotNull",               FUN_NotNull},
  {"ReadOnly",              FUN_ReadOnly},
  {"schemeNum",             FUN_schemeNum},
  {"standardDampLink",      FUN_standardDampLink},
  {"Table",                 FUN_Table},
  {"TableQ",                FUN_TableQ},
  {"uf",                    FUN_uf},
  {"uf150-70",              FUN_uf150_70},
  {"utverdit",              FUN_utverdit},
  {"viewNsDb",              FUN_viewNsDb},
  {"Доля местных потерь",   FUN_dolya},
  {"Местные сопротивления", FUN_mestn},
//------------------------------------------
  {"nodes_new_baza", FUN_nodes_new_baza},
  {"list_of_coluns", FUN_list_of_coluns},
  {"viewText",       FUN_viewText},

  {"alsecoNagr",     FUN_alsecoNagr},
  {"alseco_nagr",    FUN_alseco_nagr},
  {"list_db",        FUN_list_db},
  {"addr_nagruzki",  FUN_addr_nagruzki},

  

};


std::map<
    FunIndex, 
    Fun
    > map_fun_id = {
  {FUN_average3,           fun_average3  },
  {FUN_climat1,            fun_climat1  },
  {FUN_color,              fun_color  },
  {FUN_diameter_std,       fun_diameter_std  },
  {FUN_diameter_std2,      fun_diameter_std2  },
  {FUN_Date,               fun_Date  },
  {FUN_Dir,                fun_Dir  },
  {FUN_File,               fun_File  },
  {FUN_FileView,           fun_FileView  },
  {FUN_heatTestsCoeff,     fun_heatTestsCoeff  },
  {FUN_Hidden,             fun_Hidden  },
  {FUN_hydroRes,           fun_hydroRes  },
  {FUN_ind_result,         fun_ind_result  },
  {FUN_ind_save,           fun_ind_save  },
  {FUN_length,             fun_length  },
  {FUN_Math,               fun_Math  },
  {FUN_ms_rs,              fun_ms_rs  },
  {FUN_NagrSum,            fun_NagrSum  },
  {FUN_nagruz_ot,          fun_nagruz_ot  },
  {FUN_nagruz_vt,          fun_nagruz_vt  },
  {FUN_nodeName,           fun_nodeName  },
  {FUN_nodes,              fun_nodes  },
  {FUN_nodes_pr,           fun_nodes_pr  },
  {FUN_node_pick,          fun_node_pick  },
  {FUN_node_pick1,         fun_node_pick1  },
  {FUN_node_pick2,         fun_node_pick2  },
  {FUN_NotNull,            fun_NotNull  },
  {FUN_ReadOnly,           fun_ReadOnly  },
  {FUN_schemeNum,          fun_schemeNum  },
  {FUN_standardDampLink,   fun_standardDampLink  },
  {FUN_Table,              fun_Table  },
  {FUN_TableQ,             fun_TableQ  },
  {FUN_uf,                 fun_uf  },
  {FUN_uf150_70,           fun_uf150_70  },
  {FUN_utverdit,           fun_utverdit  },
  {FUN_viewNsDb,           fun_viewNsDb  },
  {FUN_dolya,              fun_dolya  },
  {FUN_mestn,              fun_mestn  },

//------------------------------------------
  {FUN_nodes_new_baza, fun_nodes_new_baza},
  {FUN_list_of_coluns, fun_list_of_coluns},
  {FUN_viewText, fun_viewText},
  {FUN_alsecoNagr, fun_alsecoNagr},
  {FUN_alseco_nagr, fun_alseco_nagr},
  {FUN_list_db, fun_list_db},
  {FUN_addr_nagruzki, fun_addr_nagruzki},


};

FunIndex funIndex(const QString & fun)
{
    QStringList lst = fun.split(QRegularExpression("\\s+|\\$"));

    if (lst.size() > 0) {
        auto it = map_fun.find(lst[0]);
        if (it != map_fun.end()) {
            return it->second;
        }
    }
    return FUN_error;
}

FunIndex funIndex1(const QString & fun)
{
    auto it = map_fun.find(fun);
    if (it != map_fun.end()) {
        return it->second;
    }
    return FUN_error;
}


QString getDir(PropertyDial *widget, const QString & fun)
{
    return QFileDialog::getExistingDirectory(widget, "Выберите каталог");
}

#include <ui/PotrcxemaDialog.h>
#include <dialog/DatePopup.h>

void init_map_b(
    QString key, const QString & val0,
    std::map <QString, std::map<QString, std::list<QString> > > &map_vis, 
    std::map<QString , bool> &map_b
    )
{
    QString val = val0;

    auto it1 = map_vis.find(key.toLower());
    if (it1 != map_vis.end()) {
        for ( auto & it2 : it1->second) {
            for ( auto & it3 : it2.second) {
                map_b[it3] = false;
            }
        }

        if (val == "") val = "1";

        auto it2 = it1->second.find(val);
        if (it2 != it1->second.end()) {
            for (auto &it3 : it2->second) {
                map_b[it3] = true;
            }
        }
    }
}

void PropertyDial::setDisable(const QString & key, const QString & val)
{
    std::map<QString , bool> map_b;

    init_map_b(key, val, map_disable, map_b);

    for (auto &it : map_b) {
        setReadOnly(it.first, !it.second);
    }
}

void PropertyDial::setVis(const QString & key, const QString & val)
{
    std::map<QString , bool> map_b;

    init_map_b(key, val, map_vis, map_b);

    for (auto &it : map_b) {
        setVisual(it.first, !it.second);
    }
}

void PropertyDial::setVisAll()
{
    std::map<QString , bool> map_b;

    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        init_map_b(pr.key, pr.value.toString(), map_vis, map_b);
    }

    for (auto &it : map_b) {
        setVisual(it.first, !it.second);
    }
}

void PropertyDial::setDisableAll()
{
    std::map<QString , bool> map_b;

    for (auto *widget : list_widget) {
        PropertyStr pr = widget->property("col").value<PropertyStr>();
        init_map_b(pr.key, QString("%1").arg(pr.index), map_disable, map_b);
    }

    for (auto &it : map_b) {
        setReadOnly(it.first, !it.second);
    }
}

bool readCxema(const QString & schemeNum, QString &otop, QString &gv, QString &vent, QString &teh, QString &cond, QString &rez, QString &gvs_all, bool &isEl);

void PropertyDial::setPR(const QString & schemeNum)
{
    QString otop, gvs, vent, teh, cond, rez, gvs_all;
    bool isEl;

    if (readCxema(schemeNum, otop, gvs, vent, teh, cond, rez, gvs_all, isEl)) {
        setVis("elev", isEl ? "1" : "0");
        setVis("otop", otop);
        setVis("gvs", gvs);
        setVis("gvs_all", gvs_all);
        setVis("vent", vent);
        setVis("rez", rez);
    }
}

QVariant prop_fun1(QWidget *widget, const QString & fun, QVariant value, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    FunIndex fIndex = funIndex(fun);
    auto it = map_fun_id.find(fIndex);

    if (it != map_fun_id.end()) {
        bool ok, yes;
        PropertyStr pr;
        QVariant var = it->second(widget, nullptr, fun, pr, "", value, -1, label, db, &ok, &yes);
        if (yes) {
            if (ok) {
                return var;
            }
        }
    }
    
    return QVariant();
}

void PropertyDial::prop_fun(PropertyStr &pr)
{
    QString fun = pr.fun;

    bool ok, yes;

    FunIndex fIndex = funIndex(fun);

    if (fIndex == FUN_error) {
        fIndex = funIndex1(fun);
    }

    if (fIndex == FUN_error) {
        QMessageBox::warning(this, "", pr.fun);
        return;
    }

    auto it = map_fun_id.find(fIndex);

    if (it != map_fun_id.end()) {
        QVariant var = it->second(this, nullptr, fun, pr, pr.key, pr.value, pr.index, pr.label, *pr.db, &ok, &yes);
        if (yes) {
            if (ok) {
                setValue(pr.key, pr.id_col, var);
            }
            return;
        }
        else {
            QVariant var = it->second(this, this, fun,  pr, pr.key, pr.value, pr.index, pr.label, *pr.db, &ok, &yes);
            if (yes) {
                if (ok) {
                    setValue(pr.key, pr.id_col, var);
                }
                return;
            }
        }
    }

/*
   if (fIndex == FUN_File) {
        viewFile(this, pr.fun);
        return;
    }
    else if (fIndex == FUN_Dir) {
        QString dir = getDir(this, pr.fun);
        if (dir != "") {
            setValue(pr.key, dir);
        }
        return;
    }
    else if (fIndex == FUN_length) {  // Длина
    }

*/
    QMessageBox::warning(this, "", pr.fun);
}



void PropertyDial::prop_after(PropertyStr &pr)
{
//        QMessageBox::warning(this, "", pr.after);
    if (pr.after == "alseco_text") {
        QString mk = getValue("mkr2").toString();
        QString st = getValue("street2").toString();
        QString dm = getValue("house2").toString();


        double otop = getValue("otop").toDouble();
        double vent = getValue("vent").toDouble();
        double gvs  = getValue("gvs").toDouble();
        double par  = getValue("par").toDouble();

        QString getAlsecoTxt(const QString &mkr2, const QString &str2, const QString &dom2, double otop, double vent, double gvs, double par);

        QString txt = getAlsecoTxt(mk, st, dm, otop*1e6, vent*1e6, gvs*1e6, par*1e6);
        setValue1("txt", txt);
    }
}




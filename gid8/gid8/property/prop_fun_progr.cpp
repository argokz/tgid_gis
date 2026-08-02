#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <dialog/TableWindow.h>

#include "PropertyDial.h"

#include <ui/PotrcxemaDialog.h>
#include <ui/UtfDialog.h>
#include <ui/typepumpdialog.h>


#include <dialog/DatePopup.h>

#include <cxema/cxema1.h>

#include <db/db.h>
#include "geo/Klassif.h"


#define ZDANIYA_2 "zdaniya_2"


QVariant fun_no_prop(bool *ok, bool *yes)
{
    *ok = false;
    *yes = false;

    return QVariant();
}

QVariant fun_average3(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

// Климат

#include <ui2/OpenServer.h>

void init_cs(ConnectStr &cs);


QVariant fun_climat1(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    QString tn = "13_klimaticheskie_dannye";
    QString q = QString("SELECT * FROM [sprav].[dbo].[%1]").arg(tn);

    if (is_POSTGRESQL()) {
        q = QString("SELECT * FROM \"%1\"").arg(tn);
    }

    QString mdb = "sprav";
    ConnectStr cs;
    init_cs(cs);

    QSqlDatabase *db0 = &db;
    
    if (is_POSTGRESQL()) {
        db0 = new QSqlDatabase();
        bool ok = connectSQL0(cs.rdbms, cs.host, cs.port, mdb, cs.user, cs.password, *db0);
        if (!ok) return QVariant();
    }

    TableWindow tab(*db0, tn, label, q, widget, -1, false, true, true);
    if (tab.exec()) {
        prop->setValue1("nasel_point", tab.value("sity"));
        prop->setValue1("t_or", tab.value("otopl_pr"));
        prop->setValue1("t_vr", tab.value("vetn_pr"));
        prop->setValue1("tn_god", tab.value("t_sr"));
        *ok = false;
    }

    *yes = true;
    return QVariant();
}

QVariant fun_color(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

void diameter_std(PropertyDial *widget, QSqlDatabase &db);

// Стандартные диаметры для участкоа


bool diameter_std(QWidget *widget, QSqlDatabase &db, const QString & stand1, double & d1, double & d2, double & d3, double & th)
{
    QString  stand = "ГОСТ";
    QString tn = "standardTubes";
    QString label = "Выберите диаметр трубопровода";

//    int standardID = prop->getValue("standardID").toInt();
    int standardID = 1;
//    QString stand = prop->getValueString("standardID");

    if (stand == "ГОСТ" || stand == "Стандарт" || stand == "Россия") {
        stand = "ГОСТ', 'Стандарт', 'Россия";
    }

    QString q = QString("SELECT id, stand, diametr_usl, diamVne, diametr, tol FROM standardTubes WHERE stand='%1'").arg(standardID);

    q = QString("SELECT tr.id, st.name, diametr_usl, diamVne, diametr, tol\n"
            "FROM standardTubes tr\n"
            "JOIN standards st ON st.name=tr.stand OR (st.name IN ('ГОСТ', 'Стандарт', 'Россия') AND tr.stand IN ('ГОСТ', 'Стандарт', 'Россия'))\n"
            "WHERE st.name IN ('%1')").arg(stand);

    TableWindow tab(db, tn, label, q, widget, -1, false, true, true, true);
    if (tab.exec()) {
        d1 = tab.value("diametr_usl").toDouble();
        d2 = tab.value("diamVne").toDouble();
        d3 = tab.value("diametr").toDouble();
        th = tab.value("tol").toDouble();
        return true;
    }

    return false;
}


QVariant fun_diameter_std(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label0, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;

//    diameter_std(prop, db);

    QString tn = "standardTubes";
    QString label = "Выберите диаметр трубопровода";

    int standardID = prop->getValue("standardID").toInt();
    QString stand = prop->getValueString("standardID");

//    stand = stand.replace("ГОСТ", "ГОСТ', 'Стандарт', 'Россия")

//    QMessageBox::warning(widget, "", v.toString());

    if (stand == "ГОСТ" || stand == "Стандарт" || stand == "Россия" || stand == "") {
        stand = "ГОСТ', 'Стандарт', 'Россия";
    }

    QString q = QString("SELECT id, stand, diametr_usl, diamVne, diametr, tol FROM standardTubes WHERE stand='%1'").arg(standardID);

    q = QString("SELECT tr.id, st.name, diametr_usl, diamVne, diametr, tol\n"
            "FROM standardTubes tr\n"
            "JOIN standards st ON st.name=tr.stand OR (st.name IN ('ГОСТ', 'Стандарт', 'Россия') AND tr.stand IN ('ГОСТ', 'Стандарт', 'Россия'))\n"
            "WHERE st.name IN ('%1')").arg(stand);

    TableWindow tab(db, tn, label, q, widget, -1, false, true, true, true);
    if (tab.exec()) {
        double d1 = tab.value("diametr_usl").toDouble();
        double d2 = tab.value("diamVne").toDouble();
        double d3 = tab.value("diametr").toDouble();
        double th = tab.value("tol").toDouble();

        prop->setValue("diameterCondit", pr.id_col, d1);
        prop->setValue("diameterExternal", pr.id_col, d2);
        prop->setValue("diameterInternal", pr.id_col, d3);
        prop->setValue("wallThickness", pr.id_col, th);
        *ok = false;
    }

    return QVariant();
}

QVariant fun_diameter_std2(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

// Выбор даты

QVariant fun_Date(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    *ok = false;
    *yes = true;

    DatePopup dlg(widget);
    dlg.setSelectedDate(value.toDate());
    if (dlg.exec() == QDialog::Accepted) {
        *ok = true;
        return dlg.selectedDate();
    }

    QVariant var;
    return var;
}

QVariant fun_Dir(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    *yes = true;

    QSettings settings;

    QString dir = value.toString();
    
    QString d = QFileDialog::getExistingDirectory(widget, label, dir); //,  QFileDialog::ShowDirsOnly
                    //| QFileDialog::DontResolveSymlinks | QFileDialog::DontUseCustomDirectoryIcons);
    *ok = d != "";
    return d;
}

QString getRealPath(const QString & code)
{
    QSettings settings;
    QString path = settings.value("paths/" + code, ".").toString();
    return path;
}

#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>

bool openDocument(const QString &filePath)
{
    QFileInfo file(filePath);
    if (!file.exists()) {
        qWarning("Файл не найден: %s", qUtf8Printable(filePath));
        return false;
    }

    // Преобразуем путь в URL с префиксом file:///
    QUrl url = QUrl::fromLocalFile(file.absoluteFilePath());
    return QDesktopServices::openUrl(url);
}

QString viewFile(QWidget *widget, const QString & fun, const QString & fn)
{
    static QRegularExpression re("File \"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*$");
    QRegularExpressionMatch match = re.match(fun);
    if (match.hasMatch()) {
        QString ext =  match.captured(1);
        QString filter = match.captured(2);
        QString path =  match.captured(3);
        path = getRealPath(path);
        
        if (fn == "") {
            QString fileName = QFileDialog::getOpenFileName(widget, ext, path, filter);
            if (!fileName.isEmpty()) {
                return fileName;
            }
        }
        else {
            QString fileName = path + "/" + fn;
            if (QFile::exists(fileName)) {
                //QMessageBox::warning(widget, "", QString("Файл %1 найден").arg(fileName));
                openDocument(fileName);
            }
            else {
                QMessageBox::warning(widget, "", QString("Файл %1 не найден").arg(fileName));
            }
        }
    }
    return "";
}

QVariant fun_File(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    *yes = true;

    QVariant f = viewFile(widget, fun, value.toString());
    *ok = f != "";
    return f;
}

QVariant fun_FileView(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

#include <ui/KtiDialog.h>

QVariant fun_heatTestsCoeff(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;

    CKtiDialog dlg(widget);

    if (dlg.exec() == QDialog::Accepted) {
        *ok = true;
        return 0.;
    }
    *ok = false;
    return 0.;
}

QVariant fun_Hidden(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QVariant fun_hydroRes(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QVariant fun_ind_result(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QVariant fun_ind_save(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QVariant fun_length(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}


#include <QString>
#include <QRegularExpression>
#include <QJSEngine>
#include <QDebug>

// Пример функции, возвращающей значение переменной по имени
double get_value(const QString& name) {
    if (name == "a1") return 10;
    if (name == "a2") return 3;
    if (name == "s3") return 5;
    return 0.0;
}

// Основная функция для вычисления выражения
bool evaluate_expression(QString expression, PropertyDial *prop, double &d) {
    // Регулярное выражение для поиска переменных (буквы + цифры)
    QRegularExpression re(R"([a-zA-Z_]\w*)");
    QRegularExpressionMatchIterator it = re.globalMatch(expression);

    // Множество переменных, чтобы избежать повторной замены
    QSet<QString> seen;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString var = match.captured();

        if (!seen.contains(var)) {
            double val = prop->getValue(var).toDouble();
            expression.replace(QRegularExpression("\\b" + QRegularExpression::escape(var) + "\\b"),
                               QString::number(val));
            seen.insert(var);
        }
    }

    // Теперь expression содержит чисто числа и операторы: "10 + 3 * 5 / 5"
    QJSEngine engine;
    QJSValue result = engine.evaluate(expression);

    if (result.isError()) {
        qWarning() << "Ошибка в выражении:" << result.toString();
        return false;
    }

    d = result.toNumber();

    return true;
}

#if 0
// Пример использования
int main() {
    QString expr = "a1+a2*s3/5";
    double result = evaluate_expression(expr);
    qDebug() << "Результат:" << result; // Пример: 10 + 3 * 5 / 5 = 13
    return 0;
}

#endif




QVariant fun_Math(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;

    QRegularExpression regex("Math\\s+([^ ]+)$");
    QRegularExpressionMatch match = regex.match(pr.fun);

    if (match.hasMatch()) {
        QString ss = match.captured(1);

        double d = 0;
        
        if (evaluate_expression(ss, prop, d)) {
            ss = QString("Установить значение поля %1").arg(d);
            if (QMessageBox::question(widget, "", ss) == QMessageBox::Yes) {
                *ok = true;
                return QVariant(d);
            }
        }
    }
    else {
        QRegularExpression regex("Math\\s+([^ ]+)\\s+(.+)$");
        QRegularExpressionMatch match = regex.match(pr.fun);

        if (match.hasMatch()) {
            QString ss = match.captured(1);
            QString ss2 = match.captured(2);
            double d = 0;

//            QString c24 = AfxGetApp()->GetProfileString(szSection, "coef24", "2.4");
            QSettings settings;
            double c24 =  settings.value("coef24", 2.4).toDouble();

            ss2.replace("$coef24$", QString("%1").arg(c24));

            if (evaluate_expression(ss, prop, d)) {
                ss = QString("Установить значение поля %1\n%2").arg(d).arg(ss2);
                if (QMessageBox::question(widget, "", ss) == QMessageBox::Yes) {
//                SetItemValue(item, d);
                    *ok = true;
                    return QVariant(d);
                }
            }
        }
    }

    *yes = true;
    return QVariant();
}

bool viewLookup(QWidget *widget, PropertyStr &pr, QString &text, int &index);


QVariant fun_ms_rs(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;

    QString text;
    int index;

    if (viewLookup(prop, pr, text, index)) {
        PropertyStr pr1 = prop->getPr("magistral", pr.id_col);
        PropertyStr pr2 = prop->getPr("magistralSite", pr.id_col);
        PropertyStr pr3 = prop->getPr("distSite", pr.id_col);

        if (key == "magistral") {
            prop->setIdValue(pr3, -1, "");
        }
        if (key == "magistralSite") {

            QString q = QString("SELECT magistral FROM uchastok_ms WHERE id=%1").arg(index);

            int id = readTableInt(db, q);

            if (id > 0) {
                prop->setId(pr1, id);
            }
            else {
                prop->setIdValue(pr1, -1, "");
            }
            prop->setIdValue(pr3, -1, "");
        }
        if (key == "distSite") {
            prop->setIdValue(pr1, -1, "");
            prop->setIdValue(pr2, -1, "");
        }

        prop->setIdValue(pr, index, text);
        *ok = false;
    }

    return 0;
}

QVariant fun_NagrSum(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

#include <ui/Nagruz_zdanDialog.h>


double get_alpha(QSqlDatabase &db, double t)
{
    double t1 = 0, t2, a1 = 0, a2;

    QSqlQuery query(db);
    query.setForwardOnly(true);

    QString q = "SELECT * FROM sprav.dbo.[40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy]";

    if (!is_MSSQL()) {
        q = "SELECT * FROM \"40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy\"";
    }



    if (query_exec(db, query, q)) {
        while (query.next()) {
            t2 = query.value("t_r").toDouble();
            a2 = query.value("alpha").toDouble();

            if (t > t2) {
                break;
            }

            t1 = t2;
            a1 = a2;
        }
    }

    double propor = (t - t1) / (t2 - t1);
    if (t2 == t1) return 1;

    return a1 * (1. - propor) + a2 * propor;
}

double Raschet(QSqlDatabase &db, double m_tn, double m_tvn, double m_v, int m_god, const QString & m_nazn, bool m_is_otopl)
{
    double m_alpha = m_tn;
    double m_udel;

    int num = -1;

    QString q, q2 = "";

    if (m_nazn == "Жилое") {
        if (m_god <= 1958) {
            q2 = QString(" AND (pr_year='С')").arg(m_nazn);
        }
        else {
            q2 = QString(" AND (pr_year='Н')").arg(m_nazn);
        }
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);

    q = QString("SELECT Vmin, Vmax, q_ot, q_vt FROM sprav.dbo.[10_teplovye_harakteristiki_zdaniy] WHERE name_zd='%1' %2").arg(m_nazn).arg(q2);

    if (!is_MSSQL()) {
//        QString buildValuesFromCsv(const QString &csvPath);
//        v = buildValuesFromCsv(const QString &csvPath);
        q = QString("SELECT Vmin, Vmax, q_ot, q_vt FROM \"10_teplovye_harakteristiki_zdaniy\" WHERE name_zd='%1' %2").arg(m_nazn).arg(q2);
    }

    q = readQ("sql/sp/s10.sql");

    if (query_exec(db, query, q)) {
        while (query.next()) {
            double v1, v2;
            v1 = query.value("Vmin").toDouble();
            v2 = query.value("Vmax").toDouble();

            if (v1 <= m_v && m_v <= v2) {
                if (m_is_otopl) {
                    m_udel = query.value("q_ot").toDouble();
                }
                else {
                    m_udel = query.value("q_vt").toDouble();
                }
            }
        }
    }

    m_alpha = get_alpha(db, m_tn);
    double m_nagr = m_udel*m_v*m_alpha*(m_tvn-m_tn)/1000000;

    return m_nagr;
}

#include <gid_data/heatSystem.h>
#include <alseco/alsecodlg.h>

QVariant fun_alseco_nagr(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;

//    QMessageBox::warning(widget, "", "fun_alseco_nagr");

    Klassif* kls = m_kl_list.findKlN(ZDANIYA_2);


    BigDialog dlg(*kls->m_db, pr.id, widget);

    if (dlg.exec() == QDialog::Accepted) {
        double q = (dlg._otop+dlg._gvs+dlg._vent+dlg._par)/1e6;


        prop->setValue1("mkr2", dlg.mk    );
        prop->setValue1("street2", dlg.st    );
        prop->setValue1("house2", dlg.dm    );
        prop->setValue1("otop", dlg._otop/1e6 );
        prop->setValue1("gvs", dlg._gvs/1e6  );
        prop->setValue1("vent", dlg._vent/1e6 );
        prop->setValue1("par", dlg._par/1e6  );
        prop->setValue1("nagr", q);

        QString getAlsecoTxt(const QString &mkr2, const QString &str2, const QString &dom2, double otop, double vent, double gvs, double par);

        QString txt = getAlsecoTxt(dlg.mk, dlg.st, dlg.dm, dlg._otop, dlg._vent, dlg._gvs, dlg._par);

        if (q == 0 ) {
            prop->setValue1("mkr2", ""   );
            prop->setValue1("street2", ""    );
            prop->setValue1("house2", ""    );
            prop->setValue1("number_1", ""    );
        }

        prop->setValue1("txt", txt);
    }
    
    *ok = false;
    return 0.;
}

#include <gidview/GidWidget.h>

GidWidget *getView();


QVariant fun_alsecoNagr(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;

    GidWidget *view = getView();
    Klassif* kls = m_kl_list.findKlN(ZDANIYA_2);

    if (view && kls && kls->ins) {
//        int kodID = prop->getValue("externalCodeID").toInt();
        QString kod = prop->getValueString("externalCodeID");
        QString name = prop->getValue("externalNodeName").toString();

        view->alseco(prop, pr.table, pr.key, kod, name);
    }
    
    *ok = false;
    return 0.;
}

QVariant fun_nagruz_ot(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    Klassif* kls = m_kl_list.findKlN(ZDANIYA_2);
    if (kls && kls->ins) {
        return fun_alsecoNagr(widget, prop, fun, pr, key, value, id, label, db, ok, yes);
    }

    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;

    CNagruz_zdanDialog dlg(widget);


    double t_or = 18;
    double t_vr = 18;
    double m_tvn = prop->getValue("calcTempHR").toDouble();
    int buildingTypeID = prop->getValue("buildingTypeID").toInt();

/*
    //        GetNameValue("calcTempVS", dlg.m_tvn);
    long lParam = (fun == "nagruz_ot") ? 5 : 6;

*/
    QString q;

    C_heatSystem d;

    getCT(db, d);

    t_or = d.t_or;
    t_vr = d.t_vr;

    QSqlQuery query(db);
    query.setForwardOnly(true);

    q = "SELECT DISTINCT min(id) AS id, name_zd FROM sprav.dbo.[10_teplovye_harakteristiki_zdaniy] GROUP BY name_zd";

    if (!is_MSSQL()) {
        q = "SELECT DISTINCT min(id) AS id, name_zd FROM \"10_teplovye_harakteristiki_zdaniy\" GROUP BY name_zd";

//        QString buildValuesFromCsv(const QString &csvPath);

        QString buildValuesFromCsv(const QString &csvContent, const QChar delimiter = ';');

        QString csv = readQUTF8("sprav/s10.csv");
        QString vals =  buildValuesFromCsv(csv);

        q = QString("SELECT DISTINCT min(id) AS id, name_zd "
                    "FROM (VALUES %1) AS tab(id,nomer,sategory,name_zd,tr_tv_co,vmin,vmax,pr_year,q_ot,q_vt) "
                    "GROUP BY name_zd").arg(vals);
    }

    if (query_exec(db, query, q)) {
        int i = 0; 
        while (query.next()) {
            int id = query.value("id").toInt();
            QString s = query.value("name_zd").toString();
            dlg.ui.combo_Combo1->addItem(s);
            if (id == 66) { // Жилое
                dlg.ui.combo_Combo1->setCurrentIndex(i);
            }
            i ++;
        }
    }

    dlg.m_strEdit2 = "0";      // Год постройки
    dlg.m_strEdit3 = "0";      // Наружный объем
    dlg.m_strEdit5 = QString("%1").arg(m_tvn);  // Расчетная температура наружного воздуха
    dlg.m_strEdit6 = QString("%1").arg(t_or);   // Расчетная температура внутри помещения
    dlg.m_strEdit7 = "1";      // Поправочный коэфф. t наружного. воздуха
    dlg.m_strEdit4 = "0";      // Удельная характеристика здания
    dlg.m_strEdit8 = "0";      // Расчетная тепловая нагрузка

    dlg.write();

    if (dlg.exec() == QDialog::Accepted) {
        *ok = true;
        return 0.;
    }
    *ok = false;
    return 0.;
}

QVariant fun_nagruz_vt(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;

    CNagruz_zdanDialog dlg(widget);

    if (dlg.exec() == QDialog::Accepted) {
        *ok = true;
        return 0.;
    }
    *ok = false;
    return 0.;
}

#include <QRegularExpression>

int getNumb(const QString & txt)
{
    static QRegularExpression re("^[^0-9]*([0-9]+)[^0-9]*$");
    QRegularExpressionMatch match = re.match(txt);
    if (match.hasMatch()) {
        int m = match.captured(1).toInt();
        return m;
    }
    return 0;
}

// Название ПС по обычному
QVariant fun_nodeName(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;
    *ok = true;

    QString kod_id = prop->getValue("standardID").toString();
    QString name = prop->getValue("externalNodeName").toString();
    int numb = getNumb(kod_id);

    QString ss = QString("%1(%2)").arg(name).arg(numb);
//    SetNameValue("nodeName", ss);
    prop->setValue(key, pr.id_col, ss);
    return QVariant();
}

QVariant fun_nodes(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;
    *ok = false;

    CCxema *cxema = prop->getCxema();

    if (cxema) {
        QString q = readQ("sql4/find_node.sql");
        q.replace(QString("$fileID$"), cxema->m_par);

        TableWindow tab(cxema->m_db, "node", "Выберите узел", q, widget, id, true, true, true);
        if (tab.exec()) {
            int index = tab.value();
            QString text = tab.value(2).toString();

            prop->setIdValue(pr, index, text);
        }
    }

    return QVariant();

    
    
//    return fun_no_prop(ok, yes);
}

QVariant fun_nodes_pr(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *yes = true;
    *ok = false;

    CCxema *cxema = prop->getCxema();

    if (cxema) {
        QString q = readQ("sql4/find_node_pr.sql");
        q.replace(QString("$fileID$"), cxema->m_par);

        TableWindow tab(cxema->m_db, "node", "Выберите узел", q, widget, id, true, true, true);
        if (tab.exec()) {
            int index = tab.value();
            QString text = tab.value(2).toString();

            prop->setIdValue(pr, index, text);
        }
    }

    return QVariant();

    
    
//    return fun_no_prop(ok, yes);
}



QVariant fun_node_pick(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QVariant fun_node_pick1(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QVariant fun_node_pick2(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QVariant fun_NotNull(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QVariant fun_ReadOnly(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}


QString fun_schemeNum(PropertyDial *parent, const QString & schemeNum);

// Номер схемы потребителя

QVariant fun_schemeNum(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    QString cx = fun_schemeNum(prop, value.toString());
    if (cx != "") {
        prop->setValue(key, pr.id_col, cx);
        prop->setPR(cx);
        *ok = false;
    }

    *yes = true;
    return QVariant();
}

QVariant fun_standardDampLink(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

QString viewTableQ(QWidget *widget, QSqlDatabase &db, const QString & label, const QString & fun, bool *ok);
QString viewTable(QWidget *widget, QSqlDatabase &db, const QString & label, const QString & fun, bool *ok);

QVariant fun_Table(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    *yes = true;
    return viewTable(widget, db, label, fun, ok);
}

QVariant fun_TableQ(QWidget *widget, PropertyDial *prop, const QString & fun0, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);
    *ok = false;

    *yes = true;

    QString fun = fun0;

    int magID = prop->getValue("fragment_resultID").toInt();
    QString kod_m = prop->getValue("kod_m").toString();

    fun = fun.replace("$magID$", QString("%1").arg(magID));
    fun = fun.replace("$kod_m$", kod_m);

    return viewTableQ(widget, db, label, fun, ok);
}


QVariant fun_uf150_70(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);

bool utfDialogCallback(CUtfDialog *prop)
{
    prop->ui.edit_T1->setEnabled(prop->m_radioM1 == 0);
    prop->ui.edit_T2->setEnabled(prop->m_radioM1 == 0);
    prop->ui.edit_T3->setEnabled(prop->m_radioM1 == 0);
    prop->ui.edit_G->setEnabled(prop->m_radioM1 != 0);
    prop->ui.edit_G0->setEnabled(prop->m_radioM1 != 0);

    double t1 = prop->m_strT1.toDouble();
    double t2 = prop->m_strT2.toDouble();
    double t3 = prop->m_strT3.toDouble();

    double t = (t1-t3)/(t3-t2);

    if (t3 > t2) {
        prop->m_strUf = QString("%1").arg(t);
    }
    else {
        prop->m_strUf = QObject::tr("Ошибка");
    }
    
    return true;
}


double utf(QWidget *widget, double t1, double t2, double t3, bool *ok )
{
    CUtfDialog dlg(widget);

    dlg.m_radioM1 = 0;

    dlg.m_strT1 = QString("%1").arg(t1);
    dlg.m_strT2 = QString("%1").arg(t2);
    dlg.m_strT3 = QString("%1").arg(t3);

    dlg.setCallback(utfDialogCallback);
    utfDialogCallback(&dlg);

    dlg.write();

    if (dlg.exec() == QDialog::Accepted) {
        *ok = true;
        return dlg.m_strUf.toDouble();
    }
    *ok = false;
    return 0.;
}

QVariant fun_uf(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);
    *yes = true;

    double t1 = prop->getValue("t1_r").toDouble();
    double t2 = prop->getValue("t2_r").toDouble();
    double t3 = prop->getValue("t3_r").toDouble();

    double u = utf(widget, t1, t2, t3, ok);

    if (ok && u > 0) {
        prop->setValue(key, pr.id_col, QVariant(u));
        *ok = false;
    }

    return QVariant();
}

// 

QVariant fun_uf150_70(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    *ok = false;
    *yes = true;

    double t1 = 150;
    double t2 = 70;
    double t3 = 95;
    if (prop) {
    }

    double u = utf(widget, t1, t2, t3, ok);

    return QVariant(u);
}

QVariant fun_utverdit(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

#include <dialog/Nasos1Dialog.h>

QVariant fun_viewNsDb(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *ok = false;
    *yes = true;

    int countPumps = prop->getValue("parallAgregCount").toInt();

    int rotorDiameterTypeID = prop->getValue("rotorDiameterTypeID").toInt();
    int driveTypeID = prop->getValue("driveTypeID").toInt();
    

    double diam_zad = prop->getValue("rotorDiameterSet").toDouble();
    double rate_zad = prop->getValue("rotorRotSpeedSet").toDouble();

    TypePumpDialog dlg(widget, db, id, countPumps, rotorDiameterTypeID, diam_zad, driveTypeID, rate_zad);
    if (dlg.exec()) {
        qDebug()<<"dlg.pReport.apH.r0 = " + QString::number(dlg.pReport.apH.r0);
        prop->setValue("r0", pr.id_col, dlg.pReport.apH.r0);
        prop->setValue("r1", pr.id_col, dlg.pReport.apH.r1);
        prop->setValue("r2", pr.id_col, dlg.pReport.apH.r2);
        prop->setValue("e0", pr.id_col, dlg.pReport.apN.r0);
        prop->setValue("e1", pr.id_col, dlg.pReport.apN.r1);
        prop->setValue("e2", pr.id_col, dlg.pReport.apN.r2);
        prop->setValue("k0", pr.id_col, dlg.pReport.apKPD.r0);
        prop->setValue("k1", pr.id_col,  dlg.pReport.apKPD.r1);
        prop->setValue("k2", pr.id_col,  dlg.pReport.apKPD.r2);

        prop->setValue("r0_z", pr.id_col, dlg.pReport.apHr.r0);
        prop->setValue("r1_z", pr.id_col, dlg.pReport.apHr.r1);
        prop->setValue("r2_z", pr.id_col, dlg.pReport.apHr.r2);
        prop->setValue("e0_z", pr.id_col, dlg.pReport.apNr.r0);
        prop->setValue("e1_z", pr.id_col, dlg.pReport.apNr.r1);
        prop->setValue("e2_z", pr.id_col, dlg.pReport.apNr.r2);
        prop->setValue("k0_z", pr.id_col, dlg.pReport.apKPDr.r0);
        prop->setValue("k1_z", pr.id_col,  dlg.pReport.apKPDr.r1);
        prop->setValue("k2_z", pr.id_col,  dlg.pReport.apKPDr.r2);


        prop->setId(pr,dlg.pReport.pump_id);
        prop->setValue("rotorRotSpeedSet", pr.id_col, dlg.pReport.f);
        prop->setValue("rotorDiameterSet", pr.id_col, dlg.pReport.d);
        prop->setValue("parallAgregCount", pr.id_col, dlg.pReport.k_kol);


        PropertyStr s1 = prop->getPr("driveTypeID", pr.id_col);
        PropertyStr s2 = prop->getPr("rotorDiameterTypeID", pr.id_col);
        prop->setId(s1, dlg.pReport.driveTypeID);
        prop->setId(s2, dlg.pReport.rotorDiameterTypeID);

        *ok = false;
    };
    return QVariant();
}

QVariant fun_dolya(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    return fun_no_prop(ok, yes);
}

#include "dialog/MestSopr3.h"


QVariant fun_mestn(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *ok = false;
    *yes = true;

    QString mestn = prop->getValue("mestn").toString();


    int id1 = prop->m_id1;

    MestSopr3 dlg(widget, db, id1, mestn);

    if (dlg.exec() ) {
        prop->setValue(key, pr.id_col, QVariant(dlg.getSum()));
        prop->setValue("mestn", pr.id_col, QVariant(dlg.getSumList()));
        *ok = false;
    }
    return QVariant();
}




QVariant fun_nodes_new_baza(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *ok = false;
    *yes = true;

    QStringList parts = fun.split(" ", Qt::SkipEmptyParts);

    if (parts.length() < 3) return QVariant();

    QString s_typeID = parts[1];
    QString s_nodeID = parts[2];

//    QString q = readQ("sql5/nodes.sql");
    QString q = readQ("sql5/nodes_rd.sql");

    q.replace("$id$", QString("%1").arg(prop->m_id1));
    q.replace("$table$", "new_baza.rd");

    TableWindow tab(db, "nodes_new_baza", label, q, widget, -1, true, true, true, true);

    tab.hide_column(1);
    tab.hide_column(6);


    if (tab.exec()) {
        *ok = true;

        int nodeID = tab.value("id").toInt();
        int typeID = tab.value("typ").toInt();
        QString typ_name = tab.value("typ_name").toString();
        QString kod = tab.value("kod").toString();
        QString nodeName = tab.value("nodeName").toString();

        PropertyStr pr1 = prop->getPr(s_nodeID, pr.id_col);
        PropertyStr pr2 = prop->getPr(s_typeID, pr.id_col);

        prop->setIdValue(pr1, nodeID, nodeName);
        prop->setIdValue(pr2, typeID, typ_name);
    }

    return QVariant();
}


QVariant fun_list_of_coluns(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *ok = false;
    *yes = true;

    auto it1 = prop->m_params.find("q");
    if (it1 == prop->m_params.end()) {
        return QVariant();
    }
    QString q = it1->second.toString();

    auto it2 = prop->m_params.find("db");
    if (it2 == prop->m_params.end()) {
        return QVariant();
    }

    QString connectionName = it2->second.toString();
    QSqlDatabase db2 = QSqlDatabase::database(connectionName);

    TableWindow tab(db2, "", label, q, widget, -1, false, true, false, true);

    if (tab.exec()) {
        prop->setValue(key, pr.id_col, tab.value(0));
        *ok = false;
    }

    return QVariant();
}


QVariant fun_viewText(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *ok = false;
    *yes = true;

    QString text = value.toString();

    QRegularExpression regex("\\\\n(?=[а-я ])");
    text = text.replace(regex, " ");
    text.replace("\\n","</p><p>");

    text = "<html><p>"+text+"</p></html>";

    QMessageBox::warning(widget, label, text);

    return QVariant();
}


QVariant fun_list_db(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *ok = false;
    *yes = true;

    QString ilike = "LIKE";
    if (is_POSTGRESQL()) {
        ilike = "ILIKE";
    }

    QString q = QString("select distinct %1 from %2 where %1 <> '' and %1 is not null \n--AND_TXT and %1 %3 '%$txt$%' \n order by %1").arg(pr.key, pr.table, ilike);

    TableWindow tab(db, pr.table, label, q, widget, -1, true, true, true);
    if (tab.exec()) {
        prop->setValue1(pr.key, tab.value(pr.key));
        *ok = true;
        return tab.value(pr.key);
    }

    *yes = true;
    return QVariant();
}



QVariant fun_addr_nagruzki(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes)
{
    if (!prop) return fun_no_prop(ok, yes);

    *ok = false;
    *yes = true;

    QString stand = prop->getValueString("standardID");

    QString city   = prop->getValueString("city");
    QString mkr    = prop->getValueString("mkr");
    QString street = prop->getValueString("street");
    QString house  = prop->getValueString("house");

    QString addr = "";

    if (city != "") {
        if (addr != "") addr += ", ";
        addr += city;
    }
    if (mkr != "") {
        if (addr != "") addr += ", ";
        addr += mkr;
    }
    if (street != "") {
        if (addr != "") addr += ", ";
        addr += street;
    }
    if (house != "") {
        if (addr != "") addr += ", д.";
        addr += house;
    }
    
    *ok = true;
    return addr;
}

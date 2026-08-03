#pragma once

#include <QtSql/QSqlDatabase>
#include <cxema/col_index.h>
#include <cxema/graph2.h>
#include <QProgressDialog>
#include <ui/PercentDialog.h>

class GidWidget;

#define TCHAR char

struct CConnect
{
    int rdbms = 0;
    QString host, baza, user, password;
    int port;
};

struct Heating_seasons {
    int y;
    QDate d0;
    QDate d1;
    QDate d2;
};


struct CText
{
  int n;
  int fileID;
  int internalNodeID;
  QString text;
//  LOGFONT lf;
  CFPoint coord;
  QRectF rect;

};


struct Fragment {
    long m_id; // int(11) NOT NULL AUTO_INCREMENT,
    QString m_name; // varchar(50) DEFAULT NULL,
    QString m_name_sys; // varchar(50) DEFAULT NULL COMMENT 'Наименование системы теплоснабжения',
    QString m_nasel_point; // varchar(50) DEFAULT NULL COMMENT 'Населенный пункт (для определения многолетних климатических данных), ссылка тбл. 13_Климатические данные, Справочная',
    QString m_sezon; // varchar(50) DEFAULT NULL COMMENT 'Признак отопительного периода: З-зимний, Л-летний',
    QString m_year; // varchar(50) DEFAULT NULL COMMENT 'Календарные года отопительного периода',
    double m_t_or; // double DEFAULT -32 COMMENT 'Расчетная температура наружного воздуха для отопления отопительного сезона, Град.С , ссылка тбл. 13_Климатические данные, Справочная',
    double m_t_vr; // double DEFAULT -32 COMMENT 'Расчетная температура наружного воздуха для вентиляции, ссылка тбл. 13_Климатические данные, Справочная',
    double m_t_vnew; // double DEFAULT 8 COMMENT 'Температура наружного воздуха конца(начала) отопительного сезона, Град.С',
    double m_tx; // double DEFAULT 5 COMMENT 'Расчетная температура подпитки холодной воды, отопительный период Град.С',
    double m_tx_leto; // double DEFAULT 15 COMMENT 'Расчетная температура подпитки холодной воды, летний период Град.С',
    QString m_tip_Qgvs; // varchar(50) DEFAULT NULL COMMENT 'Задаваемое значение нагрузки горячего водоснабжения в системах горячего водоснабжения тепловых пунктах системы теплоснабжения',
//  `begin_year` DATE DEFAULT NULL COMMENT 'Дата начала отопительного периода',
//  `end_year` DATE DEFAULT NULL COMMENT 'Дата окончания отопительного периода',
    double m_tn_god; // double DEFAULT 0 COMMENT 'Среднесезонная температура наружного воздуха в отопительный период',
    double m_tg_god; // double DEFAULT 0 COMMENT 'Среднесезонная температура грунта в отопительный период',
    double m_tn_god_leto; // double DEFAULT 0 COMMENT 'Среднесезонная температура наружного воздуха в летний период',
    double m_tg_god_leto; // double DEFAULT 0 COMMENT 'Среднесезонная температура грунта в летний период',
    double m_a; // double DEFAULT 0.25 COMMENT 'Нормативное удельное значение потерь сетевой воды с утечкой из ТС',
//  `year_audit` DATE DEFAULT NULL COMMENT 'Отопительный сезон аудитной проверки',
    double m_tvn_1; // double DEFAULT 8 COMMENT 'Температура наружного воздуха конца(начала) отопительного сезона (1-ая характерная точка), Град.С',
    double m_tvn_2; // double DEFAULT 0 COMMENT 'Температура наружного воздуха нижней срезки (излома) графика (2-ая характерная точка), Град.С',
    double m_tvn_3; // double DEFAULT -15 COMMENT 'Промежуточная температура наружного воздуха между нижней и верхней срезками графика (3-ая характерная точка), Град.С',
    double m_tvn_4; // double DEFAULT -32 COMMENT 'Температура наружного воздуха верхней срезки (излома) графика (4-ая характерная точка), Град.С',
    double m_tvn_5; // double DEFAULT -32 COMMENT 'Расчетная температура наружного воздуха для отопления (5-ая характерная точка), Град.С , ссылка тбл. 13_Климатические данные, Справочная',

    QString m_phone_manager; // varchar(50) DEFAULT NULL COMMENT 'Телефон/факс руководителя: Ссылка на тблТелфонСправочн',
};


struct NAGR
{
    double d_otopl_zco;
    double d_otopl_nco;
    double d_ventil_cv;
    double d_otopl_pr;
    double d_gv_pr;
    double d_rez_pr;
    double d_otopl_sm;
    double d_gv_sm;
    double d_rez_sm;
    double d_otopl_ps;
    double d_gv_ps;
    double d_rez_ps;
    double d_otopl_pw;
    double d_gv_pw;
    double d_rez_pw;
    double d_gv_op;
    double d_gv_oo;
    double d_rez_op;

    double d_otoplz;
    double d_otopln;
    double d_otn_fs;
    double d_otopl_tp;
    double d_ventil;
    double d_dolja_vent;
    double d_kondiz;
    double d_txz;
    double d_txop;
    double d_txoo;
    double d_gvop;
    double d_gvoo;
    double d_gvpr;
    double d_gvsm;
    double d_gvps;
    double d_gvpw;
};

enum SignLine {
    SignLine_ob = 1,
    SignLine_pp = 2,
    SignLine_oo = 3,
    SignLine_po = 4,
    SignLine_op = 5
};

enum SignNode {
    SignNode_ob = 1,
    SignNode_p = 2,
    SignNode_o = 3
};


enum { 
    TIP_TEPLO,
    TIP_VODA,
    TIP_KANAL
};

struct UCH2F
{
    int typ;
    QString table;
};


struct NODEF
{
    int typ_of_program;
    int tr1, tr2;
    int typ;
    QString code;
    QString table;
    QString tableMySQL;
    QString table1;
    QString opis;
    QString out;
};

struct LINEF
{
    int typ_of_program;
    int tr1, tr2;
    int typ;
    QString code;
    QString table;
    QString tableMySQL;
    QString table1;
    QString opis;
    QString out;
    QString otkr;
    signed char n_sost;
    signed char n_diam;
    signed char n_dlina;
};

struct LINEFOUT
{
    QString q;
    QString table;
    int typ;
};

struct menuitem {
    QString name;
    QString kode;
    QString query;
    QString nameMySQL;
    int node_line;
};

extern int nodefsize;
extern int linefsize;
extern int uch2fsize;
extern int menuitemsize;

extern NODEF nodef[];
extern LINEF linef[];
extern menuitem menudocUnvis[];


QString getOutTable(const QString & tn);

QString getLineTable(int typ);
QString getLineCode(int typ);
QString getLineTableMySQL(int typ);
QString getLineTableOutMySQL(int typ);

QString getNodeTable(int typ);
QString getNodeCode(int typ);
QString getNodeTableMySQL(int typ);
QString getNodeTableOutMySQL(int typ);


int getLineTypOut(const QString & tn);
int getNodeTypOut(const QString & tn);

int getLineTyp(const QString & tn);
int getNodeTyp(const QString & tn);

int getLineTyp2(const QString & tn);
int getNodeTyp2(const QString & tn);


int getLineTypName(const QString & tn);
int getNodeTypName(const QString & tn);

struct RasCx {
    int id;
    QString name;

    int fileID;
    int belongMagistral; // Принадлежность магистрали
    int objectID; // Объект РС, ссылка на Вспомогательную таблицу B10_Виды объектов расчетной схемы',
    int heatSourceID; // Код источника тепла , ссылка на Источник
};

struct CGidFile
{
    CGidFile() {
        m_rect = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);
        name = "";
        m_schema = "";
        id = 0;
    }

    long id;
    QString  name;
    QString  m_schema;
    CFRect m_rect;
    list<RasCx> m_list_rs;
};

class CCxema
{
public:
    CCxema();
    virtual ~CCxema();

    CFRect rect() { return m_rect; };
    void setRect(CFRect rect) { m_rect = rect; };
    CGraph2 *graph() { return m_graph; };

    bool isOpen() { return true;};

    RasCx *getRasCx(int id);

    bool addText(CFPoint, const QString &text, int fileID, int parent_id);

    bool connectSQL(int rdbms, const QString & host, int port, const QString & baza, const QString & user, const QString & password);
    bool connectSQL2(int rdbms, const QString & host, int port, const QString & baza, const QString & user, const QString & password);
    bool connectSqlite(const QString & baza);
    bool openZN();
    bool openUP();
    bool open_cxema(GidWidget *view, const QString & par, QProgressDialog *percent);
    bool open_cxema_2(const QString & par, QProgressDialog *percent);
    bool open_heating_seasons();

    Heating_seasons get_Heating_seasons(int y);

    bool print();
    QString getFileName(int fileID);
    QString getFileName();

    bool info(CFPoint point, double delta, QWidget *window);

    bool setLength(CLINE2 *ll, bool ismap, bool ask);

    bool isGeobaza() {return true;};
    int fileID() {return m_fileID;};

    Fragment *getFragment(int id);

    CGraph2 *m_graph = nullptr;
    CGraph2 *m_graph_old = nullptr;
    QSqlDatabase m_db;
    QSqlDatabase m_db_2; // для синхронизации

    bool read_lines(const QString & par, QProgressDialog *percent, bool dubl);
    bool read_nodes(const QString & par, QProgressDialog *percent, bool first);

    bool openFragments(const QString & par);

private:
    CFRect m_rect;

    bool openheatSystem();
    bool openIst();
    bool read_text(const QString &par, QProgressDialog *percent);
//    bool read_lines(const QString & par, QProgressDialog *percent, bool dubl);
//    bool read_nodes(const QString & par, QProgressDialog *percent, bool first);
    
private:
    int m_fileID;
    map<int, int> map_ist;
    map<int, RasCx> map_rs;

public:
    std::map<int, Fragment> map_fragments;
    std::map<int, int> m_map_fileID_mag;
    std::list<CText> set_text;

    std::map<int, Heating_seasons> map_Heating_seasons;

    QString m_city = "Неизвестный город";
    QString m_par;

    map <int, int > map_init_UR;
    map <int, int > map_init_KV;
    map <int, int > map_init_TR;

    map <int, QString> map_UR;
    map <int, QString> map_KV;
    map <int, QString> map_TR;
    map <int, QString> map_TP;

    CConnect m_connect;
};


bool readLineNew(QSqlQuery &query, const ColIndex &C, CLine2* line, int typ, CGraph2* m_graph, bool is_out = true);
bool readNodeNew(QSqlQuery& query, const ColIndex &C, CNode2* node2, int typ, CGraph2* m_graph, bool is_out = true);

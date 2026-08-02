#include <QApplication>
#include <QFile>
#include <QRegularExpression>

#include <dialog/MMenuDial.h>

#include <map>
#include <set>

#include <geo/geofile.h>
#include <geo/Klassif.h>


#include <mystd.h>

//QString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
//QString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);

//void initPassport(CAdoFile* ado, CGraph2* graph, HWND CWnd, const QString & argPath);

static map<QString, Klassif*> map_pts;
static map<QString, QString> map_onlygeo;
static set<QString> set_onlygeo;


std::map<QString, Klassif *> *get_map_pts()
{
    return &map_pts;
}


void init_set_file(const QString & fn0, std::set<QString> & set1)
{
    QString fn = QString("%1/kls/%2.lst").arg(argpath(), fn0);
    set1.clear();
    QFile file(fn);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString sl = readCP1251Line(file).trimmed().toLower();
            set1.insert(sl);
        }
    }
}


void init_set_file0(const QString & fn1, set<QString>& set_file)
{
    QString fn = QString("%1/kls/%2.lst").arg(argpath(), fn1);

    QFile file(fn);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString sl = readCP1251Line(file).trimmed().toLower();
            set_file.insert(sl);
        }
    }
}





bool is_in_set(const QString & tn, set<QString> & set1)
{
    QString sl = tn.toLower();

    auto it = set1.find(sl);
    if (it != set1.end()) {
        return true;
    }
    return false;    
}


static std::set<QString> set_onlyline;
static std::set<QString> set_onlypts;

void init_onlyline_list_file()
{
    init_set_file("onlyline", set_onlyline);
    init_set_file("onlypts", set_onlypts);
}


void init_onlypts_list_file()
{
}


bool is_onlyline(const QString & tn)
{
    return is_in_set(tn, set_onlyline);
}


bool is_onlypts(const QString & tn)
{
    return is_in_set(tn, set_onlypts);
}


static set<QString> set_noend;

void init_noend_list_file()
{
    init_set_file("noend", set_noend);
}


bool is_noend(const QString & tn)
{
    return is_in_set(tn, set_noend);
}


void init_onlygeo()
{
    QString fn = QString("%1/kls/onlygeo.lst").arg(argpath());
    QString tab1 = "";

    QFile file(fn);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString s = readCP1251Line(file);

            if (s.left(1) != " ") {
                tab1 = s;
                set_onlygeo.insert(s.toLower());
            }
            else {
                map_onlygeo[s.trimmed().toLower()] = tab1;
            }
        }
    }
}


bool isOnlygeo(const QString & tn)
{
    QString sl = tn.toLower();

    auto it = set_onlygeo.find(sl);
    if (it != set_onlygeo.end()) {
        return true;
    }
    return false;
}


QString getOnlygeo(const QString & tn)
{
    QString sl = tn.toLower();

    auto it = map_onlygeo.find(sl);
    if (it != map_onlygeo.end()) {
        return it->second;
    }
    return "";
}


const map<QString, Klassif*> *init_pts_list_file(const QString & fn1)
{
    QString fn = QString("%1/kls/%2.lst").arg(argpath(), fn1);

    map_pts.clear();

    QFile file(fn);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString s = readCP1251Line(file).trimmed();
            Klassif* kls = m_kl_list.findKlN(s);
            if (kls) {
                map_pts[kls->RusName()] = kls;
            }
        }
    }
    return &map_pts;
}

const map<QString, Klassif*> *init_pts_table(const QString & tab)
{
    map_pts.clear();

    Klassif* kls = m_kl_list.findKlN(tab);
    if (kls) {
        map_pts[kls->RusName()] = kls;
        return &map_pts;
    }
    return nullptr;
}


void init_pts_list_table(const QString & tab)
{
   Klassif* kls = m_kl_list.findKlN(tab);
   if (kls) {
       map_pts[kls->RusName()] = kls;
   }
}



Klassif *menu_list_file(QWidget *widget, const QString & title, const QString & fn1)
{
    MMenuDial menu(widget, title);

    init_pts_list_file(fn1);

    for (auto it : map_pts) {
        menu.Add(it.first, QVariant::fromValue(it.second));
    }

    if (menu.exec()) {
        return menu.value().value<Klassif*>();
    }
    return NULL;
}



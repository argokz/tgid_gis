#include <QtGui>
#include <QtSql>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <db/db.h>

bool isEditGeo(const QString & tn, bool isMessage = true);

QString transl(const QString & s);

bool isDeletable(QSqlDatabase &db, const QString & fn1, int n, bool isMessage)
{
    QString fn = transl(fn1).toLower();

    if (fn == transl("здания ту").toLower()) {
        QString q = QString("SELECT * FROM %1 WHERE %2 = %3").arg(transl("Технические условия"), transl("Здание")).arg(n);

        if (sizeOfQ(db, q) > 0) {
            QMessageBox::warning(nullptr, "", QObject::tr("Нельзя удалить здание с заданными техническими условиями"));
            return false;
        }
    }
    else if (fn == transl("здания потребителей").toLower()) {
        QString q = QString("SELECT * FROM %1 WHERE %2 = %3").arg(transl("Жилье"), transl("Здание")).arg(n);
        if (sizeOfQ(db, q) > 0) {
            QMessageBox::warning(nullptr, "", QObject::tr("Нельзя удалить здание с заданными Нагрузками"));
            return false;
        }
        q = QString("SELECT * FROM %1 WHERE %2 = %3").arg(transl("Организации"), transl("Здание")).arg(n);
        if (sizeOfQ(db, q) > 0) {
            QMessageBox::warning(nullptr, "", QObject::tr("Нельзя удалить здание с заданными Нагрузками"));
            return false;
        }
    }

    return true;
}

//void findXY(CFPoint p, double md, Klassif *kls, list<KlGeo> & geo_list);

std::map<QString, Klassif*> *get_map_pts();
//void init_pts_list_file(const QString & fn1);


// Ищет один геообъект по точке

int find_geo_first(const CFPoint &point, double md, const std::map<QString, Klassif*> *map_pts, QString &fn_out)
{
    for (auto &it : *map_pts) {
        QString fn0 = it.first;
        QString fn = it.second->nazv;

        Klassif *kls = m_kl_list.findKlN(fn);

        if (kls) {
            list<KlGeo> geo_list;
            findXYkls( point, md, kls,  geo_list);

            if (geo_list.size() > 0) {
                fn_out = kls->nazv;
                return  geo_list.begin()->gl->nom;
            }
        }
    }
   return -1;
}


bool GidWidget::pts_del(const CFPoint &point, const std::map<QString, Klassif*> *map_pts)
{
    if (!map_pts) return false;
    if (!isEditGeo("")) return false;
    if (isPot()) return false;

    QString fn;

    int n = find_geo_first(point, geom.masx*D5/mas_otn, map_pts, fn);

    if (n > 0) {
        if (!isDeletable(m_cxema.m_db, fn, n, true)) return false;
        if (!isEditGeo(fn)) return false;

        if (n) {
            delGeoObj(fn, n);
            repaint();
            return true;
        }
    }
    return false;
}

bool is_onlyline(const QString & tn);


bool GidWidget::pts_info(const CFPoint &point, const std::map<QString, Klassif*> *map_pts)
{
    if (!map_pts) return false;
    if (isPot()) return false;

    QString fn;

    int n = find_geo_first(point, geom.masx*D5/mas_otn, map_pts, fn);

    if (n > 0) {
        if (!isEditGeo(fn)) return false;

        if (n) {
            Klassif *kls = m_kl_list.findKlN(fn);
            if (kls) {
                CGeoObject* gl = kls->getGeoObjectById(n);
                if (gl) {
                    endDraw();
                    m_CurKlassif = kls;
                    m_CurGeoObject = gl;

//    if (!isEditGeo("")) return false;
//                    info(*kls->m_db, kls->RusName(), kls->nazv, n, -1, pr_type_geo);

                    CLINE2 *line = nullptr;

                    if (is_onlyline(kls->nazv)) {
                        double delta = geom.masx*D5;
                        CFPoint pt = gl->cl.first();
                        line = m_cxema.graph()->findLine( m_parent_id, pt, delta);
                    }

                    if (line) {
                        CLine2 *l = bline(line);
                        QString table = l->getTableMySQL();
                        info_with_line(*kls->m_db, kls->RusName(), kls->nazv, gl->nom, -1, table, l->line.nomP, l->line.nomO, pr_type_geo, kls->shape, kls->id);
                    }
                    else {
                        info(*kls->m_db, kls->RusName(), kls->nazv, gl->nom, -1, pr_type_geo, kls->shape, kls->id);
                    }
                }
            }
        }
    }
    return false;
}

void isprClLoc6(CCoordList &cl);


bool GidWidget::pts_move(const CFPoint &point, const std::map<QString, Klassif*> *map_pts)
{
    if (!map_pts) return false;
    if (!isEditGeo("")) return false;
    if (isPot()) return false;

    QString fn;

    int n = find_geo_first(point, geom.masx*D5/mas_otn, map_pts, fn);

    if (n > 0) {
        if (!isEditGeo(fn)) return false;

        if (n) {
            Klassif *kls = m_kl_list.findKlN(fn);
            if (kls) {
                CGeoObject* gl = kls->getGeoObjectById(n);
                if (gl) {
                    endDraw();

                    KlGeo klgeo;

                    klgeo.gl = gl;
                    klgeo.kls = kls;

                    m_geo_for_move = klgeo;
                    m_cl_for_move = gl->cl;

                    if (kls->loc == LOC6) {
                        isprClLoc6(m_cl_for_move);
                    }
                    repaint();
                }
            }
        }
    }

    return false;
}


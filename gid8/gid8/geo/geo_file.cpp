#include <geo/geofile.h>
#include <geo/Klassif.h>
#include <cxema/coordlis.h>
#include <db/db.h>
#include <dialog/MMenuDial.h>

#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QtGlobal>

void findXY3(CFPoint p, double md, Klassif *kls, list<KlGeo> & geo_list)
{
    for (auto gl: kls->geo4) {
        CFRect rect = gl->rect;

        if (rect.PtInRect(p)) {
            if (gl->cl.find3(p)) {
                KlGeo klgeo;
                klgeo.kls = kls;
                klgeo.gl = gl;

                geo_list.push_back(klgeo);
            }
        }
    }
}

void findXY2(CFPoint p, double md, Klassif *kls, list<KlGeo> & geo_list)
{
    for (auto gl: kls->geo4) {
        CFRect rect = gl->rect;
        rect.InflateRect(md, md);

        if (rect.PtInRect(p)) {
            if (gl->cl.find2(p, md, kls->loc)) {
                KlGeo klgeo;
                klgeo.kls = kls;
                klgeo.gl = gl;

                geo_list.push_back(klgeo);
            }
        }
    }
}

void findXY1(CFPoint p, double md, Klassif *kls, list<KlGeo> & geo_list)
{
    for (auto gl: kls->geo4) {
        CFRect rect = gl->rect;
        rect.InflateRect(md, md);

        if (rect.PtInRect(p)) {
            KlGeo klgeo;
            klgeo.kls = kls;
            klgeo.gl = gl;

            geo_list.push_back(klgeo);
        }
    }
}

bool isPoint(int loc);

void findXYkls(CFPoint p, double md, Klassif *kls, list<KlGeo> & geo_list, bool loc2 = false)
{
    if (!kls) return;

    if (kls->loc == LOC3 && (kls->colb == 0xFFFFFF || loc2)) { // WHITE
        findXY2(p, md, kls, geo_list);
    }
    else if (kls->loc == LOC3) {
        findXY3(p, md, kls, geo_list);
    }
    else if (kls->loc == LOC2) {
        findXY2(p, md, kls, geo_list);
    }
    else if (isPoint(kls->loc)) {
        findXY1(p, md, kls, geo_list);
    }
}

void findXY(CFPoint p, double md, list<KlGeo> & geo_list, bool loc2 = false)
{
//    double mas = geom.mas*geom.dmas;
    geo_list.clear();

    int aloc[] = {LOC3, LOC2, LOC6, LOC1, LOC11, 0};

    for (int loc = 0; aloc[loc]; loc++) {
//        if (aloc[loc] == 6 && mas > 5000) continue;
//        if (aloc[loc] == 1 && mas > 5000) continue;

        for (int prio = 0; prio < 11; prio++) {
            for (auto & kl : m_kl_list) {
                if (kl.prio == prio && kl.loc == aloc[loc] && kl.ins) {
                    findXYkls(p, md, &kl, geo_list, loc2);
                }
            }
        }
    }
}


#if 0

KlGeo *findXY(CFPoint p, double md)
{
    list<KlGeo> geo_list;
//    double mas = geom.mas*geom.dmas;

//    int aloc[] = {LOC3, LOC2, LOC6, LOC1, LOC11, 0};
    int aloc[] = {LOC6, LOC1, LOC11, LOC2, LOC3, 0};

    for (int loc = 0; aloc[loc]; loc++) {
        for (int prio = 11-1; prio >= 0; prio--) {
            for (auto & kl : m_kl_list) {
                if (kl.prio == prio && kl.loc == aloc[loc] && kl.ins) {
                    findXY(p, md, &kl, geo_list);
                    if (geo_list.size() > 0) {
                        return &geo_list.front();                        
                    }
                }
            }
        }
    }
    return nullptr;
}

#endif






int GeoFile::NMark(const QString &fn)
{
    Klassif* kls = m_kl_list.findKlN(fn);
    if (kls) return NMark(kls);

    return 0;
}


int GeoFile::NMark(Klassif* kl)
{
    int n = 0;

    int nn = kl->geo4.size();
    for (int i = 0; i < nn; i++) if (kl->geo4[i]->mark) n++;

    return n;
}


int GeoFile::NMark(const QString & fn, list<int> &list_id)
{
    Klassif* kls = m_kl_list.findKlN(fn);
    if (kls) return NMark(kls, list_id);

    return 0;
}


int GeoFile::NMark(Klassif* kl, list<int> &list_id)
{
    int n = 0;

    list_id.clear();

    int nn = kl->geo4.size();
    for (int i = 0; i < nn; i++) {
        if (kl->geo4[i]->mark) {
            list_id.push_back(kl->geo4[i]->nom);
            n++;
        }
    }

    return n;
}



int GeoFile::NMark()
{
    int n = 0, nn, i;

    KlList::const_iterator it = m_kl_list.begin();

//    for (n = 0; it != m_kl_list.end(); ++it) {
    for (auto & kl : m_kl_list) {
//        const Klassif* kl = &it->second;
        nn = kl.geo4.size();
        for (i = 0; i < nn; i++) if (kl.geo4[i]->mark) n++;
    }
    return n;
}


void GeoFile::clearMark()
{
    for (auto & kl : m_kl_list) {
        for (int i = 0; i < kl.geo4.size(); i++) {
            if (kl.geo4[i]->mark) {
//                ret = true;
            }
            kl.geo4[i]->mark = 0;
        }
    }
}


bool GeoFile::runFindQuery(Klassif *kls, const QString &pattern, QList<int> *ids)
{
    if (!kls || !kls->m_db || !kls->m_db->isOpen() || !ids) return false;
    ids->clear();

    QString pat = pattern.trimmed();
    if (pat.isEmpty()) return false;
    pat.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
    pat.replace(QLatin1Char('%'), QStringLiteral("\\%"));
    pat.replace(QLatin1Char('_'), QStringLiteral("\\_"));
    pat.replace(QLatin1Char('\''), QStringLiteral("''"));

    const QString idCol = kls->id.isEmpty() ? QStringLiteral("id") : kls->id;
    const QString tnSql = tbl_sql(kls->nazv);

    std::list<AdoField> fields;
    get_table_columns(*kls->m_db, kls->nazv, fields);

    QStringList orParts;
    const QString likeLit = QStringLiteral("'%") + pat + QStringLiteral("%'");
    orParts << QStringLiteral("CAST(%1 AS text) ILIKE %2 ESCAPE '\\'")
                 .arg(br_text(idCol), likeLit);

    for (auto &field : fields) {
        const QString cn = field.COLUMN_NAME;
        if (QString::compare(cn, idCol, Qt::CaseInsensitive) == 0) continue;
        if (QString::compare(cn, kls->shape, Qt::CaseInsensitive) == 0) continue;
        if (cn.compare(QLatin1String("geom"), Qt::CaseInsensitive) == 0) continue;
        if (cn.compare(QLatin1String("shape"), Qt::CaseInsensitive) == 0) continue;

        const QString dt = field.DATA_TYPE.toLower();
        const bool isText =
            dt.contains(QLatin1String("char")) ||
            dt.contains(QLatin1String("text")) ||
            dt.contains(QLatin1String("name")) ||
            dt.contains(QLatin1String("citext")) ||
            dt == QLatin1String("varchar") ||
            dt == QLatin1String("nvarchar") ||
            dt == QLatin1String("character varying");
        if (!isText) continue;

        orParts << QStringLiteral("CAST(%1 AS text) ILIKE %2 ESCAPE '\\'")
                     .arg(br_text(cn), likeLit);
    }

    QString q = QString("SELECT %1 FROM %2 WHERE (%3) ORDER BY %1 LIMIT 500")
                    .arg(br_text(idCol), tnSql, orParts.join(QLatin1String(" OR ")));

    QSqlQuery query(*kls->m_db);
    if (!query_exec(*kls->m_db, query, q)) return false;

    while (query.next()) {
        ids->append(query.value(0).toInt());
    }
    return !ids->isEmpty();
}


bool GeoFile::emitFindHit(QWidget *parent, QString *outTn, int *outId)
{
    if (m_findIndex < 0 || m_findIndex >= m_findIds.size()) {
        if (parent) {
            QMessageBox::information(parent, QString(),
                QObject::tr("Больше совпадений нет."));
        }
        m_findIndex = -1;
        return false;
    }

    const int id = m_findIds.at(m_findIndex);
    if (outTn) *outTn = m_findTn;
    if (outId) *outId = id;
    return true;
}


bool GeoFile::find(QWidget *parent, QString *outTn, int *outId)
{
    m_findTn.clear();
    m_findPattern.clear();
    m_findIds.clear();
    m_findIndex = -1;

    MMenuDial menu(parent, QObject::tr("Слой геобазы"));
    for (auto it = m_kl_list.begin(); it != m_kl_list.end(); ++it) {
        Klassif &kl = it.value();
        if (!kl.m_db || !kl.m_db->isOpen()) continue;
        if (kl.geo4.empty() && kl.ins == 0) continue;

        QString title = kl.RusName();
        if (title.isEmpty() || title == QLatin1String("???")) title = kl.nazv;
        else title = QStringLiteral("%1 (%2)").arg(title, kl.nazv);
        menu.Add(title, kl.nazv);
    }

    if (menu.cnt() == 0) {
        if (parent) {
            QMessageBox::information(parent, QString(),
                QObject::tr("Нет открытых слоёв геобазы для поиска."));
        }
        return false;
    }

    if (menu.exec() != QDialog::Accepted) return false;

    Klassif *kls = m_kl_list.findKlN(menu.value().toString());
    if (!kls) return false;

    bool ok = false;
    QString pattern = QInputDialog::getText(
        parent, QObject::tr("Поиск в геобазе"),
        QObject::tr("Текст или id (слой: %1):").arg(kls->nazv),
        QLineEdit::Normal, QString(), &ok);
    if (!ok || pattern.trimmed().isEmpty()) return false;

    if (!runFindQuery(kls, pattern, &m_findIds)) {
        // Запасной путь: совпадение id среди уже загруженных объектов.
        bool isInt = false;
        const int wantId = pattern.trimmed().toInt(&isInt);
        if (isInt) {
            for (auto *go : kls->geo4) {
                if (go && go->nom == wantId) m_findIds.append(wantId);
            }
        }
        if (m_findIds.isEmpty()) {
            for (auto *go : kls->geo4) {
                if (!go) continue;
                if (QString::number(go->nom).contains(pattern.trimmed(), Qt::CaseInsensitive)) {
                    m_findIds.append(go->nom);
                }
            }
        }
    }

    if (m_findIds.isEmpty()) {
        if (parent) {
            QMessageBox::information(parent, QString(), QObject::tr("Не найдено."));
        }
        return false;
    }

    m_findTn = kls->nazv;
    m_findPattern = pattern.trimmed();
    m_findIndex = 0;
    return emitFindHit(parent, outTn, outId);
}


bool GeoFile::findNext(QWidget *parent, QString *outTn, int *outId)
{
    if (m_findTn.isEmpty() || m_findIds.isEmpty() || m_findIndex < 0) {
        if (parent) {
            QMessageBox::information(parent, QString(),
                QObject::tr("Сначала выполните «Поиск в геобазе»."));
        }
        return false;
    }

    m_findIndex++;
    if (m_findIndex >= m_findIds.size()) {
        if (parent) {
            QMessageBox::information(parent, QString(),
                QObject::tr("Достигнут конец списка (%1). Поиск с начала.")
                    .arg(m_findIds.size()));
        }
        m_findIndex = 0;
    }
    return emitFindHit(parent, outTn, outId);
}

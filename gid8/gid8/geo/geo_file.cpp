#include <geo/geofile.h>
#include <geo/Klassif.h>
#include <cxema/coordlis.h>

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

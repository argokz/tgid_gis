#include "stdafx.h"
#include "resource.h"
#include "Scroll.h"
#include "InputDia.h"
#include "PrintIni.h"
#include "win.h"
#include <math.h>
#include "MapDlg.h"
#include "mmenu.h"
#include "geomenu.h"
#include "ps_alma.h"
#include "json.h"

#include "ado.h"
#include "maptabs.h"

const int DMAS96 = 96;


CString num_MSRS(const char *s);
CFPoint get_google_to_coord(double x, double y);


struct lessMSRS
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    result_type operator() (const CString &arg1, const CString & arg2) const
    {
        CString n1 = num_MSRS(arg1);
        CString n2 = num_MSRS(arg2);

        if (n1 == n2 && arg1 != arg2) {
            return arg1 < arg2;
        }

        return n1 < n2;
    }
};


static map<int, UMSRS> mapMSRS;

CGidrView *getGidView();
void initMSRE(map<int, UMSRS> &mapMSRS);
int isMagMsRs(int ue, int ms_rs);


static void initMag2(JSON &json, int id, BOOL is_mag)
{
  json.add("\"children\": [");

  int qq = 0;
  

  {
    map<CString, UMSRS, lessMSRS> map2;
    map<int, UMSRS>::const_iterator it = mapMSRS.begin();

    for (; it !=  mapMSRS.end(); it++) {
      UMSRS rs = it->second;
      map2[rs.opis] = rs;
    }

    map<CString, UMSRS, lessMSRS>::const_iterator it2 = map2.begin();
    for (; it2 !=  map2.end(); it2++) {
      UMSRS rs = it2->second;

      map<int, UE>::const_iterator itUE = ps_alma.map_UE.find(rs.ue);
      if (itUE != ps_alma.map_UE.end()) {
        UE ue = itUE->second;
        if (ue.id == id) {

          if (isMagMsRs(ue.id, is_mag)) {
              json.add("{");
              json.add("id", rs.id);
              json.add("text", rs.opis);
              json.add("value", rs.opis);
              json.add("icon", "fas fa-file icon-state-default");
              json.add("opened", false);
              json.add("selected", false);
              json.add("disabled", false);
              json.add("loading", false);

              json.add("\"children\": [");


              json.add("{");
              json.add("id", rs.id);
              json.add("typ", rs.id);
              json.add("text", "Перейти к фрагменту");
              json.add("value", "Перейти к фрагменту");
              json.add("icon", "fas fa-file icon-state-default");

              json.add("opened", false);
              json.add("selected", false);
              json.add("disabled", false);
              json.add("loading", false);
              json.add("}");

              json.add("{");
              json.add("id", rs.id);
              json.add("typ", rs.id);
              CGidrView* getGidView();

              CCxema* cxema = getGidView()->m_cxema;;

              json.add("fragments", cxema->m_par);
              json.add("type", is_mag ? "ms" : "rs");
              json.add("text", "Паспорт");
              json.add("value", "Паспорт");
              json.add("icon", "fas fa-file icon-state-default");
              json.add("opened", false);
              json.add("selected", false);
              json.add("disabled", false);
              json.add("loading", false);
              json.add("}");

              json.add("]");


              json.add("}");

              qq ++;
//              if (qq == 3) break;

          }
        }
      }
    }
  }
    json.add("]");
}



static void initNach1(JSON &json, BOOL is_mag)
{
    initMSRE(mapMSRS);

    CGidrView *gidr_view = getGidView();

    if (!gidr_view) return;

    CCxema *cxema = gidr_view->m_cxema;


    json.add("\"children\": [");


    int qq = 0;

    map<CString, UMSRS> mapMS;

    map<int, UMSRS>::const_iterator it = mapMSRS.begin();

    for (; it != mapMSRS.end(); it++) {
        UMSRS rs = it->second;
        if (is_mag == rs.mag) {
            CString s = rs.opis;
            if (s == "") s = rs.name;

            mapMS[s] = rs;
        }
    }

    map<CString, UE> map3;

    {
        map<int, UE>::const_iterator it = ps_alma.map_UE.begin();

        for (; it != ps_alma.map_UE.end(); it++) {

            UE ue = it->second;

            CString re = "";
            CString nach = "";

            map<int, NACH>::const_iterator itN = ps_alma.map_NACH.find(ue.nach);
            if (itN != ps_alma.map_NACH.end()) {
                nach = itN->second.name;
            }

            map<int, CString>::const_iterator itR = ps_alma.map_RE.find(ue.re);
            if (itR != ps_alma.map_RE.end()) {
                re = itR->second;
            }



            map<CString, UMSRS>::const_iterator it2 = mapMS.begin();
            for (; it2 != mapMS.end(); it2++) {
                UMSRS rs = it2->second;
                if (rs.ue == ue.id) {

                    //          CString s = re + " " + ue.nomer+ " " +nach;
                    CString s = nach;

                    json.add("{");
                    json.add("id", ue.id);
                    json.add("text", nach);
                    json.add("value", nach);

                    json.add("icon", "fas fa-file icon-state-default");
                    json.add("opened", false);
                    json.add("selected", false);
                    json.add("disabled", false);
                    json.add("loading", false);


                    initMag2(json, ue.id, is_mag);

                   
                    
                    json.add("}");

                    map3[s] = ue;
                    break;

                    qq ++;
//                    if (qq == 3) break;
                }
            }
        }
    }

    map<CString, UE>::const_iterator it2 = map3.begin();

    for (; it2 != map3.end(); it2++) {
        CString s = it2->first;
        UE ue = it2->second;

        //    HTREEITEM i = m_ctrlTree.InsertItem(s, i1);
        //    m_ctrlTree.SetItemData(i, (DWORD) ue.id);
    }

    json.add("]");
}


void http_nach_ms_rs(JSON &json, BOOL is_mag)
{
    json.add("{");
    json.add("id", 1);
    json.add("text", _TR("Начальники участков"));
    json.add("value", _TR("Начальники участков"));
    json.add("icon", "fas fa-file icon-state-default");
    json.add("opened", false);
    json.add("selected", false);
    json.add("disabled", false);
    json.add("loading", false);
    initNach1(json, is_mag);
    
    json.add("}");
    json.add("]");
}

CString http_tree_ms()
{
    JSON json;
    json.add("{\"data\": [");
    http_nach_ms_rs(json, 0);
    json.add("}");
    return json.str();
}

CString http_tree_rs()
{
    JSON json;
    json.add("{\"data\": [");
    http_nach_ms_rs(json, 1);
    json.add("}");
    return json.str();
}

int viewUchastok(CGraph2* m_graph, tuple<int, int> msrs, int type /*1 - Магистраль 2 Распредсеть 3 Магистраль и распредсеть*/, bool is_only_big_pipe);
bool getVydRect(CGraph2 *graph, CFRect &rect);

static map<int, CFRect> map_ms;
static map<int, CFRect> map_rs;

CFRect getMsRect(int id) 
{
    CFRect rr(0,0,0,0);

    map<int, CFRect>::const_iterator it = map_ms.find(id);
    if (it != map_ms.end()) {
      rr = it->second;
    }

    return rr;
}

CFRect getRsRect(int id) 
{
    CFRect rr(0,0,0,0);

    map<int, CFRect>::const_iterator it = map_rs.find(id);
    if (it != map_rs.end()) {
      rr = it->second;
    }

    return rr;
}











void initMsRect(CGidrView *gidr_view, int id)
{
    if (id <= 0) return;
    CFRect rr;

    CGraph2 *graph = gidr_view->m_cxema->m_graph;

    graph->save_pjezo();
    graph->reset();

    viewUchastok(graph, make_tuple(id, 0), 1, true);

    CFRect r;

    if (getVydRect(graph, r)) {
        CFRect rect = gidr_view->getMaxRect();
        double dmas = gidr_view->geom.dmas;

        rr.left = (r.left - rect.left)*dmas / 1000000;
        rr.right = (r.right - rect.left)*dmas / 1000000;
        rr.top = (r.top - rect.top)*dmas / 1000000;
        rr.bottom = (r.bottom - rect.top)*dmas / 1000000;

        map_ms[id] = rr;
    }

    graph->restore_pjezo();
}



void initRsRect(CGidrView *gidr_view, int id)
{
    if (id <= 0) return;
    CFRect rr;

    CGraph2 *graph = gidr_view->m_cxema->m_graph;

    graph->save_pjezo();
    graph->reset();

    viewUchastok(graph, make_tuple(0, id), 2, true);

    CFRect r;

    if (getVydRect(graph, r)) {
        CFRect rect = gidr_view->getMaxRect();
        double dmas = gidr_view->geom.dmas;

        rr.left = (r.left - rect.left)*dmas / 1000000;
        rr.right = (r.right - rect.left)*dmas / 1000000;
        rr.top = (r.top - rect.top)*dmas / 1000000;
        rr.bottom = (r.bottom - rect.top)*dmas / 1000000;

        map_rs[id] = rr;
    }

    graph->restore_pjezo();
}


void get_google_coord(CFPoint pt, double& x, double& y);


void get_google_rect_json(CFRect rect, int width, int height, JSON& json);

CString http_get_ms (int id)
{
    CGidrView *gidr_view = getGidView();
    if (!gidr_view) return "";
    if (id <= 0) return "";
    CFRect rr;

    JSON json;

    map<int, CFRect>::const_iterator it = map_ms.find(id);
    if (it != map_ms.end()) {
      rr = it->second;
      
      get_google_rect_json(rr, 0, 0, json);
      return json.str();
    }

    CGraph2 *graph = gidr_view->m_cxema->m_graph;

    graph->save_pjezo();
    graph->reset();

    viewUchastok(graph, make_tuple(id, 0), 1, true);

    CFRect r;

    if (getVydRect(graph, r)) {
//        CFRect rect = gidr_view->getMaxRect();
        get_google_rect_json(r, 0, 0, json);
        map_ms[id] = r;
    }

    graph->restore_pjezo();
    
    return json.str();
}



CString http_get_rs(int id)
{
    CGidrView *gidr_view = getGidView();
    if (!gidr_view) return "";
    if (id <= 0) return "";
    CFRect rr;

    JSON json;

    map<int, CFRect>::const_iterator it = map_rs.find(id);
    if (it != map_rs.end()) {
      rr = it->second;
      
      json.add("{");
      json.add("x1", rr.left);
      json.add("x2", rr.right);
      json.add("y1", rr.top);
      json.add("y2", rr.bottom);
      json.add("}");
      
      return json.str();
    }


    CGraph2 *graph = gidr_view->m_cxema->m_graph;

    graph->save_pjezo();
    graph->reset();

    viewUchastok(graph, make_tuple(0, id), 2, true);

    CFRect r;

    if (getVydRect(graph, r)) {
        CFRect rect = gidr_view->getMaxRect();
        double dmas = gidr_view->geom.dmas;

        rr.left = (r.left - rect.left)*dmas / 1000000;
        rr.right = (r.right - rect.left)*dmas / 1000000;
        rr.top = (r.top - rect.top)*dmas / 1000000;
        rr.bottom = (r.bottom - rect.top)*dmas / 1000000;

        json.add("{");
        json.add("x1", rr.left);
        json.add("x2", rr.right);
        json.add("y1", rr.top);
        json.add("y2", rr.bottom);
        json.add("}");

        map_rs[id] = rr;
    }

    graph->restore_pjezo();

    return json.str();
}

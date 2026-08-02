#include <QCoreApplication>
#include <QDateTime>

#include "graph2.h"

#include <queue>
#include <stack>

#include "cxema1.h"

//#include "cxema.h"
//#include "ado.h"

int nn = 0;

bool isSameName(const QString & s1, const QString & s2)
{
    if (s1 == s2) return true;

    int len1 = s1.length();
    int len2 = s2.length();

    if (len1 != len2) return false;

    for (int i = 0; i < len1; i++) {
        if (s1[i] != s2[i] && s2[i] != '_') return false;
    }

    return true;
}


bool isMarked(const CNode2 *n)
{
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.isPjezo) {
            return true;
        }
    }
    return false;
}


CGraph2::CGraph2(CCxema *cxema)
{
    is_changed = true;
    m_cxema = cxema;
    m_rect = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);
}

CGraph2::~CGraph2()
{
//  Ошибка какая-то нужно искать
    clear();

}


CNode2 * CGraph2::ins(int id)
{
    CNode2 *node = new CNode2();
    node->id = id;
    node->node.m_graph = this;

    map_node[id] = node;

    is_changed = true;

    return node;
}

CNode2 * CGraph2::find_ins(int id)
{
    is_changed = true;

    auto it = map_node.find(id);

    if (it != map_node.end()) {
        m_rect.AddPoint(it->second->node.coord);
        return it->second;
    }
    return ins(id);
}

CNode2 * CGraph2::find(int id)
{
    if (id == 0) return nullptr;

    auto it = map_node.find(id);

    if (it != map_node.end()) {
        return it->second;
    }

    return nullptr;
}

//-----------------------------------------------
// Найти линию между узлами
//-----------------------------------------------

CLINE2 * CGraph2::Link(const CNode2 *c1, const CNode2 *c2)
{
    CLINE2 *l;

    if (!c1 || !c2) return nullptr;
    if (!c1->lines || !c2->lines) return nullptr;

    for ( l = c1->lines; l; l = nextl(l)) {
        if (other(l) == c2) return l;
    }
    return nullptr;
}

CFRect GetLineRect(CFPoint pt1, CFPoint pt2, const CCoordList &cl);



CLine2 * CGraph2::insert_line(CNode2 *n1, CNode2 *n2, const CCoordList &NP, bool paral)
{
    CNode2 *c1, *c2;
    CLINE2 *ll;
    CLine2 *l = 0;
    is_changed = true;

//  if (n1 == n2) return nullptr;

//  c1 = find(*n1);  if (!c1) return nullptr;
//  c2 = find(*n2);  if (!c2) return nullptr;

    if (n1 == n2) return nullptr;

    c1 = n1;
    if (!c1) return nullptr;
    c2 = n2;
    if (!c2) return nullptr;

    if (n1->node.fileID != n2->node.fileID) return nullptr;


    ll = Link(c1, c2);
    if (ll) l = bline(ll);

    if (!ll || bline(ll)->line.m_NP != NP || paral) {

        l = new CLine2;

        l->_where[0] = c1;
        l->_next[0] = c1->lines;
        c1->lines = l;

        l->_where[1] = c2;
        l->_next[1] = c2->lines;

        UINT64 s = (UINT64)bline(l);

        c2->lines = (CLINE2*) (s|0x1);

        l->line.m_NP = NP;
        l->line.rect = GetLineRect(n1->node.coord, n2->node.coord, NP);
    }

    return l;
}

/*
CLine2 * CGraph2::insert_line2(CNode2 *n1, CNode2 *n2, const CCoordList &NP, bool paral)
{
  CNode2 *c1, *c2;
  CLINE2 *ll;
  CLine2 *l;


  c1 = n1;  if (!c1) return nullptr;
  c2 = n2;  if (!c2) return nullptr;


  if (n1->file != n2->file) return nullptr;

  ll = Link(c1, c2);
  if (ll) l = bline(ll);

  if (!ll || paral) {

    l = new CLine2;

    l->_where[0] = c1;
    l->_next[0] = c1->lines;
    c1->lines = l;

    l->_where[1] = c2;
    l->_next[1] = c2->lines;

    UINT64 s = (UINT64)bline(l);

    c2->lines = (CLINE2*) (s|0x1);

    l->line.m_NP = NP;
    l->line.rect = GetLineRect(n1->node.coord, n2->node.coord, NP);
  }

  return l;
}
*/


bool CGraph2::getBorderMap(int fileID, int internalNodeID, CFRect &rect)
{
    auto it1 = m_map_border.find(fileID);
    if (it1 == m_map_border.end()) return false;
    auto it2 = it1->second.find(internalNodeID);
    if (it2 == it1->second.end()) return false;

    rect = it2->second;

    return true;
}


CFRect CGraph2::getBorderFile(int fileID)
{
//  return getBorder(0);

    int internalNodeID = 0;

    CFRect rect = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);


    if (getBorderMap(fileID, 0, rect)) {
        return rect;
    }

    bool points = false;

    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *pp = p->second;

        if (pp->node.fileID == fileID && pp->node.internalNodeID == internalNodeID) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                rect.AddPoint(pp->node.coord);
                points = true;
            }

            for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
                if (IsBegin(l)) {
//                    int qq = bline(l)->line.m_NP.size();
                    if (bline(l)->line.m_NP.size() > 0) {
                        CFRect r = bline(l)->line.m_NP.GetFRect();
                        rect.AddPoint(r.BottomRight());
                        rect.AddPoint(r.TopLeft());
                    }
                }
            }
        }
        p++;
    }

    if (!points) {
        rect = CFRect(0, 0, 10000, 10000);
    }




//  m_map_border[file][0] = rect;

    return rect;
}


CFRect CGraph2::getBorder(int internalNodeID)
{
    CFRect rect = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);

    if (!is_changed) {
        if (getBorderMap(0, internalNodeID, rect)) {
            return rect;
        }
    }

    auto p = map_node.begin();

    bool points = false;

    while (p != map_node.end()) {
        CNode2 *pp = p->second;

        if (pp->node.internalNodeID == internalNodeID) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                rect.AddPoint(pp->node.coord);
                points = true;
            }

            for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
                if (IsBegin(l)) {
//                    int qq = bline(l)->line.m_NP.size();
                    if (bline(l)->line.m_NP.size() > 0) {
                        CFRect r = bline(l)->line.m_NP.GetFRect();
                        rect.AddPoint(r.BottomRight());
                        rect.AddPoint(r.TopLeft());
                    }
                }
            }
        }
        p++;
    }

    if (rect.left < rect.right) {
        rect.InflateRect(1000, 1000);
    }

    if (!points) {
        rect = CFRect(0, 0, 10000, 10000);
    }


    m_map_border[0][internalNodeID] = rect;
    is_changed = false;

    return rect;
}


//-----------------------------------------------
// Найти узел по координатам
// в радиусе r
//-----------------------------------------------


CNode2 * CGraph2::find(int parent_id, CFPoint pt, double r)
{
    CNode2 *node, *n0 = nullptr;
    double h, minh = LONG_MAX;
    r = max(2., r);

    auto p = map_node.begin();

    while (p != map_node.end()) {
        node = p->second;
        if (node->node.internalNodeID == parent_id) {
            h = hypot(pt.x-node->node.coord.x, pt.y-node->node.coord.y);
            if (h < minh && h < r) {
                minh = h;
                n0 = node;
            }
        }
        p++;
    }
    return n0;
}


//-----------------------------------------------
// Найти узлы по координатам
// в радиусе r
//-----------------------------------------------


std::list<CNode2 *> CGraph2::find_node_list(int parent_id, CFPoint pt, double r)
{
    std::list<CNode2 *> lst;

    r = max(2., r);

    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *node = p->second;
        if (node->node.internalNodeID == parent_id) {
            double h = hypot(pt.x-node->node.coord.x, pt.y-node->node.coord.y);
            if (h < r) {
                lst.push_back(node);
            }
        }
        p++;
    }
    return lst;
}




//-----------------------------------------------
// Найти узел внутри многоугольника
//-----------------------------------------------


CNode2 * CGraph2::find(int parent_id, const CCoordList &cl)
{
    CNode2 *node;

    CFRect rect = cl.GetFRect();

    auto p = map_node.begin();

    while (p != map_node.end()) {
        node = p->second;

        if (node->node.internalNodeID == parent_id) {
            if (rect.PtInRect(node->node.coord)) {
                if (cl.find3(node->node.coord)) {
                    return node;
                }
            }
        }
        p++;
    }
    return nullptr;
}


CNode2 * CGraph2::find(int parent_id, int typ, const CCoordList &cl)
{
    CNode2 *n = nullptr;
    CFRect rect = cl.GetFRect();

    auto p = map_node.begin();

    while (p != map_node.end()) {
        n = p->second;
        if (n->node.internalNodeID == parent_id) {
            if (n->node.typ == typ) {
                if (rect.PtInRect(n->node.coord ) ) {
                    if (cl.find3(n->node.coord)) return n;
                }
            }
        }
        p++;
    }

    return nullptr;
}





bool chline(CLine2 *l, double x, double y, double dx);

//-----------------------------------------------
// Найти линию по координатам
// в радиусе r
//-----------------------------------------------

CLINE2 * CGraph2::findLine(int parent_id, CFPoint pt, double r)
{
    CLINE2 *l;

    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *node = p->second;

        if (node->node.internalNodeID == parent_id) {
            if (node->node.coord.x != 0 && node->node.coord.y != 0) {
                for (l = node->lines; l; l = nextl(l)) {
                    if (other(l)->node.coord.x != 0 && other(l)->node.coord.y != 0) {
                        if (IsBegin(l) && chline(bline(l), pt.x, pt.y, r)) {
                            return l;
                        }
                    }
                }
            }
        }
        p++;
    }

    return nullptr;
}


//-----------------------------------------------
// Найти линии по координатам
// в радиусе r
//-----------------------------------------------


std::list<CLINE2 *> CGraph2::find_line_list(int parent_id, CFPoint pt, double r)
{
    CLINE2 *l;
    std::list<CLINE2 *> lst;

    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *node = p->second;

        if (node->node.internalNodeID == parent_id) {
            if (node->node.coord.x != 0 && node->node.coord.y != 0) {
                for (l = node->lines; l; l = nextl(l)) {
                    if (other(l)->node.coord.x != 0 && other(l)->node.coord.y != 0) {
                        if (IsBegin(l) && chline(bline(l), pt.x, pt.y, r)) {
                            lst.push_back(l);
                        }
                    }
                }
            }
        }
        p++;
    }

    return lst;
}


bool chline(double x1, double y1, double x2, double y2, double xx, double yy, double dx);

static int numb_of_line = 0;


bool chline(CLine2 *l, double x, double y, double dx)
{

    long x1, y1, x2, y2;
    CNode2 *n1, *n2;
    numb_of_line = 0;

//  QString ss;
//  ss.Format("l->m_NP.size()=%d", l->m_NP.size());
//LOG3(ss);

    auto it = l->line.m_NP.begin();
    if (l->line.m_NP.size() == 0 && it != l->line.m_NP.end()) {
//    AfxMessageBox("Error!!!", MB_OK|MB_ICONINFORMATION);
    }


    n1 = where(l);
    n2 = other(l);

    if (l->line.m_NP.size() == 0 || it == l->line.m_NP.end()) {
//LOG2();
        x1 = n1->node.coord.x;
        y1 = n1->node.coord.y;
        x2 = n2->node.coord.x;
        y2 = n2->node.coord.y;
        if (::chline(x1, y1, x2, y2, x, y, dx)) {
//LOG2();
            return true;
        }
//LOG2();
    }
    else {
//LOG2();
        x1 = n1->node.coord.x;
        y1 = n1->node.coord.y;

        for (; it != l->line.m_NP.end(); ++it) {
            x2 = it->x;
            y2 = it->y;
            numb_of_line ++;
            if (::chline(x1, y1, x2, y2, x, y, dx)) return true;
            x1 = x2;
            y1 = y2;
        }
        x2 = n2->node.coord.x;
        y2 = n2->node.coord.y;
        numb_of_line ++;
//LOG2();
        if (::chline(x1, y1, x2, y2, x, y, dx)) return true;
    }
//LOG2();

    return false;
}


void CGraph2::clear()
{
#if 0
    CLINE2 *l, *ll;
    auto p = map_node.begin();

    while (p != map_node.end()) {

        for ( l = p->second->lines; l; l = ll) {
            ll = nextl(l);
//        if (IsBegin(l)) {
//            delete l;
//        }
            nextl(l) = nullptr;
        }
        p->second = nullptr;
        delete p->second;
        p++;
    }

    map_node.clear();
#else

    CLINE2 *l, *ll;
    auto p = map_node.begin();

    while (p != map_node.end()) {

        CNode2 *pp = p->second;

        for ( l = pp->lines; l; l = ll) {
            ll = nextl(l);
            remove_line(l);
        }
        p++;
    }


    p = map_node.begin();

    while (p != map_node.end()) {
        delete p->second;
        p++;
    }


    map_node.clear();
#endif
}


void CGraph2::remove_node(CNode2 &node)
{
    is_changed = true;

    auto p = map_node.find(node.id);
    if (p != map_node.end()) map_node.erase(p);
}

void CGraph2::remove_line(CLINE2 *l1)
{
    is_changed = true;

    if (!l1) return;

    m_map_line[bline(l1)->line.nomP] = nullptr;
    m_map_line[bline(l1)->line.nomO] = nullptr;


    CNode2 *n1 = where(l1);
    CNode2 *n2 = other(l1);

    if (n1 == n2) return;

    CLINE2 *l2 = sym(l1);

    if (n1->lines == l1) {
        n1->lines = nextl(l1);
    }
    if (n2->lines == l2) {
        n2->lines = nextl(l2);
    }


    CLINE2 *ll = nullptr;

    for ( CLINE2 *l = n1->lines; l; l = ll) {
        ll = nextl(l);

        if (ll && l1 == ll) {
            nextl(l) = nextl(ll);
            break;
        }
    }

    for ( CLINE2 *l = n2->lines; l; l = ll) {
        ll = nextl(l);

        if (ll && l2 == ll) {
            nextl(l) = nextl(ll);
            break;
        }
    }

    delete bline(l1);
}


CNode2 * CGraph2::sameMarkNode()
{
    auto p = map_node.begin();
    while (p != map_node.end()) {
        if (p->second->node.isPjezo) {
            return p->second;
        }
        p++;
    }
    return nullptr;
}


int CGraph2::getNodeCount(int parent_id)
{
    int n = 0;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        if (p->second->node.internalNodeID == parent_id) {
            if (p->second->node.coord.x != 0 && p->second->node.coord.x != 0) {
                n++;
            }
        }
        p++;
    }
    return n;
}


bool CGraph2::reset()
{
    auto p = map_node.begin();
    bool ret = false;

    while (p != map_node.end()) {
        if (p->second->node.isPjezo) {
            ret = true;
        }
        p->second->node.len = -1;
        p->second->node.isPjezo = false;
        for ( CLINE2 *l = p->second->lines; l; l = nextl(l)) {
            if (bline(l)->line.isPjezo ) {
                ret = true;
            }
            bline(l)->line.isPjezoP = false;
            bline(l)->line.isPjezoO = false;

            
            bline(l)->line.isPjezo = false;
            bline(l)->line.isPjezo2 = false;
        }
        p++;
    }
    list_pjezo.clear();
    list_pjezo_min.clear();
    return ret;
}


bool CGraph2::set_pipeSectionID_(int id)
{
    auto p = map_node.begin();
    bool ret = false;

    while (p != map_node.end()) {
        p->second->node.isPjezo = false;
        p->second->node.len = -1;
        for ( CLINE2 *l = p->second->lines; l; l = nextl(l)) {
            bline(l)->line.isPjezo = bline(l)->line.pipeSectionID == id;
            if (bline(l)->line.isPjezo) {
                p->second->node.isPjezo = true;
            }
        }
        p++;
    }
    list_pjezo.clear();
    list_pjezo_min.clear();
    return ret;
}


bool CGraph2::set_lineSetID(set<int>& set_l)
{
    auto p = map_node.begin();
    CLINE2* l;
    bool ret = false;

    while (p != map_node.end()) {
        p->second->node.isPjezo = false;
        p->second->node.len = -1;
        for (l = p->second->lines; l; l = nextl(l)) {

            auto it = set_l.find(bline(l)->line.nomP);

            bline(l)->line.isPjezo = it != set_l.end();
            if (bline(l)->line.isPjezo) {
                p->second->node.isPjezo = true;
            }
        }
        p++;
    }
    list_pjezo.clear();
    list_pjezo_min.clear();
    return ret;
}



bool CGraph2::vyd_all()
{
    auto p = map_node.begin();
    CLINE2 *l;
    bool ret = false;

    while (p != map_node.end()) {
        if (!p->second->node.isPjezo) {
            ret = true;
        }
        p->second->node.len = -1;
        p->second->node.isPjezo = false;
        for ( l = p->second->lines; l; l = nextl(l)) {
            if (!bline(l)->line.isPjezo ) {
                ret = true;
            }
            bline(l)->line.isPjezo = true;
            bline(l)->line.isPjezo2 = true;
        }
        p++;
    }
    list_pjezo.clear();
    list_pjezo_min.clear();
    return ret;
}


CLINE2 * CGraph2::find_line_obr(const CLINE2 *line)
{
    CNode2 *n1 = where(line);
    CNode2 *n2 = other(line);

    for ( CLINE2 *l = n1->lines; l; l = nextl(l)) {
        if (other(l) == n2 && l != line) {
            return l;
        }
    }
    return nullptr;
}


void CGraph2::WS1(CNode2 *v, CNode2 *t, bool zakr)
{
    queue<CNode2*> q;
    CNode2 *u, *p;
    CLINE2 *l;
//  int maxlen = 32767;

    q.push(v);
    v->node.len = 0;

    while ( !q.empty() ) {
        p = q.front();
        q.pop();
//    if (p->len > maxlen) break;

        for ( l = p->lines; l; l = nextl(l)) {
            u = other(l);
//      if (u->len == -1 && u != v && !l->uch->otkl && l->uch->nom_m != -1) {
            if (u->node.len == -1 && (!zakr || !bline(l)->line.isOtkl())) {

                u->node.len = where(l)->node.len+1;
                q.push(u);
                if (u == t) {
                    return;
//          maxlen = u->len;
                }
            }
        }
    }
}

void CGraph2::WG1(CNode2 *s, CNode2 *t, bool zakr)
{
    stack<CNode2*> st;
    CNode2 *v, *u, *p;
    CLINE2 *l;

    st.push(t);
    v = t;

    while (v != s) {
        for ( l = v->lines; l; l = nextl(l)) {
            u = other(l);
            if (u->node.len == v->node.len-1) break;
        }
        if (!l) break;
        st.push(u);
        v = u;
    }

    p = nullptr;

    while ( !st.empty() ) {
        v = st.top();
        st.pop();
        v->node.isPjezo = true;
        if (list_pjezo.empty() || !(v->id == list_pjezo.back())) {
            list_pjezo.push_back(v->id);
        }
        if (p) {
            for ( l = p->lines; l; l = nextl(l) ) {
//        if (l->other_end->where == v && !l->uch->otkl && l->uch->nom_m != -1) {
                if (other(l) == v && (!zakr || !bline(l)->line.isOtkl())) {
                    bline(l)->line.isPjezo = true;
                }
            }
        }
        p = v;
    }
}


void CGraph2::WS1_h(CNode2 *v, CNode2 *t, double dh)
{
    queue<CNode2*> q;
    CNode2 *u, *p;
    CLINE2 *l;
    double h;

    q.push(v);
    v->node.len = 0;

    while ( !q.empty() ) {
        p = q.front();
        q.pop();

        for ( l = p->lines; l; l = nextl(l)) {
            u = other(l);
            h = u->node.hz;
//      if (u->len == -1 && (u->typ != TIP_PR || fabs(v->data.geodz-(u->data.geodz+h)) < dh)) {
            if (u->node.len == -1 && (fabs(v->node.geoMarkTopTube-(u->node.geoMarkTopTube+h)) < dh)) {
                u->node.len = where(l)->node.len+1;
                bline(l)->line.isPjezo = 1;
                q.push(u);
                if (u == t) {
                    return;
                }
            }
        }
    }
}



CNode2 * CGraph2::findPS1(const QString & name)
{
    if (name == "") return nullptr;

    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *n = p->second;
        if (isSameName(n->node.namePS, name)) {
            return n;
        }
        p++;
    }
    return nullptr;
}


CNode2 * CGraph2::findPS(const QString & name)
{
    if (name == "") return nullptr;

    CNode2 *n = findPS1(name);
    return n;
}

void CGraph2::reset_len()
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        p->second->node.len = -1;
        p++;
    }
}




void CGraph2::link(CNode2 *s, CNode2 *t)
{
    if (!s || !t) return;

    reset_len();
    WS1(s, t);

    WG1(s, t);
}

bool CLine::isOtkl() const
{
    return pod.isOtkl && obr.isOtkl;
}

bool CLine::isLineOtkl() const
{
    return (pod.isOtkl && obr.isOtkl) || (pod.isOtklZD && obr.isOtklZD);
}


QString CLine2::getTable() const
{
  return getLineTable(line.typ);
}

QString CLine2::getTableMySQL() const
{
  return getLineTableMySQL(line.typ);
}

QString CLine2::getTableOutMySQL() const
{
  return getLineTableOutMySQL(line.typ);
}

QString getLineTableOut(int typ);
QString getNodeTableOut(int typ);


QString CLine2::getTableOut() const
{
    return getLineTableOut(line.typ);
}


QString CNode2::getTable() const
{
    if (node.typ < 0 || node.typ >= nodefsize) return "";

    return nodef[node.typ].table;
}

QString CNode2::getTableMySQL() const
{
    return getNodeTableMySQL(node.typ);
}

QString CNode2::getTableOut() const
{
    return getNodeTableOut(node.typ);
}

QString CNode2::getTableOutMySQL() const
{
    return getNodeTableOutMySQL(node.typ);
}


CCoordList CLine2::getCLall()
{
    CCoordList cl;
    CNode2 *n1 = where(this);
    CNode2 *n2 = other(this);

    if (IsBegin(this)) {
        cl.push_back(n1->node.coord);
    }
    else {
        cl.push_back(n2->node.coord);
    }
    auto it = bline(this)->line.m_NP.begin();
    for (  ; it != bline(this)->line.m_NP.end(); ++it) {
        cl.push_back(*it);
    }
    if (IsBegin(this)) {
        cl.push_back(n2->node.coord);
    }
    else {
        cl.push_back(n1->node.coord);
    }

    return cl;
}


//-----------------------------------------------
// Найти узел
// если общий, то на подаче или на обратке
//-----------------------------------------------

CNode2 * CGraph2::find2(const CNode2& n)
{
    CNode2 *nn = find(n.id);
    return nn;
}

int CNode2::typPS() const
{
    if (PSName(this) == "") return -1;

    return node._typPS;
}

QString PSName(const CNode2 *n)
{
    if (!n) return "";

    if (n->node.namePS != "") return n->node.namePS;

    extern bool _isDopNameAstana;

//  if (_isDopNameAstana) return "";

//  if (n->node.name[0] == '#') return "";

    return n->node.name;
}


QString PSFullName(const CNode2 *n)
{
    if (!n) return "";

    if (n->node.namePS != "") return n->node.namePS;

    extern bool _isDopNameAstana;

//  if (_isDopNameAstana) return "";

    return n->getName();
}


int incP(const CNode2 *n)
{
    int k = 0;
    for ( CLINE2 *l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.isPjezo) k++;
    }
    return k;
}

int inc(const CNode2 *n)
{
    int k = 0;
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        k++;
    }
    return k;
}

int inc_pod(const CNode2 *n)
{
    int k = 0;
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.nomP != -1) {
            k++;
        }
    }
    return k;
}


int inc_ms(const CNode2 *n, bool ms, int ms_rs)
{
    int k = 0;
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        if ((ms && bline(l)->line.ms == ms_rs) || (!ms && bline(l)->line.rs == ms_rs)) k++;
    }
    return k;
}

int inc_pod_ms(const CNode2 *n, bool ms, int ms_rs)
{
    int k = 0;
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.nomP != -1) {
            if ((ms && bline(l)->line.ms == ms_rs) || (!ms && bline(l)->line.rs == ms_rs)) k++;
        }
    }
    return k;
}

int inc_obr_ms(const CNode2 *n, bool ms, int ms_rs)
{
    int k = 0;
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.nomO != -1) {
            if ((ms && bline(l)->line.ms == ms_rs) || (!ms && bline(l)->line.rs == ms_rs)) k++;
        }
    }
    return k;
}



int inc_pod_pjezo(const CNode2 *n)
{
    int k = 0;
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.nomP != -1) {
            if (bline(l)->line.isPjezo) k++;
        }
    }
    return k;
}

int inc_obr_pjezo(const CNode2 *n)
{
    int k = 0;
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.nomO != -1) {
            if (bline(l)->line.isPjezo) k++;
        }
    }
    return k;
}




int inc_obr(const CNode2 *n)
{
    int k = 0;
    CLINE2 *l;
    for ( l = n->lines; l; l = nextl(l)) {
        if (bline(l)->line.nomO != -1) {
            k++;
        }
    }
    return k;
}



QString CNode2::getKod(long id_kod) const
{
    auto it = node.m_graph->map_kod.find(id_kod);

    if (it != node.m_graph->map_kod.end()) {
        return it->second;
    }
    return "";
}


QString CNode2::getKod() const
{
    return getKod(node.id_kod);
}


QString CNode2::getNamePT() const
{
    QString str;

    str = node.namePS;

//  if (str != "") return str;
//  str.Format("%s %s", getKod(), node.name);
    return str;
}



QString CNode2::getNamePTfull() const
{
    QString str;

    str = node.namePS;

    if (str != "") return str;
    str = QString("%1 %2").arg(getKod()).arg(node.name);
    return str;
}

QString CNode2::getNameFull() const
{
    return QString("%1 %2 %3").arg(getNodeTable(node.typ), getKod(), node.name);
}

QString CNode2::getNameFullFile() const
{
    QString fn = getFileName();
    QString name = QString("%1 %2 %3").arg(getNodeTable(node.typ), getKod(), node.name);

    if (fn != "") {
        name = QString("%1 (%2)").arg(name, fn);
    }

    return name;
}

QString CLine2::getNameFullFile() const
{
    QString fn = where(this)->getFileName();
    QString name = QString("%1 %2 - %3").arg(getLineTable(line.typ), where(this)->getName(), other(this)->getName());;

    if (fn != "") {
        name = QString("%1 (%2)").arg(name, fn);
    }

    return name;
}



QString CLine2::getNameFull() const
{
    return QString("%1 %2 - %3").arg(getLineTable(line.typ), where(this)->getName(), other(this)->getName());
}



QString CLine2::getNamePT() const
{
    QString str;
    str = QString("%1 - %2").arg(where(this)->getNamePT()).arg(other(this)->getNamePT());
    return str;
}


QString CNode2::getName() const
{
    QString str;

    str = QString("%1 %2").arg(getKod()).arg(node.name);

    if (node.externalSignID == SignNode_p) str = str + " " + S_POD;
    if (node.externalSignID == SignNode_o) str = str + " " + S_OBR;

    return str;
}

QString CLine2::getNameParent() const
{
//    CNode2 *parent = nullptr;

/*

    if (node.m_graph && internalNodeID > 0) {
        parent = node.m_graph->find(internalNodeID);
    }
*/
    
    QString str = getName();

    return str;
}



QString CNode2::getNameParent() const
{
    QString str;

    str = QString("%1 %2").arg(getKod()).arg(node.name);

//    CNode2 *p = pargent();


    return str;
}




QString CLine2::getName() const
{
    QString str;
//    str = QString("%1 - %2").arg(where(this)->getNamePT()).arg(other(this)->getNamePT());
    str = QString("%1 - %2").arg(where(this)->getName()).arg(other(this)->getName());
    return str;
}

int mark(CNode2 *n, bool zakr)
{
    if (!n) return 0;

    n->node.isPjezo = true;

    stack<CNode2 *> st;
    st.push(n);

    while ( !st.empty() ) {
        CNode2 *v = st.top();
        st.pop();

        for (CLINE2 *l = v->lines; l; l = nextl(l)) {
            if (!bline(l)->line.isPjezo && (!zakr || !bline(l)->line.isOtkl()) ) {
                bline(l)->line.isPjezo = true;
                other(l)->node.isPjezo = true;
                st.push(other(l));
            }
        }
    }
    return (int)st.size();
}


void CGraph2::WS1_2(CNode2 *v, CNode2 *t, bool zakr)
{
    queue<CNode2*> q;
    CNode2 *u, *p;
    CLINE2 *l;

    q.push(v);
    v->node.len2 = 0;

    while ( !q.empty() ) {
        p = q.front();
        q.pop();



        for ( l = p->lines; l; l = nextl(l)) {
            u = other(l);
//      if (u->node.len2 == -1) {
            if (u->node.len2 == -1 && (!zakr || !bline(l)->line.isOtkl())) {

                u->node.len2 = where(l)->node.len2+1;
                q.push(u);
                if (u == t) {
                    return;
                }
            }
        }
    }
}


/*
  связь мкжду узлами с учетом закрытыз если zakr
*/


void CGraph2::link2(CNode2 *s, CNode2 *t, bool zakr)
{
    reset_len();

    if (!t) {
        mark(s, zakr);
        return;
    }

    auto p = map_node.begin();

    while (p != map_node.end()) {
        p->second->node.len = -1;
        p->second->node.len2 = -1;
        p++;
    }

    WS1(s, NULL, zakr);
    WS1_2(t, NULL, zakr);

    CLINE2 *l;
    int k = 0;

    p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *n = p->second;

//    if (n->len != -1 && n->len2 != -1) {
        if ((n->node.len != -1 && n->node.len2 != -1 && n->node.len != t->node.len + n->node.len2  && n->node.len2 != s->node.len2 + n->node.len) || n == t || n == s) {
//    if (n->node.len != -1 && n->node.len2 != -1 && n->node.len != t->len + n->node.len2 || n == t) {

            n->node.isPjezo = 1;

            for ( l = n->lines; l; l = nextl(l)) {
                CNode2 *o = other(l);
                if ((o->node.len != -1 && o->node.len2 != -1 && o->node.len != t->node.len + o->node.len2 && o->node.len2 != s->node.len2 + o->node.len) || o == t || o == s) {
                    bline(l)->line.isPjezo = 1;
                    k++;
                }
            }
        }
        p++;
    }

    if (s) s->node.isPjezo = 1;
    if (t) t->node.isPjezo = 1;

}

long getExternalSignLineId(CLINE2* line)
{
    CLine2* l = bline(line);

    if (l->line.nomP > 0 && l->line.nomO > 0) {
        return 1;
    }
    if (l->line.nomP > 0 && other(line)->node.nomP > 0) {
        return 2;
    }
    if (l->line.nomP > 0 && other(line)->node.nomP <= 0) {
        return 4;
    }
    if (l->line.nomO > 0 && other(line)->node.nomO > 0) {
        return 3;
    }
    if (l->line.nomO > 0 && other(line)->node.nomO <= 0) {
        return 5;
    }
    return 0;
}




void CGraph2::swap(CLINE2 *ll)
{

    CNode2 *n1, *n2;
    CLine2 *l = bline(ll), *l2;
    CCoordList cl = l->line.m_NP;

    CLine line_old = l->line;

    n1 = where(ll);
    n2 = other(ll);

    remove_line(ll);
    l2 = insert_line(n2, n1, cl);

    if (l2) {
        l2->line = line_old;
        l2->line.m_NP.Swap();

        double a;

        int externalSignLineID = getExternalSignLineId(l2);

        if (externalSignLineID == SignLine_po || externalSignLineID == SignLine_op) {
            a = l2->line.nomP;
            l2->line.nomP = l2->line.nomO;
            l2->line.nomO = a;
            a = l2->line.idP2;
            l2->line.idP2 = l2->line.idO2;
            l2->line.idO2 = a;
            a = l2->line.nomgP;
            l2->line.nomgP = l2->line.nomgO;
            l2->line.nomgO = a;

            if (externalSignLineID == SignLine_po) {
                externalSignLineID = SignLine_op;
            }
            else if (externalSignLineID == SignLine_op) {
                externalSignLineID = SignLine_po;
            }
        }
    }
}


CLINE2 * CGraph2::find_line_typ(const CNode2 *c1, const CNode2 *c2, int typ)
{
    CLINE2 *l;

    if (!c1 || !c2) return nullptr;
    if (!c1->lines || !c2->lines) return nullptr;

    for ( l = c1->lines; l; l = nextl(l)) {

        CNode2 *o = other(l);
        CLine2 *ll = bline(l);

        if (o == c2 && (typ == -1 || ll->line.typ == typ)) return l;
//        if (other(l) == c2 && (typ == -1 || bline(l)->line.typ == typ)) return l;
    }
    return nullptr;
}

CFPoint centroid(CLINE2 *line)
{
    CFPoint pt;

    if (!line) return pt;
    CLine2 *l = bline(line);
    CNode2 *n1 = where(line);
    CNode2 *n2 = other(line);

    if (l->line.m_NP.size() == 0) return CFPoint((n1->node.coord.x+n2->node.coord.x)/2, (n1->node.coord.y+n2->node.coord.y)/2);

    return l->line.m_NP.centroid2();
}

void CGraph2::init_find_line_nom()
{
    m_map_line.clear();

    CLINE2 *l;
    auto p = map_node.begin();
    while (p != map_node.end()) {
        for ( l = p->second->lines; l; l = nextl(l)) {
            m_map_line[bline(l)->line.nomP] = l;
            m_map_line[bline(l)->line.nomO] = l;
        }
        p++;
    }
}

CLINE2 *CGraph2::find_line_nom(int id)
{
    if (id == -1) return nullptr;

    auto it = m_map_line.find(id);
    if (it != m_map_line.end()) {
        return it->second;
    }

    return nullptr;
}



void CGraph2::init_find_line_nom_hps()
{
    m_map_line_hps.clear();

    CLINE2 *l;
    auto p = map_node.begin();
    while (p != map_node.end()) {
        for ( l = p->second->lines; l; l = nextl(l)) {
            m_map_line_hps[bline(l)->line.idP2] = l;
            m_map_line_hps[bline(l)->line.idO2] = l;
        }
        p++;
    }
}

CLINE2 *CGraph2::find_line_nom_hps(int id)
{
    if (id == -1) return nullptr;

    auto it = m_map_line_hps.find(id);
    if (it != m_map_line_hps.end()) {
        return it->second;
    }

    return nullptr;

}




void CGraph2::init_find_line_nom_out(int typ)
{
    m_map_line_out.clear();

    CLINE2 *l;
    auto p = map_node.begin();
    while (p != map_node.end()) {
        for ( l = p->second->lines; l; l = nextl(l)) {

            if (bline(l)->line.typ == typ) {
                m_map_line_out[bline(l)->line.nomgP] = l;
                m_map_line_out[bline(l)->line.nomgO] = l;
            }
        }
        p++;
    }
}



CLINE2 *CGraph2::find_line_nom_out(int id)
{
    if (id == -1) return nullptr;

    auto it = m_map_line_out.find(id);
    if (it != m_map_line_out.end()) {
        return it->second;
    }

    return nullptr;
}




CNode2 * CGraph2::findPO(int id, char po)
{
//  if ( n.node_name.p == ' ') {
    return find(id);
//  }
//  CNode n2 = n;
//  n2.node_name.p = po;
//  return find(n2);
}


CLINE2 * CGraph2::find_line(const CNode2 *c1, const CNode2 *c2, char p, int typ)
{
    if (!c1 || !c2) return nullptr;
    if (!c1->lines || !c2->lines) return nullptr;

    for ( CLINE2 *ll = c1->lines; ll; ll = nextl(ll)) {
        CLine2 *l = bline(ll);
        CNode2 *n2 = other(ll);

        if (n2 == c2 && ((p == CPOD && l->line.nomP >= 0) || (p == COBR && l->line.nomO >= 0)) && (typ == -1 || l->line.typ == typ)) {
            return ll;
        }
    }


/*
    for ( l = c1->lines; l; l = nextl(l)) {
        if (other(l) == c2 && (
                    p == CPOD && bline(l)->line.nomP >= 0 || p == COBR && bline(l)->line.nomO >= 0)
                && (typ == -1 || bline(l)->line.typ == typ)
           ) return l;
    }
*/
    return nullptr;
}


CNode2 * CGraph2::findPO(int id)
{
    /*
      CNode *nn;

      if ( n.node_name.p == ' ') {
        return find(n);
      }
      CNode n2 = n;
      if (n2.node_name.p == 'П') {
        n2.node_name.p = 'О';
        return find(n2);
      }
      else {
        n2.node_name.p = 'П';
        return find(n2);
      }

      return nn;
    */

    return find(id);

}


bool CGraph2::isMark()
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        if (p->second->node.isPjezo) {
            return true;
        }
        p++;
    }
    return false;
}

void CGraph2::set_pjezo(bool yes)
{
    auto p = map_node.begin();
    while (p != map_node.end()) {
        CNode2 *n = p->second;
        n->node.isPjezo = yes;
        for (CLINE2 *l = n->lines; l; l = nextl(l)) {
            bline(l)->line.isPjezo = yes;
        }
        p++;
    }
}



void CGraph2::save_pjezo()
{
    auto p = map_node.begin();
    while (p != map_node.end()) {
        CNode2 *n = p->second;
        n->node.isPjezoSave = n->node.isPjezo;
        for (CLINE2 *l = n->lines; l; l = nextl(l)) {
            bline(l)->line.isPjezoSave = bline(l)->line.isPjezo;
        }
        p++;
    }
}

void CGraph2::restore_pjezo()
{
    auto p = map_node.begin();
    while (p != map_node.end()) {
        CNode2 *n = p->second;
        n->node.isPjezo = n->node.isPjezoSave;
        for (CLINE2 *l = n->lines; l; l = nextl(l)) {
            bline(l)->line.isPjezo = bline(l)->line.isPjezoSave;
        }
        p++;
    }
}


void CGraph2::clearVnutr(int id)
{
    CLINE2 *l, *ll;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *n = p->second;

        if (n->node.internalNodeID == id) {
            for ( l = n->lines; l; l = ll) {
                ll = nextl(l);
                remove_line(l);
            }
            p = map_node.erase(p);
        }
        else
            p++;
    }
}



void CGraph2::remove_fragments(set<int> & v)
{
    CLINE2 *l, *ll;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *n = p->second;

        auto it = v.find(n->node.fileID);

        if (it != v.end()) {
            for ( l = n->lines; l; l = ll) {
                ll = nextl(l);
                remove_line(l);
            }
            p = map_node.erase(p);
        }
        else
            p++;
    }
}


void CGraph2::remove_fragments(int fileID)
{
    CLINE2 *l, *ll;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *n = p->second;

        if (n->node.fileID == fileID) {
            for ( l = n->lines; l; l = ll) {
                ll = nextl(l);
                remove_line(l);
            }
            p = map_node.erase(p);
        }
        else
            p++;
    }
}



/*

void CGraph2::remove_lines_for_node(CNode2 *node)
{
    for ( l = n->lines; l; l = ll) {
        ll = nextl(l);
        remove_line(l);
    }
//    p = map_node.erase(p);
}
*/


int CGraph2::getKodId(const QString & kod)
{
    int id = -1;
    auto it = map_kod.begin();

    for (; it != map_kod.end(); it++) {
        if (it->second == kod) {
            id = it->first;
            break;
        }
    }
    return id;
}

void CGraph2::getKodIds(const QString & kod, set<int> &set_id)
{
    int id = -1;

    set_id.clear();

    auto it = map_kod.begin();

    for (; it != map_kod.end(); it++) {
        if (it->second == kod) {
            id = it->first;
            set_id.insert(id);
        }
    }
//  return id;
}




CNode2* CGraph2::findNodeName(const QString &  kod, const QString &  name, const char p1)
{
    set<int> set_id;
    getKodIds(kod, set_id);

    //  QString sss;
    //  sss.Format("id=%d", id);
    //  AfxMessageBox(sss, MB_OK|MB_ICONINFORMATION);


    if (set_id.size() >= 0) {
        auto p = map_node.begin();

        while (p != map_node.end()) {
            CNode2* n = p->second;

            if (set_id.find(n->node.id_kod) != set_id.end()) {
                if (isSameName(n->node.name, name)) {
                    return n;
                }
            }
            p++;
        }
    }

    return nullptr;
}


CLINE2* CGraph2::findLineName_vn(const QString &  kod_p, const QString &  name_p, const char p_p, const QString &  kod1, const QString &  name1, const char p1, const QString &  kod2, const QString &  name2, const char p2)
{
    CNode2* np = findNodeName(kod_p, name_p, p_p);

    set<int> set_id1;
    set<int> set_id2;

    getKodIds(kod1, set_id1);
    getKodIds(kod2, set_id2);

    if (set_id1.size() >= 0 && set_id2.size() >= 0) {
        auto p = map_node.begin();

        while (p != map_node.end()) {
            CNode2* n = p->second;

            if (set_id1.find(n->node.id_kod) != set_id1.end() && n->node.internalNodeID == np->id) {
                if (isSameName(n->node.name, name1)) {
                    for (CLINE2* l = n->lines; l; l = nextl(l)) {
                        CNode2* n2 = other(l);
                        if (set_id2.find(n2->node.id_kod) != set_id2.end()) {
                            if (isSameName(n2->node.name, name2)) {
                                return l;
                            }
                        }
                    }
                }
            }
            p++;
        }
    }

    return nullptr;
}


CLINE2* CGraph2::findLineName(const QString &  kod1, const QString &  name1, const char p1, const QString &  kod2, const QString &  name2, const char p2)
{
    set<int> set_id1;
    set<int> set_id2;

    getKodIds(kod1, set_id1);
    getKodIds(kod2, set_id2);

    if (set_id1.size() >= 0 && set_id2.size() >= 0) {
//        if (id1 >= 0 && id2 >= 0) {
        auto p = map_node.begin();

        while (p != map_node.end()) {
            CNode2* n = p->second;

            if (set_id1.find(n->node.id_kod) != set_id1.end() && n->node.internalNodeID == 0) {
                if (isSameName(n->node.name, name1)) {
                    for (CLINE2* l = n->lines; l; l = nextl(l)) {
                        CNode2* n2 = other(l);
                        if (set_id2.find(n2->node.id_kod) != set_id2.end()) {
                            if (isSameName(n2->node.name, name2)) {
                                return l;
                            }
                        }
                    }
                }
            }
            p++;
        }
    }

    return nullptr;
}


CNode2* CGraph2::findNodeName_vn(const QString &  kod_p, const QString &  name_p, const char p_p, const QString &  kod1, const QString &  name1, const char p1)
{
    CNode2* np = findNodeName(kod_p, name_p, p_p);

    set<int> set_id1;

    getKodIds(kod1, set_id1);

    if (set_id1.size() >= 0) {
        auto p = map_node.begin();

        while (p != map_node.end()) {
            CNode2* n = p->second;

            if (set_id1.find(n->node.id_kod) != set_id1.end() && n->node.internalNodeID == np->id) {
                if (isSameName(n->node.name, name1)) {
                    return n;
                }
            }
            p++;
        }
    }

    return nullptr;
}



CLINE2* CGraph2::findLine(int id)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        CLINE2* l;
        for (l = pp->lines; l; l = nextl(l)) {
            if (IsBegin(l)) {

                if (bline(l)->line.nomP == id || bline(l)->line.nomO == id) {
                    return l;
                }
            }
        }
        p++;
    }
    return nullptr;
}



double getLength(CLINE2* l, CFPoint pt)
{
    CNode2* n1 = where(l);
    CNode2* n2 = other(l);

    CCoordList cl;

    if (IsBegin(l)) {
        cl.push_back(n1->node.coord);
    }
    else {
        cl.push_back(n2->node.coord);
    }
    auto it = bline(l)->line.m_NP.begin();
    for (; it != bline(l)->line.m_NP.end(); ++it) {
        cl.push_back(*it);
    }
    if (IsBegin(l)) {
        cl.push_back(n2->node.coord);
    }
    else {
        cl.push_back(n1->node.coord);
    }

    double len = cl.Length2(pt, 2);

    return  len;
}


CFRect getRect(CLINE2* l)
{
    CFRect rect;

    CNode2* n1 = where(l);
    CNode2* n2 = other(l);

    rect.left = min(n1->node.coord.x, n2->node.coord.x);
    rect.right = max(n1->node.coord.x, n2->node.coord.x);

    rect.top = min(n1->node.coord.y, n2->node.coord.y);
    rect.bottom = max(n1->node.coord.y, n2->node.coord.y);


    auto it = bline(l)->line.m_NP.begin();
    for (; it != bline(l)->line.m_NP.end(); ++it) {
        //    cl.push_back(*it);

        rect.left = min(rect.left, it->x);
        rect.right = max(rect.right, it->x);

        rect.top = min(rect.top, it->y);
        rect.bottom = max(rect.bottom, it->y);
    }

    return rect;
}


int getLineTyp(const QString & tn);
int getNodeTyp(const QString & tn);

CLINE2* CGraph2::findLineOut(const QString & tn, int id)
{
    int typ = getLineTyp(tn);
    return findLineOut(typ, id);
}

CNode2* CGraph2::findNodeOut(const QString & tn, int id)
{
    int typ = getNodeTyp(tn);
    return findNodeOut(typ, id);
}

CLINE2* CGraph2::findLineOut(int typ, int id)
{
    if (typ < 0) return nullptr;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == 0) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                for (CLINE2* l = pp->lines; l; l = nextl(l)) {
                    if (IsBegin(l)) {
                        int idP = bline(l)->line.nomgP;
                        int idO = bline(l)->line.nomgO;
                        if ((bline(l)->line.typ == typ) && (id == idP || id == idO)) {
                            return l;
                        }
                    }
                }
            }
        }
        p++;
    }
    return nullptr;
}


void CGraph2::init_find_node_nom_pt_out()
{
    m_map_pt_out.clear();

    auto p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* pp = p->second;
        m_map_pt_out[pp->node.nomPT] = pp;
        p++;
    }
}


void CGraph2::init_find_node_2()
{
    m_map_pt_out.clear();

    auto p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* pp = p->second;
        m_map_node_2[pp->node.id2] = pp;
        p++;
    }
}

CNode2 *CGraph2::find_node_2(int id)
{
    if (id == -1) return nullptr;

    auto it = m_map_node_2.find(id);
    if (it != m_map_node_2.end()) {
        return it->second;
    }

    return nullptr;
}




CNode2 *CGraph2::find_pt_out(int id)
{
    if (id == -1) return nullptr;

    auto it = m_map_pt_out.find(id);
    if (it != m_map_pt_out.end()) {
        return it->second;
    }

    return nullptr;
}



CNode2* CGraph2::findPtOut(int id)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == 0) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                if ((pp->node.typ == TIP_PR || pp->node.typ == TIP_PO) && (pp->node.nomPT == id)) {
                    return pp;
                }
            }
        }
        p++;
    }
    return nullptr;
}



CNode2* CGraph2::findNodeOut(int typ, int id)
{
    if (typ < 0) return nullptr;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == 0) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                if ((pp->node.typ == typ) && (id == pp->node.nomgP || id == pp->node.nomgO)) {
                    return pp;
                }
            }
        }
        p++;
    }
    return nullptr;
}

CNode2* CGraph2::findNode2(int typ, int id2)
{
    if (typ < 0) return nullptr;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if ((pp->node.typ == typ) && (id2 == pp->node.id2)) {
            return pp;
        }
        p++;
    }
    return nullptr;
}


CLINE2* CGraph2::findLine2(int typ, int id)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        CLINE2* l;
        for (l = pp->lines; l; l = nextl(l)) {
            if (IsBegin(l)) {

                if ((bline(l)->line.typ == typ) && (id == bline(l)->line.idP2 || id == bline(l)->line.idO2)) {
                    return l;
                }
            }
        }
        p++;
    }
    return nullptr;
}




/**
CNode2 * CGraph2::findPtOut(int typ, int id)
{
    return nullptr;
}
*/

void CGraph2::vyd_lines(set<int>& v)
{
    reset();
    auto p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
            for (CLINE2* line = pp->lines; line; line = nextl(line)) {
                if (IsBegin(line)) {
                    CLine2* l = bline(line);
                    if (v.find(l->line.nomP) != v.end()) {
                        l->line.isPjezo = true;
                    }
                    if (l->line.nomP != l->line.nomO) if (v.find(l->line.nomO) != v.end()) {
                        l->line.isPjezo = true;
                    }
                }
            }
        }
        p++;
    }
}

// Выделяет участки по 

void CGraph2::vyd_lines_out(set<int>& v)
{
    reset();
    auto p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
            for (CLINE2* line = pp->lines; line; line = nextl(line)) {
                if (IsBegin(line)) {
                    CLine2* l = bline(line);
                    if (v.find(l->line.nomgP) != v.end()) {
//                        l->line.isPjezo = true;
                        l->line.isPjezoP = true;
                        where(l)->node.isPjezo = true;
                        other(l)->node.isPjezo = true;
                    }
//                    if (l->line.nomP != l->line.nomgO) if (v.find(l->line.nomgO) != v.end()) {
                    if (v.find(l->line.nomgO) != v.end()) {
//                        l->line.isPjezo = true;
                        l->line.isPjezoO = true;
                        where(l)->node.isPjezo = true;
                        other(l)->node.isPjezo = true;
                    }
                }
            }
        }
        p++;
    }
}





void CGraph2::vyd_nodes(set<int>& v)
{
    reset();
    auto p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
            if (v.find(pp->id) != v.end()) pp->node.isPjezo = true;
        }
        p++;
    }
}



bool CGraph2::inGraphNode(int typ, int parent_id, int fileID)
{
    if (typ < 0) return false;

    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.typ == typ && pp->node.internalNodeID == parent_id && pp->node.fileID == fileID) {
            return true;
        }
        p++;
    }

    return false;
}

bool CGraph2::inGraphLine(int typ, int parent_id, int fileID)
{
    if (typ < 0) return false;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == parent_id && pp->node.fileID == fileID) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                for (CLINE2* l = pp->lines; l; l = nextl(l)) {
                    if (bline(l)->line.typ == typ) {
                        return l;
                    }
                }
            }
        }
        p++;
    }

    return false;
}




bool CGraph2::inGraphNodeVyd(int typ, int parent_id)
{
    if (typ < 0) return false;

    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.typ == typ && pp->node.internalNodeID == parent_id && pp->node.isPjezo) {
            return true;
        }
        p++;
    }

    return false;
}

bool CGraph2::inGraphLineVyd(int typ, int parent_id)
{
    if (typ < 0) return false;
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == parent_id) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                for (CLINE2* l = pp->lines; l; l = nextl(l)) {
                    if (bline(l)->line.typ == typ && (bline(l)->line.isPjezo || bline(l)->line.isPjezoP || bline(l)->line.isPjezoO)) {
                        return l;
                    }
                }
            }
        }
        p++;
    }

    return false;
}



CGidFile* CNode2::getFile()
{
    if (node.m_graph->m_cxema) {
//        return node.m_graph->m_cxema->getGidFile(node.fileID);
    }
    return 0;
}

QString CNode2::getFileName() const
{
    Fragment * fr = this->node.m_graph->m_cxema->getFragment(this->node.fileID);

    if (fr) {
        return fr->m_name;
    }
    return "";
}

CCoordList get_NP(const CLINE2* l)
{
    if (IsBegin(l)) return bline(l)->line.m_NP;

    CCoordList cl = bline(l)->line.m_NP;
    cl.Swap();
    return cl;
}

QString AnyName(const CNode2* n)
{
    if (!n) return "";
    if (n->node.namePS != "") return n->node.namePS;
    return n->node.name;
}


// Вернуть rect по выделенным участков

bool CGraph2::GetMarkRect(CFRect& rect)
{
    int internalNodeID = 0;

    rect = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);

    auto p = map_node.begin();

    bool ret = false;

    while (p != map_node.end()) {
        CNode2 *pp = p->second;

        if (pp->node.internalNodeID == internalNodeID) {
            if (pp->node.isPjezo && (pp->node.coord.x != 0 || pp->node.coord.y != 0)) {
                rect.AddPoint(pp->node.coord);
                ret = true;
            }

            CLINE2 *l;
            for (l = pp->lines; l; l = nextl(l)) {
                if (IsBegin(l) && bline(l)->line.isPjezo) {
//                    int qq = bline(l)->line.m_NP.size();
                    if (bline(l)->line.m_NP.size() > 0) {
                        CFRect r = bline(l)->line.m_NP.GetFRect();
                        rect.AddPoint(r.BottomRight());
                        rect.AddPoint(r.TopLeft());
                    }
                    ret = true;
                }
            }
        }
        p++;
    }

    return ret;
}

void CGraph2::setOnOff(bool on_off)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *pp = p->second;

        if (pp->node.isPjezo) {
            pp->node.isPodp = on_off;
        }


        if (1) {

            CLINE2 *l;
            for (l = pp->lines; l; l = nextl(l)) {
                if (IsBegin(l)) {
                    if (bline(l)->line.isPjezo) {
                        bline(l)->line.isPodp = on_off;
                    }
                }
            }
        }
        p++;
    }
}


double rasst(CFPoint p1, CFPoint p2, CFPoint p)
{
    double l1 = Length(p1, p);
    double l2 = Length(p2, p);
    double l = Length(p1, p2);

    double ll = LengthPL(p.x, p.y, p1.x, p1.y, p2.x, p2.y);

    if (fabs(l1+l2-l)) {
    }
    return 0;
}

double rasst(CLINE2 *l, CFPoint p)
{
    if (bline(l)->line.m_NP.size() == 0) {
        return Length(p, where(l)->node.coord);
    }

    CFPoint p1 = where(l)->node.coord, p2;

    auto it = bline(l)->line.m_NP.begin();

    double d_len = 0, len = 0, len2 = 0;

    for (; it != bline(l)->line.m_NP.end(); it++) {
        CFPoint p2 = *it;

        double ll = LengthPL(p.x, p.y, p1.x, p1.y, p2.x, p2.y);

        if (ll < D5) {
            len += Length(p1, p);
            return len;
        }

        len += Length(p1, p2);
        p1 = p2;
    }

    p2 = other(l)->node.coord;

    double ll = LengthPL(p.x, p.y, p1.x, p1.y, p2.x, p2.y);

    if (ll < D5) {
        len += Length(p1, p);
        return len;
    }

    len += Length(p1, p2);

    return len;
}

void CGraph2::clean_out(set<int> & set_fileID)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2 *pp = p->second;

        if (set_fileID.find(pp->node.fileID) != set_fileID.end()) {
            pp->node.calculationID = -1;
            pp->node.nomgP = -1;
            pp->node.nomgO = -1;

            pp->node.qz = 0;
            pp->node.qP = 0;
            pp->node.qO = 0;

            for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
                bline(l)->line.nomgP = -1;
                bline(l)->line.nomgO = -1;

                bline(l)->line.pod.set_zero();
                bline(l)->line.obr.set_zero();
            }
        }
        p++;
    }
}

bool CGraph2::setMark(const CCoordList &NP, bool val)
{
  auto p = map_node.begin();

  while (p != map_node.end()) {
    CNode2 *n = p->second;

    if (!n->node.isHide && NP.find3(n->node.coord)) {
      n->node.isPjezo = val;

      for (CLINE2 *l = n->lines; l; l = nextl(l)) {
        if (!IsBegin(l)) continue;
        CNode2 *n2 = other(l);
        if (n2 && !bline(l)->line.isHide && NP.find3(n2->node.coord)) {
          bline(l)->line.isPjezo = val;
        }
      }
    }
    p++;
  }

  return true;
}

// Выделить участок по ms или rs
// typ 1 - ms 2 - rs


void CGraph2::vydMag(int id)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        for ( CLINE2 *l = p->second->lines; l; l = nextl(l)) {
            if (bline(l)->line.mag == id) {


                CLine2 *ll = bline(l);

                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;
            }
        }
        p++;
    }
}



void CGraph2::vydMS(int id)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        for ( CLINE2 *l = p->second->lines; l; l = nextl(l)) {
            if (bline(l)->line.ms == id) {


                CLine2 *ll = bline(l);

                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;
            }
        }
        p++;
    }
}

void CGraph2::vydRS(int id)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        for ( CLINE2 *l = p->second->lines; l; l = nextl(l)) {
            if (bline(l)->line.rs == id) {
                CLine2 *ll = bline(l);

                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;
            }
        }
        p++;
    }
}

void CGraph2::vydMSRS(int typ, int id)
{
}


void CGraph2::clearFile(set<int>& v)
{
    CLINE2* l, * ll;
//    CMapGraph::iterator p = map_node.begin();

//    while (p != map_node.end()) {

    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* n = p->second;

        set<int>::const_iterator it = v.find(n->node.fileID);

        if (it != v.end()) {
            for (l = n->lines; l; l = ll) {
                ll = nextl(l);
                remove_line(l);
            }
            p = map_node.erase(p);
        }
        else
            p++;
    }
}

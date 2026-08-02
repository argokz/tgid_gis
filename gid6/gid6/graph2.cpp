#include "stdafx.h"

#include "graph2.h"

#include "cxema.h"

#include "ado.h"

int nn = 0;

bool isSameName(CString s1, CString s2)
{
    if (s1 == s2) return true;

    int len1 = s1.GetLength();
    int len2 = s2.GetLength();

    if (len1 != len2) return false;

    for (int i = 0; i < len1; i++) {
        if (s1[i] != s2[i] && s2[i] != '_') return false;
    }

    return true;
}


bool isMarked(const CNode2* n)
{
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (bline(l)->line.isPjezo) {
            return true;
        }
    }
    return false;
}


CGraph2::CGraph2(CCxema* cxema)
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


CNode2* CGraph2::ins(int id)
{
    CNode2* node = new CNode2();
    node->id = id;
    node->node.m_graph = this;

    map_node[id] = node;

    is_changed = true;

    return node;
}

CNode2* CGraph2::find_ins(int id)
{
    is_changed = true;

    map<long, CNode2*>::iterator it = map_node.find(id);

    if (it != map_node.end()) {
        m_rect.AddPoint(it->second->node.coord);
        return it->second;
    }
    return ins(id);
}

CNode2* CGraph2::find(int id)
{
    if (id == 0) return nullptr;

    map<long, CNode2*>::iterator it = map_node.find(id);

    if (it != map_node.end()) {
        if (!it->second->node.isHide) {
            return it->second;
        }
    }

    return nullptr;
}

//-----------------------------------------------
// Найти линию между узлами
//-----------------------------------------------

CLINE2* CGraph2::Link(const CNode2* c1, const CNode2* c2)
{
    CLINE2* l;

    if (!c1 || !c2) return nullptr;
    if (!c1->lines || !c2->lines) return nullptr;

    for (l = c1->lines; l; l = next(l)) {
        if (other(l) == c2) return l;
    }
    return nullptr;
}

CFRect GetLineRect(CFPoint pt1, CFPoint pt2, const CCoordList& cl);



CLine2* CGraph2::insert_line(CNode2* n1, CNode2* n2, const CCoordList& NP, bool paral)
{
    CNode2* c1, * c2;
    CLINE2* ll;
    CLine2* l = 0;
    is_changed = true;

    //  if (n1 == n2) return nullptr;

    //  c1 = find(*n1);  if (!c1) return nullptr;
    //  c2 = find(*n2);  if (!c2) return nullptr;

    if (n1 == n2) return nullptr;

    c1 = n1;  if (!c1) return nullptr;
    c2 = n2;  if (!c2) return nullptr;

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

        unsigned int s = (unsigned int)bline(l);

        c2->lines = (CLINE2*)(s | 0x1);

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

    unsigned int s = (unsigned int)bline(l);

    c2->lines = (CLINE2*) (s|0x1);

    l->line.m_NP = NP;
    l->line.rect = GetLineRect(n1->node.coord, n2->node.coord, NP);
  }

  return l;
}
*/


bool CGraph2::getBorderMap(int fileID, int internalNodeID, CFRect& rect)
{
    map<int, map<int, CFRect> >::const_iterator it1 = m_map_border.find(fileID);
    if (it1 == m_map_border.end()) return false;
    map<int, CFRect>::const_iterator it2 = it1->second.find(internalNodeID);
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

    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.fileID == fileID && pp->node.internalNodeID == internalNodeID) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                rect.AddPoint(pp->node.coord);
            }

            CLINE2* l;
            for (l = pp->lines; l; l = next(l)) {
                if (IsBegin(l)) {
                    int qq = bline(l)->line.m_NP.size();
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

    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == internalNodeID) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                rect.AddPoint(pp->node.coord);
            }

            CLINE2* l;
            for (l = pp->lines; l; l = next(l)) {
                if (IsBegin(l)) {
                    int qq = bline(l)->line.m_NP.size();
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


    m_map_border[0][internalNodeID] = rect;
    is_changed = false;

    return rect;
}


//-----------------------------------------------
// Найти узел по координатам
// в радиусе r
//-----------------------------------------------


CNode2* CGraph2::find(int parent_id, CFPoint pt, double r)
{
    CNode2* node, * n0 = nullptr;
    double h, minh = LONG_MAX;
    r = max(2., r);

    map<long, CNode2*>::iterator p = map_node.begin();

    while (p != map_node.end()) {
        node = p->second;
        if (!node->node.isHide && node->node.internalNodeID == parent_id) {
            h = hypot(pt.x - node->node.coord.x, pt.y - node->node.coord.y);
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
// Найти узел внутри многоугольника
//-----------------------------------------------


CNode2* CGraph2::find(int parent_id, const CCoordList& cl)
{
    CNode2* node;

    CFRect rect = cl.GetFRect();

    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {
        node = p->second;

        if (!node->node.isHide && node->node.internalNodeID == parent_id) {
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


CNode2* CGraph2::find(int parent_id, int typ, const CCoordList& cl)
{
    CNode2* n = nullptr;
    CFRect rect = cl.GetFRect();

    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {
        n = p->second;
        if (!n->node.isHide && n->node.internalNodeID == parent_id) {
            if (n->node.typ == typ) {
                if (rect.PtInRect(n->node.coord)) {
                    if (cl.find3(n->node.coord)) return n;
                }
            }
        }
        p++;
    }

    return nullptr;
}





bool chline(CLine2* l, double x, double y, double dx);

//-----------------------------------------------
// Найти линию по координатам
// в радиусе r
//-----------------------------------------------

bool IsRezhim(const CLINE2* l);


CLINE2* CGraph2::findLine(int parent_id, CFPoint pt, double r)
{
    CLINE2* l;

    map<long, CNode2*>::iterator p = map_node.begin();

    int qq = 0;

    while (p != map_node.end()) {
        CNode2* node = p->second;

        if (!node->node.isHide && node->node.internalNodeID == parent_id) {
            if (node->node.coord.x != 0 && node->node.coord.y != 0) {
                for (l = node->lines; l; l = next(l)) {
                    if (!bline(l)->line.isHide && other(l)->node.coord.x != 0 && other(l)->node.coord.y != 0) {
                        if (IsBegin(l) && chline(bline(l), pt.x, pt.y, r)) {
                            if (!IsRezhim(l)) {  // Не искать П1 и П2 для Астагны
                                return l;
                            }
                        }
                    }
                }
            }
        }
        p++;
    }

    return nullptr;
}



bool chline(double x1, double y1, double x2, double y2, double xx, double yy, double dx);

static int numb_of_line = 0;


bool chline(CLine2* l, double x, double y, double dx)
{

    long x1, y1, x2, y2;
    CNode2* n1, * n2;
    numb_of_line = 0;

    CCoordList::const_iterator it;

    //  CString ss;
    //  ss.Format("l->m_NP.size()=%d", l->m_NP.size());
    //LOG3(ss);

    it = l->line.m_NP.begin();
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
            numb_of_line++;
            if (::chline(x1, y1, x2, y2, x, y, dx)) return true;
            x1 = x2;  y1 = y2;
        }
        x2 = n2->node.coord.x;
        y2 = n2->node.coord.y;
        numb_of_line++;
        //LOG2();
        if (::chline(x1, y1, x2, y2, x, y, dx)) return true;
    }
    //LOG2();

    return false;
}


void CGraph2::clear()
{
#if 0
    CLINE2* l, * ll;
    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {

        for (l = p->second->lines; l; l = ll) {
            ll = next(l);
            //        if (IsBegin(l)) {
            //            delete l;
            //        }
            next(l) = nullptr;
        }
        p->second = nullptr;
        delete p->second;
        p++;
    }

    map_node.clear();
#else

    CLINE2* l, * ll;
    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {

        CNode2* pp = p->second;

        for (l = pp->lines; l; l = ll) {
            ll = next(l);
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
#endif;
}


void CGraph2::remove_node(CNode2& node)
{
    is_changed = true;

    CMapGraph::iterator p = map_node.find(node.id);
    if (p != map_node.end()) map_node.erase(p);
}

void CGraph2::remove_line(CLINE2* l1)
{
    is_changed = true;

    if (!l1) return;

    CNode2* n1 = where(l1);
    CNode2* n2 = other(l1);

    if (n1 == n2) return;

    CLINE2* l2 = sym(l1);

    if (n1->lines == l1) {
        n1->lines = next(l1);
    }
    if (n2->lines == l2) {
        n2->lines = next(l2);
    }


    CLINE2* ll = nullptr;

    for (CLINE2* l = n1->lines; l; l = ll) {
        ll = next(l);

        if (ll && l1 == ll) {
            next(l) = next(ll);
            break;
        }
    }

    for (CLINE2* l = n2->lines; l; l = ll) {
        ll = next(l);

        if (ll && l2 == ll) {
            next(l) = next(ll);
            break;
        }
    }

    delete bline(l1);
}


CNode2* CGraph2::sameMarkNode()
{
    CMapGraph::iterator p = map_node.begin();
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

    CMapGraph::const_iterator p;
    p = map_node.begin();

    while (p != map_node.end()) {
        if (p->second->node.internalNodeID == parent_id) {
            if (p->second->node.coord.x != 0 && p->second->node.coord.y != 0) {
                n++;
            }
        }
        p++;
    }
    return n;
}


bool CGraph2::reset()
{
    CMapGraph::iterator p = map_node.begin();
    CLINE2* l;
    bool ret = false;

    while (p != map_node.end()) {
        if (p->second->node.isPjezo) {
            ret = true;
        }
        p->second->node.len = -1;
        p->second->node.isPjezo = false;

        for (l = p->second->lines; l; l = next(l)) {
            if (bline(l)->line.isPjezo) {
                ret = true;
            }
            bline(l)->line.isOsmotr = false;
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
    CMapGraph::iterator p = map_node.begin();
    CLINE2* l;
    bool ret = false;

    while (p != map_node.end()) {
        p->second->node.isPjezo = false;
        p->second->node.len = -1;
        for (l = p->second->lines; l; l = next(l)) {
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
    CMapGraph::iterator p = map_node.begin();
    CLINE2* l;
    bool ret = false;

    while (p != map_node.end()) {
        p->second->node.isPjezo = false;
        p->second->node.len = -1;
        for (l = p->second->lines; l; l = next(l)) {

            set<int>::const_iterator it = set_l.find(bline(l)->line.nomP);

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
    CMapGraph::iterator p = map_node.begin();
    CLINE2* l;
    bool ret = false;

    while (p != map_node.end()) {
        if (!p->second->node.isPjezo) {
            ret = true;
        }
        p->second->node.len = -1;
        p->second->node.isPjezo = false;
        for (l = p->second->lines; l; l = next(l)) {
            if (!bline(l)->line.isPjezo) {
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


CLINE2* CGraph2::find_line_obr(const CLINE2* line)
{
    CNode2* n1 = where(line);
    CNode2* n2 = other(line);

    for (CLINE2* l = n1->lines; l; l = next(l)) {
        if (other(l) == n2 && l != line) {
            return l;
        }
    }
    return nullptr;
}


void CGraph2::WS1(CNode2* v, CNode2* t, bool zakr)
{
    queue<CNode2*> q;
    CNode2* u, * p;
    CLINE2* l;
    //  int maxlen = 32767;

    q.push(v);
    v->node.len = 0;

    while (!q.empty()) {
        p = q.front();
        q.pop();
        //    if (p->len > maxlen) break;

        for (l = p->lines; l; l = next(l)) {
            u = other(l);
            //      if (u->len == -1 && u != v && !l->uch->otkl && l->uch->nom_m != -1) {
            if (u->node.len == -1 && (!zakr || !bline(l)->line.isLineOtkl())) {

                u->node.len = where(l)->node.len + 1;
                q.push(u);
                if (u == t) {
                    return;
                    //          maxlen = u->len;
                }
            }
        }
    }
}

void CGraph2::WG1(CNode2* s, CNode2* t, bool zakr)
{
    stack<CNode2*> st;
    CNode2* v, * u, * p;
    CLINE2* l;

    st.push(t);
    v = t;

    while (v != s) {
        for (l = v->lines; l; l = next(l)) {
            u = other(l);
            if (u->node.len == v->node.len - 1) break;
        }
        if (!l) break;
        st.push(u);
        v = u;
    }

    p = nullptr;

    while (!st.empty()) {
        v = st.top();
        st.pop();
        v->node.isPjezo = true;
        if (list_pjezo.empty() || !(v->id == list_pjezo.back())) {
            list_pjezo.push_back(v->id);
        }
        if (p) {
            for (l = p->lines; l; l = next(l)) {
                //        if (l->other_end->where == v && !l->uch->otkl && l->uch->nom_m != -1) {
                if (other(l) == v && (!zakr || !bline(l)->line.isLineOtkl())) {
                    bline(l)->line.isPjezo = true;
                }
            }
        }
        p = v;
    }
}


void CGraph2::WS1_h(CNode2* v, CNode2* t, double dh)
{
    queue<CNode2*> q;
    CNode2* u, * p;
    CLINE2* l;
    double h;

    q.push(v);
    v->node.len = 0;

    while (!q.empty()) {
        p = q.front();
        q.pop();

        for (l = p->lines; l; l = next(l)) {
            u = other(l);
            h = u->node.hz;
            //      if (u->len == -1 && (u->typ != TIP_PR || fabs(v->data.geodz-(u->data.geodz+h)) < dh)) {
            if (u->node.len == -1 && (fabs(v->node.geoMarkTopTube - (u->node.geoMarkTopTube + h)) < dh)) {
                u->node.len = where(l)->node.len + 1;
                bline(l)->line.isPjezo = 1;
                q.push(u);
                if (u == t) {
                    return;
                }
            }
        }
    }
}



CNode2* CGraph2::findPS1(const char* name)
{
    if (name[0] == 0) return nullptr;

    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* n = p->second;
        if (isSameName(n->node.namePS, name)) {
            return n;
        }
        p++;
    }
    return nullptr;
}


CNode2* CGraph2::findPS(const char* name)
{
    if (name[0] == 0) return nullptr;

    CNode2* n = findPS1(name);
    return n;
}

void CGraph2::reset_len()
{
    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {
        p->second->node.len = -1;
        p++;
    }
}




void CGraph2::link(CNode2* s, CNode2* t)
{
    if (!s || !t) return;

    reset_len();
    WS1(s, t);

    WG1(s, t);
}

bool CLine::isLineOtkl() const
{
    return (pod.isOtkl && obr.isOtkl) || (pod.isOtklZD && obr.isOtklZD);
}



CString CLine2::getTable() const
{
    return getLineTable(line.typ);
}

CString CLine2::getTableMySQL() const
{
    return getLineTableMySQL(line.typ);
}

CString CLine2::getTableOutMySQL() const
{
    return getLineTableOutMySQL(line.typ);
}



CString getLineTableOut(int typ);
CString getNodeTableOut(int typ);


CString CLine2::getTableOut() const
{
    return getLineTableOut(line.typ);
}


CString CNode2::getTable() const
{
    if (node.typ < 0 || node.typ >= nodefsize) return "";

    return nodef[node.typ].table;
}

CString CNode2::getTableMySQL() const
{
    return getNodeTableMySQL(node.typ);
}

CString CNode2::getTableOut() const
{
    return getNodeTableOut(node.typ);
}

CString CNode2::getTableOutMySQL() const
{
    return getNodeTableOutMySQL(node.typ);
}


CCoordList CLine2::getCLall()
{
    CCoordList cl;
    CNode2* n1 = where(this);
    CNode2* n2 = other(this);

    if (IsBegin(this)) {
        cl.push_back(n1->node.coord);
    }
    else {
        cl.push_back(n2->node.coord);
    }
    CCoordList::const_iterator it = bline(this)->line.m_NP.begin();
    for (; it != bline(this)->line.m_NP.end(); ++it) {
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

CNode2* CGraph2::find2(const CNode2& n)
{
    CNode2* nn = find(n.id);
    return nn;
}

int CNode2::typPS() const
{
    if (PSName(this) == "") return -1;

    return node._typPS;
}

CString PSName(const CNode2* n)
{
    if (!n) return "";

    if (n->node.namePS != "") return n->node.namePS;

    extern BOOL _isDopNameAstana;

    //  if (_isDopNameAstana) return "";

    //  if (n->node.name[0] == '#') return "";

    return n->node.name;
}


CString PSFullName(const CNode2* n)
{
    if (!n) return "";

    if (n->node.namePS != "") return n->node.namePS;

    extern BOOL _isDopNameAstana;

    //  if (_isDopNameAstana) return "";

    return n->getName();
}


int incP(const CNode2* n)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (bline(l)->line.isPjezo) k++;
    }
    return k;
}

int inc(const CNode2* n)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        k++;
    }
    return k;
}

int inc_pod(const CNode2* n)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (bline(l)->line.nomP != -1) {
            k++;
        }
    }
    return k;
}


int inc_ms(const CNode2* n, bool ms, int ms_rs)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (ms && bline(l)->line.ms == ms_rs || !ms && bline(l)->line.rs == ms_rs) k++;
    }
    return k;
}

int inc_pod_ms(const CNode2* n, bool ms, int ms_rs)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (bline(l)->line.nomP != -1) {
            if (ms && bline(l)->line.ms == ms_rs || !ms && bline(l)->line.rs == ms_rs) k++;
        }
    }
    return k;
}

int inc_obr_ms(const CNode2* n, bool ms, int ms_rs)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (bline(l)->line.nomO != -1) {
            if (ms && bline(l)->line.ms == ms_rs || !ms && bline(l)->line.rs == ms_rs) k++;
        }
    }
    return k;
}



int inc_pod_pjezo(const CNode2* n)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (bline(l)->line.nomP != -1) {
            if (bline(l)->line.isPjezo) k++;
        }
    }
    return k;
}

int inc_obr_pjezo(const CNode2* n)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (bline(l)->line.nomO != -1) {
            if (bline(l)->line.isPjezo) k++;
        }
    }
    return k;
}




int inc_obr(const CNode2* n)
{
    int k = 0;
    CLINE2* l;
    for (l = n->lines; l; l = next(l)) {
        if (bline(l)->line.nomO != -1) {
            k++;
        }
    }
    return k;
}



CString CNode2::getKod(long id_kod) const
{
    map<long, CString>::const_iterator it = node.m_graph->map_kod.find(id_kod);

    if (it != node.m_graph->map_kod.end()) {
        return it->second;
    }
    return "";
}


CString CNode2::getKod() const
{
    return getKod(node.id_kod);
}


CString CNode2::getNamePT() const
{
    CString str;

    str = node.namePS;

    //  if (str != "") return str;
    //  str.Format("%s %s", getKod(), node.name);
    return str;
}



CString CNode2::getNamePTfull() const
{
    CString str;

    str = node.namePS;

    if (str != "") return str;
    str.Format("%s %s", getKod(), node.name);
    return str;
}


CString CLine2::getNamePT() const
{
    CString str;
    str.Format("%s - %s", where(this)->getNamePT(), other(this)->getNamePT());
    return str;
}


CString CNode2::getName() const
{
    CString str;

    str.Format("%s %s", getKod(), node.name);
    return str;
}

CString CNode2::getNameFull() const
{
    CString name = getName();

    if (node.internalNodeID) {
        CNode2* nn = node.m_graph->m_cxema->m_graph->find(node.internalNodeID);
        CString name_n = nn->getName();
        name.Format("(%s) %s", name_n, name);
    }
    return name;
}




CString CLine2::getName() const
{
    CString str;
    str.Format("%s - %s", where(this)->getName(), other(this)->getName());
    return str;
}

int mark(CNode2* n, bool zakr)
{
    if (!n) return 0;

    n->node.isPjezo = true;

    stack<CNode2*> st;
    st.push(n);

    while (!st.empty()) {
        CNode2* v = st.top();
        st.pop();

        for (CLINE2* l = v->lines; l; l = next(l)) {
            if (!bline(l)->line.isPjezo && (!zakr || !bline(l)->line.isLineOtkl())) {
                bline(l)->line.isPjezo = true;
                other(l)->node.isPjezo = true;
                st.push(other(l));
            }
        }
    }
    return st.size();
}


void CGraph2::WS1_2(CNode2* v, CNode2* t, bool zakr)
{
    queue<CNode2*> q;
    CNode2* u, * p;
    CLINE2* l;

    q.push(v);
    v->node.len2 = 0;

    while (!q.empty()) {
        p = q.front();
        q.pop();



        for (l = p->lines; l; l = next(l)) {
            u = other(l);
            //      if (u->node.len2 == -1) {
            if (u->node.len2 == -1 && (!zakr || !bline(l)->line.isLineOtkl())) {

                u->node.len2 = where(l)->node.len2 + 1;
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


void CGraph2::link2(CNode2* s, CNode2* t, bool zakr)
{
    reset_len();

    if (!t) {
        mark(s, zakr);
        return;
    }

    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {
        p->second->node.len = -1;
        p->second->node.len2 = -1;
        p++;
    }

    WS1(s, NULL, zakr);
    WS1_2(t, NULL, zakr);

    CLINE2* l;
    int k = 0;

    p = map_node.begin();
    
    while (p != map_node.end()) {
        CNode2* n = p->second;

        if (n->node.len != -1 && n->node.len2 != -1 && n->node.len != t->node.len + n->node.len2 && n->node.len2 != s->node.len2 + n->node.len || n == t || n == s) {
            n->node.isPjezo = 1;

// Временная мера, пока не придумаю нормальный алгоритм
        int dl = t->node.len;

            if (n->node.len < dl*2) {
                for (l = n->lines; l; l = next(l)) {
                    CNode2* o = other(l);
                    if (o->node.len != -1 && o->node.len2 != -1 && o->node.len != t->node.len + o->node.len2 && o->node.len2 != s->node.len2 + o->node.len || o == t || o == s) {
                        bline(l)->line.isPjezo = 1;
                        k++;
                    }
                }
            }
        }

        p++;
    }

    if (s) s->node.isPjezo = 1;
    if (t) t->node.isPjezo = 1;
}

long getExternalSignLineId(CLINE2* line);


void CGraph2::swap(CLINE2* ll)
{

    CNode2* n1, * n2;
    CLine2* l = bline(ll), * l2;
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
            a = l2->line.nomP; l2->line.nomP = l2->line.nomO; l2->line.nomO = a;
            a = l2->line.idP2; l2->line.idP2 = l2->line.idO2; l2->line.idO2 = a;
            a = l2->line.nomgP; l2->line.nomgP = l2->line.nomgO; l2->line.nomgO = a;

            if (externalSignLineID == SignLine_po) {
                externalSignLineID = SignLine_op;
            }
            else if (externalSignLineID == SignLine_op) {
                externalSignLineID = SignLine_po;
            }
        }
    }
}


CLINE2* CGraph2::find_line_typ(const CNode2* c1, const CNode2* c2, int typ)
{
    CLINE2* l;

    if (!c1 || !c2) return nullptr;
    if (!c1->lines || !c2->lines) return nullptr;

    for (l = c1->lines; l; l = next(l)) {
        if (other(l) == c2 && (typ == -1 || bline(l)->line.typ == typ)
            ) return l;
    }
    return nullptr;
}

CFPoint centroid(CLINE2* line)
{
    CFPoint pt;

    if (!line) return pt;
    CLine2* l = bline(line);
    CNode2* n1 = where(line);
    CNode2* n2 = other(line);

    if (l->line.m_NP.size() == 0) return CFPoint((n1->node.coord.x + n2->node.coord.x) / 2, (n1->node.coord.y + n2->node.coord.y) / 2);

    return l->line.m_NP.centroid2();
}

void CGraph2::init_find_line_nom()
{
    m_map_line.clear();

    CLINE2* l;
    CMapGraph::iterator p = map_node.begin();
    while (p != map_node.end()) {
        for (l = p->second->lines; l; l = next(l)) {
            m_map_line[bline(l)->line.nomP] = l;
            m_map_line[bline(l)->line.nomO] = l;
        }
        p++;
    }
}

CLINE2* CGraph2::find_line_nom(int id)
{
    if (id == -1) return nullptr;

    map<int, CLINE2*>::const_iterator it = m_map_line.find(id);
    if (it != m_map_line.end()) {
        return it->second;
    }

    return nullptr;
}


CLINE2 *CGraph2::find_line_big(int id)
{
    m_map_line.clear();

    CLINE2* l;
    CMapGraph::iterator p = map_node.begin();
    while (p != map_node.end()) {
        for (l = p->second->lines; l; l = next(l)) {
            if (bline(l)->line.pipeSectionID == id) {
                return l;
            }
        }
        p++;
    }

    return nullptr;
}



void CGraph2::init_find_line_nom_hps()
{
    m_map_line_hps.clear();

    CLINE2* l;
    CMapGraph::iterator p = map_node.begin();
    while (p != map_node.end()) {
        for (l = p->second->lines; l; l = next(l)) {
            m_map_line_hps[bline(l)->line.idP2] = l;
            m_map_line_hps[bline(l)->line.idO2] = l;
        }
        p++;
    }
}

CLINE2* CGraph2::find_line_nom_hps(int id)
{
    if (id == -1) return nullptr;

    map<int, CLINE2*>::const_iterator it = m_map_line_hps.find(id);
    if (it != m_map_line_hps.end()) {
        return it->second;
    }

    return nullptr;

}




void CGraph2::init_find_line_nom_out(int typ)
{
    m_map_line_out.clear();

    CLINE2* l;
    CMapGraph::iterator p = map_node.begin();
    while (p != map_node.end()) {
        for (l = p->second->lines; l; l = next(l)) {

            if (bline(l)->line.typ == typ) {
                m_map_line_out[bline(l)->line.nomgP] = l;
                m_map_line_out[bline(l)->line.nomgO] = l;
            }
        }
        p++;
    }
}



CLINE2* CGraph2::find_line_nom_out(int id)
{
    if (id == -1) return nullptr;

    map<int, CLINE2*>::const_iterator it = m_map_line_out.find(id);
    if (it != m_map_line_out.end()) {
        return it->second;
    }

    return nullptr;
}




CNode2* CGraph2::findPO(int id, char po)
{
    //  if ( n.node_name.p == ' ') {
    return find(id);
    //  }
    //  CNode n2 = n;
    //  n2.node_name.p = po;
    //  return find(n2);
}


CLINE2* CGraph2::find_line(const CNode2* c1, const CNode2* c2, char p, int typ)
{
    CLINE2* l;

    if (!c1 || !c2) return nullptr;
    if (!c1->lines || !c2->lines) return nullptr;

    for (l = c1->lines; l; l = next(l)) {
        if (other(l) == c2 && (
            p == CPOD && bline(l)->line.nomP >= 0 || p == COBR && bline(l)->line.nomO >= 0)
            && (typ == -1 || bline(l)->line.typ == typ)
            && !bline(l)->line.isHide

            ) return l;
    }
    return nullptr;
}


CNode2* CGraph2::findPO(int id)
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

void CGraph2::set_pjezo(bool yes)
{
    CMapGraph::iterator p;
    p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* n = p->second;
        n->node.isPjezo = yes;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            bline(l)->line.isPjezo = yes;
        }
        p++;
    }
}



void CGraph2::save_pjezo()
{
    CMapGraph::iterator p;
    p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* n = p->second;
        n->node.isPjezoSave = n->node.isPjezo;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            bline(l)->line.isPjezoSave = bline(l)->line.isPjezo;
        }
        p++;
    }
}

void CGraph2::restore_pjezo()
{
    CMapGraph::iterator p;
    p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* n = p->second;
        n->node.isPjezo = n->node.isPjezoSave;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            bline(l)->line.isPjezo = bline(l)->line.isPjezoSave;
        }
        p++;
    }
}

void CGraph2::save_otkl()
{
/*
    CMapGraph::iterator p;
    p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            bline(l)->line.pod.isOtklSave = bline(l)->line.pod.isOtkl;
            bline(l)->line.obr.isOtklSave = bline(l)->line.obr.isOtkl;
        }
        p++;
    }
*/
}

void CGraph2::restore_otkl()
{
/*
    CMapGraph::iterator p;
    p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* n = p->second;
        n->node.isPjezo = n->node.isPjezoSave;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            bline(l)->line.pod.isOtkl = bline(l)->line.pod.isOtklSave;
            bline(l)->line.obr.isOtkl = bline(l)->line.obr.isOtklSave;
        }
        p++;
    }
*/
}




void CGraph2::clearVnutr(int id)
{
    CLINE2* l, * ll;
    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* n = p->second;

        if (n->node.internalNodeID == id) {
            for (l = n->lines; l; l = ll) {
                ll = next(l);
                remove_line(l);
            }
            p = map_node.erase(p);
        }
        else
            p++;
    }
}



void CGraph2::clearFile(set<int>& v)
{
    CLINE2* l, * ll;
    CMapGraph::iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* n = p->second;

        set<int>::const_iterator it = v.find(n->node.fileID);

        if (it != v.end()) {
            for (l = n->lines; l; l = ll) {
                ll = next(l);
                remove_line(l);
            }
            p = map_node.erase(p);
        }
        else
            p++;
    }
}

int CGraph2::getKodId(const char* kod)
{
    int id = -1;
    map<long, CString>::const_iterator it = map_kod.begin();

    for (; it != map_kod.end(); it++) {
        if (it->second == kod) {
            id = it->first;
            break;
        }
    }
    return id;
}

void CGraph2::getKodIds(const char* kod, set<int>& set_id)
{
    int id = -1;

    set_id.clear();

    map<long, CString>::const_iterator it = map_kod.begin();

    for (; it != map_kod.end(); it++) {
        if (it->second == kod) {
            id = it->first;
            set_id.insert(id);
        }
    }
    //  return id;
}




CNode2* CGraph2::findNodeName(const char* kod, const char* name, const char p1)
{
    set<int> set_id;
    getKodIds(kod, set_id);

    //  CString sss;
    //  sss.Format("id=%d", id);
    //  AfxMessageBox(sss, MB_OK|MB_ICONINFORMATION);


    if (set_id.size() >= 0) {
        CMapGraph::iterator p = map_node.begin();

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


CLINE2* CGraph2::findLineName_vn(const char* kod_p, const char* name_p, const char p_p, const char* kod1, const char* name1, const char p1, const char* kod2, const char* name2, const char p2)
{
    CNode2* np = findNodeName(kod_p, name_p, p_p);

    set<int> set_id1;
    set<int> set_id2;

    getKodIds(kod1, set_id1);
    getKodIds(kod2, set_id2);

    if (set_id1.size() >= 0 && set_id2.size() >= 0) {
        CMapGraph::iterator p = map_node.begin();

        while (p != map_node.end()) {
            CNode2* n = p->second;

            if (set_id1.find(n->node.id_kod) != set_id1.end() && n->node.internalNodeID == np->id) {
                if (isSameName(n->node.name, name1)) {
                    for (CLINE2* l = n->lines; l; l = next(l)) {
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


CLINE2* CGraph2::findLineName(const char* kod1, const char* name1, const char p1, const char* kod2, const char* name2, const char p2)
{
    set<int> set_id1;
    set<int> set_id2;

    getKodIds(kod1, set_id1);
    getKodIds(kod2, set_id2);

    if (set_id1.size() >= 0 && set_id2.size() >= 0) {
        //        if (id1 >= 0 && id2 >= 0) {
        CMapGraph::iterator p = map_node.begin();

        while (p != map_node.end()) {
            CNode2* n = p->second;

            if (set_id1.find(n->node.id_kod) != set_id1.end() && n->node.internalNodeID == 0) {
                if (isSameName(n->node.name, name1)) {
                    for (CLINE2* l = n->lines; l; l = next(l)) {
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


CNode2* CGraph2::findNodeName_vn(const char* kod_p, const char* name_p, const char p_p, const char* kod1, const char* name1, const char p1)
{
    CNode2* np = findNodeName(kod_p, name_p, p_p);

    set<int> set_id1;

    getKodIds(kod1, set_id1);

    if (set_id1.size() >= 0) {
        CMapGraph::iterator p = map_node.begin();

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
    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        CLINE2* l;
        for (l = pp->lines; l; l = next(l)) {
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
    CCoordList::const_iterator it = bline(l)->line.m_NP.begin();
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


    CCoordList::const_iterator it = bline(l)->line.m_NP.begin();
    for (; it != bline(l)->line.m_NP.end(); ++it) {
        //    cl.push_back(*it);

        rect.left = min(rect.left, it->x);
        rect.right = max(rect.right, it->x);

        rect.top = min(rect.top, it->y);
        rect.bottom = max(rect.bottom, it->y);


    }

    return rect;
}


int getLineTyp(const char* tn);
int getNodeTyp(const char* tn);

CLINE2* CGraph2::findLineOut(const char* tn, int id)
{
    int typ = getLineTyp(tn);
    return findLineOut(typ, id);
}

CNode2* CGraph2::findNodeOut(const char* tn, int id)
{
    int typ = getNodeTyp(tn);
    return findNodeOut(typ, id);
}

CLINE2* CGraph2::findLineOut(int typ, int id)
{
    if (typ < 0) return nullptr;
    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == 0) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                for (CLINE2* l = pp->lines; l; l = next(l)) {
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

    CMapGraph::iterator p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* pp = p->second;
        m_map_pt_out[pp->node.nomPT] = pp;
        p++;
    }
}


CNode2* CGraph2::find_pt_out(int id)
{
    if (id == -1) return nullptr;

    map<int, CNode2*>::const_iterator it = m_map_pt_out.find(id);
    if (it != m_map_pt_out.end()) {
        return it->second;
    }

    return nullptr;
}



CNode2* CGraph2::findPtOut(int id)
{
    map<long, CNode2*>::const_iterator p = map_node.begin();

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
    map<long, CNode2*>::const_iterator p = map_node.begin();

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
    map<long, CNode2*>::const_iterator p = map_node.begin();

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
    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        CLINE2* l;
        for (l = pp->lines; l; l = next(l)) {
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

void CGraph2::vyd_lines(set<int>& v, bool nodes)
{
    reset();
    map<long, CNode2*>::const_iterator p = map_node.begin();
    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
            for (CLINE2* line = pp->lines; line; line = next(line)) {
                if (IsBegin(line)) {
                    CLine2* l = bline(line);
                    if (v.find(l->line.nomP) != v.end()) {
                        l->line.isPjezo = true; 
                        if (nodes) {
                            where(l)->node.isPjezo = true;
                            other(l)->node.isPjezo = true;
                        }
                    }

                    if (l->line.nomP != l->line.nomO) if (v.find(l->line.nomO) != v.end()) {
                        l->line.isPjezo = true;
                        if (nodes) {
                            where(l)->node.isPjezo = true;
                            other(l)->node.isPjezo = true;
                        }
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
    map<long, CNode2*>::const_iterator p = map_node.begin();
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

    map<long, CNode2*>::const_iterator p = map_node.begin();

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
    if (typ < 0) return nullptr;
    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == parent_id && pp->node.fileID == fileID) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                for (CLINE2* l = pp->lines; l; l = next(l)) {
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

    map<long, CNode2*>::const_iterator p = map_node.begin();

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
    if (typ < 0) return nullptr;
    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == parent_id) {
            if (pp->node.coord.x != 0 || pp->node.coord.y != 0) {
                for (CLINE2* l = pp->lines; l; l = next(l)) {
                    if (bline(l)->line.typ == typ && bline(l)->line.isPjezo) {
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
        return node.m_graph->m_cxema->getGidFile(node.fileID);
    }

    return 0;
}


CString CNode2::getFileName()
{
    CGidFile* file = getFile();

    if (file) return file->name;

    return "";
}


CCoordList get_NP(const CLINE2* l)
{
    if (IsBegin(l)) return bline(l)->line.m_NP;

    CCoordList cl = bline(l)->line.m_NP;
    cl.Swap();
    return cl;
}


CString AnyName(const CNode2* n)
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

    map<long, CNode2*>::const_iterator p = map_node.begin();

    bool ret = false;

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == internalNodeID) {
            if (pp->node.isPjezo && (pp->node.coord.x != 0 || pp->node.coord.y != 0)) {
                rect.AddPoint(pp->node.coord);
            }

            CLINE2* l;
            for (l = pp->lines; l; l = next(l)) {
                if (IsBegin(l) && bline(l)->line.isPjezo) {
                    int qq = bline(l)->line.m_NP.size();
                    if (bline(l)->line.m_NP.size() > 0) {
                        CFRect r = bline(l)->line.m_NP.GetFRect();
                        rect.AddPoint(r.BottomRight());
                        rect.AddPoint(r.TopLeft());
                        ret = true;
                    }
                }
            }
        }
        p++;
    }

    return ret;
}



void CGraph2::setOnOff(bool on_off)
{
    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.isPjezo) {
            pp->node.isPodp = on_off;
        }


        if (1) {

            CLINE2* l;
            for (l = pp->lines; l; l = next(l)) {
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

    if (fabs(l1 + l2 - l)) {
    }
    return 0;
}


double rasst(CLINE2* l, CFPoint p)
{
    if (bline(l)->line.m_NP.size() == 0) {
        return Length(p, where(l)->node.coord);
    }

    CFPoint p1 = where(l)->node.coord, p2;

    CCoordList::const_iterator it = bline(l)->line.m_NP.begin();

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



void CGraph2::clean_out(int fileID)
{
    map<long, CNode2*>::const_iterator p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.fileID == fileID) {
            pp->node.calculationID = -1;
            pp->node.nomgP = -1;
            pp->node.nomgO = -1;

            pp->node.qz = 0;
            pp->node.qP = 0;
            pp->node.qO = 0;

            CLINE2* l;
            for (l = pp->lines; l; l = next(l)) {
                bline(l)->line.nomgP = -1;
                bline(l)->line.nomgO = -1;

                bline(l)->line.pod.set_zero();
                bline(l)->line.obr.set_zero();
            }
        }
        p++;
    }

}


void CGraph2::allVisible()
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;
        pp->node.isHide = false;
        for (CLINE2* l = pp->lines; l; l = next(l)) {
            bline(l)->line.isHide = false;
        }
        p++;
    }
}

void CGraph2::visibleMSRS(set<int> & set_ms, set<int> & set_rs)
{
    auto p = map_node.begin();

    while (p != map_node.end()) {
        CNode2* pp = p->second;
        pp->node.isHide = true;
        for (CLINE2* l = pp->lines; l; l = next(l)) {
            bline(l)->line.isHide = true;
            if (set_ms.find(bline(l)->line.ms) != set_ms.end()) {
                bline(l)->line.isHide = false;
                where(l)->node.isHide = false;
                other(l)->node.isHide = false;
            }
            else if (set_rs.find(bline(l)->line.rs) != set_rs.end()) {
                bline(l)->line.isHide = false;
                where(l)->node.isHide = false;
                other(l)->node.isHide = false;
            }
        }
        p++;
    }
}



bool CGraph2::setMark(const CCoordList &NP, bool val)
{
  CLINE2 *l;

  CMapGraph::iterator p = map_node.begin();

  while (p != map_node.end()) {
    CNode2 *n = p->second;

    if (!n->node.isHide && NP.find3(n->node.coord)) {
      n->node.isPjezo = val;

      for (l = n->lines; l; l = next(l)) {
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


bool CGraph2::isMark()
{
  CMapGraph::iterator pp = map_node.begin();

  while (pp != map_node.end()) {
    if (pp->second->node.isPjezo) return true;
    pp++;
  }
  return false;
}


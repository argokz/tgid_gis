// ProtEdit.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "ProtEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "gid6Doc.h"
#include "gidrView.h"


/////////////////////////////////////////////////////////////////////////////
// CProtEdit

CProtEdit::CProtEdit()
{
}

CProtEdit::~CProtEdit()
{
}


BEGIN_MESSAGE_MAP(CProtEdit, CEdit)
  //{{AFX_MSG_MAP(CProtEdit)
  ON_WM_LBUTTONDBLCLK()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProtEdit message handlers

int regex_match(const char *p, const char *buf, vector<CString> & match);

#include "graph.h"

bool parse_node(CGraph2* graph, CString s, CString &kod, CString &name, CString& pp)
{
    vector<CString> match;

    for (auto it : graph->map_kod) {

        CString p;
        p.Format("^(%s)\\s+(.+?)\\s+([ПО ])\\s*$", it.second);

        int l = regex_match(p, s, match);
        if (l > 0) {
            kod = match[1];
            name = match[2];
            pp = match[3];
            return true;
        }
    }
    return false;
}

int parse_node_or_line(CGraph2* graph, CString s, CString& kod1, CString& name1, CString& pp1, CString& kod2, CString& name2, CString& pp2)
{
    CString p = "(.+) - (.+)";
    vector<CString> match;
    
    int l = regex_match(p, s, match);
    if (l > 0) {
        parse_node(graph, match[1], kod1, name1, pp1);
        parse_node(graph, match[2], kod2, name2, pp2);
        return 2;
    }
    else {
        parse_node(graph, s, kod1, name1, pp1);
        return 1;
    }
    return 0;
}



void parse_str2(CGidrView* pView, CGraph2* graph, CString s)
{
    if (s == "") return;

    CString kod0, name0, pp0;
    CString kod1, name1, pp1;
    CString kod2, name2, pp2;

    if (s[0] == '(') {
        CString p = "\\((.+?)\\) (.+)\\s*";
        vector<CString> match;

        int l = regex_match(p, s, match);

        if (l > 0) {
            bool y1 = parse_node(graph, match[1], kod0, name0, pp0);
            if (y1) {
                CNode2 *n0 = graph->findNodeName(kod0, name0, ' ');

                int nl = parse_node_or_line(graph, match[2], kod1, name1, pp1, kod2, name2, pp2);
                if (nl == 1) {
                    CNode2* n = graph->findNodeName_vn(kod0, name0, ' ', kod1, name1, ' ');
                    if (n) {
                        pView->moveNode(n);
                        pView->BeginDrawNode(n);
                    }
                }
                else if (nl == 2) {
                    CLINE2* line = graph->findLineName_vn(kod0, name0, ' ', kod1, name1, ' ', kod2, name2, ' ');
                    if (line) {
                        pView->moveNode(where(line));
                        pView->BeginDrawLine(line);
                        return;
                    }
                }
            }
        }
    }
    else {
        int nl = parse_node_or_line(graph, s, kod1, name1, pp1, kod2, name2, pp2);
        if (nl == 1) {
            CNode2* n = graph->findNodeName(kod1, name1, pp1[0]);
            if (n) {
                pView->moveNode(n);
                pView->BeginDrawNode(n);
            }
        }
        else if (nl == 2) {
            CLINE2* line = graph->findLineName(kod1, name1, ' ', kod2, name2, ' ');
            if (line) {
                pView->moveNode(where(line));
                pView->BeginDrawLine(line);
                return;
            }
        }
    }
}


CString name(const CNode2 *n)
{
  CString s;
  s.Format("%-*s %-*s %c", N3, n->getKod(), N10, n->node.name, ' ');
  return s;
}

CString name2(const CNode2 *n)
{
  CString s;
  s.Format("%-*s %-*s", N3, n->getKod(), N10, n->node.name);
  return s;
}

CString name_line(const CLINE2 *l)
{
    CString s, s2;
    CNode2 *p1 = where(l);
    CNode2 *p2 = other(l);

    CNode2* n0 = nullptr;

    CGraph2 *graph = p1->node.m_graph;

    if (p1->node.internalNodeID) {
        n0 = graph->find(p1->node.internalNodeID);
    }

    s2.Format("%s - %s", name(where(l)), name(other(l)));

    s = s2;

    if (n0) {
        s.Format("(%s) %s", name2(n0), s2);
    }
    return s; 
}

CString name_node(const CNode2* n)
{
    CString s, s2;

    CNode2* n0 = nullptr;

    CGraph2* graph = n->node.m_graph;

    if (n->node.internalNodeID) {
        n0 = graph->find(n->node.internalNodeID);
    }

    s2 = name(n);

    s = s2;

    if (n0) {
        s.Format("(%s) %s", name2(n0), s2);
    }
    return s;
}



void parse_str(CGidrView* pView, CGraph2* graph, CString s)
{
//    s = "Регулятор давления [(0     НС-7                     ) 0     У321322227                  - 0     У654987811                 ] не может обеспечить давление в узле [0     У321322221                 ]";
    CString p = ".*?\\[(.+?)\\]";
    vector<CString> match;

    int l = regex_match(p, s, match);

    if (l > 0) {
//        parse_str2(pView, graph, match[1]);
        CString s = match[1];

    	CMapGraph::const_iterator p = graph->map_node.begin();

    	while (p != graph->map_node.end()) {
    		CNode2* n = p->second;
    		if (n->node.coord.x != 0 || n->node.coord.y != 0) {
                for (CLINE2* l = n->lines; l; l = next(l)) {
                    CString nm = name_line(l);
                    if (nm == s) {
                        pView->moveNode(where(l));
                        pView->BeginDrawLine(l);
                        return;
                    }
                }


                CString nm = name_node(n);

                CString s1 = nm;  s1.Trim();
                CString s2 = s;   s2.Trim();


//                if (nm == s) {
                if (s1 == s2) {
                    pView->moveNode(n);
                    pView->BeginDrawNode(n);
                    return;
                }
            }
    		p++;
    	}
    }
}


void CProtEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  
  int n = CharFromPos(point);
  int nLineIndex = HIWORD(n);
  int nCharIndex = LOWORD(n);

  CString str;
  char s[2560];
  char ss1[256];
  char ss2[256];

  CGidrView *pView = getView();
  
  if (!pView) return;


  ss1[0] = ss2[0] = 0;

  str.Format("nLineIndex = %d, nCharIndex = %d", nLineIndex, nCharIndex);

  if (nLineIndex >= 0) {
    int nl = GetLine( nLineIndex, s , 2500);
    s[nl] = 0;

    int l;

//    CString ss = _TR("Не заданы расчетные температуры для [Р24                              ]");
//    CString ss = _TR("В регуляторе [(0     НС-5                     ) М12   У321322119                  - М12   У321322111                 ] не задан узел с регулируемым напором");

    parse_str(pView, pView->m_cxema->m_graph, s);

    return;


    CString ss = s;


    vector<CString> match;

//    CString p = ".*\[\(([^\s]+)\s+([^\s]+)\s+(.)\) ([^\s]+)\s+([^\s]+)\s+(.) - ([^\s]+)\s+([^\s]+)\s+(.)\]";

//    CString p = ".*\\[\\(([^\\s]+)\\s+([^\\s]+)\\s+([ПО ])\\) ([^\\s]+)\\s+([^\\s]+)\\s+([ПО ])\\s+-\\s+([^\\s]+)\\s+([^\\s]+)\\s+([ПО ])\\]";
    CString p = ".*\\[\\((.{5}) (.+?) ([ПО ])\\) (.{5}) (.+?) ([ПО ]) - (.{5}) (.+?) ([ПО ])\\]";

    p.Replace(".{5}", "[^ ]*");


    l = regex_match(p, ss, match);


    if (l > 0) {
        CString kod = match[1].Trim();
        CString name = match[2].Trim();
        CString p = match[3];

        CString kod1 = match[4].Trim();
        CString name1 = match[5].Trim();
        CString p1 = match[6];

        CString kod2 = match[7].Trim();
        CString name2 = match[8].Trim();
        CString p2 = match[9];

        char pr = p[0];

/**
        CLINE2 *line = pView->m_cxema->m_graph->findLineName_vn(kod, name, ' ', kod1, name1, ' ', kod2, name2, ' ');
        if (line) {
            pView->moveNode(where(line));
            pView->BeginDrawLine(line);
        }
*/

        CLINE2* line = pView->m_cxema->m_graph->findLineName_vn(kod, name, ' ', kod1, name1, ' ', kod2, name2, ' ');
        if (line) {
            CNode2* node = pView->m_cxema->m_graph->findNodeName(kod, name, pr);
            if (!node) {
                node = pView->m_cxema->m_graph->findNodeName(kod, name, ' ');
            }
            if (node) {
                pView->moveNode(where(line));
                pView->BeginDrawLine(line);
                return;
            }
        }
    }


//    p = ".*\\[\\(([^\\s]+)\\s+([^\\s]+)\\s+([ПО ])\\) ([^\\s]+)\\s+([^\\s]+)\\s+([ПО ])\\]";

    p = ".*\\[\\((.{5}) (.+?) ([ПО ])\\) (.{5}) (.+?) ([ПО ])\\]";
    p.Replace(".{5}", "[^ ]*");

    l = regex_match(p, ss, match);


    if (l > 0) {
        CString kod = match[1].Trim();
        CString name = match[2].Trim();
        CString p = match[3];

        CString kod1 = match[4].Trim();
        CString name1 = match[5].Trim();
        CString p1 = match[6];


        char pr = p[0];

        CNode2 *n = pView->m_cxema->m_graph->findNodeName_vn(kod, name, ' ', kod1, name1, ' ');
        if (n) {
            pView->moveNode(n);
            pView->BeginDrawNode(n);
            return;
        }
    }


//    p = ".*\\[([^\\s]+)\\s+([^\\s]+)\\s+([ПО ])\\s+-\\s+([^\\s]+)\\s+([^\\s]+)\\s+([ПО ])\\]";
    p = ".*\\[(.{5}) (.+?) ([ПО ]) - (.{5}) (.+?) ([ПО ])\\]";
    p.Replace(".{5}", "[^ ]*");

    l = regex_match(p, ss, match);

    if (l > 0) {
        CString kod1 = match[1].Trim();
        CString name1 = match[2].Trim();
        CString p1 = match[3];

        CString kod2 = match[4].Trim();
        CString name2 = match[5].Trim();
        CString p2 = match[6];

        CLINE2 *line = pView->m_cxema->m_graph->findLineName(kod1, name1, ' ', kod2, name2, ' ');
        if (line) {
            pView->moveNode(where(line));
            pView->BeginDrawLine(line);
            return;
        }
    }



//    l = regex_match(".*\\[([^ ]+)\\s+(.+)\\s+([ПО ])\\]", ss, match);

    p = ".*\\[(.{5}) (.+?) ([ПО ])\\]";
    p.Replace(".{5}", "[^ ]*");

    l = regex_match(p, ss, match);

    if (l > 0) {
        CString kod =  match[1].Trim();
        CString name = match[2]; name.Trim();
        CString p =  match[3];
        char pr = p[0];

//        CString sss;
//        sss.Format("`%s`\n`%s`\n`%s`", kod, name, p);;

        CNode2 *node = pView->m_cxema->m_graph->findNodeName(kod, name, pr);
        if (!node) {
            node = pView->m_cxema->m_graph->findNodeName(kod, name, ' ');
        }
        if (node) {
            pView->moveNode(node);
            pView->BeginDrawNode(node);
            return;
        }
    }
  }

//  CEdit::OnLButtonDblClk(nFlags, point);
}

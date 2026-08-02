// gidrView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"

#include "gid6Doc.h"
#include "gidrView.h"
#include "win.h"
#include "Podl.h"
#include "Bmp.h"
#include "ps_alma.h"


#include "colorgtd.h"

#include "OPCThread2.h"
extern COPCThread2* pOPCThread;


//const unsigned long PODOTKL = 0xcfcfFF;
//const unsigned long OBROTKL = 0xFFcfcf;


bool log_draw()
{
    return true;
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CGeoFile* m_geofile;
extern double m_MasPic;


static int m_isDop = 0;

int isDop()
{
    return m_isDop;
}

bool IsRezhim(const CNode2* n)
{
    //  return false;
    //  if (n && !n->m_graph->isRS() && m_bIsRezhim) return (!strcmp(n->node_name.kod, _TR("П1")) || !strcmp(n->node_name.kod, _TR("П2")));

    //  if (n && m_bIsRezhim) return (!strcmp(n->node.node_name.kod, _TR("П1")) || !strcmp(n->node.node_name.kod, _TR("П2")));

    if (n && m_bIsRezhim) {
        CCxema* cxema = n->node.m_graph->m_cxema;

        if (cxema) {
            auto it = cxema->m_AstanaMagSet.find(n->node.fileID);

            if (n->node.fileID == cxema->m_AstanaMag || it != cxema->m_AstanaMagSet.end()) {
                CString kod = n->getKod();
                return kod == _TR("П1") || kod == _TR("П2");
            }
        }
    }

    return false;
}

bool IsRezhim(const CLINE2* l)
{
    //  return false;

    if (!l) return false;
    return IsRezhim(where(l)) || IsRezhim(other(l));
}

bool LiangBar(int XS1, int YS1, int XS2, int YS2, double& x0, double& y0, double& x1, double& y1);
bool LiangBar2(int XS1, int YS1, int XS2, int YS2, double x0, double y0, double x1, double y1);
void drawDxf(cdxf* dxf, CDC* pDC, CRect m_rect, double m_bx, double m_by, double masx, double masy);

void CGidrView::BeginDrawNode(const CNode2* node)
{
    drawMark(false);
    //-----------------------------------------------
    EndDrawNode();
    EndDrawLine();
    //-----------------------------------------------
    m_CurNode = node->node.m_graph->find(node->id);
    //#ifndef NO_ALL_FI1ND
    //  m_CurNode1 = *(CNode*) node;
    //#endif
}

void CGidrView::EndDrawNode()
{
    drawMark(false);
    m_CurLine = NULL;
    m_CurNode = NULL;
}

void CGidrView::BeginDrawLine(const CLINE2* Line)
{
    drawMark(false);
    //-----------------------------------------------
    EndDrawLine();
    EndDrawNode();
    //-----------------------------------------------

    m_CurLine = bline(Line);
}

void CGidrView::EndDrawLine()
{
    drawMark(false);
    m_CurLine = NULL;
    m_CurNode = NULL;
}

bool isNeotr(const CNode2* node)
{
    CLINE2* l;
    CNode2* n2;

    for (l = node->lines; l; l = next(l)) {
        n2 = other(l);
        if (n2->node.coord.x == 0 && n2->node.coord.y == 0) {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------
// Нарисовать узел без текста
//-----------------------------------------------

int getPSTyp(int n);


void CGidrView::drawNodeTyp4(CDC* m_dc, double x, double y, int typ, int color)
{
    //  drawNodeTyp(m_dc, x, y, typ, color);

    color = LIGHTGREEN;

    int m_bk_color_old = m_bk_color;

    switch (typ)
    {
    case 1:
        m_bk_color = LIGHTRED;
        drawNodeTyp(m_dc, x, y, 35, color);
        break;
    case 2:
        m_bk_color = LIGHTRED;
        drawNodeTyp(m_dc, x, y, 35, color);
        break;
    case 3:
        m_bk_color = WHITE;
        drawNodeTyp(m_dc, x, y, 35, color);
        break;
    case 4:
        m_bk_color = WHITE;
        drawNodeTyp(m_dc, x, y, 35, color);
        break;
    case 5:
        m_bk_color = LIGHTRED;
        drawNodeTyp(m_dc, x, y, 35, color);
        break;
    case 6:
        m_bk_color = LIGHTRED;
        drawNodeTyp(m_dc, x, y, 36, color);
        break;
    case 7:
        m_bk_color = LIGHTBLUE;
        drawNodeTyp(m_dc, x, y, 36, color);
        break;
    case 8:
        m_bk_color = LIGHTRED;
        //    drawNodeTyp(m_dc, x, y, 36, color);
        drawNodeTyp(m_dc, x, y, 37, color);
        break;
    case 9:
        m_bk_color = WHITE;
        drawNodeTyp(m_dc, x, y, TIP_US, color);
        break;
    }

    m_bk_color = m_bk_color_old;
}



void CGidrView::drawNode0(CDC* m_dc, const CNode2* node, bool redr)
{
    int color;
    double x, y;
    
    if (!isFull() && !node->node.isMarkAnalyze) return;

    color = m_colors.color(&node->node, m_cxema->m_bIsGidrInf);

    if (isNeotr(node)) color = COLOR_NEOTR;
    if (redr) color = CUR_COLOR;
    if (node->node.isOtkl) color = CUR_OTKL;

    if (node->node.pr_avtomat == 'А') {
        color = 0x0000FF;
    }

    int typ = node->node.typ;

    x = node->node.coord.x / geom.masx - geom.m_bx, y = node->node.coord.y / geom.masy - geom.m_by;

    if (isPassport()) {
        if (m_bIsFlag && node->node.nZN != -1) {
            drawNodeTyp(m_dc, x, y, TIP3_ZN, color);
        }
        //    drawNodeTyp4(m_dc, x, y, node->node.typPS, color);

        int m_bk_color_old = m_bk_color;

        m_bk_color = WHITE;

        if (node->typPS() <= 0 || node->node.typ == TIP_PR) {
            typ = node->node.typ;

            if (node->node.typ == TIP_PR) {
                if (node->node.isEl) {
                    typ = 27;
                }
                else if (node->node.isNez) {
                    typ = 28;
                }
                if (node->node.pr_avtomat == '3') {
                    typ = 32;
                }
            }
        }
//        else if (node->typPS() == PS_NEUST) {  // ???
//            typ = TIP_US;
//        }
        else if (node->typPS() == PS_IS || node->node.typ == TIP_IS) {  // Источник
            typ = TIP_IS;
        }
        else if (node->typPS() == PS_NS || node->node.typ == TIP_HS) {  // Насос
            typ = TIP_HS;
        }
        else if (node->typPS() == PS_VV) {  // Ввод в здание
            typ = 37;
            m_bk_color = LIGHTRED;
        }
        else {
            //      typ = getPSTyp(node->node.typPS);
            typ = 36;
            m_bk_color = CYAN;
        }

        if (node->typPS() == PS_NEUST) {
            m_bk_color = WHITE;
        }

        drawNodeTyp(m_dc, x, y, typ, color);

        if (node == m_node_first) {
            m_bk_color = RED;
            drawNodeTyp(m_dc, x, y, TIP3_ZN, RED);
        }
        if (node == m_node_last) {
            m_bk_color = BLUE;
            drawNodeTyp(m_dc, x, y, TIP3_ZN, BLUE);
        }

        m_bk_color = m_bk_color_old;

        //    drawNodeTyp(m_dc, x, y, typ, color);

        m_dc->MoveTo((int)x, (int)y);
        return;
    }

    if (!m_bIsPTS) {
        if (node->node.nZN != -1) {
            drawNodeTyp(m_dc, x, y, TIP3_ZN, color);
        }
        if (node->node.nORP != -1) {
            drawNodeTyp(m_dc, x, y, TIP3_ORP, color);
        }
    }

    //  if ( node->node.typPS >= 0) {
    //    drawNodeTyp(m_dc, x, y, TIP3_ZN, color);
    //  }

    if (m_bIsPribor) {
        if (node->node.nPRIBOR != -1) {
            drawNodeTyp(m_dc, x, y, 31, color);
        }
    }

    if (node->node.typ == TIP_PR) {

        if (m_bIsPribor) {
            if (node->node.pribor_ucheta) {
                drawNodeTyp(m_dc, x, y, 31, color);
            }
        }


        if (node->node.isEl) {
            typ = 27;
        }
        else if (node->node.isNez) {
            typ = 28;
        }
        if (node->node.pr_avtomat == '3') {
            typ = 32;
        }
    }

    if (node->node.colorP != 0xFFFFFFFF) {

        if (node->node.isMarkAnalyze)
            drawNodeTyp(m_dc, x, y, 38, node->node.colorP, 5.0);
        else
            drawNodeTyp(m_dc, x, y, 30, node->node.colorP);
    }

    drawNodeTyp(m_dc, x, y, typ, color);

    if (node->node.isP && node->node.typ == TIP_US) {
        drawNodeTyp(m_dc, x, y, 25, color);
    }
    else if (node->node.isP && node->node.typ != TIP_PR) {
        //    drawNodeTyp(m_dc, x, y, 22, color);
    }
    if (node->node.nUP != -1) {
        drawNodeTyp(m_dc, x, y, TIP_PODP, color);
    }
    if (node->node.nVP != -1) {
        drawNodeTyp(m_dc, x, y, TIP_VODOR, color);
    }

    if (node->node.typ == TIP_PR || node->node.typ == TIP_PO) {
        if (!node->node.isValid || (!node->node.isValid_f && m_isDop)) {
            drawNodeTyp(m_dc, x, y, 26, BLACK);
        }
    }

    if (m_isPasport && node->typPS() > 0) {  // 
/*
        if (node->node.typ == TIP_PR || node->node.typ == TIP_IS || node->node.typ == TIP_HS) {
            drawNodeTyp(m_dc, x, y, node->node.typ, color);
        }
        else {
*/
            drawNodeTyp4(m_dc, x, y, node->typPS(), color);
//        }
    }

    if (node == m_node_first) {
        drawNodeTyp(m_dc, x, y, TIP3_ZN, RED);
    }
    if (node == m_node_last) {
        drawNodeTyp(m_dc, x, y, TIP3_ZN, BLUE);
    }

    m_dc->MoveTo(x, y);
}

CString strnum(double o);


CString formatOutNode(const char *c, int np, int no, double p, double o)
{
    CString str;

    if (fabs(p) > 1e9) p = 0.;
    if (fabs(o) > 1e9) o = 0.;

    if (np != -1 && no != -1)
        str.Format("%.0f,%.0f", p, o);
    else if (np != -1)
        str.Format("%.0f", p);
    else if (no != -1)
        str.Format("%.0f", o);

    CString oo, pp;

    oo = strnum(o);
    pp = strnum(p);

    if (np != -1 && no != -1)
        str.Format("%s, %s", pp, oo);
    else if (np != -1)
        str = pp;
    else if (no != -1)
        str = oo;

    if (c[0]) {
        CString s;
        s.Format("%s=%s", c, str);
        return s;
    }

    return str;

}


void drawPodp(CDC* dc, const char* c, int cx, int cy, CString& str, int np, int no, double p, double o, GID_COLORS colors)
{
    CSize sz;

    str = formatOutNode(c, np, no, p, o);
    sz = dc->GetTextExtent(str);
    cx -= sz.cx / 2;
    //  cy += sz.cy*3/2;

    if (np != -1 && no != -1) {
        str = formatOutNode(c, np, -1, p, o);
        str += ",";
        sz = dc->GetTextExtent(str);
        dc->SetTextColor(colors.pod(0, 0, 0));
        dc->TextOut(cx, cy, str);

        str = formatOutNode("",  - 1, no, p, o);
        dc->SetTextColor(colors.obr(0, 0, 0));
        dc->TextOut(cx + sz.cx, cy, str);
    }
    else {
        dc->SetTextColor(np > 0 ? colors.pod(0, 0, 0) : colors.obr(0, 0, 0));
        dc->TextOut(cx, cy, str);
    }
}

void drawText(CDC* dc, int cx, int cy, const char* str)
{
    CSize sz;

    sz = dc->GetTextExtent(str);
    cx -= sz.cx / 2;
    //  cy += sz.cy*3/2;

    dc->TextOut(cx, cy, str);
}

CString CGidrView::getOPC(const char* str)
{
    if (str[0] == 0) return CString(str);

    map<CString, CString>::const_iterator it;
    it = mapOPC.find(CString(str));
    if (it != mapOPC.end()) {
        return it->second;
    }

    return CString(str);
}

bool CGidrView::isPodp(CNode2* node)
{
    CString str = node->node.name;

    if (isPassport()) {
        str = PSName(node);
        str = node->node.namePS;
    }

    if (str.IsEmpty()) return false;

    if (m_parent_id != 0 && str[0] == 'У' && '0' <= str[1] && str[1] <= '9') {
        return false;
    }
    //  if (m_parent_id != 0 && (m_parent_kam->typ == TIP_PR || m_parent_kam->typ == TIP_PO)) {
    //  if (m_parent_id != 0 && (m_parent_kam->typ == TIP_PR || m_parent_kam->typ == TIP_PO)) {
    //    return false;
    //  }
//    if (str[0] == '#' && !m_bIsPodpRes && m_parent_id == 0 && ((!isPassport() && !m_isPasport) || m_bIsPodpNiz)) {
    if (str[0] == '#' && !m_bIsPodpRes && m_parent_id == 0 && ((!isPassport() && !m_isPasport) || m_bIsPodpNiz)) {
        return false;
    }


    /*
      if (m_bIsPodp || isPassport()) {
        if (!m_bIsPTS || node->node.typ == TIP_PO) {
          return true;
        }
      }
    */
    if (m_bIsPodp) return true;

    return false;
}

CString getUzRas(const CNode2* node);



void CGidrView::drawPodpNode(CDC* m_dc, CNode2* node)
{
    CPoint pt = m_dc->GetCurrentPosition();
    CString str = node->node.name;

    if (isPassport()) {
        str = PSName(node);
    }

    CSize sz = m_dc->GetTextExtent("0");
    CLINE2* l;

    if (sz.cy <= 4.) return;

    if (str != "") {
        sz = m_dc->GetTextExtent(str);
    }

    double max_len = 0.;

    if (node->node.typ != TIP_PO && !isPassport()) {
        for (l = node->lines; l; l = next(l)) {
            CNode2* n2 = other(l);
            if (n2->node.coord.x != 0) {
                max_len = max(max_len, Length(node->node.coord, n2->node.coord));
            }
        }
        if (max_len != 0 && max_len < 20 * geom.masx / 100) return;
    }

    if (isPassport()) {
        for (l = node->lines; l; l = next(l)) {
            CNode2* n2 = other(l);
            if (n2->node.coord.x != 0 || n2->node.coord.y != 0) {
                if (isPodp(n2)) {
                    max_len = max(max_len, Length(node->node.coord, n2->node.coord));
                }
            }
        }
        if (max_len != 0 && max_len < 20 * geom.masx / 100) return;
    }

    m_dc->SetTextColor(0);

    double dy = node->node.typ == TIP_US && !node->node.isP ? 3 : node->node.typ == TIP_PR || node->node.typ == TIP_PO ? 5 : 7;
    if (node->node.typ == TIP_US && node->node.isP) dy = 9;
    if (node->node.typ == TIP_C3) dy = 9;
    if (node->node.typ == TIP_PR && node->node.pr_avtomat == '3') dy = 15;
    if (isPassport()) dy = 10;

    dy *= m_ring;


    dy /= mas_otn;
    //  pt.y += dy/mas_1*m_h;
    pt.y += dy * m_h;

    //  if (node->node.n_sort > 0) str.Format("%d", node->node.n_sort);

    if (!str.IsEmpty()) {
        if (isPodp(node)) {
            m_dc->TextOut(pt.x - sz.cx / 2, pt.y, str);
            pt.y += sz.cy - 1;
        }
    }

    if (isPassport()) return;
    if (!m_cxema->m_bIsGidrInf) return;

    bool is_out = node->node.data.pihP != 0 || node->node.data.pihO != 0;

    if (is_out) {
        if (m_cxema->m_bIsInfo[0] || m_cxema->m_bIsInfo[6] || m_cxema->m_bIsInfo[10]) {
            if (node->node.nomgP > 0 || node->node.nomgO > 0) {
                if (m_cxema->m_bIsInfo[0]) {   // Напоры
                    drawPodp(m_dc, "P", pt.x, pt.y, str, node->node.nomP, node->node.nomO, node->node.data.pihP, node->node.data.pihO, m_colors);
                    pt.y += sz.cy - 1;
                }
                if (m_cxema->m_bIsInfo[6]) {  // Температуры
                    drawPodp(m_dc, "t", pt.x, pt.y, str, node->node.nomP, node->node.nomO, node->node.data.tempP, node->node.data.tempO, m_colors);
                    pt.y += sz.cy - 1;
                }
            }
        }

        if (m_cxema->m_bIsInfo[11]) {  // Располагаемый напор, dH
            if (node->node.nomgP > 0 && node->node.nomgO > 0) {
                CString str;
                str.Format("dH=%.1f", node->node.data.pihP - node->node.data.pihO);
                drawText(m_dc, pt.x, pt.y, str);
                pt.y += sz.cy - 1;
            }
        }
    }
        if (m_cxema->m_bIsInfo[12]) {  // Геодезическая отметка, h
            //    if (node->node.nomgP > 0 || node->node.nomgO > 0) {
            CString str;
            str.Format("h=%.1f", node->node.geoMarkTopTube);
            drawText(m_dc, pt.x, pt.y, str);
            pt.y += sz.cy - 1;
            //    }
        }


    if (is_out) {
        if (m_cxema->m_bIsInfo[7] && (node->node.typ == TIP_PR || node->node.typ == TIP_PO)) {  // Узловой расход
            CString str = getUzRas(node);
//            str.Format("Gз=%.1f; Gп=%.1f; Gо=%.1f", node->node.qz, node->node.qP, node->node.qO);

            if (str != "") {
                drawText(m_dc, pt.x, pt.y, str);
                pt.y += sz.cy - 1;
            }
        }
    }

        if (m_cxema->m_bIsInfo[10]) { // Нагрузки
            if (node->node.Qot != 0 || node->node.Qgvs != 0 || node->node.Qvent != 0) {


                CString str;
                //    str.Format("%.1f; %.1f; %.1f", node->node.Qot, node->node.Qgvs, node->node.Qvent);
                //    drawText(m_dc, pt.x, pt.y, str);
                //    pt.y += sz.cy-1;

                str.Format("Qот=%.3f", node->node.Qot);
                drawText(m_dc, pt.x, pt.y, str);

                pt.y += sz.cy - 1;
                str.Format("Qв=%.3f", node->node.Qvent);

                drawText(m_dc, pt.x, pt.y, str);

                pt.y += sz.cy - 1;
                str.Format("Qгвс=%.3f", node->node.Qgvs);
                drawText(m_dc, pt.x, pt.y, str);
                pt.y += sz.cy - 1;
                str.Format("Qсум=%.3f", node->node.Qot + node->node.Qgvs + node->node.Qvent);

                drawText(m_dc, pt.x, pt.y, str);

                pt.y += sz.cy - 1;
            }
        }


    if (is_out) {
        if (m_cxema->m_bIsInfo[14]) {
            if (node->node.nomgP > 0 || node->node.nomgO > 0) {
                if (m_cxema->m_bIsInfo[14]) {   // Полные напоры
                    drawPodp(m_dc, "H", pt.x, pt.y, str, node->node.nomP, node->node.nomO, node->node.data.pihP+node->node.geoMarkTopTube, node->node.data.pihO+node->node.geoMarkTopTube, m_colors);
                    pt.y += sz.cy - 1;
                }
            }
        }
    }

    if (!node->node.strOpc1.IsEmpty()) {
        opc_var.push_back(OPCvar(node->node.strOpc1, pt.x, pt.y));
        pt.y += sz.cy - 1;
    }

    if (!node->node.strOpc2.IsEmpty()) {
        opc_var.push_back(OPCvar(node->node.strOpc2, pt.x, pt.y));
        pt.y += sz.cy - 1;
    }



    /*
      if ( m_bIsInfo[10] ) { // OPC
        if ( !node->node.strOpc1.IsEmpty() ) {
          opc_var.push_back(OPCvar(node->node.strOpc1, pt.x, pt.y));
          str = getOPC(node->node.strOpc1);
          drawText(m_dc, pt.x, pt.y, str);
          pt.y += sz.cy-1;
        }

        if ( !node->node.strOpc2.IsEmpty() ) {
          opc_var.push_back(OPCvar(node->node.strOpc2, pt.x, pt.y));
          str = getOPC(node->node.strOpc2);
          drawText(m_dc, pt.x, pt.y, str);
          pt.y += sz.cy-1;
        }

        if ( !node->node.strOpc2.IsEmpty() ) {
          opc_var.push_back(OPCvar(node->node.strOpc2, pt.x, pt.y));
          str = getOPC(node->node.strOpc3);
          drawText(m_dc, pt.x, pt.y, str);
          pt.y += sz.cy-1;
        }
      }
    */
}

int inc1(const CNode2* n);

void CGidrView::drawNode(CDC* m_dc, const CNode2* node, bool redr)
{
    if (!isFull() && !node->node.isMarkAnalyze) return;

    if (IsRezhim(node) && !node->node.isMarkAnalyze) {
        return;
    }

    //  if ( m_parent_id && m_parent_kam->typ == TIP_PO ) {
    //    if ( inc1(node) == 0) {
    //      return;
    //    }
    //  }

    m_dc->SetBkColor(m_bk_color);
    drawNode0(m_dc, node, redr);

    if ((node->node.isPodp || m_isPodpAll) && !redr && m_szY > MIN_FONT) {
        drawPodpNode(m_dc, (CNode2*)node);
    }
}

bool CGidrView::DR_otkr(CLine2* l)
{
    if (!l || !l->getTableOut()) return false;

    double dru_home = 0;
    if (l->line.nomP > 0) {
        dru_home = l->line.pod.dru_home;
    }
    else if (l->line.nomO > 0) {
        dru_home = l->line.obr.dru_home;
    }

    return dru_home > 0;
}


bool CGidrView::ZD_otkr(CLine2* l)
{
    return true;
}


void CGidrView::drawLineTypL(CLine2* l, char po, BOOL redr, CDC* m_dc, double thickness, double x1, double y1, double x2, double y2, int typ, int color, int colorF, BOOL ris, BOOL napr, BOOL strelka, BOOL isQ)
{
    drawLineTyp(m_dc, thickness, x1, y1, x2, y2, typ, color, colorF, ris, napr, strelka, isQ, false);

    if (l) {
        if (m_szY > 4 && m_cxema->m_bIsGidrInf && !redr && ris == 1) {
            bool isOpcOnOff = !l->line.strOpc1.IsEmpty() && (l->line.typ == TIP_HC || l->line.typ == TIP_ZD || l->line.typ == TIP_ZD2);

            if (isOpcOnOff && !redr && ris == 1 && pOPCThread) {
                opc_var.push_back(OPCvar(l->line.strOpc1, l, po, x1, y1, x2, y2));
            }
        }
    }
}

void CGidrView::drawLine00(CDC* m_dc, const CLINE2* ll, BOOL ris, double x1, double y1, double x2, double y2, char podobr, bool redr, int t, int typ, int vyd, bool is_hide)
{
    CLine2* l = bline(ll);

    if (l->line.pod.isOtkl) {
    }

    double d = hypot(x2 - x1, y2 - y1);
    if (d < 1.) return;

    unsigned long colP, colO, colPF, colOF, col, colF;

    BOOL strelka = m_bIsNaprGid && !m_bIsPTS;

    if (d < 10) {
//        strelka = FALSE;
    }

    colP = m_colors.pod(&l->line, m_cxema->m_bIsGidrInf);
    colO = m_colors.obr(&l->line, m_cxema->m_bIsGidrInf);

    colPF = m_colors.podF(&l->line, m_cxema->m_bIsGidrInf);
    colOF = m_colors.obrF(&l->line, m_cxema->m_bIsGidrInf);

    int rm = -1;

    int cP = m_colors.pod(&l->line, 0, 1);
    int cO = m_colors.obr(&l->line, 0, 1);

    int alma = m_idMenu == 4;

    /////////////////
    //  
    int ms_rs = -1;

    map<int, URS>::const_iterator itR = ps_alma.map_URS.find(l->line.rs);
    if (itR != ps_alma.map_URS.end()) {
        ms_rs = 1;
        rm = ms_rs;
    }

    /*
      map<int, UMAG>::const_iterator itM = ps_alma.mapMAG.find(l->line.mag);
      if (itM != ps_alma.mapMAG.end()) {
        ms_rs = 0;
        rm = ms_rs;
      }
    */


    map<int, UMS>::const_iterator itM = ps_alma.map_UMS.find(l->line.ms);
    if (itM != ps_alma.map_UMS.end()) {
        ms_rs = 0;
        rm = ms_rs;
    }





    /*
      map<CString, UMSRS>::const_iterator it = ps_alma.mapMSRS2.find(l->line.kod0);
      if (it != ps_alma.mapMSRS2.end()) {
        ms_rs = it->second.mag;
        rm = ms_rs;
      }

      {
        map<int, UMAG>::const_iterator it = ps_alma.mapMAG.find(l->line.mag);
        if (it != ps_alma.mapMAG.end()) {
          ms_rs = 0;
          rm = ms_rs;
        }
      }
    */

    //////////////////


    /*

      if (alma) {
    //    colP = colO = BLACK;
        if (l->line.m_vid == 1) {  // Магистраль
          rm = 0;
        }
        else if (l->line.m_vid == 2) { // Распредсеть
          rm = 1;
        }
      }

      if (m_idMenu == 4) {
        colP = colO = 0;
        if (l->line.ms > 0) {
          rm = 0;
        }
        else if (l->line.rs > 0) {
          rm = 1;
        }
        else if (l->line.owner > 0) {
          colP = colO = BROWN;
        }
      }
    */
    if (alma) {
        colP = colO = BLACK;

        if (bline(l)->line.pipeSectionID <= 0) {
            colP = colO = DARKGRAY;
        }

        if (bline(ll)->line.pipeSectionID > 0 || !m_isOnlyPTSColor) {
            if (rm >= 0 && rm < 20) {

                int old_pod_otl = l->line.pod.isOtkl;
                int old_obr_otl = l->line.obr.isOtkl;
                l->line.pod.isOtkl = false; // Не показывать закрытые для ПТС
                l->line.obr.isOtkl = false;

                colP = m_colors.pod(&l->line, rm, m_cxema->m_bIsGidrInf);
                colO = m_colors.obr(&l->line, rm, m_cxema->m_bIsGidrInf);
                l->line.pod.isOtkl = old_pod_otl;
                l->line.obr.isOtkl = old_obr_otl;
            }

            if (bline(l)->line.pipeSectionID <= 0) {
                colP = colO = DARKGRAY;
            }
        }


        if (l->line.owner > 2) {
            colP = colO = BROWN;
            //      colP = colO = YELLOW;
        }
    }


#if 0

    if (m_isMS_RS_KOT_Yes) {
        CNode2* n = other(ll);

        map<long, RasCx>::const_iterator it = m_cxema->map_rs.find(n->node.id_kod);

        colP = colO = BLACK;

        if (it != m_cxema->map_rs.end()) {
            RasCx cx = it->second;


            if (bline(ll)->line.ms || bline(ll)->line.rs || !m_isOnlyPTSColor) {
                if (cx.objectID == 1) { // Магистраль
                    colP = colO = GREEN;
                }
                else if (cx.objectID == 2) {  // Распередсеть
                    colP = colO = RED;
                }
            }

            if (cx.objectID == 8) {  // Котельная
                colP = colO = BROWN;
            }
        }
    }
#endif

    GID_STYLE style1 = m_colors.style(&l->line);
    double thickness = style1.m_thickness;
    double distance = style1.m_distance;

    if (vyd == 4) {
        colP = colO = colPF = colOF = LIGHTCYAN;  // ПТС
        colP = colO = colPF = colOF = LIGHTBLUE;  // ПТС
    }
    else if (vyd == 1) {
        colP = colO = colPF = colOF = YELLOW;
    }
    else if (vyd == 2) {
        colP = colO = colPF = colOF = 0x0000FF;
    }
    else if (vyd == 3) { // Опрессовка
  //    unsigned long getColorOpressovka(int id, unsigned long color);
        bool getColorOpressovka(int id, unsigned long& color);

        if (getColorOpressovka(l->line.m_opressovka, colP)) {
            colP = colO = colPF = colOF = colP;
        }
        else {
            colP = colO = colPF = colOF = 0x7F7F7F;
            //      vyd = 0;
        }
    }
    else if (vyd == 111) {  // Просмотренная выделенная
        colP = colO = colPF = colOF = LIGHTGREEN;
    }

    if (vyd) {
        double tol = 2;
        if (vyd == 4 && bline(l)->line.isPjezo) { // Участок ПТС
            tol = 4;
        }

        double dd = tol * mas_otn_geotext * 1500 / m_MasPic;
        if (dd / mas_otn < tol*2) dd = tol*2 * mas_otn;
        thickness += dd;
    }

    if (redr) {
        colP = colO = colPF = colOF = CUR_COLOR;
    }

    if (is_hide) {

//    LIGHTGRAY    = 0xBFBFBF,


        colP = colO = colPF = colOF = 0xDFDFDF;
    }


    double m_sX = distance * m_h / mas_otn, dx, dy;
    dx = (x2 - x1) * m_sX / d;
    dy = (y2 - y1) * m_sX / d;

    int naprP = l->line.pod.q >= 0;
    int naprO = l->line.obr.q >= 0;

    if (naprP) {
        t = -1;
    }
    else {
        t = 1;
    }


    if (l->line.nomP >= 0 && l->line.nomO >= 0 && isFull()) {
        drawLineTypL(l, COBR, redr, m_dc, thickness, x1 - dy * t, y1 + dx * t, x2 - dy * t, y2 + dx * t, typ, colO, colOF, ris, naprO, strelka, !isNul(l->line.obr.q));
        drawLineTypL(l, CPOD, redr, m_dc, thickness, x1 + dy * t, y1 - dx * t, x2 + dy * t, y2 - dx * t, typ, colP, colPF, ris, naprP, strelka, !isNul(l->line.pod.q));
    }
    else {
        col = podobr == CPOD ? colP : colO;
        colF = podobr == CPOD ? colPF : colOF;

        if (m_parent_id) t = 0;

        if (podobr == CPOD) {
            drawLineTypL(l, CPOD, redr, m_dc, thickness, x1 + dy * t, y1 - dx * t, x2 + dy * t, y2 - dx * t, typ, colP, colPF, ris, naprP, strelka, !isNul(l->line.pod.q));
        }
        else {
            drawLineTypL(l, COBR, redr, m_dc, thickness, x1 - dy * t, y1 + dx * t, x2 - dy * t, y2 + dx * t, typ, colO, colOF, ris, naprO, strelka, !isNul(l->line.obr.q));
        }
    }
}

void CGidrView::drawLine2(CDC* m_dc, const CLINE2* ll, BOOL ris, double xx1, double yy1, double xx2, double yy2, char podobr, bool redr, bool is_text, int t, bool is_hide)
{
   
    if (ris == 1 && !isFull()) return;

    double x1, y1, x2, y2;

    BOOL strelka = m_bIsNaprGid && !m_bIsPTS;

    x1 = xx1 / geom.masx - geom.m_bx, y1 = yy1 / geom.masy - geom.m_by,
        x2 = xx2 / geom.masx - geom.m_bx, y2 = yy2 / geom.masy - geom.m_by;

    CLine2* l = bline(ll);
    int typ = l->line.typ;

    double d = hypot(xx2 - xx1, yy2 - yy1);
    if (d < 0.01) return;

    if (d > 3 * geom.masx) {
        if (typ == TIP_DR && !DR_otkr(l)) typ = 55;
        if (typ == TIP_ZD && !ZD_otkr(l)) typ = 56;
        if (typ == TIP_ZD2 && !ZD_otkr(l)) typ = 156;
    }

    if (!isFull()) {
        typ = TIP_UT;
        strelka = FALSE;
        len_dot = 0;
        ris = 0;
    }


    //  bool isPjezo2 = where(ll)->node.node_name.kod == m_kod_vyd || other(ll)->node.node_name.kod == m_kod_vyd;
//    bool isPjezo2 = false;
    bool isPjezo2 = m_kod_vyd != -1 && (where(ll)->node.id_kod == m_kod_vyd || other(ll)->node.id_kod == m_kod_vyd);

    bool isY = false;

    if (l->line.pipeSectionID == m_linePTS_ID) { // Участок ПТС
        drawLine00(m_dc, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 4, is_hide);
        isY = true;
    }   

    if (l->line.isPjezo) { // Выделенные желтым
        if (l->line.isOsmotr) { // Просмотренные
            drawLine00(m_dc, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 111, is_hide);
        }
        else {
            drawLine00(m_dc, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 1, is_hide);
        }
        isY = true;
    }

    else if (isPjezo2) {
        drawLine00(m_dc, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 2, is_hide);
        isY = true;
    }
    else if ((l->line.m_opressovka > 0 || m_isNeOpr) && m_isOpressovaka) {  // Опрессовка
        drawLine00(m_dc, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 3, is_hide);
        isY = true;
    }

    if (!isY || mas_otn <= 2) {
        drawLine00(m_dc, ll, ris, x1, y1, x2, y2, podobr, redr, t, typ, 0, is_hide);
    }

    /*
      if (ll) {
        if (m_szY > 4 && m_cxema->m_bIsGidrInf && !redr && ris == 1)  {
          bool isOpcOnOff = !l->strOpc1.IsEmpty() && (l->typ == TIP_HC || l->typ == TIP_ZD || l->typ == TIP_ZD2);

          if (isOpcOnOff && !redr && ris == 1 && pOPCThread) {
            opc_var.push_back(OPCvar(l->strOpc1, l, po, x1, y1, x2, y2));
          }
        }
      }
    */

    if (is_hide) return;


    bool isOpc = !l->line.strOpc1.IsEmpty() && l->line.typ == TIP_US;
    bool isInfo = m_cxema->m_bIsInfo[1] || m_cxema->m_bIsInfo[2] || m_cxema->m_bIsInfo[3] || m_cxema->m_bIsInfo[4] 
        || m_cxema->m_bIsInfo[5] || m_cxema->m_bIsInfo[8] || m_cxema->m_bIsInfo[9] || m_cxema->m_bIsInfo[10] || m_cxema->m_bIsInfo[13];

    if (m_szY > 4 && is_text && m_cxema->m_bIsGidrInf && (isInfo || isOpc) && !redr && ris == 1) {
        if (!isPassport()) drawLinePodp(m_dc, ll, x1, y1, x2, y2);
    }

    if (m_szY > 4 && is_text && m_cxema->m_bIsGidrInf && (!redr || isPassport()) && ris == 1) {

        int l3 = MagRasPar(&l->line);

        if (isInfo && isPassport()) {
            CCoordList cl, m_NP = bline(ll)->line.m_NP;

            CCoordList::const_iterator it;
            it = m_NP.begin();
            cl.push_back(where(l)->node.coord);
            for (; it != m_NP.end(); ++it) {
                cl.push_back(*it);
            }
            cl.push_back(other(l)->node.coord);

            if (podobr == CPOD) {
                CString str;
                if (m_cxema->m_bIsInfo[1]) {
                    str.Format("%g", bline(ll)->line.pod.diam + bline(ll)->line.pod.tol * 2);
                    DrawLabelMulty(m_dc, cl, str, 1);
                }
                if (m_cxema->m_bIsInfo[0]) {
                    str.Format("%g", bline(ll)->line.pod.dl);
                    DrawLabelMulty(m_dc, cl, str, 0);
                }
            }
        }

        if (m_cxema->m_bIsInfo[14] && (l->line.isPodp || m_isPodpAll) && l3 == 0) { // Номер магистрали
            CCoordList cl, m_NP = bline(ll)->line.m_NP;

            CCoordList::const_iterator it;
            it = m_NP.begin();
            cl.push_back(where(l)->node.coord);
            for (; it != m_NP.end(); ++it) {
                cl.push_back(*it);
            }
            cl.push_back(other(l)->node.coord);

            if (podobr == CPOD) DrawLabelMulty(m_dc, cl, where(ll)->node.node_name.kod, 1);
        }
    }
}

void CGidrView::drawPodz(CDC* m_dc, const CLINE2* ll)
{
    int i;
    CNode2* n1, * n2;
    CLine2* l = bline(ll);
    CCoordList cl = l->line.m_NP;
    int m_l = 2, m_r = 2, n;

    double x0, y0, x1, y1, x2, y2, dx1, dy1, dx2, dy2, ddx, ddy;

    CArray <double, double> coor_cx, coor_cy;

    CCoordList::const_iterator it = cl.begin();

    n1 = where(ll);
    n2 = other(ll);

    coor_cx.Add(n1->node.coord.y);
    coor_cy.Add(n1->node.coord.x);

    for (; it != cl.end(); ++it) {
        n = coor_cy.GetSize();
        if (it->x != coor_cy[n - 1] && it->y != coor_cx[n - 1]) {
            coor_cx.Add(it->y);
            coor_cy.Add(it->x);
        }
    }
    n = coor_cy.GetSize();
    if (n2->node.coord.y != coor_cx[n - 1] && n2->node.coord.x != coor_cy[n - 1]) {
        coor_cx.Add(n2->node.coord.y);
        coor_cy.Add(n2->node.coord.x);
    }

    int ns = coor_cx.GetSize();
    int tec = ns * 4;

    if (ns < 2) return;

    double* fx, * fy;

    fx = new double[ns * 2];
    fy = new double[ns * 2];

    for (i = 0; i < ns; i++) {
        x0 = coor_cx[i];
        y0 = coor_cy[i];
        if (i > 0) {
            x1 = coor_cx[i - 1];
            y1 = coor_cy[i - 1];
            dx1 = (x1 - x0) / hypot(y1 - y0, x1 - x0);
            dy1 = (y1 - y0) / hypot(y1 - y0, x1 - x0);
        }
        else {
            x2 = coor_cx[i + 1];
            y2 = coor_cy[i + 1];
            dx1 = -(x2 - x0) / hypot(y2 - y0, x2 - x0);
            dy1 = -(y2 - y0) / hypot(y2 - y0, x2 - x0);
        }

        if (i < ns - 1) {
            x2 = coor_cx[i + 1];
            y2 = coor_cy[i + 1];
            dx2 = (x2 - x0) / hypot(y2 - y0, x2 - x0);
            dy2 = (y2 - y0) / hypot(y2 - y0, x2 - x0);
        }
        else {
            x1 = coor_cx[i - 1];
            y1 = coor_cy[i - 1];
            dx2 = -(x1 - x0) / hypot(y1 - y0, x1 - x0);
            dy2 = -(y1 - y0) / hypot(y1 - y0, x1 - x0);
        }

        if (dx2 + dx1 != 0. && dy2 + dy1 != 0) {
            ddx = (dx1 + dx2) / hypot(dx2 + dx1, dy2 + dy1);
            ddy = (dy1 + dy2) / hypot(dx2 + dx1, dy2 + dy1);
        }
        else {
            ddx = dy2;
            ddy = dx2;
        }

        if (dx2 * ddy > dy2 * ddx) {
            ddx = -ddx;
            ddy = -ddy;
        }

        fx[i] = coor_cy[i] - ddy * m_l * 100.;
        fy[i] = coor_cx[i] + ddx * m_l * 100.;
        fx[ns * 2 - i - 1] = coor_cy[i] + ddy * m_r * 100.;
        fy[ns * 2 - i - 1] = coor_cx[i] - ddx * m_r * 100.;
    }

    cl.clear();

    for (i = 0; i < ns * 2; i++) {
        cl.push_back(CFPoint(fx[i], fy[i]));
    }

    delete[] fx;
    delete[] fy;

    DrawPolygon3(m_dc, cl);
}

void CGidrView::drawLine1(CDC* m_dc, const CLINE2* ll, bool redr, bool is_text, bool is_hide)
{
    CNode2* n1, * n2;
    CLine2* l;
    int i;

    l = bline(ll);

    if (l->line.mag != 0 && l->line.mag == m_mag || l->line.ms != 0 && l->line.ms == m_ms || l->line.rs != 0 && l->line.rs == m_rs) redr = 1;

    int podobr = l->line.nomP != -1 ? CPOD : COBR;

    n1 = where(l);
    n2 = other(l);

    if (IsRezhim(l)) {
        return;
    }

    if (n2->node.coord.x == 0 && n2->node.coord.y == 0) return;

    double x1, y1, x2, y2;

    BOOL ris = TRUE;

    if (is_hide) ris = FALSE;

    double len = hypot(n1->node.coord.x - n2->node.coord.x, n1->node.coord.y - n2->node.coord.y);

    //  if (IsMain() && m_cxema->isGeobaza()) {
    if (IsMain()) {
        if (len < m_szY * 3) {
            ris = FALSE;
        }
    }

    int t = (n2->node.coord.y > n1->node.coord.y || n2->node.coord.y == n1->node.coord.y && n2->node.coord.x > n1->node.coord.x) ? -1 : 1;
    t = -1;

    if (m_colors.style(&l->line).m_style) {
        len_dot = 0;
    }
    else {
        len_dot = 40 * m_h;
    }


    if (l->line.m_NP.size() == 0 || len / geom.masx < 10) {
        x1 = n1->node.coord.x;    y1 = n1->node.coord.y;
        x2 = n2->node.coord.x;    y2 = n2->node.coord.y;

        drawLine2(m_dc, ll, 0, x1, y1, x2, y2, podobr, redr, is_text, t, is_hide);
        drawLine2(m_dc, ll, ris, x1, y1, x2, y2, podobr, redr, is_text, t, is_hide);
    }
    else {
        double x, y;

        x = n1->node.coord.x;
        y = n1->node.coord.y;

        int i_pic = l->line.m_NP.size() / 2;

        CCoordList::const_iterator it = l->line.m_NP.begin();

        for (i = 0; it != l->line.m_NP.end(); ++it, i++) {
            if (i == i_pic) {
                if (x == it->x && y == it->y) {
                    i_pic++;
                }
                else {
                    drawLine2(m_dc, ll, 0, x, y, it->x, it->y, podobr, redr, is_text, t, is_hide);
                    drawLine2(m_dc, ll, ris, x, y, it->x, it->y, podobr, redr, is_text, t, is_hide);
                }
            }
            else {
                drawLine2(m_dc, ll, -1, x, y, it->x, it->y, podobr, redr, is_text, t, is_hide);
            }
            x = it->x;
            y = it->y;
        }
        if (i_pic == l->line.m_NP.size()) {
            drawLine2(m_dc, ll, 0, x, y, n2->node.coord.x, n2->node.coord.y, podobr, redr, is_text, t, is_hide);
            drawLine2(m_dc, ll, ris, x, y, n2->node.coord.x, n2->node.coord.y, podobr, redr, is_text, t, is_hide);
        }
        else {
            drawLine2(m_dc, ll, -1, x, y, n2->node.coord.x, n2->node.coord.y, podobr, redr, is_text, t, is_hide);
        }
    }
}

void SaveFont(const TCHAR* szSection, const LOGFONT& lf, COLORREF color);
bool LoadFont(const TCHAR* szSection, LOGFONT& lf, COLORREF& color);


void CGidrView::init_mas_otn(CDC* dc)
{
    double dm = dc->GetDeviceCaps(LOGPIXELSX) / 2.54;

    //  if (IsMain() && m_cxema->isGeobaza()) {
    //  if (IsMain()) {
    mas_otn = geom.masx * dm / m_MasPic;
    //  }
    //  else {
    //    mas_otn = masx*dm/m_MasPic;
    //    mas_otn = masx*dm/37 / (m_MasPic/1500);
    //  }

    mas_otn0 = mas_otn;

    //  mas_otn_geotext = mas_otn;
    mas_otn_geotext = geom.masx * dm / (500);
    if (!m_bIsMas && mas_otn < 1) mas_otn = 1;
}


CFont* CGidrView::initFont(CDC* dc)
{
    CFont font, * oldfont;
    LOGFONT lf;
    COLORREF color = BLACK;

    init_mas_otn(dc);

    font.CreatePointFont(100, _T("Arial"));
    font.GetLogFont(&lf);
    font.DeleteObject();

    LoadFont(_T("Font Gidr"), lf, color);

    LoadFont(_T("Font Gidr Mag"), m_lf_mag, m_color_mag);

    //  lf.lfHeight *= m_h/mas_otn;
    lf.lfHeight *= m_h;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    if (lf.lfHeight == 0) lf.lfHeight = 1;


    if (mas_otn > 1.)  lf.lfHeight /= mas_otn;


    lf.lfHeight = max(1, abs(lf.lfHeight));


    memcpy(&m_lf, &lf, sizeof(LOGFONT));


    font.CreateFontIndirect(&lf);
    oldfont = (CFont*)dc->SelectObject(&font);

    m_szY = dc->GetTextExtent("TK-008").cy;


    return oldfont;
}

CFont* CGidrView::initFont2(CDC* m_dc, LOGFONT& lf)
{
    CFont font, * oldfont;


    double dm = m_dc->GetDeviceCaps(LOGPIXELSX) / 2.54;

    //  if (IsMain() && m_cxema->isGeobaza()) {
    if (IsMain()) {
        lf.lfHeight *= m_MasPic / dm;
    }
    else {
        lf.lfHeight *= 20;
    }
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;


    lf.lfHeight /= geom.masx;

    if (lf.lfHeight == 0) lf.lfHeight = 1;
    font.CreateFontIndirect(&lf);

    oldfont = (CFont*)m_dc->SelectObject(&font);

    return oldfont;
}


void CGidrView::closeFont(CDC* dc, CFont* oldfont)
{
    CFont* font = dc->SelectObject(oldfont);
    if (font) font->DeleteObject();
}

void CGidrView::redrawPaint(CDC* m_dc)
{

    if (is_paint) {
        log1("1 is_paint");
        return;
    }

//    log1("Начали рисовать 1");
    is_paint = 1;
    drawPaint(m_dc);
    is_paint = 0;

//    log1("Закончили рисовать 1");

}

void CGidrView::redrawPaint2(CDC* m_dc)
{
    if (is_paint) {
        log1("Не рисую.");
        return;
    }

//    log1("Начали рисовать 2");
    
    is_paint = 1;
    drawPaint2(m_dc);
    is_paint = 0;
//    log1("Закончили рисовать 2");
}

static CGraph2* m_graph1 = NULL;

void setGraph(CGraph2* m_graph)
{
    m_graph1 = m_graph;
}


bool CGidrView::isOpressovka()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_DEFECT));
    if (!kls) return false;

    if (!isToolbar(IDR_TOOLBAR_REMONT2_OPRES)) return false;

    return true;
}


/*
bool CGidrView::isRemont2()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_DEFECT));
    if (!kls) return false;

    if (!isToolbar(ID_VIEW_TOOLBAR_REMONT2)) return false;

    return true;
}
*/


#include "graph2.h"


void Node2Node1(const CNode2* n2, CNode* n1)
{
}

bool isWeb();

void CGidrView::drawGid(CDC* m_dc, CGraph2* m_graph, CFRect& rectScr)
{
//    log1("Начали рисовать");

    bool check_update_key();
    if (check_update_key()) return;


    if (m_isPrint) {
        m_CurNode = NULL;
        m_CurLine = NULL;
    }

    if (!m_isPrint && isWeb()) return;


    CFRect rr;

    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();

    m_isPasport = false;

    //  if (isToolbar(IDR_TOOLBAR_PTS_NEW)) {
    //    m_isPasport = true;
    //  }


    if (isPassport()) {
        m_isPasport = true;
        //    m_colors.set_defPS();
    }

    m_isOpressovaka = isOpressovka();

    setGraph(m_graph);

    /*

      m_rectBorder = m_cxema->m_graph->getBorder(m_parent_id);

      if (m_rectBorder.left != LONG_MAX) {
        m_rectBorder.InflateRect(100, 100);
      }
      else {
        if (m_dxf && m_graph->map_node.size() == 0) {
          double xmin, xmax, ymin, ymax;
          void dxf_min_max(cdxf *dxf, double &xmin, double &xmax, double &ymin, double &ymax);

          dxf_min_max(m_dxf, xmin, xmax, ymin, ymax);
          m_rectBorder = CFRect(xmin, ymin, xmax, ymax);
        }
      }
    */

    /*
      if (m_rectBorder.left > rectScr.right) return;
      if (m_rectBorder.right < rectScr.left) return;

      if (m_rectBorder.top > rectScr.bottom) return;
      if (m_rectBorder.bottom < rectScr.top) return;
    */

    CFRect rf = ScreenToCoord(CRect(0, 0, 800, 600));

    int nq = (m_rectBorder.Width() * m_rectBorder.Height()) / (rf.Width() * rf.Height());
//    m_n_txt = m_graph->getNodeCount(m_parent_id) / (nq + 1);
    m_n_txt = 0;


    int qqq_n = 0;
    int qqq_l = 0;

    CFRect rectScr2 = rectScr;
//    rectScr2.InflateRect(rectScr.Width(), rectScr.Height());
    rectScr2.InflateRect(rectScr.Width()/5., rectScr.Height()/5.);

//    line(m_dc, 0, 0, 1000, 1000);
//    line(m_dc, 0, 1000, 1000, 0);

#if 1

// Спрятанные участки

    clock_t t1 = clock();

    for (auto& p : m_cxema->m_graph->map_node) {
        CNode2* pp = p.second;

        if (pp->node.coord.x != 0 && pp->node.coord.y != 0 && pp->node.internalNodeID == m_parent_id) {

            for (CLINE2* l = pp->lines; l; l = next(l)) {
//                if (IsBegin(l) && (!bline(l)->line.isHide || m_idMenu != 3)) {
               if (IsBegin(l) && (bline(l)->line.isHide)) {
//               if (IsBegin(l)) {
                    CNode2* n1 = where(l);
                    CNode2* n2 = other(l);
                    if (n1->node.coord.x != 0 && n2->node.coord.x != 0) {
                        CFRect rl = bline(l)->line.rect;

                        if (rl.right == 0 && rl.left == 0 && rl.top == 0 && rl.bottom == 0) {
                            bline(l)->line.rect = CFRect(n1->node.coord, n2->node.coord);
                            rl = bline(l)->line.rect;
                        }

                        rl.NormalizeRect();
                        if (rr.IntersectRect(rectScr2, rl)) {
                            drawLine1(m_dc, l, false, true, bline(l)->line.isHide);
                            qqq_l++;
                        }
                    }
                }
            }
        }
    }
    
    
    clock_t t2 = clock();
    
// Неспрятанныее участки
    
    for (auto& p : m_cxema->m_graph->map_node) {
        CNode2* pp = p.second;

        if (pp->node.coord.x != 0 && pp->node.coord.y != 0 && pp->node.internalNodeID == m_parent_id) {

            for (CLINE2* l = pp->lines; l; l = next(l)) {
//                if (IsBegin(l) && (!bline(l)->line.isHide || m_idMenu != 3)) {
               if (IsBegin(l) && (!bline(l)->line.isHide)) {
//               if (IsBegin(l)) {
                    CNode2* n1 = where(l);
                    CNode2* n2 = other(l);
                    if (n1->node.coord.x != 0 && n2->node.coord.x != 0) {
                        CFRect rl = bline(l)->line.rect;

                        if (rl.right == 0 && rl.left == 0 && rl.top == 0 && rl.bottom == 0) {
                            bline(l)->line.rect = CFRect(n1->node.coord, n2->node.coord);
                            rl = bline(l)->line.rect;
                        }

                        rl.NormalizeRect();
                        if (rr.IntersectRect(rectScr2, rl)) {
                            drawLine1(m_dc, l, false, true, bline(l)->line.isHide);
                            qqq_l++;
                        }
                    }
                }
            }
        }
    }

    clock_t t3 = clock();

        CFont* oldfont = initFont(m_dc);

        m_dc->SetTextColor(color);
        m_dc->SetTextAlign(TA_LEFT);

        for (auto& p : m_cxema->m_graph->map_node) {
            CNode2* node = p.second;
            if ((isFull() || node->node.isMarkAnalyze) && node->node.internalNodeID == m_parent_id && !node->node.isHide) {
                if (!m_isUkrup || (node->node.typ != TIP_UT || node->node.isP)) {
                    if (/*m_n_txt < 200 && */rectScr2.PtInRect(node->node.coord)) {
                            drawNode(m_dc, node, false);
                            qqq_n++;
                    }
                }
            }
        }

        closeFont(m_dc, oldfont);
    

    clock_t t4 = clock();


    if (log_draw()) {
        log1("drawGid l1 %g, l2 %g, node %g", (double)(t2-t1) / CLOCKS_PER_SEC, (double)(t3-t2) / CLOCKS_PER_SEC, (double)(t4-t3) / CLOCKS_PER_SEC);
    }

#endif

//    log1("Закончили рисовать");

}

void setTransp(int tr, int bk_color);


void CGidrView::drawPaint(CDC* m_dc2)
{
    CDC* m_dc = BeginAlpha(m_dc2);
    drawPaint2(m_dc);
    EndAlpha(m_dc2, m_dc);
}

void CGidrView::drawLoc1(CDC* adc, int isText, bool is_GID)
{
    try {
        m_geofile->Draw(adc, this, m_rect, geom, 1, m_isMarkOnly, isText, is_GID);
    }
    catch (...) {
    }
}

// Ремонт  линейные

void CGidrView::drawLoc2_REM(CDC* adc, int isText, bool is_GID)
{
    try {
        m_geofile->Draw(adc, this, m_rect, geom, 12, m_isMarkOnly, isText, is_GID);
    }
    catch (...) {
    }
}



bool redrawMap(CMapsThread* m_pMapsThread, CGidrView* view, CDC* dc, CRect& m_rect, int typ_map, int is_reread);

static int m_loc1_over = 1;

/*

void CGidrView::drawPaint2(CDC* m_dc)
{
    int isText = 1;

    m_isDop = AfxGetApp()->GetProfileInt("Settings", _T("IsDop"), 0);

    opc_var.clear();

    if (m_cxema->m_graph == NULL) return;

    if (!m_bOnlyGid) {
        if (m_bk_color != 0xFFFFFF) {
            CRect rect1;
            m_dc->SetBkColor(m_bk_color);
            GetClientRect(rect1);
            CBrush brush(m_bk_color);
            m_dc->FillRect(rect1, &brush);
        }

#if 1

        init_mas_otn(m_dc);

        CDC* adc = m_dc;

        //    if (m_map != ID_NOMAP && !m_isMarkOnly) {
        if (!m_isMarkOnly) {
            setTransp(0, m_bk_color);
            if (::redrawMap(m_pMapsThread, this, adc, m_rect, m_map, m_reread)) {
//                Invalidate();
            }
            m_reread = FALSE;
        }


        setTransp(m_nTransp, m_bk_color);

        if (m_bIsPicture && m_parent_id == 0 && !m_isMarkOnly) {
            if (m_dxf) {
                drawDxf(m_dxf, adc, m_rect, m_bx, m_by, masx, masy);
            }

            try {
                qqq_list->Draw(adc, m_rect, m_bx, m_by, masx, masy);
            }
            catch (...) {
            }
        }
        if (m_isGeobaza && m_parent_id == 0) {
            try {
                m_geofile->Draw(adc, this, m_rect, m_bx, m_by, masx, masy, -2, m_isMarkOnly, isText, false);
//                if (!m_loc1_over) 
                drawLoc1(m_dc, isText, false);
            }
            catch (...) {
            }
        }

        //    m_dc = EndAlpha();
        //    EndAlpha(m_dc, adc);

        setTransp(0, m_bk_color);

#endif
    }


    //  m_dc->SetBkMode(TRANSPARENT);
    m_dc->SetBkMode(OPAQUE);

    //////////// Установка шрифтов /////////////

    CFont* oldfont = initFont(m_dc);

    /////////////////

    m_dc->SetTextColor(color);
    m_dc->SetTextAlign(TA_LEFT);

    CRect rect = m_rect;
    //  if (!m_dc->IsPrinting()) GetClientRect(rect);

    rect.InflateRect(50, 50);
    CFRect rectScr = ScreenToCoord(rect);

    void setSystemCoord(CFPoint pt);

   // CFPoint pt(-(rectScr.top+rectScr.bottom)/2/100, (rectScr.left+rectScr.right)/2/100); 
   // setSystemCoord(pt);


    setTransp(0, m_bk_color);

    if (!m_isMarkOnly) {
        setTransp(0, m_bk_color);
        drawPaint(m_dc, m_cxema->m_graph, rectScr);
        closeFont(m_dc, oldfont);
        drawTexts(m_dc);
    }

//    setTransp(m_nTransp, m_bk_color);
//    setcolor(m_dc, 0);

    //  if ( m_rectBorder.left == LONG_MAX && m_isGeobaza && IsMain()) {
    //    m_geofile->GetBorder(m_rectBorder);
    //    SetBorder(m_rectBorder);
    //  }

//    if (m_isGeobaza && m_parent_id == 0 && m_loc1_over) {
//        drawLoc1(m_dc, isText, true);
//        drawLoc2_REM(m_dc, isText, true);
//    }
    setTransp(0, m_bk_color);

    if (m_parent_id == 0 ) {
        m_geofile->Draw(m_dc, this, m_rect, m_bx, m_by, masx, masy, -2, m_isMarkOnly, isText, true);
        drawLoc1(m_dc, isText, true);
    }


    drawBorder(m_dc);
    EndPaint2(m_dc);
}

*/


void CGidrView::drawPaint2(CDC* m_dc, bool is_gid, bool is_geo, bool is_map)
{
    int isText = 1;

    clock_t t1 = clock();

    if (log_draw()) {
        log1("---------------------------------------------------------------------");
    }


    m_isDop = AfxGetApp()->GetProfileInt("Settings", _T("IsDop"), 0);

    opc_var.clear();

    if (m_cxema->m_graph == NULL) return;

    if (!m_bOnlyGid) {
        if (m_bk_color != 0xFFFFFF) {
            CRect rect1;
            m_dc->SetBkColor(m_bk_color);
            GetClientRect(rect1);
            CBrush brush(m_bk_color);
            m_dc->FillRect(rect1, &brush);
        }

#if 1

        init_mas_otn(m_dc);

        CDC* adc = m_dc;

        //    if (m_map != ID_NOMAP && !m_isMarkOnly) {
        if (!m_isMarkOnly) {
            setTransp(0, m_bk_color);
            ::redrawMap(m_pMapsThread, this, adc, m_rect, m_map, m_reread);
            m_reread = FALSE;
        }


        setTransp(m_nTransp, m_bk_color);

        if (m_bIsPicture && m_parent_id == 0 && !m_isMarkOnly) {
            if (m_dxf) {
                drawDxf(m_dxf, adc, m_rect, geom.m_bx, geom.m_by, geom.masx, geom.masy);
            }

            try {
                qqq_list->Draw(adc, m_rect, geom.m_bx, geom.m_by, geom.masx, geom.masy);
            }
            catch (...) {
            }
        }
        if (m_isGeobaza && m_parent_id == 0) {
            try {
                m_geofile->Draw(adc, this, m_rect, geom, -2, m_isMarkOnly, isText, true);
                if (!m_loc1_over) drawLoc1(m_dc, isText, true);
            }
            catch (...) {
            }
        }

        //    m_dc = EndAlpha();
        //    EndAlpha(m_dc, adc);

        setTransp(0, m_bk_color);

#endif
    }


    //  m_dc->SetBkMode(TRANSPARENT);
    m_dc->SetBkMode(OPAQUE);

    //////////// Установка шрифтов /////////////


    /////////////////

    m_dc->SetTextColor(color);
    m_dc->SetTextAlign(TA_LEFT);

    CRect rect = m_rect;
    //  if (!m_dc->IsPrinting()) GetClientRect(rect);

    rect.InflateRect(50, 50);
    CFRect rectScr = ScreenToCoord(rect);

    void setSystemCoord(CFPoint pt);

   // CFPoint pt(-(rectScr.top+rectScr.bottom)/2/100, (rectScr.left+rectScr.right)/2/100); 
   // setSystemCoord(pt);


    setTransp(0, m_bk_color);

    if (!m_isMarkOnly && !m_no_gid && (m_web_layer == L_ALL || m_web_layer == L_GID)) {
        setTransp(0, m_bk_color);
        CFont* oldfont = initFont(m_dc);
        drawGid(m_dc, m_cxema->m_graph, rectScr);
        closeFont(m_dc, oldfont);
        drawTexts(m_dc);
    }

    setTransp(m_nTransp, m_bk_color);
    setcolor(m_dc, 0);

    //  if ( m_rectBorder.left == LONG_MAX && m_isGeobaza && IsMain()) {
    //    m_geofile->GetBorder(m_rectBorder);
    //    SetBorder(m_rectBorder);
    //  }

    if (m_isGeobaza && m_parent_id == 0 && m_loc1_over) {
        drawLoc1(m_dc, isText, true);
        drawLoc2_REM(m_dc, isText, true);
    }

    if (m_nRegim == ID_BEGIN_CANAL_2) {
        double x = m_pt1_canal.x / geom.masx - geom.m_bx, y = m_pt1_canal.y / geom.masy - geom.m_by;
        drawNodeTyp(m_dc, x, y, TIP3_ZN, RED);
    }


    setTransp(0, m_bk_color);
    drawBorder(m_dc);
    EndPaint2(m_dc);

    clock_t t2 = clock();


    if (log_draw()) {
        log1("Итого - %g секунд", (double)(t2-t1) / CLOCKS_PER_SEC);
        log1("---------------------------------------------------------------------");
    }

}



void CGidrView::drawTexts(CDC* m_dc)
{
    /////////////////
    m_dc->SetTextColor(0);

    set<CText>::const_iterator pt = m_cxema->set_text.begin();

    m_dc->SetTextAlign(TA_LEFT | TA_BOTTOM);

    while (pt != m_cxema->set_text.end()) {
        if (pt->internalNodeID == m_parent_id) {
            LOGFONT lf = pt->lf;

            oldfont = initFont2(m_dc, lf);
            m_dc->ExtTextOut(pt->coord.x / geom.masx - geom.m_bx, pt->coord.y / geom.masy - geom.m_by, ETO_CLIPPED, m_rect, pt->text, NULL);
            closeFont(m_dc, oldfont);
        }
        pt++;
    }
    /////////////////
}


void CGidrView::drawBorder(CDC* m_dc)
{
    int width, height, x0, y0, x, y;

    //  if (m_rectBorder.left == LONG_MAX)
    //    SetBorder(CFRect(0.+40000., 0.-40000., 400.+40000., 400.-40000.));
    //  else
    //    SetBorder(m_rectBorder);

    if (m_dc->IsPrinting()) {
//        hideU(m_dc, m_rect);
    }

    setline(m_dc, YELLOW, 3);
    setline(m_dc, 0, 3);

    if (m_isRamka) {
        //    CFRect m_rectBorder0 = getMaxRect();
        CFRect m_rectBorder0 = m_rectBorder;
        CFRect rgeo;

        if (0) {
            m_geofile->GetBorder(rgeo);

            if (rgeo.left != LONG_MAX) {
                m_rectBorder0.UnionRect(&rgeo, &m_rectBorder0);
            }
        }


        int x1 = m_rectBorder0.left / geom.masx - geom.m_bx;
        int x2 = m_rectBorder0.right / geom.masx - geom.m_bx;
        int y1 = m_rectBorder0.top / geom.masy - geom.m_by;
        int y2 = m_rectBorder0.bottom / geom.masy - geom.m_by;

//        rectangle(m_dc, x1, y1, x2, y2);

        int xx1 = max(-1, x1);
        int xx2 = min(x2, m_rect.Width()+1);
        int yy1 = max(-1, y1);
        int yy2 = min(y2, m_rect.Height()+1);

        if (0 <= x1 && x1 <= m_rect.Width()) {
            line(m_dc, x1, yy1, x1, yy2);
        }
        if (0 <= y2 && y2 <= m_rect.Height()) {
            line(m_dc, xx1, y2, xx2, y2);
        }
        if (0 <= x2 && x2 <= m_rect.Width()) {
            line(m_dc, x2, yy1, x2, yy2);
        }
        if (0 <= y1 && y1 <= m_rect.Height()) {
            line(m_dc, xx1, y1, xx2, y1);
        }




    }

    setcolor(m_dc, 0);

    //  if (m_dX != 0 && !m_dc->IsPrinting() && m_dX/masx >= 5 && !m_cxema->isGeobaza()) {
    if (m_dX != 0 && !m_dc->IsPrinting() && m_dX / geom.masx >= 5 && m_parent_id != 0) {
        width = m_rect.Width() * geom.masx + m_dX;
        height = m_rect.Height() * geom.masy + m_dX;

        for (x = okr((geom.m_bx + m_rect.left) * geom.masx, m_dX), x0 = 0; x0 <= width; x += m_dX, x0 += m_dX)
            for (y = okr((geom.m_by + m_rect.top) * geom.masy, m_dX), y0 = 0; y0 <= height; y += m_dX, y0 += m_dX)
                putpixel(m_dc, x / geom.masx - geom.m_bx, y / geom.masy - geom.m_by, 0);
    }
}


void CGidrView::drawAll(CDC* dc, CRect rect)
{
    double masx_old = geom.masx, masy_old = geom.masy, bx_old = geom.m_bx, by_old = geom.m_by;

    SetDC(dc);
    SetRect(rect);

    rect.NormalizeRect();

    double m = max((double)m_rectBorder.Width() / rect.Width(), (double)m_rectBorder.Height() / rect.Height());

    geom.m_bx = m_rectBorder.left / m;
    geom.m_by = m_rectBorder.top / m;
    geom.masx = geom.masy = m;

    dc->SetBkColor(m_bk_color);
    CBrush brush(m_bk_color);
    dc->FillRect(rect, &brush);

    redrawPaint(dc);

    geom.masx = masx_old; geom.masy = masy_old; geom.m_bx = bx_old; geom.m_by = by_old;
}


void CGidrView::drawOPC2()
{
    if (!pOPCThread) return;
    if (m_cxema->m_graph == NULL) return;
    if (m_isSavedImage) return;
    if (m_bIsTic) return;
    //  if (m_dc) return;

    list<OPCvar> opc_var2 = opc_var;

    if (opc_var2.size() > 0) {
        CDC* m_dc;
        if ((m_dc = BeginPaint()) != NULL) {
            CFont* oldfont = initFont(m_dc);
            if (oldfont) {

                CSize sz = m_dc->GetTextExtent("0");
                m_dc->SetTextColor(0);

                if (sz.cy > 4) {
                    if (!m_bIsMas) mas_otn = 1;

                    list<OPCvar>::const_iterator it;

                    for (it = opc_var2.begin(); it != opc_var2.end(); ++it) {
                        CString str = it->str;

                        if (!it->str.IsEmpty()) {
                            str = pOPCThread->get_value(it->str);
                            if (it->is_on_off) {
                                CLine2* l = it->l;
                                int col, colF;
                                bool otkl = false;

                                if (str == "On" || str == "1") {
                                    otkl = false;
                                }
                                else if (str == "Off" || str == "0") {
                                    otkl = true;
                                }

                                //                otkl = false;

                                if (it->po == CPOD) {
                                    l->line.pod.isOtkl = otkl;
                                    col = m_colors.pod(&l->line, m_cxema->m_bIsGidrInf);
                                    colF = m_colors.podF(&l->line, m_cxema->m_bIsGidrInf);
                                }
                                else {
                                    l->line.obr.isOtkl = otkl;
                                    col = m_colors.obr(&l->line, m_cxema->m_bIsGidrInf);
                                    colF = m_colors.obrF(&l->line, m_cxema->m_bIsGidrInf);
                                }

                                drawLineTyp(m_dc, m_colors.style(&l->line).m_thickness, it->x1, it->y1, it->x2, it->y2, l->line.typ, col, colF, 1, 1, 1, 1, false);
                            }
                            else {
                                if (str.GetLength() < 12) {
                                    CString sp(' ', (13 - str.GetLength()) / 2);
                                    str = sp + str + sp;
                                }

                                drawText(m_dc, it->x, it->y, str);
                            }
                        }
                    }
                    closeFont(m_dc, oldfont);
                }
            }
            EndPaint(m_dc);
        }
    }

    if (pOPCThread) pOPCThread->Tic();
}

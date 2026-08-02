// gidrView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "win.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString get_SelectLine(int typ, const char* tn, const CNode* parent = NULL);
CString get_SelectNode(int typ, const char* tn, const CNode* parent = NULL);


#define BLACK 0

int inc1(const CNode2* n)
{
    int k = 0;
    CLINE2* l;
    CLine2* ll;
    for (l = n->lines; l; l = next(l)) {
        ll = bline(l);
        if (ll->line.pod.isOtkl) continue;
        if (ll->line.pod.isOtkl) continue;
        if (ll->line.nomP >= 0 && ll->line.pod.q == 0 || ll->line.nomO >= 0 && ll->line.obr.q == 0) continue;
        k++;
    }
    if (n->node.nUP != -1) k++;

    return k;
}


CString strnum(double o)
{
    CString str;

    if (fabs(o) - floor(fabs(o)) < 0.05) {
        str.Format("%.0f", o);
    }
    else {
        if (o < 100) {
            str.Format("%.2f", o);
        }
        else {
            str.Format("%.1f", o);
        }
    }
    return str;
}


CString strnum2(double o)
{
    CString str;

    if (o < 0.001) {
        str = "0";
        return str;
    }

    if (o < 10) {
        str.Format("%.3f", o);
        return str;
    }

    if (o < 100) {
        str.Format("%.2f", o);
        return str;
    }

    str.Format("%.1f", o);
    return str;
}



CString formatOutLine(const char* c, int np, int no, double p, double o)
{
    CString str = "";

    /*
      if (np != -1 && no != -1 && p != o)
        str.Format("%s=%.1f;%.1f", c, p, o);
      else if (np != -1 && no != -1)
        str.Format("%s=%.1f", c, p);
      else if (np != -1)
        str.Format("%s=%.1f;", c, p);
      else if (no != -1)
        str.Format("%s=;%.1f", c, o);

    */

    CString oo, pp;

    if (c[0] == 'Q' || c[0] == 'w') {
       oo = strnum2(o);
       pp = strnum2(p);
    }
    else {
       oo = strnum(o);
       pp = strnum(p);
    }

/*
    if (np != -1 && no != -1 && p != o)
        str.Format("%s=%s; %s", c, pp, oo);
    else if (np != -1 && no != -1)
        str.Format("%s=%s", c, pp);
    else if (np != -1)
        str.Format("%s=%s;", c, pp);
    else if (no != -1)
        str.Format("%s=;%s", c, oo);
*/
    CString pv = "п";
    CString ov = "о";
    
    if (strlen(c) > 1)  {
        pv = " п";
        ov = " о";
    }

    if (np != -1 && no != -1 && p != o)
        str.Format("%s%s=%s; %s%s=%s", c, pv, pp, c, ov, oo);
    else if (np != -1 && no != -1)
        str.Format("%s=%s", c, pp);
    else if (np != -1)
        str.Format("%s%s=%s", c, pv, pp);
    else if (no != -1)
        str.Format("%s%s=%s", c, ov, oo);

    return str;
}

void drawPodp2(CDC* dc, int cx, int cy, CString str)
{
    int n;
    CSize sz;
    CString str1, str2;

    cx += 1;
    sz = dc->GetTextExtent(str);

    str1 = str.Left(2);
    sz = dc->GetTextExtent(str1);
    dc->SetTextColor(0);
    dc->TextOut(cx, cy, str1);
    cx += sz.cx;

    str = str.Mid(2);

    n = str.Find(';');

    if (n == 0) {
        str = str.Mid(1);
        dc->SetTextColor(BLUE);
        dc->TextOut(cx, cy, str);
    }
    else if (n == str.GetLength() - 1) {
        str = str.Left(str.GetLength() - 1);
        dc->SetTextColor(RED);
        dc->TextOut(cx, cy, str);
    }
    else if (n != -1) {
        str1 = str.Left(n + 1);
        sz = dc->GetTextExtent(str1);
        dc->SetTextColor(RED);
        dc->TextOut(cx, cy, str1);

        str1 = str.Mid(n + 1);
        dc->SetTextColor(BLUE);
        dc->TextOut(cx + sz.cx, cy, str1);
    }
    else {
        dc->SetTextColor(0);
        dc->TextOut(cx, cy, str);
    }
}

//-----------------------------------------------
// Нарисовать подпись к линии
//-----------------------------------------------

#include "OPCThread2.h"


bool CScroll::isFull()
{

    return /*mas_otn < 3;/**/ geom.masx * geom.dmas < m_MasPodp;/**/

//    return /*mas_otn < 3;/**/ masx * dmas < 5000;/**/ 
}


void CGidrView::drawLinePodp(CDC* m_dc, const CLINE2* ll, double dx1, double dy1, double dx2, double dy2)
{
    int t1, t2, x1, y1, x2, y2, x, y, xx, yy, dc;
    int i = 0;
    CLine2* l = bline(ll);

    bool f = isFull();
    if (!(l->line.isPodp || m_isPodpAll)) return;

    if (l->line.nomP == -1 && l->line.nomO == -1) return;
    //  if (l->line.nomgP == -1 && l->line.nomgO == -1) return;

    m_dc->SetBkColor(m_bk_color);

    if (!f || l == NULL || m_n_txt > 100) return;

    if (l->line.nomP <= 0 || l->line.nomO <= 0) {
        if (inc1(where(ll)) == 2 && inc1(other(ll)) == 2) {
            //      return;
        }
    }

    double dd, dx, dy;

    CString str[256];

    m_dc->SetTextAlign(TA_LEFT);

    if (!linef[l->line.typ].out) return;

    int nn = 0;

    if (m_cxema->m_bIsInfo[1]) {
        if (l->line.pod.q != 0. || l->line.obr.q != 0.) {
            str[nn++] = formatOutLine("G", l->line.nomP, l->line.nomO, fabs(l->line.pod.q), fabs(l->line.obr.q));
        }
    }
    if (m_cxema->m_bIsInfo[2] && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("L", l->line.nomP, l->line.nomO, l->line.pod.dl, l->line.obr.dl);
    }
    if (m_cxema->m_bIsInfo[3] && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("Двн", l->line.nomP, l->line.nomO, l->line.pod.diam, l->line.obr.diam);
    }
    if (m_cxema->m_bIsInfo[4] && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("Ду", l->line.nomP, l->line.nomO, l->line.pod.diam_usl, l->line.obr.diam_usl);
    }
    if (m_cxema->m_bIsInfo[5] && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("V", l->line.nomP, l->line.nomO, l->line.pod.v, l->line.obr.v);
    }
    if (m_cxema->m_bIsInfo[8] && l->line.typ == TIP_UT) {
        if (l->line.pod.q != 0. || l->line.obr.q != 0.) {
            str[nn++] = formatOutLine("w", l->line.nomP, l->line.nomO, l->line.pod.w, l->line.obr.w);
        }
    }
    if (m_cxema->m_bIsInfo[9] && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("R", l->line.nomP, l->line.nomO, l->line.pod.poter, l->line.obr.poter);
    }
    if (m_cxema->m_bIsInfo[13] && l->line.typ == TIP_UT) {
        if (l->line.pod.q != 0. || l->line.obr.q != 0.) {
            //      str[nn++] = formatOutLine("Qр", l->line.nomP, l->line.nomO, l->line.pod.tzam, l->line.obr.tzam);

            str[nn++] = formatOutLine("Q", l->line.nomP, l->line.nomO, l->line.pod.ql, l->line.obr.ql);

            if (l->line.pod.ql_ot != 0 || l->line.obr.ql_ot != 0) {
                str[nn++] = formatOutLine("Qот", l->line.nomP, l->line.nomO, l->line.pod.ql_ot, l->line.obr.ql_ot);
            }
            if (l->line.pod.ql_v != 0 || l->line.obr.ql_v != 0) {
                str[nn++] = formatOutLine("Qвент", l->line.nomP, l->line.nomO, l->line.pod.ql_v, l->line.obr.ql_v);
            }
            if (l->line.pod.ql_gv != 0 || l->line.obr.ql_gv != 0) {
                str[nn++] = formatOutLine("Qгвс", l->line.nomP, l->line.nomO, l->line.pod.ql_gv, l->line.obr.ql_gv);
            }
        }
    }

    if (l->line.typ == TIP_UT) {
        if (l->line.pod.q != 0. || l->line.obr.q != 0.) {
            //            if (m_QG < 0) 
            //                m_QG = getQG();

            //            formatOutLine(str[nn++], "Q", l->line.nomP, l->line.nomO, fabs(l->line.pod.q)*m_QG, fabs(l->line.obr.q)*m_QG);
        }
    }





    extern COPCThread2* pOPCThread;

    if (!l->line.strOpc1.IsEmpty() && pOPCThread) {
        str[nn++] = "                ";
    }

    //  if (!l->line.strOpcP.IsEmpty()) {
    //    str[nn++] = l->line.strOpcP;
    //  }


    CFont* oldfont, * font = new CFont();

    //        font->DeleteObject();


    font->CreateFontIndirect(&m_lf);
    oldfont = m_dc->SelectObject(font);




    CSize sz(0, 0), sz1;

    for (i = 0; i < nn; i++) {
        sz1 = m_dc->GetTextExtent(str[i]);
        sz = CSize(max(sz.cx, sz1.cx), max(sz.cy, sz1.cy));
    }

    if (sz.cy <= 4) goto EXIT;

    dc = sz.cy / 5;
    xx = sz.cx + dc;
    yy = sz.cy * nn + dc;

    dd = hypot(dx2 - dx1, dy2 - dy1);  if (dd < 1.) goto EXIT;
    dx = (dx2 - dx1) / dd * dc * 4;
    dy = (dy2 - dy1) / dd * dc * 4;

    t1 = (dy > 0) ? 1 : -1;
    t2 = (dx < 0) ? 1 : -1;

    x1 = (dx1 + dx2) / 2 + dy - dc / 2; x2 = x1 + t1 * xx + dc / 2;
    y1 = (dy1 + dy2) / 2 - dx - dc / 2; y2 = y1 + t2 * yy + dc / 2;

    if (dx1 == dx2) {
        y1 = (dy1 + dy2) / 2 - yy / 2;
        y2 = (dy1 + dy2) / 2 + yy / 2;
    }
    if (dy1 == dy2) {
        x1 = (dx1 + dx2) / 2 - xx / 2;
        x2 = (dx1 + dx2) / 2 + xx / 2;
    }

    setline(m_dc, 0, 0);
    line(m_dc, (dx1 + dx2) / 2, (dy1 + dy2) / 2, x1, y1);

    m_dc->FillSolidRect(CRect(x1, y1, x2, y2), m_bk_color);

    if (x2 < x1) { x = x1; x1 = x2; x2 = x; }
    if (y2 < y1) { y = y1; y1 = y2; y2 = y; }

    for (i = 0; i < nn; i++) {
        //    m_dc->TextOut(x1+dc/2, y1+sz.cy*i+dc/2, str[i]);
        m_dc->SetBkColor(m_bk_color);
        drawPodp2(m_dc, x1 + dc / 2, y1 + sz.cy * i + dc / 2, str[i]);
    }
    if (!l->line.strOpc1.IsEmpty() && pOPCThread) {
        opc_var.push_back(OPCvar(l->line.strOpc1, x1 + dc / 2 + sz.cx / 2, y1 + sz.cy * (i - 1) + dc / 2));
        //    drawPodp2(m_dc, x1+dc/2, y1+sz.cy*i+dc/2, getOPC(l->line.strOpcP));
    }


    setline(m_dc, 0, 0);
    rectangle(m_dc, x1, y1, x2 + dc, y2);

EXIT:

    m_dc->SelectObject(oldfont);
    delete font;


}   
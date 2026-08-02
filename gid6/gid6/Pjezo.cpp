// Pjezo.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "gid6Doc.h"
#include "GidrView.h"
#include "gid6.h"
#include "Pjezo.h"
//#include "PjezoTable.h"
#include "InputDia.h"
#include "mmenu.h"
#include <math.h>
#include "excel.h"
#include "win.h"
#include "ado.h"

#include "wm_user.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const double PIHNULL = -100000;


/////////////////////////////////////////////////////////////////////////////
// CPjezo

IMPLEMENT_DYNCREATE(CPjezo, CScroll)

CPjezo::CPjezo()
{
    //  m_gid_file = NULL;
    m_mas = 1;
    m_n = 0;
    m_napor = TRUE;
    m_vysot = TRUE;
    m_otmet = TRUE;
    m_okno = TRUE;
    m_stat = TRUE;
    m_real = FALSE;
    m_dbl = FALSE;

    m_podacha = TRUE;
    m_obratka = TRUE;
    m_fact = TRUE;

    m_bIsFragment = TRUE;

    m_calculationID = 0;
    m_fileID = 0;

    m_graph2 = nullptr;

    //  CClientDC screenDC(NULL);
    //  dmas = screenDC.GetDeviceCaps(LOGPIXELSX)/2.54;

}

CPjezo::~CPjezo()
{
    if (m_graph2) delete m_graph2;

    for (int i = 0; i < sizeof(m_bIsInfo) / sizeof(m_bIsInfo[0]); i++) {
        CString sss;
        sss.Format("IsInfoP%d", i);
        AfxGetApp()->WriteProfileInt(szSection, sss, m_bIsInfo[i]);
    }
}


BEGIN_MESSAGE_MAP(CPjezo, CScroll)
    //{{AFX_MSG_MAP(CPjezo)
    ON_COMMAND(ID_PLUS, OnPlus)
    ON_COMMAND(ID_MINUS, OnMinus)
    ON_COMMAND(ID_MASALL_PJEZO, OnMasall)
    ON_COMMAND(ID_FONT, OnFont)
    ON_COMMAND(ID_EXCEL, OnExcel)
    ON_WM_MOUSEWHEEL()
    ON_WM_MBUTTONDOWN()
    ON_WM_KEYDOWN()
    ON_UPDATE_COMMAND_UI(ID_NAPOR, OnUpdateNapor)
    ON_COMMAND(ID_NAPOR, OnNapor)
    ON_COMMAND(ID_OTMET, OnOtmet)
    ON_UPDATE_COMMAND_UI(ID_OTMET, OnUpdateOtmet)
    ON_COMMAND(ID_OKNO, OnOkno)
    ON_UPDATE_COMMAND_UI(ID_OKNO, OnUpdateOkno)
    ON_COMMAND(ID_VYSOT, OnVysot)
    ON_UPDATE_COMMAND_UI(ID_VYSOT, OnUpdateVysot)
    ON_COMMAND(ID_INF, OnInf)

    ON_COMMAND(ID_PODACHA, OnPodacha)
    ON_UPDATE_COMMAND_UI(ID_PODACHA, OnUpdatePodacha)
    ON_COMMAND(ID_OBRATKA, OnObratka)
    ON_UPDATE_COMMAND_UI(ID_OBRATKA, OnUpdateObratka)
    //  ON_COMMAND(ID_FACT, OnFact)
    //  ON_UPDATE_COMMAND_UI(ID_FACT, OnUpdateFact)

    ON_COMMAND(ID_DOUBLE, OnDouble)
    ON_UPDATE_COMMAND_UI(ID_DOUBLE, OnUpdateDouble)

    ON_COMMAND(ID_EDIT_COPY2, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_STAT, OnUpdateStat)
    ON_COMMAND(ID_STAT, OnStat)
    ON_COMMAND(ID_TECHINFO, OnTechinfo)
    ON_COMMAND(ID_REAL_P, OnRealP)
    ON_UPDATE_COMMAND_UI(ID_REAL_P, OnUpdateRealP)
    ON_COMMAND(ID_SAVEPJEZO, OnSavePjezo)
    ON_COMMAND(ID_MOVE_TO_PJEZO, OnMoveTo)
    ON_COMMAND(ID_LIST_PJEZO, OnListPjezo)
    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CScroll::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CScroll::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScroll::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPjezo drawing


/////////////////////////////////////////////////////////////////////////////
// CPjezo diagnostics

#ifdef _DEBUG
void CPjezo::AssertValid() const
{
    CScroll::AssertValid();
}

void CPjezo::Dump(CDumpContext& dc) const
{
    CScroll::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPjezo message handlers

void SaveFont(const TCHAR* szSection, const LOGFONT& lf, COLORREF color);
bool LoadFont(const TCHAR* szSection, LOGFONT& lf, COLORREF& color);

void CPjezo::House(CDC* m_dc, int x, int y, int hz)
{
    int col = 0xaFaF00;

    setline(m_dc, col, 2);
    setfillstyle(m_dc, 0, col);

    double et = 1.5;

    rectangle(m_dc, x - 4 * m_h, y, x + 4 * m_h, y - hz * h / (max_pih - min_pih));
    for (double hh = 0; hh < hz; hh += et * 2) {
        double hhh1 = y - hh * h / (max_pih - min_pih);
        double hhh2 = y - (hh + et) * h / (max_pih - min_pih);
        hhh2 = max(hhh2, y - hz * h / (max_pih - min_pih));
        fillrectangle(m_dc, x - 4 * m_h, hhh1, x + 4 * m_h, hhh2);
        //    line(x-4*m_h, hhh, x+4*m_h, hhh);
    }

    CString str;
    str.Format("%d", hz);
    double hhh2 = y - hz * h / (max_pih - min_pih);
    m_dc->TextOut(x, hhh2, str);

}

double CPjezo::getPih(int i, int typ, bool first, int nras)
{
    double pih;

    if (typ == 1 && first) {
        pih = m_vnode[i].rn[nras].pihP1 + m_vnode[i].rn[nras].geodz;
    }
    else if (typ == 1 && !first) {
        if (m_vnode[i].rn[nras].pihP2 == PIHNULL) return PIHNULL;
        pih = m_vnode[i].rn[nras].pihP2 + m_vnode[i].rn[nras].geodz;
    }
    else if (typ == 2 && first) {

        double a1 = m_vnode[i].rn[nras].pihO1;
        double a2 = m_vnode[i].rn[nras].geodz;

        pih = m_vnode[i].rn[nras].pihO1 + m_vnode[i].rn[nras].geodz;
    }
    else if (typ == 2 && !first) {
        if (m_vnode[i].rn[nras].pihO2 == PIHNULL) return PIHNULL;
        pih = m_vnode[i].rn[nras].pihO2 + m_vnode[i].rn[nras].geodz;
    }


    /// Старые значения
    /*
      else if (typ == 7 && first) {
        pih = m_vnode[i].rn[nras].pihP1_old+m_vnode[i].rn[nras].geodz;
      }
      else if (typ == 7 && !first) {
        if (m_vnode[i].rn[nras].pihP2_old == PIHNULL) return PIHNULL;
        pih = m_vnode[i].rn[nras].pihP2_old+m_vnode[i].rn[nras].geodz;
      }
      else if (typ == 8 && first) {
        double a1 = m_vnode[i].rn[nras].pihO1_old;
        double a2 = m_vnode[i].rn[nras].geodz;

        pih = m_vnode[i].rn[nras].pihO1_old+m_vnode[i].rn[nras].geodz;
      }
      else if (typ == 8 && !first) {
        if (m_vnode[i].rn[nras].pihO2_old == PIHNULL) return PIHNULL;
        pih = m_vnode[i].rn[nras].pihO2_old+m_vnode[i].rn[nras].geodz;
      }
    */


    else if (typ == 3)
        pih = m_vnode[i].rn[nras].geodz;
    else if (typ == 4)
        pih = m_vnode[i].rn[nras].h;
    else if (typ == 5)
        pih = m_vnode[i].rn[nras].pP_fact != 0 ? m_vnode[i].rn[nras].pP_fact + m_vnode[i].rn[nras].geodz : 0;
    else if (typ == 6)
        pih = m_vnode[i].rn[nras].pO_fact != 0 ? m_vnode[i].rn[nras].pO_fact + m_vnode[i].rn[nras].geodz : 0;

    /*
        else if (typ == 7)
            pih = m_vnode[i].rn[nras].pP_old != 0 ? m_vnode[i].rn[nras].pP_old + m_vnode[i].rn[nras].geodz : 0;
        else if (typ == 8)
            pih = m_vnode[i].rn[nras].pO_old != 0 ? m_vnode[i].rn[nras].pO_old + m_vnode[i].rn[nras].geodz : 0;
    */
    return pih;
}

void CPjezo::drawGr(CDC* m_dc, int color, int typ, int nras)
{
    int i;
    int len;
    int y;
    double pih, pih1, pih2;
    int xx1, yy1, xx2, yy2;
    CString str;
    CFont* oldfont;

    oldfont = (CFont*)m_dc->SelectObject(&fontH);
    //  oldfont = (CFont *) m_dc->SelectObject(&fontV);

    if (typ == 3) {
        int* xy, n = 0;
        xy = new int[(m_vnode.size() + 2) * 2];
        setline(m_dc, color, 1);

        CBrush brush1(HS_BDIAGONAL, 0x007F00), * oldbrush;
        oldbrush = (CBrush*)m_dc->SelectObject(&brush1);

        for (i = 0; i < m_vnode.size(); i++) {
            pih = m_vnode[i].rn[nras].h;
            len = m_vnode[i].rn[nras].len * w / m_len;
            y = (pih - min_pih) * h / (max_pih - min_pih);

            xy[n++] = x1 - geom.m_bx + len;
            xy[n++] = y2 - geom.m_by - y;
        }
        xy[n++] = x2;
        xy[n++] = y2;
        xy[n++] = x1;
        xy[n++] = y2;

        fillpoly(m_dc, n / 2, xy);

        m_dc->SelectObject(oldbrush);

        delete[] xy;
    }

    m_dc->SetTextAlign(TA_LEFT | TA_BOTTOM);

    if (typ == 5 || typ == 6 || typ == 7 || typ == 8) {
        int f = 0;
        for (i = 0; i < m_vnode.size(); i++) {
            //      setline(m_dc, color, 0, PS_DASH);
            setline(m_dc, color, 3);
            len = m_vnode[i].rn[nras].len * w / m_len;

            xx2 = x1 - geom.m_bx + len;
            pih1 = getPih(i, typ, true, nras);

            if (pih1 != 0) {
                y = (pih1 - min_pih) * h / (max_pih - min_pih);

                yy2 = y2 - geom.m_by - y;

                if (f != 0) line(m_dc, xx1, yy1, xx2, yy2);
                xx1 = xx2;
                yy1 = yy2;
                f = 1;
            }
        }
    }
    else {
        for (i = 0; i < m_vnode.size(); i++) {
            setline(m_dc, color, 3);

            CPLine* l1;
            if (i > 0) {
                if (typ == 1) {
                    l1 = &m_vlineP[i - 1];
                    if (l1->rl[nras].g == 0) setline(m_dc, GREEN, 3);
                }

                if (typ == 2) {
                    l1 = &m_vlineO[i - 1];
                    if (l1->rl[nras].g == 0) setline(m_dc, GREEN, 3);
                }
            }


            len = m_vnode[i].rn[nras].len * w / m_len;

            xx2 = x1 - geom.m_bx + len;

            ///double pih11 = getPih(i, 1, true, 0);
            ///double pih21 = getPih(i, 1, false, 0);
            ///double pih12 = getPih(i, 1, true, 1);
            ///double pih22 = getPih(i, 1, false, 1);



            pih1 = getPih(i, typ, true, nras);
            pih2 = getPih(i, typ, false, nras);

            if (pih2 != PIHNULL) {
                y = (pih1 - min_pih) * h / (max_pih - min_pih);

                yy2 = y2 - geom.m_by - y;

                if (i != 0) line(m_dc, xx1, yy1, xx2, yy2);
                xx1 = xx2;
                yy1 = yy2;

                y = (pih2 - min_pih) * h / (max_pih - min_pih);
                yy2 = y2 - geom.m_by - y;
                if (i != 0) line(m_dc, xx1, yy1, xx2, yy2);

                if (typ == 4 && m_vnode[i].rn[nras].hz != 0 && m_vysot) {
                    setline(m_dc, color, 1);
                    House(m_dc, xx2, yy2, m_vnode[i].rn[nras].hz);
                }
                xx1 = xx2;
                yy1 = yy2;
            }
        }
    }

    for (i = 0; i < m_vnode.size(); i++) {
        len = m_vnode[i].rn[nras].len * w / m_len;

        pih = getPih(i, typ, TRUE, nras);

        y = (pih - min_pih) * h / (max_pih - min_pih);

        xx2 = x1 - geom.m_bx + len + 3;
        yy2 = y2 - geom.m_by - y;

        if ((typ != 5 && typ != 6) || pih != 0) {
            setline(m_dc, color, 1);
            fillellipse(m_dc, xx2, yy2, 3 * m_h, 3 * m_h);
        }

        if (m_napor && (typ == 1 || typ == 2 || typ == 5 || typ == 6 || typ == 7 || typ == 8) || m_otmet && typ == 3) {
            str.Format("%.0f", typ == 3 || typ == 4 ? pih : pih - m_vnode[i].rn[nras].geodz);

            CSize sz;
            sz = m_dc->GetTextExtent(str);
            int cy = (i == 0) ? 0 : m_vlineP[i - 1].rl[nras].len * w / m_len;

            if (i == 0 || i == m_vnode.size() - 1 || sz.cy < cy)
                m_dc->TextOut(xx2, yy2, str);
        }
    }
    m_dc->SelectObject(oldfont);
}

void Form(CString& str, double v1, double v2, const char* b, const char* frm, const char* izm)
{
    CString f;

    if (v1 != v2) {
        f.Format("%%s=%s,%s %%s", frm, frm);
        str.Format(f, b, v1, v2, izm);
    }
    else {
        f.Format("%%s=%s %%s", frm);
        str.Format(f, b, v1, izm);
    }
}

void CPjezo::drawSetka(CDC* m_dc)
{
    CFont* oldfont;
    CSize sz;
    int cy;
    int i;
    int len = 0;
    CString str;

    oldfont = (CFont*)m_dc->SelectObject(&fontV);

    int l_podp = 60 * m_h;
    for (i = 0; i < m_vnode.size(); i++) {
        sz = m_dc->GetTextExtent(m_vnode[i].rn[0].n->getName());
        l_podp = max(l_podp, sz.cy);
    }

    y2 -= (l_podp + m_h);

    setcolor(m_dc, 0);
    rectangle(m_dc, x1 - geom.m_bx, y1 - geom.m_by, x2 - geom.m_bx, y2 - geom.m_by);

    w = (x2 - x1);
    h = (y2 - y1);

    if (m_stat) {
        setline(m_dc, 0x00007F, 2);
        int y = (h_max - min_pih + 5) * h / (max_pih - min_pih);
        y = y2 - geom.m_by - y;
        line(m_dc, x1 - geom.m_bx, y, x2 - geom.m_bx, y);
    }

    CPen pen1(PS_DOT, 1, (COLORREF)0), * oldpen;
    oldpen = (CPen*)m_dc->SelectObject(&pen1);

    for (i = 0; i < m_vnode.size() - 1; i++) {
        len = m_vnode[i].rn[0].len * w / m_len;
        line(m_dc, x1 - geom.m_bx + len, y1 - geom.m_by, x1 - geom.m_bx + len, y2 - geom.m_by);
    }

    oldfont = (CFont*)m_dc->SelectObject(&fontH);
    m_dc->SetTextAlign(TA_RIGHT | TA_BASELINE);


    bool ch;
    double pih;

    for (pih = min_pih, ch = true; pih < max_pih; pih += 10., ch = !ch) {
        int y = (pih - min_pih) * h / (max_pih - min_pih);
        y = y2 - geom.m_by - y;
        line(m_dc, x1 - geom.m_bx, y, x2 - geom.m_bx, y);

        if (ch) {
            str.Format("%.0f", pih);
            m_dc->TextOut(x1 - geom.m_bx - 5, y, str);
        }
    }

    m_dc->SelectObject(oldpen);

    m_dc->SetTextAlign(TA_CENTER | TA_TOP);


    CString s1, s2, s3, s4;
    s1.Format("D=8888,8888 т/ч");
    sz = m_dc->GetTextExtent(s1);

    for (i = 0; i < m_vnode.size(); i++) {
        CPLine* l1, * l2;
        l1 = &m_vlineP[i];
        l2 = &m_vlineO[i];

        if (m_vlineP[i].rl[0].len * w / m_len > sz.cx) {
            int x = (m_vnode[i].rn[0].len + m_vlineP[i].rl[0].len / 2) * w / m_len + x1 - geom.m_bx;

            if (l1->rl[0].diam < 0 || l2->rl[0].diam < 0) {
                l1 = l1;
            }

            int yy2 = y2 + sz.cy * 0.5;

            if (m_bIsInfo[0]) {
                Form(s1, l1->rl[0].diam, l2->rl[0].diam, "D", "%.0f", _TR("мм"));
                //        s1.Format("D=%.0f,%.0f мм", l1->rl[0].diam, l2->rl[0].diam);
                m_dc->TextOut(x, yy2, s1);
                yy2 += sz.cy;
            }

            if (m_bIsInfo[1]) {
                if (fabs(l1->rl[0].g) < 10. && fabs(l2->rl[0].g) < 10.) {
                    Form(s2, l1->rl[0].g, l2->rl[0].g, "G", "%.1f", _TR("т/ч"));
                }
                else {
                    Form(s2, l1->rl[0].g, l2->rl[0].g, "G", "%.0f", _TR("т/ч"));
                }
                //        s2.Format("G=%.0f,%.0f т/ч", l1->rl[0].g, l2->rl[0].g);
                m_dc->TextOut(x, yy2, s2);
                yy2 += sz.cy;
            }

            if (m_bIsInfo[2]) {
                Form(s3, l1->rl[0].len, l2->rl[0].len, "L", "%.0f", _TR("м"));
                //        s3.Format("L=%.0f,%.0f м", l1->rl[0].len, l2->rl[0].len);
                m_dc->TextOut(x, yy2, s3);
                yy2 += sz.cy;
            }

            if (m_bIsInfo[3]) {
                Form(s4, m_vnode[i].rn[0].len, m_vnode[i].rn[0].lenO, "R", "%.0f", _TR("м"));
                //        s4.Format("R=%.0f,%.0f м", m_vnode[i].rn[0].len, m_vnode[i].rn[0].lenO);
                m_dc->TextOut(x, yy2, s4);
                yy2 += sz.cy;
            }

            if (m_bIsInfo[4]) {
                Form(s4, l1->rl[0].a14, l2->rl[0].a14, "Pуд", "%.1f", _TR("мм/м"));
                //        s4.Format("Pуд.=%.1f,%.1f мм/м", l1->rl[0].a14, l2->rl[0].a14);
                m_dc->TextOut(x, yy2, s4);
                yy2 += sz.cy;
            }
        }
    }

    oldfont = (CFont*)m_dc->SelectObject(&fontV);

    m_dc->SetTextAlign(TA_RIGHT | TA_TOP);

    // Вывод названий узлов

    for (i = 0; i < m_vnode.size(); i++) {
        len = m_vnode[i].rn[0].len * w / m_len;
        if (i != 0) cy = m_vlineP[i - 1].rl[0].len * w / m_len;
        CNode2* n = m_vnode[i].rn[0].n;
        str = m_vnode[i].rn[0].n->getName();
        sz = m_dc->GetTextExtent(str);
        if (i == m_vnode.size() - 1 || i == 0 || (sz.cy < cy && !str.IsEmpty() && str[0] != '#'))
            m_dc->TextOut(x1 - geom.m_bx + len, y2 - geom.m_by + 5, str);
    }

    oldfont = (CFont*)m_dc->SelectObject(&fontH);
    m_dc->SetTextAlign(TA_LEFT);
    str.Format(_TR("Пьезометрический график, направление %s - %s"), m_vnode[0].rn[0].n->getName(), m_vnode[m_vnode.size() - 1].rn[0].n->getName());
    m_dc->TextOut(x1 - geom.m_bx + 40 * m_h, y1 - 20 * m_h, str);

    str.Format(_TR("Шкала полных напоров м.вод.ст"));
    oldfont = (CFont*)m_dc->SelectObject(&fontV);
    m_dc->SetTextAlign(TA_CENTER);
    m_dc->TextOut(x1 - 50 * m_h - geom.m_bx, (y1 + y2) / 2, str);

    m_dc->SelectObject(oldfont);
}

void CPjezo::drawOkno(CDC* m_dc)
{

    if (!okno.m_hWnd) return;

    CString str;
    CNode2* n1 = m_vnode[m_n].rn[0].n, * n2 = m_vnode[m_n + 1].rn[0].n;
    CPLine* l1, * l2;

    l1 = &m_vlineP[m_n];
    l2 = &m_vlineO[m_n];


    if (m_dbl) {
        str.Format(
            _TR("%s\r\n"
                "%s\r\n"
                "D=%.0f,%.0f мм\r\n"
                "G=%.0f,%.0f т/ч\r\n"
                "L=%.0f,%.0f м\r\n"
                "R=%.0f,%.0f м\r\n\r\n"
                "Hп=%.0f м.вод.ст.\r\n"
                "Hо=%.0f м.вод.ст.\r\n\r\n"
                "Hп=%.0f м.вод.ст.\r\n"
                "Hо=%.0f м.вод.ст.\r\n\r\n"
                "Отм.мес=%.0f м\r\n"
                "V=%.1f,%.1f м/c\r\n"
                "hуд.л=%.2f,%.2f м.вод.ст.\r\n"
            )
            ,

            n1->getName(),
            n2->getName(),
            l1->rl[0].diam, l2->rl[0].diam,
            l1->rl[0].g, l2->rl[0].g,
            l1->rl[0].len, l2->rl[0].len,
            m_vnode[m_n].rn[0].len, m_vnode[m_n].rn[0].lenO,
            m_vnode[m_n + 1].rn[0].pihP1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihP1,
            m_vnode[m_n + 1].rn[0].pihO1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihO1,

            m_vnode[m_n + 1].rn[1].pihP1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[1].pihP1,
            m_vnode[m_n + 1].rn[1].pihO1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[1].pihO1,

            m_vnode[m_n + 1].rn[0].h,
            l1->rl[0].w, l2->rl[0].w,
            l1->rl[0].a14, l2->rl[0].a14
        );
    }
    else {

        if (m_vnode[m_n].rn[0].pP_fact || m_vnode[m_n].rn[0].pO_fact) {

            str.Format(
                _TR("%s\r\n"
                    "%s\r\n"
                    "D=%.0f,%.0f мм\r\n"
                    "G=%.0f,%.0f т/ч\r\n"
                    "L=%.0f,%.0f м\r\n"
                    "R=%.0f,%.0f м\r\n\r\n"
                    "Hп=%.0f м.вод.ст.\r\n"
                    "Hо=%.0f м.вод.ст.\r\n\r\n"
                    "Hп факт=%.0f м.вод.ст.\r\n"
                    "Hо факт=%.0f м.вод.ст.\r\n\r\n"
                    "Отм.мес=%.0f м\r\n"
                    "V=%.1f,%.1f м/c\r\n"
                    "hуд.л=%.2f,%.2f м.вод.ст.\r\n"
                )
                ,

                n1->getName(),
                n2->getName(),
                l1->rl[0].diam, l2->rl[0].diam,
                l1->rl[0].g, l2->rl[0].g,
                l1->rl[0].len, l2->rl[0].len,
                m_vnode[m_n].rn[0].len, m_vnode[m_n].rn[0].lenO,
                m_vnode[m_n + 1].rn[0].pihP1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihP1,
                m_vnode[m_n + 1].rn[0].pihO1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihO1,
                m_vnode[m_n + 1].rn[0].pP_fact,
                m_vnode[m_n + 1].rn[0].pO_fact,
                m_vnode[m_n + 1].rn[0].h,
//                "V=%.0f,%.0f м/c\r\n"
//                "hуд.л=%.2f,%.2f м.вод.ст.\r\n"
                l1->rl[0].w, l2->rl[0].w,
                l1->rl[0].a14, l2->rl[0].a14
            );
        }
        else {

            str.Format(_TR(
                "%s\r\n"
                "%s\r\n"
                "D=%.0f,%.0f мм\r\n"
                "G=%.0f,%.0f т/ч\r\n"
                "L=%.0f,%.0f м\r\n"
                "R=%.0f,%.0f м\r\n\r\n"
                "Hп=%.0f м.вод.ст.\r\n"
                "Hо=%.0f м.вод.ст.\r\n\r\n"
                "Отм.мес=%.0f м\r\n"
                "V=%.1f,%.1f м/c\r\n"
                "hуд.л=%.2f,%.2f м.вод.ст.\r\n"
            )
                ,

                n1->getName(),
                n2->getName(),
                l1->rl[0].diam, l2->rl[0].diam,
                l1->rl[0].g, l2->rl[0].g,
                l1->rl[0].len, l2->rl[0].len,
                m_vnode[m_n + 1].rn[0].len, m_vnode[m_n + 1].rn[0].lenO,
                m_vnode[m_n + 1].rn[0].pihP1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihP1,
                m_vnode[m_n + 1].rn[0].pihO1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihO1,
                m_vnode[m_n + 1].rn[0].h,
                l1->rl[0].w, l2->rl[0].w,
                l1->rl[0].a14, l2->rl[0].a14
            );
        }
    }


    if (l1->rl[0].l) {
        okno.SetWindowText(linef[l1->rl[0].l->line.typ].opis);
        okno.SetText(str);
    }
    else if (l2->rl[0].l) {
        okno.SetWindowText(linef[l2->rl[0].l->line.typ].opis);
        okno.SetText(str);
    }
}

void CPjezo::redrawPaint(CDC* m_dc)
{
    int mas = m_mas;
    double bx = geom.m_bx, by = geom.m_by;
    CSize sz;

    nkx = 60 * m_h;
    nky = 30 * m_h;

    CRect rect = m_rect;
    //  GetClientRect(&rect);

    width = (rect.right - rect.left) * mas;
    height = (rect.bottom - rect.top);

    m_pt1 = CFPoint(0, 0);
    m_pt2 = ScreenToCoord(CPoint(width, height));

    oldfont = (CFont*)m_dc->SelectObject(&fontV);
    sz = m_dc->GetTextExtent("0");

    x1 = nkx;  y1 = nky;  x2 = width - 20 * m_h;  y2 = height - 20 * m_h;

    int n_podp = 0;
    for (int i = 0; i < 5; i++) {
        n_podp += m_bIsInfo[i] ? 1 : 0;
    }

    y2 = height - max(20 * m_h, n_podp * sz.cy);

    CString str;

    LOGFONT lf;
    COLORREF color = 0;

    fontH.CreatePointFont(100, _T("Arial"));

    fontH.GetLogFont(&lf);
    fontH.DeleteObject();

    LoadFont(_T("Font Pjezo"), lf, color);

    lf.lfHeight *= m_h;
    lf.lfCharSet = DEFAULT_CHARSET;

    fontH.CreateFontIndirect(&lf);

    lf.lfEscapement = 900;
    lf.lfOrientation = 900;

    fontV.CreateFontIndirect(&lf);

    drawSetka(m_dc);

    drawGr(m_dc, BLACK, 3, 0);

    drawGr(m_dc, GREEN, 4, 0);
    if (m_podacha) drawGr(m_dc, RED, 1, 0);
    if (m_obratka) drawGr(m_dc, BLUE, 2, 0);

    if (m_real) {
        drawGr(m_dc, YELLOW, 5, 0);
        drawGr(m_dc, CYAN, 6, 0);
    }


    if (m_dbl) {
        //        if (m_podacha) drawGr(m_dc, YELLOW, 7, 1);
        //        if (m_obratka) drawGr(m_dc, CYAN, 8, 1);
        if (m_podacha) drawGr(m_dc, YELLOW, 1, 1);
        if (m_obratka) drawGr(m_dc, CYAN, 2, 1);
    }


    x1 = nkx;  y1 = nky;  x2 = width - 20 * m_h;  y2 = height - 20 * m_h;

    y2 = height;
    y2 = height - max(20 * m_h, n_podp * sz.cy);


    drawSetka(m_dc);

    setline(m_dc, YELLOW, 3);
    //  setline(m_dc, RED, 3);
    int xb, xe;
    xb = x1 + m_vnode[m_n].rn[0].len * w / m_len;
    xe = xb + m_vlineP[m_n].rl[0].len * w / m_len;

    line(m_dc, xb - geom.m_bx, y2 - geom.m_by, xe - geom.m_bx, y2 - geom.m_by);

    if (m_vlineP[m_n].rl[0].len < 1) {
        fillellipse(m_dc, xb, y2, 2 * m_h, 2 * m_h);
    }

    m_dc->SelectObject(oldfont);

    fontH.DeleteObject();
    fontV.DeleteObject();

    drawOkno(m_dc);
}

void CPjezo::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    if (!pDC->IsPrinting()) {
        SetDC(pDC);
        //    m_rect = ((CPaintDC*)pDC)->m_ps.rcPaint;
        GetClientRect(&m_rect);

        if (m_isSavedImage) {
            pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_pDibDC, m_rect.left, m_rect.top, SRCCOPY);
        }
        else {
            SetDC(pDC);
            redrawPaint(pDC);
            DrawLine(pDC);
        }
    }
}

int naprP(CGraph2* m_graph, const CNode2* n1, const CNode2* n2)
{
    CNode2* nn1 = m_graph->findPO(n1->id, CPOD);
    CNode2* nn2 = m_graph->findPO(n2->id, CPOD);

    CLINE2* LP = m_graph->find_line(nn1, nn2, CPOD);

    if (LP) return bline(LP)->line.pod.q >= 0 && IsBegin(LP) || bline(LP)->line.pod.q < 0 && !IsBegin(LP);

    return 1;
}

int naprO(CGraph2* m_graph, const CNode2* n1, const CNode2* n2)
{
    CNode2* nn1 = m_graph->findPO(n1->id, COBR);
    CNode2* nn2 = m_graph->findPO(n2->id, COBR);

    CLINE2* LO = m_graph->find_line(nn1, nn2, COBR);

    if (LO) return bline(LO)->line.obr.q >= 0 && IsBegin(LO) || bline(LO)->line.obr.q < 0 && !IsBegin(LO);

    return 0;
}

void CPjezo::OnInitialUpdate()
{
    CScroll::OnInitialUpdate();

    for (int i = 0; i < sizeof(m_bIsInfo) / sizeof(m_bIsInfo[0]); i++) {
        CString sss;
        sss.Format("IsInfoP%d", i);
        m_bIsInfo[i] = AfxGetApp()->GetProfileInt(szSection, sss, 1);
    }

    GetParentFrame()->SetWindowText(_TR("Пьезометр"));

    initPjezo();
    //  CRect rect(100, 100, 240, 360);
    CRect rect(100, 100, 280, 360);
    okno.Create(rect, this, 1999);
    okno.SetWindowText("");
}

void CPjezo::readUS_OUT(CGraph2* m_graph, int num, int nras)
{
    int k;
    CNode2* n;

    for (k = 0; k < m_vnode.size(); k++) {

        //    LO = m_graph->find_line(m_graph->findPO(*n1, 'О'), m_graph->findPO(*n2, 'О'), COBR);

        n = m_graph->findPO(m_vnode[k].rn[nras].n->id, CPOD);

        CNode2* n1 = NULL, * n2 = NULL;
        double pihP1 = PIHNULL, pihP2 = PIHNULL, pihO1 = PIHNULL, pihO2 = PIHNULL;

        n = m_graph->findPO(m_vnode[k].rn[nras].n->id, CPOD);

        if (n && n->node.externalSignID != SignNode_o) {
            //            if (n->node.externalSignID != SignNode_o) {
            pihP1 = n->node.data.pihP;
            pihP2 = n->node.data.pihP;
            //            }
            m_vnode[k].rn[nras].tP = n->node.data.tempP;

            if (n->node.isP && (n->node.typ != TIP_PR && n->node.typ != TIP_PO)) {
                if (k > 0) n1 = m_graph->findPO(m_vnode[k - 1].rn[nras].n->id, CPOD);
                if (k < m_vnode.size() - 1) n2 = m_graph->findPO(m_vnode[k + 1].rn[nras].n->id, COBR);

                CMapGraph::const_iterator pp = m_graph->map_node.begin();
                while (pp != m_graph->map_node.end()) {
                    CNode2* p = pp->second;
                    if (p->node.internalNodeID == n->id && p->node.typ == TIP_US2) {
                        if (n1 && p->node.connectID == n1->id && p->node.data.pihP != 0) {
                            pihP1 = p->node.data.pihP;
                        }
                        if (n2 && p->node.connectID == n2->id && p->node.data.pihP != 0) {
                            pihP2 = p->node.data.pihP;
                        }
                    }
                    pp++;
                }
            }
        }

        n = m_graph->findPO(m_vnode[k].rn[nras].n->id, COBR);

        if (n && n->node.externalSignID != SignNode_p) {
            pihO1 = n->node.data.pihO;
            pihO2 = n->node.data.pihO;
            m_vnode[k].rn[nras].tO = n->node.data.tempO;

            if (n->node.isP && (n->node.typ != TIP_PR && n->node.typ != TIP_PO)) {
                if (k > 0) n1 = m_graph->findPO(m_vnode[k - 1].rn[nras].n->id, COBR);
                if (k < m_vnode.size() - 1) n2 = m_graph->findPO(m_vnode[k + 1].rn[nras].n->id, COBR);

                CMapGraph::const_iterator pp = m_graph->map_node.begin();
                while (pp != m_graph->map_node.end()) {
                    CNode2* p = pp->second;
                    if (p->node.internalNodeID == n->id && p->node.typ == TIP_US2) {
                        if (n1 && p->node.connectID == n1->id && p->node.data.pihO != 0) {
                            pihO1 = p->node.data.pihO;
                        }
                        if (n2 && p->node.connectID == n2->id && p->node.data.pihO != 0) {
                            pihO2 = p->node.data.pihO;
                        }
                    }
                    pp++;
                }
            }
        }

        if (num == 1) {
            m_vnode[k].rn[nras].pihP1 = pihP1;
            m_vnode[k].rn[nras].pihP2 = pihP2;
            m_vnode[k].rn[nras].pihO1 = pihO1;
            m_vnode[k].rn[nras].pihO2 = pihO2;
        }
        else {
            m_vnode[k].rn[nras].pihP1_old = pihP1;
            m_vnode[k].rn[nras].pihP2_old = pihP2;
            m_vnode[k].rn[nras].pihO1_old = pihO1;
            m_vnode[k].rn[nras].pihO2_old = pihO2;
        }

        m_vnode[k].rn[nras].pP_fact = n ? n->node.pP_fact : 0;
        m_vnode[k].rn[nras].pO_fact = n ? n->node.pO_fact : 0;

        m_vnode[k].rn[nras].pP_old = n ? n->node.pP_old : 0;
        m_vnode[k].rn[nras].pO_old = n ? n->node.pO_old : 0;

        //       if (nras > 0) return;

        double h = m_vnode[k].rn[nras].h;
        double geodz = m_vnode[k].rn[nras].geodz;

        double h1 = min(h, geodz);
        double h2 = max(h, geodz);

        if (pihP1 != PIHNULL) min_pih = min(h1, min_pih);
        if (pihP1 != PIHNULL) max_pih = max(h2 + pihP1, max_pih);
        if (pihO1 != PIHNULL) min_pih = min(h1, min_pih);
        if (pihO1 != PIHNULL) max_pih = max(h2 + pihO1, max_pih);

        //    min_pih = min(.geodz, min_pih);
        //    max_pih = max(geodz + m_vnode[k].h, max_pih);
    }
}

void CPjezo::initPjezo()
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    //  m_cxema = pDoc->m_cxema;
    m_cxema = pDoc->m_cxema;
    m_graph = m_cxema->m_graph;

    {
        h_max = -1e10;
        CMapGraph::iterator p;
        p = m_graph->map_node.begin();

        while (p != m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.geoMarkTopTube + n->node.hz > h_max) {
                h_max = n->node.geoMarkTopTube + n->node.hz;
            }
            p++;
        }
    }


    deque<long>::const_iterator it = m_graph->list_pjezo.begin();

    list_pjezo.clear();

    for (; it != m_graph->list_pjezo.end(); it++) {
        CNode2* node = m_graph->find(*it);
        if (node) {
            list_pjezo.push_back(*node);

            m_calculationID = node->node.calculationID;
            m_fileID = node->node.fileID;
        }
    }




    m_len = 0.;
    double m_len2 = 0.;
    double lenO = 0., vO = 0.;
    double lenP = 0., vP = 0.;
    double lenO2 = 0., vO2 = 0.;
    double lenP2 = 0., vP2 = 0.;
    CString str, ss;
    CNode2* n1, * n2;
    CNode2* n1_2 = nullptr, * n2_2 = nullptr;
    CLINE2* LO, * LP;
    CLine2* lP, * lO;

    CLINE2* LO2 = nullptr, * LP2 = nullptr;

    m_vlineP.resize(list_pjezo.size());
    m_vlineO.resize(list_pjezo.size());
    m_vnode.resize(list_pjezo.size());

    deque<CNode2>::iterator p = list_pjezo.begin();

    n1 = m_graph->find(p->id);

    if (n1) {
        m_vnode[0].rn[0].n = n1;
    }
    m_vnode[0].rn[0].len = 0.;
    m_vnode[0].rn[0].lenO = 0.;
    m_vnode[0].rn[0].vO = 0.;
    m_vnode[0].rn[0].lenP = 0.;
    m_vnode[0].rn[0].vP = 0.;


    if (m_dbl) {
        n1_2 = m_graph2->find(p->id);

        if (n1_2) {
            m_vnode[0].rn[1].n = n1_2;
        }
        m_vnode[0].rn[1].len = 0.;
        m_vnode[0].rn[1].lenO = 0.;
        m_vnode[0].rn[1].vO = 0.;
        m_vnode[0].rn[1].lenP = 0.;
        m_vnode[0].rn[1].vP = 0.;
    }


    p++;

    int k = 0;

    while (p != list_pjezo.end()) {
        n2 = m_graph->find(p->id);
        if (m_dbl) {
            n2_2 = m_graph2->find(p->id);
        }

        //    L = m_graph->Link(n1, n2);
        LP = m_graph->find_line(m_graph->findPO(n1->id, 'П'), m_graph->findPO(n2->id, 'П'), CPOD);
        LO = m_graph->find_line(m_graph->findPO(n1->id, 'О'), m_graph->findPO(n2->id, 'О'), COBR);

        if (LP || LO) {
            lP = bline(LP);
            lO = bline(LO);

            m_vlineP[k].rl[0].len = 0;
            m_vlineP[k].rl[0].diam = 0;
            m_vlineP[k].rl[0].g = 0;
            m_vlineP[k].rl[0].w = 0;
            m_vlineP[k].rl[0].a15 = 0;
            m_vlineP[k].rl[0].a16 = 0;
            m_vlineP[k].rl[0].a17 = 0;

            m_vlineO[k].rl[0].len = 0;
            m_vlineO[k].rl[0].diam = 0;
            m_vlineO[k].rl[0].g = 0;
            m_vlineO[k].rl[0].w = 0;
            m_vlineO[k].rl[0].a15 = 0;
            m_vlineO[k].rl[0].a16 = 0;
            m_vlineO[k].rl[0].a17 = 0;


            if (lP) {
                m_vlineP[k].rl[0].len = lP->line.pod.dl;
                m_vlineP[k].rl[0].diam = lP->line.pod.diam;
                m_vlineP[k].rl[0].g = fabs(lP->line.pod.q);

                m_vlineP[k].rl[0].w = fabs(lP->line.pod.w);
                m_vlineP[k].rl[0].a14 = fabs(lP->line.pod.poter);
                m_vlineP[k].rl[0].a15 = fabs(lP->line.pod.a15);
                m_vlineP[k].rl[0].a16 = fabs(lP->line.pod.a16);
                m_vlineP[k].rl[0].a17 = fabs(lP->line.pod.a17);

                m_len += lP->line.pod.dl;
                vP += lP->line.pod.v;
                lenP += lP->line.pod.dl;
            }
            m_vlineP[k].rl[0].l = lP;

            if (lO) {
                m_vlineO[k].rl[0].len = lO->line.obr.dl;
                m_vlineO[k].rl[0].diam = lO->line.obr.diam;
                m_vlineO[k].rl[0].g = fabs(lO->line.obr.q);

                m_vlineO[k].rl[0].w = fabs(lO->line.obr.w);
                m_vlineO[k].rl[0].a14 = fabs(lO->line.obr.poter);
                m_vlineO[k].rl[0].a15 = fabs(lO->line.obr.a15);
                m_vlineO[k].rl[0].a16 = fabs(lO->line.obr.a16);
                m_vlineO[k].rl[0].a17 = lO->line.obr.a17;


                lenO += lO->line.obr.dl;
                vO += lO->line.obr.v;
            }
            m_vlineO[k].rl[0].l = lO;


            //// Добавил 2009.10.07
            if (lP && !lO) {
                m_vlineO[k].rl[0].len = lP->line.pod.dl;
                m_vlineO[k].rl[0].diam = lP->line.pod.diam;
                m_vlineO[k].rl[0].g = fabs(lP->line.pod.q);

                m_vlineO[k].rl[0].w = fabs(lP->line.pod.w);
                m_vlineO[k].rl[0].a14 = fabs(lP->line.pod.poter);
                m_vlineO[k].rl[0].a15 = fabs(lP->line.pod.a15);
                m_vlineO[k].rl[0].a16 = fabs(lP->line.pod.a16);
                m_vlineO[k].rl[0].a17 = lP->line.pod.a17;


                lenO += lP->line.pod.dl;
                vO += lP->line.pod.v;
            }

            if (lO && !lP) {
                m_vlineP[k].rl[0].len = lO->line.obr.dl;
                m_vlineP[k].rl[0].diam = lO->line.obr.diam;
                m_vlineP[k].rl[0].g = fabs(lO->line.obr.q);

                m_vlineP[k].rl[0].w = fabs(lO->line.obr.w);
                m_vlineP[k].rl[0].a14 = fabs(lO->line.obr.poter);
                m_vlineP[k].rl[0].a15 = fabs(lO->line.obr.a15);
                m_vlineP[k].rl[0].a16 = fabs(lO->line.obr.a16);
                m_vlineP[k].rl[0].a17 = lO->line.obr.a17;

                m_len += lO->line.obr.dl;
                vP += lO->line.obr.v;
                lenP += lO->line.obr.dl;
            }
            ///////////////////////////////////////

            m_vnode[k + 1].rn[0].pihP1 = PIHNULL;
            m_vnode[k + 1].rn[0].pihP2 = PIHNULL;

            m_vnode[k + 1].rn[0].len = m_len;
            m_vnode[k + 1].rn[0].lenP = lenP;
            m_vnode[k + 1].rn[0].lenO = lenO;
            m_vnode[k + 1].rn[0].vP = vP;
            m_vnode[k + 1].rn[0].vO = vO;
        }

        if (m_dbl) {
            if (m_graph2) {
                LP = m_graph2->find_line(m_graph2->findPO(n1_2->id, 'П'), m_graph2->findPO(n2_2->id, 'П'), CPOD);
                LO = m_graph2->find_line(m_graph2->findPO(n1_2->id, 'О'), m_graph2->findPO(n2_2->id, 'О'), COBR);
            }

            if (LP || LO) {
                lP = bline(LP);
                lO = bline(LO);

                m_vlineP[k].rl[1].len = 0;
                m_vlineP[k].rl[1].diam = 0;
                m_vlineP[k].rl[1].g = 0;
                m_vlineP[k].rl[1].w = 0;
                m_vlineP[k].rl[1].a15 = 0;
                m_vlineP[k].rl[1].a16 = 0;
                m_vlineP[k].rl[1].a17 = 0;

                m_vlineO[k].rl[1].len = 0;
                m_vlineO[k].rl[1].diam = 0;
                m_vlineO[k].rl[1].g = 0;
                m_vlineO[k].rl[1].w = 0;
                m_vlineO[k].rl[1].a15 = 0;
                m_vlineO[k].rl[1].a16 = 0;
                m_vlineO[k].rl[1].a17 = 0;


                if (lP) {
                    m_vlineP[k].rl[1].len = lP->line.pod.dl;
                    m_vlineP[k].rl[1].diam = lP->line.pod.diam;
                    m_vlineP[k].rl[1].g = fabs(lP->line.pod.q);

                    m_vlineP[k].rl[1].w = fabs(lP->line.pod.w);
                    m_vlineP[k].rl[1].a14 = fabs(lP->line.pod.poter);
                    m_vlineP[k].rl[1].a15 = fabs(lP->line.pod.a15);
                    m_vlineP[k].rl[1].a16 = fabs(lP->line.pod.a16);
                    m_vlineP[k].rl[1].a17 = fabs(lP->line.pod.a17);

                    m_len2 += lP->line.pod.dl;
                    vP2 += lP->line.pod.v;
                    lenP2 += lP->line.pod.dl;
                }
                m_vlineP[k].rl[1].l = lP;

                if (lO) {
                    m_vlineO[k].rl[1].len = lO->line.obr.dl;
                    m_vlineO[k].rl[1].diam = lO->line.obr.diam;
                    m_vlineO[k].rl[1].g = fabs(lO->line.obr.q);

                    m_vlineO[k].rl[1].w = fabs(lO->line.obr.w);
                    m_vlineO[k].rl[1].a14 = fabs(lO->line.obr.poter);
                    m_vlineO[k].rl[1].a15 = fabs(lO->line.obr.a15);
                    m_vlineO[k].rl[1].a16 = fabs(lO->line.obr.a16);
                    m_vlineO[k].rl[1].a17 = lO->line.obr.a17;


                    lenO2 += lO->line.obr.dl;
                    vO2 += lO->line.obr.v;
                }
                m_vlineO[k].rl[1].l = lO;


                //// Добавил 2009.10.07
                if (lP && !lO) {
                    m_vlineO[k].rl[1].len = lP->line.pod.dl;
                    m_vlineO[k].rl[1].diam = lP->line.pod.diam;
                    m_vlineO[k].rl[1].g = fabs(lP->line.pod.q);

                    m_vlineO[k].rl[1].w = fabs(lP->line.pod.w);
                    m_vlineO[k].rl[1].a14 = fabs(lP->line.pod.poter);
                    m_vlineO[k].rl[1].a15 = fabs(lP->line.pod.a15);
                    m_vlineO[k].rl[1].a16 = fabs(lP->line.pod.a16);
                    m_vlineO[k].rl[1].a17 = lP->line.pod.a17;


                    lenO2 += lP->line.pod.dl;
                    vO2 += lP->line.pod.v;
                }

                if (lO && !lP) {
                    m_vlineP[k].rl[1].len = lO->line.obr.dl;
                    m_vlineP[k].rl[1].diam = lO->line.obr.diam;
                    m_vlineP[k].rl[1].g = fabs(lO->line.obr.q);

                    m_vlineP[k].rl[1].w = fabs(lO->line.obr.w);
                    m_vlineP[k].rl[1].a14 = fabs(lO->line.obr.poter);
                    m_vlineP[k].rl[1].a15 = fabs(lO->line.obr.a15);
                    m_vlineP[k].rl[1].a16 = fabs(lO->line.obr.a16);
                    m_vlineP[k].rl[1].a17 = lO->line.obr.a17;

                    m_len2 += lO->line.obr.dl;
                    vP2 += lO->line.obr.v;
                    lenP2 += lO->line.obr.dl;
                }
                ///////////////////////////////////////

                m_vnode[k + 1].rn[1].pihP1 = PIHNULL;
                m_vnode[k + 1].rn[1].pihP2 = PIHNULL;

                m_vnode[k + 1].rn[1].len = m_len2;
                m_vnode[k + 1].rn[1].lenP = lenP2;
                m_vnode[k + 1].rn[1].lenO = lenO2;
                m_vnode[k + 1].rn[1].vP = vP2;
                m_vnode[k + 1].rn[1].vO = vO2;
            }
        }

        if (LP || LO) {
            k++;
        }

        n1 = m_graph->find(p->id);

        if (m_dbl) {
            n1_2 = m_graph2->find(p->id);
        }


        p++;
    }


    k = 0;
    p = list_pjezo.begin();

    while (p != list_pjezo.end()) {
        CNode2* node = &*p;

        n2 = m_graph->find(p->id);

        m_vnode[k].rn[0].n = node;

        m_vnode[k].rn[0].geodz = node->node.geoMarkTopTube;
        m_vnode[k].rn[0].hz = node->node.hz;  // Высота зданий

        m_vnode[k].rn[0].pP_fact = node->node.pP_fact;
        m_vnode[k].rn[0].pO_fact = node->node.pO_fact;

        m_vnode[k].rn[0].h = node->node.geoMarkNodeArea; // Высота земли

        if (node->node.geoMarkNodeArea == 0) {
            m_vnode[k].rn[0].h = node->node.geoMarkTopTube; // Высота земли
        }
        if (m_dbl) {
            n2 = m_graph2->find(p->id);

            m_vnode[k].rn[1].n = node;

            m_vnode[k].rn[1].geodz = node->node.geoMarkTopTube;
            m_vnode[k].rn[1].hz = node->node.hz;  // Высота зданий

            m_vnode[k].rn[1].h = node->node.geoMarkNodeArea; // Высота земли

            if (node->node.geoMarkNodeArea == 0) {
                m_vnode[k].rn[1].h = node->node.geoMarkTopTube; // Высота земли
            }
        }

        k++;
        p++;
    }

    m_vnode.resize(k);

    min_pih = 1000000;
    max_pih = 0;


    readUS_OUT(m_graph, 1, 0);
    if (m_dbl) {
        readUS_OUT(m_graph2, 1, 1);
    }
    //  readUS_OUT("US_OUT2", 2);

    max_pih = floor(max_pih / 10.) * 10. + 10.;
    min_pih = floor(min_pih / 10.) * 10.;


    if (h_max > max_pih) {
        AfxMessageBox("Проверьте высоту зданий на теповой сети.\nГидростатический напор расположен выше напора подающей линии пьезометра");
    }
}

void CPjezo::OnPlus()
{
    m_mas = min(10, m_mas + 1);
    Invalidate();
}

void CPjezo::OnMinus()
{
    m_mas = max(1, m_mas - 1);
    Invalidate();
}

void CPjezo::OnMasall()
{
    m_mas = 1;
    geom.m_bx = geom.m_by = 0;
    Invalidate();

}

void CPjezo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    const int Dx = 160, Dy = 160;

    CRect rect;
    GetClientRect(&rect);

    switch (nChar) {
    case VK_LEFT:
        m_n = max(0, m_n - 1);
        geom.m_bx = min(geom.m_bx, m_vnode[m_n].rn[0].len * w / m_len);
        Invalidate();
        break;
    case VK_RIGHT:
        m_n = min(m_vnode.size() - 2, m_n + 1);
        geom.m_bx = max(geom.m_bx, (m_vnode[m_n + 1].rn[0].len) * w / m_len - rect.Width() + 20 + nkx);
        Invalidate();
        break;
    case VK_HOME:
        m_n = 0;
        geom.m_bx = 0;
        Invalidate();
        break;
    case VK_END:
        m_n = m_vnode.size() - 2, m_n + 1;
        geom.m_bx = max(geom.m_bx, (m_vnode[m_n + 1].rn[0].len) * w / m_len - rect.Width() + 20 + nkx);
        Invalidate();
        break;
    }

    //  CScroll::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPjezo::OnUpdateNapor(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_napor);
}


void CPjezo::OnNapor()
{
    m_napor = !m_napor;
    Invalidate();
}

void CPjezo::OnOtmet()
{
    m_otmet = !m_otmet;
    Invalidate();
}

void CPjezo::OnUpdateOtmet(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_otmet);
}

void CPjezo::OnOkno()
{
    m_okno = !m_okno;

    okno.ShowWindow(m_okno ? SW_SHOW : SW_HIDE);
    Invalidate();
}

void CPjezo::OnUpdateOkno(CCmdUI* pCmdUI)
{
    m_okno = okno.IsWindowVisible();

    pCmdUI->SetCheck(m_okno);
}

void CPjezo::OnVysot()
{
    m_vysot = !m_vysot;
    Invalidate();
}

void CPjezo::OnUpdateVysot(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_vysot);
}


bool GetPaperSize(DEVMODE* devmode, int& lx, int& ly);


void CPjezo::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
    int PrevMode;

    CSize OldVExt, OldWExt;

    m_rect = pInfo->m_rectDraw;

    PrevMode = pDC->SetMapMode(MM_ANISOTROPIC);


    m_rect.bottom = MulDiv(m_rect.bottom,
        pDC->GetDeviceCaps(LOGPIXELSX),
        pDC->GetDeviceCaps(LOGPIXELSY));


    OldVExt = pDC->SetViewportExt(
        pDC->GetDeviceCaps(HORZRES),
        pDC->GetDeviceCaps(VERTRES)

    );

    OldWExt = pDC->SetWindowExt(
        m_rect.Width(), m_rect.Height()
    );


    pDC->SetWindowOrg(0, 0);

    SetDC(pDC);

    double bx = geom.m_bx;

    geom.m_bx = (pInfo->m_nCurPage - 1) * m_rect.Width();

    redrawPaint(pDC);

    geom.m_bx = bx;

    pDC->SetViewportExt(OldVExt);
    pDC->SetWindowExt(OldWExt);

    pDC->SetMapMode(PrevMode);
}

BOOL CPjezo::OnPreparePrinting(CPrintInfo* pInfo)
{
    pInfo->SetMaxPage(m_mas);

    return DoPreparePrinting(pInfo);
}

void CPjezo::CopyBlockP(CFRect rect)
{

    //  CRect r;
    //  GetClientRect(&r);
    //  CFRect rect = ScreenToCoord(r);
    //  CScroll::CopyBlock(rect);
    //  return;
    //  rect.NormalizeRect();


    int dx1, dy1;

    rect.NormalizeRect();

    CRect r = CoordToScreen(rect);


    OpenClipboard();

    if (::EmptyClipboard()) {
        CWaitCursor cur;

        CClientDC screenDC(NULL);
        double dm = screenDC.GetDeviceCaps(LOGPIXELSX) / 2.54;

        CClientDC odc(this);

        dx1 = 1400. * rect.Width() / dm;
        dy1 = 1400. * rect.Height() / dm;

        CMetaFileDC pMetaDC;
        pMetaDC.CreateEnhanced(&odc, NULL, CRect(0, 0, dx1, dy1), _T("whatever"));

        pMetaDC.m_hAttribDC = odc.m_hDC;
        SetDC(&pMetaDC);

        pMetaDC.SetMapMode(MM_TEXT);

        SetRect(r);
        redrawPaint(&pMetaDC);

        HENHMETAFILE hMF = pMetaDC.CloseEnhanced();
        ::SetClipboardData(CF_ENHMETAFILE, hMF);
    }

    CloseClipboard();
}

void CPjezo::OnEditCopy()
{

    CRect r;
    GetClientRect(&r);
    CFRect rect = ScreenToCoord(r);
    //  CScroll::CopyBlock(rect);
    CopyBlockP(rect);
    return;
    //  rect.NormalizeRect();


    int dx1, dy1;

    rect.NormalizeRect();

    OpenClipboard();

    if (::EmptyClipboard()) {
        CWaitCursor cur;

        CClientDC screenDC(NULL);
        double dm = screenDC.GetDeviceCaps(LOGPIXELSX) / 2.54;

        CClientDC odc(this);

        dx1 = 1400. * rect.Width() / dm;
        dy1 = 1400. * rect.Height() / dm;

        CMetaFileDC pMetaDC;
        pMetaDC.CreateEnhanced(&odc, NULL, CRect(0, 0, dx1, dy1), _T("whatever"));

        pMetaDC.m_hAttribDC = odc.m_hDC;
        SetDC(&pMetaDC);

        pMetaDC.SetMapMode(MM_TEXT);

        SetRect(r);
        redrawPaint(&pMetaDC);

        HENHMETAFILE hMF = pMetaDC.CloseEnhanced();
        ::SetClipboardData(CF_ENHMETAFILE, hMF);
    }

    CloseClipboard();

}

void CPjezo::OnUpdateStat(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_stat);
}

void CPjezo::OnStat()
{
    m_stat = !m_stat;
    Invalidate();
}

int getOutID(CAdoFile* ado, int fileID);

void CPjezo::OnTechinfo()
{
    CGidrView* pView = getView();

    int i;

    CString s, sql;
    CString par = "";

    CString tn = "##pjezo";


    sql.Format("CREATE TABLE %s (id int, id2 int,"
        "kod1 nvarchar(20), uzel1 nvarchar(70), pr1 nvarchar(20),"
        "kod2 nvarchar(20), uzel2 nvarchar(70), pr2 nvarchar(20),"
        "q float, "
        "pih1 float, "
        "pih2 float, "
        "a14 float, "
        "a15 float, "
        "a16 float, "
        "a17 float, "

        "w float, "
        "dl float, "
        "diam float, "
        "dl2 float, "
        "v2 float "
        ")", tn);

    long affected;

    CAdoFile* ado = m_cxema->m_ado;

    bool r1 = ado->Execute(sql, &affected);

    sql.Format("DELETE FROM %s", tn);
    bool r2 = ado->Execute(sql, &affected);

    long num = 1;

    double dlP = 0, dlO = 0, vP = 0, vO = 0;


    //    CAdoInsert ins(ado, tn, "id");

    for (i = 0; i < m_vnode.size() - 1; i++) {
        ado->AddNew(tn);


        double dh = m_vnode[i].rn[0].h - m_vnode[i + 1].rn[0].h;

        if (bline(m_vlineP[i].rl[0].l)->line.nomP > 0) {
            ado->write("id", bline(m_vlineP[i].rl[0].l)->line.nomP);


            ado->write("id2", num++);
            ado->write("kod1", m_vnode[i].rn[0].n->getKod());
            ado->write("uzel1", m_vnode[i].rn[0].n->node.node_name.name);
            ado->write("kod2", m_vnode[i + 1].rn[0].n->getKod());
            ado->write("uzel2", m_vnode[i + 1].rn[0].n->node.node_name.name);

            ado->write("pr1", _TR("П"));
            ado->write("pr2", _TR("П"));

            ado->write("q", m_vlineP[i].rl[0].g);

            ado->write("pih1", m_vnode[i].rn[0].pihP1);
            ado->write("pih2", m_vnode[i + 1].rn[0].pihP1);

            ado->write("a14", m_vlineP[i].rl[0].a14);
            ado->write("a15", m_vlineP[i].rl[0].a15);
            ado->write("a16", m_vlineP[i].rl[0].a16);
            ado->write("a17", m_vlineP[i].rl[0].a17);

            ado->write("w", m_vlineP[i].rl[0].w);
            ado->write("dl", m_vlineP[i].rl[0].len);
            ado->write("diam", m_vlineP[i].rl[0].diam);
            ado->write("dl2", m_vnode[i + 1].rn[0].lenP);
            ado->write("v2", m_vnode[i + 1].rn[0].vP);

            ado->Update();
        }


        if (bline(m_vlineP[i].rl[0].l)->line.nomO > 0) {

            ado->AddNew(tn);
            ado->write("id", bline(m_vlineP[i].rl[0].l)->line.nomO);


            ado->write("id2", num++);
            ado->write("kod1", m_vnode[i].rn[0].n->getKod());
            ado->write("uzel1", m_vnode[i].rn[0].n->node.node_name.name);
            ado->write("kod2", m_vnode[i + 1].rn[0].n->getKod());
            ado->write("uzel2", m_vnode[i + 1].rn[0].n->node.node_name.name);

            ado->write("pr1", _TR("О"));
            ado->write("pr2", _TR("О"));

            ado->write("q", m_vlineO[i].rl[0].g);

            ado->write("pih1", m_vnode[i].rn[0].pihO1);
            ado->write("pih2", m_vnode[i + 1].rn[0].pihO1);

            ado->write("a14", m_vlineO[i].rl[0].a14);
            ado->write("a15", m_vlineO[i].rl[0].a15);
            ado->write("a16", m_vlineO[i].rl[0].a16);
            ado->write("a17", m_vlineO[i].rl[0].a17);

            ado->write("w", m_vlineO[i].rl[0].w);
            ado->write("dl", m_vlineO[i].rl[0].len);
            ado->write("diam", m_vlineO[i].rl[0].diam);
            ado->write("dl2", m_vnode[i + 1].rn[0].lenO);
            ado->write("v2", m_vnode[i + 1].rn[0].vO);


            ado->Update();
        }

    }

    if (pView) {
        CString q;
        q = "SELECT * FROM ##pjezo ORDER BY id2";
        pView->viewQ2(ado, getGidAdoName(), q, _TR("Техническая информация"), _TR("Техническая информация"), virt_data_line_out, "", _TR("Техническая информация"));
    }
}

void CPjezo::OnRealP()
{
    m_real = !m_real;
    Invalidate();
}


void CPjezo::OnUpdateRealP(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_real);
}


void CPjezo::OnSavePjezo()
{
    CGidrView* pView = getView();

    if (pView) {
        pView->savePjezo();
    }
    else {
        AfxMessageBox("Ошибка", MB_OK | MB_ICONINFORMATION);
    }
}

void CPjezo::OnMoveTo()
{
    CString str;
    CNode2* n1 = m_vnode[m_n].rn[0].n, * n2 = m_vnode[m_n + 1].rn[0].n;
    CPLine* l1, * l2;

    l1 = &m_vlineP[m_n];
    l2 = &m_vlineO[m_n];

    int id = 0;

    if (l1 && l1->rl[0].l) id = l1->rl[0].l->line.nomP;
    if (id == 0 && l2 && l2->rl[0].l) id = l2->rl[0].l->line.nomP;

    CGidrView* pView = getView();
    if (pView) {
        setView(pView);
        pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
    }
}


BOOL CPjezo::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    return TRUE;
}

void CPjezo::OnMButtonDown(UINT nFlags, CPoint point)
{
}

void CPjezo::OnFont()
{
    LOGFONT lf;
    COLORREF color;
    CFont font;

    font.CreatePointFont(100, _T("Arial"));
    font.GetLogFont(&lf);
    font.DeleteObject();

    LoadFont(_T("Font Pjezo"), lf, color);

    CFontDialog fd(&lf);

    if (fd.DoModal() == IDOK) {
        fd.GetCurrentFont(&lf);

        SaveFont(_T("Font Pjezo"), lf, fd.GetColor());
        Invalidate();
    }
}


void CPjezo::OnSavePjezo2()
{
#if 0
    CString q;
    CString text = "";
    CInputDialog inD(this, _TR("Название направления"), _TR("Введите название направления"), text);
    if (inD.DoModal() == IDOK) {
        CAdoFile dbf(m_gid_file->m_dbf);

        q.Format("SELECT * FROM [Направления] WHERE [name]='%s'", inD.m_strInput);

        if (dbf.openTable(q)) {
            int nr = dbf.NRecs();
            dbf.closeTable();

            if (nr) {
                q.Format(_TR("Направление %s уже существует\nУдалить его?"), inD.m_strInput);
                if (AfxMessageBox(q) != IDOK) {
                    return;
                }
                q.Format("DELETE FROM [Направления] WHERE [name]='%s'", inD.m_strInput);
                dbf.db->Execute(q, dbSeeChanges0);
            }
        }

        if (dbf.openTable("Napr")) {

            for (int i = 0; i < m_vnode.size(); i++) {
                double pihP, pihO;

                pihP = getPih(i, 1);
                pihO = getPih(i, 2);

                dbf.AddNew();
                dbf.write(1, inD.m_strInput);
                dbf.write(5, pihP);
                dbf.write(6, pihO);

                CNode2* n = m_vnode[i].rn[0].n;

                dbf.writeNode(n->node_name.kod, n->node_name.name, n->node_name.p);

                dbf.Update();
            }
            dbf.closeTable();
        }
    }
#endif
}

CString get_Select(const char* tn, const CNode* parent = NULL);

#include "CalcTree.h"
#include "mmenu2.h"

void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);

void copyGraph(CGraph2 *graph1, CGraph2 *graph2) 
{
  CMapGraph::iterator p = graph1->map_node.begin();
  while (p != graph1->map_node.end()) {
    CNode2 *n = p->second;
    CNode2 *n2 = graph2->find_ins(n->id);
    if (n2) {
      n2->node = n->node;
    }
    p++;
  }

  p = graph1->map_node.begin();
  
  while (p != graph1->map_node.end()) {
    CNode2 *n = p->second;

      CNode2 *n2 = graph2->find(n->id);

      if (n2) {
        for (CLINE2 *l = n->lines; l; l = next(l)) {
            CNode2 *o = other(l);
            CNode2 *o2 = graph2->find(o->id);
            if (o2) {
                CCoordList cl;

              CLINE2 *ll = graph2->insert_line(n2, o2, cl, true);
              if (ll) {
                bline(ll)->line = bline(l)->line;
                bline(ll)->line.isPjezo = false;
              }
            }
        }
    }
    p++;
  }
}



void CPjezo::OnDouble()
{
    if (m_dbl) {
        m_dbl = FALSE;
        if (m_graph2) delete m_graph2;
        m_graph2 = nullptr;
        initPjezo();
        Invalidate();
        return;
    }



    CAdoFile* ado = getAdo(getGidAdoName());

    CMMenu2 menu(AfxGetMainWnd(), IDD_MENU_DIAM, _TR("Выбрать второй расчет"));


    //  CCalcTree dlg(this, m_cxema->m_graph, ado);

    //  dlg.DoModal();
    menu.AddColumn(_TR("Расчет"));
    menu.AddColumn(_TR("Время"));

    menu.setWidth(1, 400);
    menu.setWidth(2, 200);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CString q;
    q.Format("SELECT TOP 10 * FROM CALCULATION WHERE fileID=%d AND user_gid='%s' ORDER BY id DESC", m_fileID, pDoc->m_user_gid);

    bool first = true;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            if (!first) {
                int id = ado->read_long("id");
                CString name = ado->readStr("name");
                COleDateTime date1 = ado->read_date("date1");

                CString str = date1.Format(_T("%d.%m.%Y %H.%M"));

                menu.Add(1, name);
                menu.Add(2, str);
                menu.Add((void*)id);
            }
            first = false;
            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (menu.m_rows.GetSize() == 0) {
        AfxMessageBox("Нет других расчетов", MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (menu.DoModal() == IDOK) {
        CString par;
        par.Format("%d", (int)menu.getV());

        if (m_graph2) delete m_graph2;

        m_graph2 = new CGraph2(NULL);

//        copyVydGraph_new(m_graph, m_graph2, false);
        copyGraph(m_graph, m_graph2);

        CMapGraph::const_iterator p = m_graph2->map_node.begin();

        while (p != m_graph2->map_node.end()) {
            CNode2* n = p->second;
            n->node.data.pihP = 0;
            n->node.data.pihO = 0;
            p++;
        }

        m_cxema->openOut2(ado, m_graph2, par);
        m_dbl = TRUE;
        initPjezo();
        Invalidate();
    }
}

void CPjezo::OnUpdateDouble(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_dbl);
}

void CPjezo::OnListPjezo()
{
    CGidrView* pView = getView();
    if (pView) {
        pView->SendMessage(WM_COMMAND, ID_LIST_PJEZO);
        pView->SendMessage(WM_COMMAND, ID_PJEZO);
        Invalidate();
    }
}

///////////////////////

void CPjezo::OnPodacha()
{
    m_podacha = !m_podacha;
    Invalidate();
}

void CPjezo::OnUpdatePodacha(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_podacha);
}

void CPjezo::OnObratka()
{
    m_obratka = !m_obratka;
    Invalidate();
}

void CPjezo::OnUpdateObratka(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_obratka);
}


void CPjezo::OnFact()
{
    m_fact = !m_fact;
    Invalidate();
}

void CPjezo::OnUpdateFact(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_fact);
}


double g_old(double g, double p1, double p2, double p1_old, double p2_old, double dh)
{
    if (p1 - p2 + dh == 0) return g;

    double g2 = g * sqrt(fabs((p1_old - p2_old + dh) / (p1 - p2 + dh)));

    return g2;
}


//void writePjezo(int i, char po, BOOL dbl, CArray<CString, CString> &v,
//  vector<CPLine> &m_vlineP, vector<CPLine> &m_vlineO,  vector<CPNode> &m_vnode) {};

void writePjezo(int i, char po, BOOL dbl, CArray<CString, CString>& v,
    vector<CPLine>& m_vlineP,
    vector<CPLine>& m_vlineO,
    vector<CPNode>& m_vnode
)
{
    int k = 0;
    CString str;


    if (po == 'П' && (!m_vlineP[i].rl[0].l || bline(m_vlineP[i].rl[0].l)->line.nomP <= 0)) return;
    if (po == 'О' && (!m_vlineO[i].rl[0].l || bline(m_vlineO[i].rl[0].l)->line.nomO <= 0)) return;


    v.Add(m_vnode[i].rn[0].n->getKod());
    v.Add(m_vnode[i].rn[0].n->node.node_name.name);
    v.Add(po == 'П' ? "П" : "О");
    v.Add(m_vnode[i + 1].rn[0].n->getKod());
    v.Add(m_vnode[i + 1].rn[0].n->node.node_name.name);
    v.Add(po == 'П' ? "П" : "О");

    double dh = m_vnode[i].rn[0].h - m_vnode[i + 1].rn[0].h;

    if (!dbl) {
        if (po == 'П') {
            str.Format("%.2f", m_vlineP[i].rl[0].g); v.Add(str);
            str.Format("%.2f", m_vnode[i].rn[0].pihP1); v.Add(str);
            str.Format("%.2f", m_vnode[i + 1].rn[0].pihP1); v.Add(str);

            str.Format("%.2f", m_vlineP[i].rl[0].a14); v.Add(str);
            str.Format("%.2f", m_vlineP[i].rl[0].a15); v.Add(str);
            str.Format("%.2f", m_vlineP[i].rl[0].a16); v.Add(str);


            double P1 = m_vnode[i].rn[0].pihP1;
            double P2 = m_vnode[i + 1].rn[0].pihP1;

            //    str.Format("%g", m_vnode[i+1].pihP1-m_vnode[i].rn[0].pihP1+dh); v.Add(str);
            str.Format("%.2f", m_vlineP[i].rl[0].a17); v.Add(str);

            str.Format("%.2f", m_vlineP[i].rl[0].w); v.Add(str);
        }
        else {
            str.Format("%.2f", m_vlineO[i].rl[0].g); v.Add(str);
            str.Format("%.2f", m_vnode[i].rn[0].pihO1); v.Add(str);
            str.Format("%.2f", m_vnode[i + 1].rn[0].pihO1); v.Add(str);

            str.Format("%.2f", m_vlineO[i].rl[0].a14); v.Add(str);
            str.Format("%.2f", m_vlineO[i].rl[0].a15); v.Add(str);
            str.Format("%.2f", m_vlineO[i].rl[0].a16); v.Add(str);

            //    str.Format("%g", m_vnode[i+1].pihO1-m_vnode[i].rn[0].pihO1+dh); v.Add(str);
            str.Format("%.2f", m_vlineO[i].rl[0].a17); v.Add(str);

            str.Format("%.2f", m_vlineO[i].rl[0].w); v.Add(str);
        }
    }

    if (dbl) {
        if (po == 'П') {
            str.Format("%.2f", m_vlineP[i].rl[0].g); v.Add(str);
            str.Format("%.2f", m_vnode[i].rn[0].pihP1); v.Add(str);
            str.Format("%.2f", m_vnode[i + 1].rn[0].pihP1); v.Add(str);
            str.Format("%.2f", m_vlineP[i].rl[0].a17); v.Add(str);
        }
        else {
            str.Format("%.2f", m_vlineO[i].rl[0].g); v.Add(str);
            str.Format("%.2f", m_vnode[i].rn[0].pihO1); v.Add(str);
            str.Format("%.2f", m_vnode[i + 1].rn[0].pihO1); v.Add(str);
            str.Format("%.2f", m_vlineO[i].rl[0].a17); v.Add(str);
        }
        if (po == 'П') {
            str.Format("%.2f", m_vlineP[i].rl[1].g); v.Add(str);
            str.Format("%.2f", m_vnode[i].rn[1].pihP1); v.Add(str);
            str.Format("%.2f", m_vnode[i + 1].rn[1].pihP1); v.Add(str);
            str.Format("%.2f", m_vlineP[i].rl[1].a17); v.Add(str);
        }
        else {
            str.Format("%.2f", m_vlineO[i].rl[1].g); v.Add(str);
            str.Format("%.2f", m_vnode[i].rn[1].pihO1); v.Add(str);
            str.Format("%.2f", m_vnode[i + 1].rn[1].pihO1); v.Add(str);
            str.Format("%.2f", m_vlineO[i].rl[1].a17); v.Add(str);
        }
    }
    //
    str.Format("%g", m_vlineP[i].rl[0].len); v.Add(str);
    str.Format("%g", m_vlineP[i].rl[0].diam); v.Add(str);
    str.Format("%g", m_vnode[i + 1].rn[0].lenO); v.Add(str);
    str.Format("%.2f", m_vnode[i + 1].rn[0].vO); v.Add(str);
}




void CPjezo::OnExcel()
{
    CExcel ex;

    CString xn1;

    int n1 = 14;

    if (m_dbl) {
        xn1.Format("%sExcel2/xls/PZ2.xls", argpath());
        n1 = 13;
        n1 = 15;
    }
    else {
        xn1.Format("%sExcel2/xls/PZ1.xls", argpath());
        n1 = 14;
    }

    ex.open(xn1);

    if (ex.IsOpen()) {
        ex.setTab(1);


        ex.init_arr(m_vnode.size() * 2 + 20, 20);

        int i;

        int r = 0;

        for (i = 0; i < m_vnode.size() - 1; i++) {
            {
                CArray<CString, CString> v;
                writePjezo(i, 'П', m_dbl, v, m_vlineP, m_vlineO, m_vnode);

                for (int k = 0; k < v.GetSize(); k++) {
                    ex.set_arr2(CRange(r + 1, k + 1), COleVariant(v[k]));
                }

                if (v.GetSize()) {
                    r++;
                }

            }
            {
                CArray<CString, CString> v;
                writePjezo(i, 'О', m_dbl, v, m_vlineP, m_vlineO, m_vnode);

                for (int k = 0; k < v.GetSize(); k++) {
                    ex.set_arr2(CRange(r + 1, k + 1), COleVariant(v[k]));
                }
                if (v.GetSize()) {
                    r++;
                }
            }
        }

        ex.set_typ(CRange(1, 1, m_vnode.size() * 2 + 20 + 20, 20), dbText);
        ex.example2(CRange(n1, 0), 1);

        CRect rect = m_rect;

        m_rect = CRect(0, 0, 1024, 768);
        CopyBlockP(m_rect);
        m_rect = rect;

        ex.copy(CRange(1, 1), 2);
        //    ShowExcel();
    }
}



//-----------------------------------------------
// Отображаемая информация
//-----------------------------------------------

#include "OtobrInf.h"
#include "Inf2.h"

void CPjezo::OnInf()
{
    CInf2 oi(this, 2, sizeof(m_bIsInfo) / sizeof(m_bIsInfo[0]), m_bIsInfo);

    if (oi.DoModal() == IDOK) {
        InvalidateG();
    }
}

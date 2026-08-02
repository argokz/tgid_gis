#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include "mainwindow.h"
#include <gidview/GidWidget.h>
#include <math.h>
#include "info.h"
#include "colorgtd.h"

static bool m_isPodpAll = false;
int m_n_txt = 0;

QString get_SelectLine(int typ, const char* tn, const CNode* parent = NULL);
QString get_SelectNode(int typ, const char* tn, const CNode* parent = NULL);


void drawText1(QPainter* painter, double x, double y, int flags, const QString & str, double angle);


#define BLACK 0

int inc1(const CNode2* n);
/*
{
    int k = 0;
    CLINE2* l;
    CLine2* ll;
    for (l = n->lines; l; l = nextl(l)) {
        ll = bline(l);
        if (ll->line.pod.isOtkl) continue;
        if (ll->line.pod.isOtkl) continue;
        if (ll->line.nomP >= 0 && ll->line.pod.q == 0 || ll->line.nomO >= 0 && ll->line.obr.q == 0) continue;

        //    if (!ll->isOtklP && !ll->isOtklP) k++;
        k++;
    }
    if (n->node.nUP != -1) k++;

    return k;
}
*/

QString strnum(double o)
{
    QString str;

    if (fabs(o) - floor(fabs(o)) < 0.05) {
        str = QString("%1").arg(o, 0, 'f', 0);
    }
    else {
        if (o < 100) {
            str = QString("%1").arg(o, 0, 'f', 2);
        }
        else {
            str = QString("%1").arg(o, 0, 'f', 1);
        }
    }
    return str;
}

QString formatOutLine(const char* c, int np, int no, double p, double o)
{
    QString str = "";

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

    QString oo, pp;

    oo = strnum(o);
    pp = strnum(p);

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
    QString pv = "п";
    QString ov = "о";
    
    if (strlen(c) > 1)  {
        pv = " п";
        ov = " о";
    }

    if (np != -1 && no != -1 && p != o) {
        if (pp != oo) {
            str = QString("%1%2=%3; %4%5=%6").arg(c, pv, pp, c, ov, oo);
        }
        else {
            str = QString("%1=%2").arg(c, pp);
        }
    }
    else if (np != -1 && no != -1) {
        str = QString("%1=%2").arg(c, pp);
    }
    else if (np != -1) {
        str = QString("%1%2=%3").arg(c, pv, pp);
    }
    else if (no != -1) {
        str = QString("%1%2=%3").arg(c, ov, oo);
    }

    return str;
}


QString formatOutLine1(const QString & c, int np, double p)
{
    QString pp = strnum(p);
    QString str = "";

    if (np != -1 && p != 0) {
        str = QString("%1=%2").arg(c, pp);
    }

    return str;
}




void drawPodp2(QPainter *painter, int cx, int cy, const QString & str0)
{
    QString str = str0;

    int n;
//    CSize sz;
    QString str1, str2;

    QRectF rect = QRectF(0, 0, 0, 0);
    QRectF rect1 = QRectF(0, 0, 0, 0);

    int flags = Qt::AlignLeft|Qt::AlignTop|Qt::TextDontClip;

    cx += 1;
//    sz = dc->GetTextExtent(str);


    str1 = str.left(2);
//    sz = dc->GetTextExtent(str1);

    rect1 = painter->boundingRect (rect, flags, str1);

    painter->setPen(QPen(Qt::black, 1.));
    drawText1(painter, cx, cy, flags, str1, 0);

    double sz_cx = rect1.width();
    double sz_cy = rect1.height();

    cx += sz_cx;

    str = str.mid(2);

    n = str.indexOf(';');

    if (n == 0) {
        str = str.mid(1);
        painter->setPen(QPen(QColor(BLUE), 1.));
        drawText1(painter, cx, cy, flags, str, 0);
    }
    else if (n == str.length() - 1) {
        str = str.left(str.length() - 1);
        painter->setPen(QPen(QColor(RED), 1.));
        drawText1(painter, cx, cy, flags, str, 0);
    }
    else if (n != -1) {
        str1 = str.left(n + 1);
//        sz = dc->GetTextExtent(str1);
        rect1 = painter->boundingRect (rect, flags, str1);

        painter->setPen(QPen(QColor(RED), 1.));
        drawText1(painter, cx, cy, flags, str1, 0);

        str1 = str.mid(n + 1);
        painter->setPen(QPen(QColor(BLUE), 1.));
        drawText1(painter, cx + rect1.width(), cy, flags, str1, 0);
    }
    else {
        painter->setPen(QPen(Qt::black, 1.));
        drawText1(painter, cx, cy, flags, str, 0);
    }
}

//-----------------------------------------------
// Нарисовать подпись к линии
//-----------------------------------------------

//#include "OPCThread2.h"

bool isFull();
/*
{
    return false;
}
*/


#if 0
bool CScroll::isFull()
{

    return /*mas_otn < 3;/**/ masx * dmas < m_MasPodp;/**/

//    return /*mas_otn < 3;/**/ masx * dmas < 5000;/**/ 
}

#endif

void GidWidget::drawLinePodp(QPainter* painter, const CLINE2* ll, double dx1, double dy1, double dx2, double dy2)
{
    if (lfHeight() < 2) return;
    if (getMasPodp() < geom.masx*geom.dmas) return;

    int t1, t2, x1, y1, x2, y2, x, y, xx, yy, dc;
    int i = 0;
    CLine2* l = bline(ll);

    if (!(l->line.isPodp || m_bIsPodpAll)) {
        return;
    }

    if (l->line.nomP == -1 && l->line.nomO == -1) return;
    //  if (l->line.nomgP == -1 && l->line.nomgO == -1) return;

//    m_dc->SetBkColor(m_bk_color);

    bool f = isFull();

    if (!f || l == NULL || m_n_txt > 100) return;

    if (l->line.nomP <= 0 || l->line.nomO <= 0) {
        if (inc1(where(ll)) == 2 && inc1(other(ll)) == 2) {
            //      return;
        }
    }

    double dd, dx, dy;

    QString str[256];

//    m_dc->SetTextAlign(TA_LEFT);

    if (linef[l->line.typ].out == "") return;

    int nn = 0;

    if (inInfo(I_RAS)) {
        if (l->line.pod.q != 0. || l->line.obr.q != 0.) {
            str[nn++] = formatOutLine("G", l->line.nomP, l->line.nomO, fabs(l->line.pod.q), fabs(l->line.obr.q));
        }
    }
    if (inInfo(I_DLINA) && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("L", l->line.nomP, l->line.nomO, l->line.pod.dl, l->line.obr.dl);
    }
    if (inInfo(I_DIAM_V) && (l->line.typ == TIP_UT || l->line.typ == TIP_DR)) {
        str[nn++] = formatOutLine("Двн", l->line.nomP, l->line.nomO, l->line.pod.diam, l->line.obr.diam);
    }
    if (inInfo(I_DIAM_U) && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("Ду", l->line.nomP, l->line.nomO, l->line.pod.diam_usl, l->line.obr.diam_usl);
    }
    if (inInfo(I_OB) && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("V", l->line.nomP, l->line.nomO, l->line.pod.v, l->line.obr.v);
    }
    if (inInfo(I_SKOR) && l->line.typ == TIP_UT) {
        if (l->line.pod.q != 0. || l->line.obr.q != 0.) {
            str[nn++] = formatOutLine("w", l->line.nomP, l->line.nomO, l->line.pod.w, l->line.obr.w);
        }
    }
    if (inInfo(I_UD_POT) && l->line.typ == TIP_UT) {
        str[nn++] = formatOutLine("R", l->line.nomP, l->line.nomO, l->line.pod.poter, l->line.obr.poter);
    }

//    if (inInfo(I_NAGR_U) && l->line.typ == TIP_UT) {
//        str[nn++] = formatOutLine("p", l->line.nomP, l->line.nomO, l->line.pod.ql, l->line.obr.ql);
//    }


    if (inInfo(I_NAGR_U) && l->line.typ == TIP_UT) {
        if (l->line.pod.q != 0. || l->line.obr.q != 0.) {
            //      str[nn++] = formatOutLine("Qр", l->line.nomP, l->line.nomO, l->line.pod.tzam, l->line.obr.tzam);

/*
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
*/

            str[nn++] = formatOutLine1("Q", l->line.nomP, l->line.pod.ql);


            if (l->line.pod.ql_ot != 0) {
                str[nn++] = formatOutLine1("Qот", l->line.nomP, l->line.pod.ql_ot);
            }
            if (l->line.pod.ql_v != 0) {
                str[nn++] = formatOutLine1("Qвент", l->line.nomP, l->line.pod.ql_v);
            }



            if (l->line.pod.ql_gv != 0) {
                str[nn++] = formatOutLine1("Qгв з", l->line.nomP, l->line.pod.ql_gv);
            }

            if (l->line.pod.ql_gv_p != 0) {
                str[nn++] = formatOutLine1("Qгв п", l->line.nomP, l->line.pod.ql_gv_p);
            }

            if (l->line.pod.ql_gv_o != 0) {
                str[nn++] = formatOutLine1("Qгв о", l->line.nomP, l->line.pod.ql_gv_o);
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





//    extern COPCThread2* pOPCThread;
#if 1

//    if (!l->line.strOpc1.IsEmpty() && pOPCThread) {
//        str[nn++] = "                ";
//    }

    //  if (!l->line.strOpcP.IsEmpty()) {
    //    str[nn++] = l->line.strOpcP;
    //  }


//    CFont* oldfont, * font = new CFont();

    //        font->DeleteObject();


//    font->CreateFontIndirect(&m_lf);


//    CSize sz(0, 0), sz1;

//    for (i = 0; i < nn; i++) {
//        sz1 = m_dc->GetTextExtent(str[i]);
//        sz = CSize(max(sz.cx, sz1.cx), max(sz.cy, sz1.cy));
//    }



    QRectF rect = QRectF(0, 0, 0, 0);
    QRectF rect1 = QRectF(0, 0, 0, 0);

    int flags = Qt::AlignHCenter|Qt::AlignTop|Qt::TextDontClip;

    rect1 = painter->boundingRect (rect1, flags, "0");

//    double sz_cx = rect1.width();
//    double sz_cy = rect1.height();

//    CSize sz(0, 0), sz1;

    double sz_cx = 0;
    double sz_cy = 0;


    for (i = 0; i < nn; i++) {
        rect1 = painter->boundingRect (rect, flags, str[i]);
//        sz1 = m_dc->GetTextExtent(str[i]);
        sz_cx = max(sz_cx, rect1.width());
        sz_cy = max(sz_cy, rect1.height());
    }



//    if (sz_cy <= 4) goto EXIT;



    dc = sz_cy / 5;
    xx = sz_cx + dc;
    yy = sz_cy * nn + dc;

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

    painter->setPen(Qt::black);
    painter->drawLine((dx1 + dx2) / 2, (dy1 + dy2) / 2, x1, y1);
    painter->fillRect(x1, y1, x2 + dc-x1, y2 -y1, QBrush(m_bk_color));

//    m_dc->FillSolidRect(CRect(x1, y1, x2, y2), m_bk_color);



    if (x2 < x1) { x = x1; x1 = x2; x2 = x; }
    if (y2 < y1) { y = y1; y1 = y2; y2 = y; }

    for (i = 0; i < nn; i++) {
        //    m_dc->TextOut(x1+dc/2, y1+sz.cy*i+dc/2, str[i]);
//        m_dc->SetBkColor(m_bk_color);
        drawPodp2(painter, x1 + dc / 2, y1 + sz_cy * i + dc / 2, str[i]);
    }
//    if (!l->line.strOpc1.IsEmpty() && pOPCThread) {
//        opc_var.push_back(OPCvar(l->line.strOpc1, x1 + dc / 2 + sz.cx / 2, y1 + sz.cy * (i - 1) + dc / 2));
        //    drawPodp2(m_dc, x1+dc/2, y1+sz.cy*i+dc/2, getOPC(l->line.strOpcP));
//    }


    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(x1, y1, x2 + dc-x1, y2 -y1  );

#endif

EXIT:
    return;

}   

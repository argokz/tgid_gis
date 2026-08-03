#include <QtGui>
#include <QElapsedTimer>
#include <cmath>


#include "std.h"

#include "mainwindow.h"
#include "GidWidget.h"
#include <cxema/graph2.h>

#include "colorgtd.h"

#include "info.h"


//long okr(double x, int dX);


//void drawLineTyp(QPainter *painter, GID_STYLE style, double x1, double y1, double x2, double y2, int typ, QColor color, QColor colorF, int ris, bool napr, bool strelka, bool isQ);

bool IsFullDraw() {
    return true;
}

bool LiangBar(int XS1, int YS1, int XS2, int YS2,
              double& x0, double& y0, double& x1, double& y1);

//int m_idMenu = 1;

bool m_isOnlyPTSColor = false;

static double m_MasPic = 1500;
static double m_MasPodp = 5000;

double getStdMas()
{
    return m_MasPic;
}

void setStdMas(double MasPic)
{
    m_MasPic = MasPic;
}


double getMasPodp()
{
    return m_MasPodp;
}

void setMasPodp(double MasPodp)
{
    m_MasPodp = MasPodp;
}


bool isFull() {
    return true;
}


void GidWidget::drawLine00(QPainter *painter, const CLINE2* ll, int ris, double x1, double y1, double x2, double y2, char podobr, int redr, int t, int typ, int vyd)
{
    double mas_otn_geotext = geom.masx * geom.dmas / (500.);

    CLine2* l = bline(ll);

    if (l->line.pod.isOtkl) {
    }

    double d = hypot(x2 - x1, y2 - y1);
    if (d < 1.) return;

    unsigned long colP, colO, colPF, colOF, colPT, colOT;

    bool strelka = m_bIsNaprGid && !m_bIsPts;

    colP = m_colors.pod(l, m_bIsGidrInf);
    colO = m_colors.obr(l, m_bIsGidrInf);
    colPF = m_colors.podF(l, m_bIsGidrInf);
    colOF = m_colors.obrF(l, m_bIsGidrInf);

    colPT = LIGHTRED;
    colOT = LIGHTBLUE;


    int rm = -1;

    int cP = m_colors.pod(l, 0, true);
    int cO = m_colors.obr(l, 0, true);

    int alma = m_idMenu == 4;

    /////////////////
    //
    int ms_rs = -1;

/*

    map<int, URS>::const_iterator itR = ps_alma.map_URS.find(l->line.rs);
    if (itR != ps_alma.map_URS.end()) {
        ms_rs = 1;
        rm = ms_rs;
    } -


    map<int, UMS>::const_iterator itM = ps_alma.map_UMS.find(l->line.ms);
    if (itM != ps_alma.map_UMS.end()) {
        ms_rs = 0;
        rm = ms_rs;
    }
*/



  if (alma) {
        colP = colO = Qt::black;
        if (bline(ll)->line.pipeSectionID > 0 || !m_isOnlyPTSColor) {
            if (rm >= 0 && rm < 20) {
                colP = m_colors.pod(l, rm, m_bIsGidrInf);
                colO = m_colors.obr(l, rm, m_bIsGidrInf);
            }
        }

        if (l->line.ms > 0) {
            rm = 0;
        }
        if (l->line.rs > 0) {
            rm = 1;
        }

        int old_pod_otl = l->line.pod.isOtkl;
        int old_obr_otl = l->line.obr.isOtkl;
        l->line.pod.isOtkl = false; // Не показывать закрытые для ПТС
        l->line.obr.isOtkl = false;

        colP = m_colors.pod(l, rm, m_bIsGidrInf);
        colO = m_colors.obr(l, rm, m_bIsGidrInf);
        l->line.pod.isOtkl = old_pod_otl;
        l->line.obr.isOtkl = old_obr_otl;



        if (l->line.owner > 1) {
            colP = colO = 0xa52a2a; //:BROWN;
            colP = colO = 0x7F7F00;
            //      colP = colO = YELLOW;
        }
    }



    GID_STYLE style;
    style.m_thickness = 1;

    bool m_bIsNaprGid = true;
    CNode * m_parent_kam = NULL;


    GID_STYLE style1 = m_colors.style(l);
    double thickness = style1.m_thickness;
    double distance = style1.m_distance;

    if (vyd == 4) {
        colP = colO = colPF = colOF = CYAN;
    }
    else if (vyd == 1) {  // Выделенные желтым
        colP = colO = colPF = colOF = YELLOW;
    }
    else if (vyd == 8) {  // Выделенные желтым по подаче
        colP = colPF = YELLOW;
    }
    else if (vyd == 9) {  // Выделенные желтым по обратке
        colO = colOF = YELLOW;
    }

    
    
    else if (vyd == 2) {
        colP = colO = colPF = colOF = 0x0000FF;
    }
    else if (vyd == 3) { // Опрессовка
  //    unsigned long getColorOpressovka(int id, unsigned long color);
        bool getColorOpressovka(int id, unsigned long& color);

//        if (getColorOpressovka(l->line.m_opressovka, colP)) {
            colP = colO = colPF = colOF = colP;
//        }
//        else {
            colP = colO = colPF = colOF = 0x7F7F7F;
            //      vyd = 0;
//        }
    }

    if (vyd) {
        double dd = 2. * mas_otn_geotext * 1500 / getStdMas();
        if (dd / mas_otn < 4) dd = 4 * mas_otn;
        thickness += dd;
    }

    if (redr == 1) {
        colP = colO = colPF = colOF = CUR_COLOR;
    }
    else if (redr == 2) {
        colP = colO = colPF = colOF = LIGHTGRAY;
    }

    double m_sX = distance * m_h / mas_otn, dx, dy;
    dx = (x2 - x1) * m_sX / d;
    dy = (y2 - y1) * m_sX / d;

    int naprP = l->line.pod.q >= 0;
    int naprO = l->line.obr.q >= 0;

    bool punct = style1.m_style;

    if (l->line.nomP >= 0 && l->line.nomO >= 0 && isFull() && podobr != N_POD && podobr != N_OBR) {
        drawLineTypL(painter, l, N_OBR, redr, thickness, x1 - dy * t, y1 + dx * t, x2 - dy * t, y2 + dx * t, typ, colO, colOT, colOF, ris, punct, naprO, strelka, !isNul(l->line.obr.q));
        drawLineTypL(painter, l, N_POD, redr, thickness, x1 + dy * t, y1 - dx * t, x2 + dy * t, y2 - dx * t, typ, colP, colPT, colPF, ris, punct, naprP, strelka, !isNul(l->line.pod.q));
    }
    else {
//        col = podobr == N_POD ? colP : colO;
//        colF = podobr == N_POD ? colPF : colOF;

        if (m_parent_id) t = 0;

        if (podobr == N_POD) {
            drawLineTypL(painter, l, N_POD, redr, thickness, x1 + dy * t, y1 - dx * t, x2 + dy * t, y2 - dx * t, typ, colP, colPT, colPF, ris, punct, naprP, strelka, !isNul(l->line.pod.q));
        }
        else if (podobr == N_OBR) {
            drawLineTypL(painter, l, N_OBR, redr, thickness, x1 - dy * t, y1 + dx * t, x2 - dy * t, y2 + dx * t, typ, colO, colOT, colOF, ris, punct, naprO, strelka, !isNul(l->line.obr.q));
        }
    }
}


int inc1(const CNode2 *n)
{
    int k = 0;
    for ( CLINE2 *l = n->lines; l; l = nextl(l)) {
        CLine2 *ll = bline(l);
        if (ll->line.pod.isOtkl) continue;
        if (ll->line.pod.isOtkl) continue;
        if ((ll->line.nomP >= 0 && ll->line.pod.q == 0) || (ll->line.nomO >= 0 && ll->line.obr.q == 0)) continue;

//    if (!ll->isOtklP && !ll->isOtklP) k++;
        k++;
    }
    if (n->node.nUP != -1) k++;

    return k;
}


QString strnum(double o);
/*
{
    if ( fabs(o)-floor(fabs(o)) < 0.05) {
        return QString("%1").arg(o, 0, 'f', 0);
    }
    else {
        return QString("%1").arg(o, 0, 'f', 1);
    }
}
*/


QString formatOut(char c, int np, int no, double p, double o)
{
    QString str = "", pp, oo;

    pp = strnum(p);
    oo = strnum(o);

    if (np != -1 && no != -1 && p != o)
        str = QString("%1;%1").arg(pp, oo);
    else if (np != -1 && no != -1)
        str = QString("%1").arg(pp);
    else if (np != -1)
        str = QString("%1;").arg(pp);
    else if (no != -1)
        str = QString(";%1").arg(oo);

    return QString("%1=%2\n").arg(c).arg(str);
}

//-----------------------------------------------
// Нарисовать подпись к линии
//-----------------------------------------------

void GidWidget::uch_pod(QPainter *painter, const CLINE2 *ll, double dx1, double dy1, double dx2, double dy2)
{
    int t1, t2, x1, y1, x2, y2, x, y, xx, yy, dc;
    CLine2 *l = bline(ll);

    if (!l->line.isPodp) return;

    if (l->line.nomP == -1 && l->line.nomO == -1) return;
    if (l->line.nomgP == -1 && l->line.nomgO == -1) return;

//  if (!IsFullDraw() || l == NULL || m_n_txt > 100) return;
    if (!IsFullDraw() || l == NULL) return;

    if (l->line.nomP <= 0 || l->line.nomO <= 0) {
        if (inc1(where(ll)) == 2 && inc1(other(ll)) == 2) {
            return;
        }
    }

    double dd, dx, dy;

//  m_dc->SetTextAlign(TA_LEFT);

    if (linef[l->line.typ].out == "") return;

    QString str = "";

    if (inInfo(I_RAS)) {
        if ( l->line.pod.q != 0. || l->line.obr.q != 0. ) {
            str += formatOut('q', l->line.nomP, l->line.nomO, fabs(l->line.pod.q), fabs(l->line.obr.q));
        }
    }
    if (inInfo(I_DLINA) && linef[l->line.typ].n_dlina != -1) {
        str += formatOut('l', l->line.nomP, l->line.nomO, l->line.pod.dl, l->line.obr.dl);
    }
    if (inInfo(I_DIAM_V) && linef[l->line.typ].n_diam != -1) {
        str += formatOut('d', l->line.nomP, l->line.nomO, l->line.pod.diam, l->line.obr.diam);
    }
    if (inInfo(I_OB) && l->line.typ == TIP_UT) {
        str += formatOut('v', l->line.nomP, l->line.nomO, l->line.pod.v, l->line.obr.v);
    }
    if (inInfo(I_SKOR) && l->line.typ == TIP_UT) {
        str += formatOut('w', l->line.nomP, l->line.nomO, l->line.pod.w, l->line.obr.w);
    }
    if (inInfo(I_UD_POT) && l->line.typ == TIP_UT) {
        str += formatOut('p', l->line.nomP, l->line.nomO, l->line.pod.poter, l->line.obr.poter);
    }

//    if (inInfo(I_NAGR_U) && l->line.typ == TIP_UT) {
//        str += formatOut('p', l->line.nomP, l->line.nomO, l->line.pod.ql, l->line.obr.ql);
//    }




//  if (!l->strOpcP.IsEmpty()) {
//    str[nn++] = l->strOpcP;
//  }

    str = str.trimmed();

    QRectF rect = QRectF(0, 0, 0, 0);
    QRectF rect1 = QRectF(0, 0, 0, 0);
    int flags = Qt::AlignHCenter|Qt::AlignTop|Qt::TextDontClip;

    rect1 = painter->boundingRect (rect1, flags, "0");
    rect = painter->boundingRect (rect, flags, str);

//  if ( rect.width() <= 4 ) return;

    dc = rect1.height()/5;
    xx = rect.width()+dc;
    yy = rect.height()+dc*0;

    dd = hypot(dx2-dx1, dy2-dy1);
    if (dd < 1.) return;
    dx = (dx2-dx1)/dd*dc*4;
    dy = (dy2-dy1)/dd*dc*4;

    t1 = (dy > 0) ? 1 : -1;
    t2 = (dx < 0) ? 1 : -1;

    x1 = (dx1+dx2)/2+dy-dc/2;
    x2 = x1+t1*xx+dc/2;
    y1 = (dy1+dy2)/2-dx-dc/2*0;
    y2 = y1+t2*yy+dc/2*0;

    if (dx1 == dx2) {
        y1 = (dy1+dy2)/2-yy/2;
        y2 = (dy1+dy2)/2+yy/2;
    }
    if (dy1 == dy2) {
        x1 = (dx1+dx2)/2-xx/2;
        x2 = (dx1+dx2)/2+xx/2;
    }

    painter->setPen(QPen(Qt::black));
    painter->setBrush(palette().window());

    painter->drawLine((dx1+dx2)/2, (dy1+dy2)/2, x1, y1);

//  m_dc->FillSolidRect( CRect(x1, y1, x2, y2), m_bk_color );

    if (x2 < x1) {
        x = x1;
        x1 = x2;
        x2 = x;
    }
    if (y2 < y1) {
        y = y1;
        y1 = y2;
        y2 = y;
    }

//    painter->drawRect(x1, y1, x2-x1+dc, y2-y1);

    rect = QRectF(x1, y1, x2-x1+dc, y2-y1);

    painter->drawRect(rect);
    painter->drawText(rect, flags, str, &rect);
}

#if 0
void GidWidget::drawLine(QPainter *painter, const CLINE2 *ll, int redr, bool is_text)
{
    QColor niceBlue(0, 0, 255);
    QPen thickPen(niceBlue, 1.);
    painter->setPen(thickPen);

    CNode2 *n1, *n2;
    CLine2 *l;
    int i;
    double x1, y1, x2, y2;

    l = bline(ll);

    n1 = where(l);
    n2 = other(l);

    if (n2->node.coord.x == 0 && n2->node.coord.y == 0) return;

    CCoordList::const_iterator it;

    it = l->line.m_NP.begin();

    int t = (n2->node.coord.y > n1->node.coord.y || (n2->node.coord.y == n1->node.coord.y && n2->node.coord.x > n1->node.coord.x)) ? -1 : 1;

//  if (m_colors.style(l).m_style) {
//    len_dot = 0;
//  }
//  else {
//    len_dot = 20;
//  }



    if (it == l->line.m_NP.end()) {
        x1 = n1->node.coord.x;
        y1 = n1->node.coord.y;
        x2 = n2->node.coord.x;
        y2 = n2->node.coord.y;


//void GidWidget::drawLine00(QPainter *painter, const CLINE2* ll, int ris, double x1, double y1, double x2, double y2, char podobr, int redr, int t, int typ, int vyd)

        drawLine00(painter, ll, 0, x1, y1, x2, y2, redr, is_text, t);
        drawLine00(painter, ll, 1, x1, y1, x2, y2, redr, is_text, t);
    }
    else {
        double x, y;

        x = n1->node.coord.x;
        y = n1->node.coord.y;

        int i_pic = l->line.m_NP.size()/2;

        for (i = 0; it != l->line.m_NP.end(); ++it, i++) {
            if (i == i_pic) {
                if (x == it->x && y == it->y) {
                    i_pic++;
                }
                else {
                    drawLine00(painter, ll, 0, x, y, it->x, it->y, redr, is_text, t);
                    drawLine00(painter, ll, 1, x, y, it->x, it->y, redr, is_text, t);
                }
            }
            else {
                drawLine00(painter, ll, -1, x, y, it->x, it->y, redr, is_text, t);
            }
            x = it->x;
            y = it->y;
        }
        if (i_pic == (int) l->line.m_NP.size()) {
            drawLine00(painter, ll, 0, x, y, n2->node.coord.x, n2->node.coord.y, redr, is_text, t);
            drawLine00(painter, ll, 1, x, y, n2->node.coord.x, n2->node.coord.y, redr, is_text, t);
        }
        else {
            drawLine00(painter, ll, -1, x, y, n2->node.coord.x, n2->node.coord.y, redr, is_text, t);
        }
    }
}

#endif

bool isNeotr(const CNode2* node);
void drawNodeTyp(QPainter *painter, double x, double y, int typ, QColor color);

void GidWidget::drawNode0(QPainter *painter, const CNode2 *node, int redr)
{
    int x, y;
    QColor color;

    if (!IsFullDraw()) return;

//  color = m_colors.color(node, m_bIsGidrInf);
    color = Qt::darkRed;

    color = m_colors.color(&node->node, m_bIsGidrInf);

    if (node->node.isOtkl) color = Qt::green;

    if (isNeotr(node)) color = Qt::green;
    if (redr == 1) color = Qt::green;
    else if (redr == 2) color = Qt::lightGray;

    CFPoint pt = CoordToScreen(node->node.coord);
    x = pt.x, y = pt.y;

    if ( node->node.nZN != -1) {
        drawNodeTyp(painter, x, y, TIP3_ZN, color);
    }

    if ( node->node.nUP != -1) {
        drawNodeTyp(painter, x, y, TIP_PODP, color);
    }

    if ( node->node.nORP != -1) {
        drawNodeTyp(painter, x, y, 29, color);
    }

    int typ = node->node.typ;

    if (node->node.typ == TIP_PR && node->node.isEl) {
        typ = 27;
    }
    else if (node->node.typ == TIP_PR && node->node.isNez) {
        typ = 28;
    }

    if (node->node.colorP != 0xFFFFFFFF) {


        if (node->node.isMarkAnalyze)
            drawNodeTyp(painter, x, y, 38, node->node.colorP, 5.0);
        else
            drawNodeTyp(painter, x, y, 30, node->node.colorP);
    }

    drawNodeTyp(painter, x, y, typ, color);

    if ( node->node.isP && node->node.typ == TIP_US) {
        drawNodeTyp(painter, x, y, 25, color);
    }
    else if ( node->node.isP && node->node.typ != TIP_PR) {
//    drawNodeTyp(painter, x, y, 22, color);
    }
    if ( node->node.nUP != -1) {
        drawNodeTyp(painter, x, y, TIP_PODP, color);
    }
//    if ( node->node.isVP ) {
    if ( node->node.nVP != -1 ) {
        drawNodeTyp(painter, x, y, TIP_VODOR, color);
    }

    if (!node->node.isValid) {
        drawNodeTyp(painter, x, y, 26, Qt::black);
    }

    if ( 0 && node->node.isP) {
        CLINE2 *l;
        CLine2 *ll;
        CNode2 *n2;
        CFPoint pt;

        for (l = node->lines; l; l = nextl(l)) {
            n2 = other(l);
            if (n2->node.coord.x != 0) {

                double x1, y1, x2, y2, xx, yy;

                CFPoint pt1 = CoordToScreen(node->node.coord);
                x1 = pt1.x, y1 = pt1.y;


                ll = bline(l);

                if (ll->line.m_NP.size() != 0) {
                    if ( IsBegin(l) ) {
                        pt = ll->line.m_NP.first();
                    }
                    else {
                        pt = ll->line.m_NP.last();
                    }
                }
                else {
                    pt = n2->node.coord;
                }

                CFPoint pt2 = CoordToScreen(pt);
                x2 = pt2.x, y2 = pt2.y;


                xx = x1+(x2-x1)*30./hypot(x2-x1, y2-y1);
                yy = y1+(y2-y1)*30./hypot(x2-x1, y2-y1);

//      GID_STYLE style = m_colors.style(ll)
                GID_STYLE style;
                style.m_thickness = 1;

                bool m_bIsNaprGid = true;

//void drawLineTyp(QPainter *painter, GID_STYLE style, double x1, double y1, double x2, double y2, int typ, QColor color, QColor colorF, int ris, bool napr, bool strelka, bool isQ);

//                drawLineTyp(painter, style, x1, y1, xx, yy, 57, Qt::blue, Qt::blue, 1, true, m_bIsGidrInf&m_bIsNaprGid, true);
            }
        }
    }

//  m_dc->MoveTo(x, y);
}

/*
QString strnum(double o)
{
  CString str;

  if ( fabs(o)-floor(fabs(o)) < 0.05) {
    str.Format("%.0f", o);
  }
  else {
    str.Format("%.1f", o);
  }
  return str;
}
*/


QString formatOut(int np, int no, double p, double o)
{
    if ( fabs(p) > 1e9 ) p = 0.;
    if ( fabs(o) > 1e9 ) o = 0.;

    if (np != -1 && no != -1)
        return QString("%1; %2").arg(p, 0, 'f', 1).arg(o, 0, 'f', 1);
    else if (np != -1)
        return QString("%1").arg(p, 0, 'f', 1);
    else if (no != -1)
        return QString("%1").arg(o, 0, 'f', 1);

    return QString("");
}


bool GidWidget::isPodpName(const QString & name)
{
    QString s1 = name.left(1);
//    QString s2 = name.mid(1, 1);

//    if (s1 == "#" && !m_parent_id && !m_bIsPodpRes) return false;
    if (s1 == "У" && m_parent_id) return false;
    return true;

}

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

double roundSignificant(double number, int digits) {
    if (number == 0.0)
        return 0.0;

    // Вычисляем множитель для сдвига числа
    double shift = std::pow(10, digits - 1 - static_cast<int>(std::floor(std::log10(std::abs(number)))));
    
    // Округляем число
    double result = std::round(number * shift) / shift;

    // Форматируем число с точным количеством значащих цифр
    std::ostringstream stream;
    stream << std::scientific << std::setprecision(digits - 1) << result;
    return std::stod(stream.str());
}


void GidWidget::drawPodpNode(QPainter *painter, CNode2 *node)
{
    if (lfHeight() < 2) return;
    if (m_MasPodp < geom.masx*geom.dmas) return;

    CFPoint pt = CoordToScreen(node->node.coord);
    QString str = node->node.name;
    CLINE2 *l;

    QRectF rect;
    rect = painter->boundingRect ( rect, Qt::AlignHCenter|Qt::AlignBottom, str);
    painter->setPen(QPen(Qt::black));

    if ( rect.width() <= 4 ) return;

    CGraph2 *graph = m_cxema.graph();
    if (graph == NULL) return;

    int max_len = 0;

    for (l = node->lines; l; l = nextl(l)) {
        CNode2 *n2 = other(l);
        if (n2->node.coord.x != 0) {
            max_len = fmax(max_len, Length(node->node.coord, n2->node.coord));
        }
    }
//  if (max_len != 0 && max_len < 20*geom.mas/100) return;
//  m_dc->SetTextColor(0);

    double dy = node->node.typ == TIP_US && !node->node.isP ? 3 : node->node.typ == TIP_PR || node->node.typ == TIP_PO ? 5 : 7;

    dy /= mas_otn;
    pt.y += dy/mas_1*m_h;

    CNode2 *m_parent_kam = NULL;
//  double cx = 10, szcy = 10;

    int flags = Qt::AlignHCenter|Qt::AlignTop|Qt::TextDontClip;

    if (!str.isEmpty()) {
        if (m_bIsPodpAll || node->node.isPodp) {
            if (!m_bIsPodp || (!isPodpName(str) &&!m_bIsPodpRes)) {
                str = "";
            }

            bool vnutr = (node->node.isP && node->node.typ != TIP_PR);

            if ( m_bIsGidrInf ) {
                if ((inInfo(I_NAPOR) || inInfo(I_TEMP) /*|| m_bIsInfo[9]*/ ) && !vnutr) {
                    if (node->node.nomgP > 0 || node->node.nomgO > 0) {
                        if (inInfo(I_NAPOR)) {
                            if (str != "") str += "\n";
                            str += formatOut(node->node.nomP, node->node.nomO, node->node.data.pihP, node->node.data.pihO);
                        }
                        if (inInfo(I_TEMP) && node->node.data.tempP != -10000) {
                            if (str != "") str += "\n";
                            str += formatOut(node->node.nomP, node->node.nomO, node->node.data.tempP, node->node.data.tempO);
                        }
                    }
                }

                if (inInfo(I_RAS_U) && (node->node.typ == TIP_PR || node->node.typ == TIP_PO)) {
                    if (str != "") str += "\n";
                    str += QString("%1;%2;%3")
                        .arg(roundSignificant(node->node.qz, 3))
                        .arg(roundSignificant(node->node.qP, 3))
                        .arg(roundSignificant(node->node.qO, 3));
                }

                if (inInfo(I_NAGR) ) { // Нагрузки
                    if (node->node.qP != 0 || node->node.Qgvs != 0 || node->node.Qvent != 0) {
                        if (str != "") str += "\n";
                        str += QString("%1\n%2\n%3\n%4")
                               .arg(node->node.qP)
                               .arg(node->node.Qvent)
                               .arg(node->node.Qgvs)
                               .arg(node->node.qO+node->node.Qgvs+node->node.Qvent);
                    }
                }

                if (inInfo(I_RASP) && !vnutr) {
                    if (node->node.nomgP > 0 || node->node.nomgO > 0) {
                        if (str != "") str += "\n";
                        str += QString("%1").arg(roundSignificant(node->node.data.pihP - node->node.data.pihO, 3));
                    }
                }


                if (inInfo(I_GEOD)) {
                    if (node->node.nomgP > 0 || node->node.nomgO > 0) {
                        double h = node->node.geoMarkTopTube;
                        if (str != "") str += "\n";
                        str += formatOut(1, -1, h, h);
                    }
                }

                if (inInfo(I_NAPOR2) && !vnutr) {
                    if (node->node.nomgP > 0 || node->node.nomgO > 0) {
                        double h = node->node.geoMarkTopTube;

                        if (node->node.internalNodeID > 0) {
                            CNode2 *nn = graph->find(node->node.internalNodeID);
                            if (nn) {
                                h = nn->node.geoMarkTopTube;
                            }
                        }


                        if (str != "") str += "\n";
                        str += formatOut(node->node.nomP, node->node.nomO, node->node.data.pihP+h, node->node.data.pihO+h);
                    }
                }
            }

        }
        rect = QRectF(pt.x, pt.y, 0, 0);
        QRectF r = painter->boundingRect ( rect, flags, str);
        painter->fillRect(r, m_bk_color);
        painter->drawText(rect, flags, str, &rect);
        pt.y += rect.height()-1;
    }

    /*
      if (node->node.nomgP > 0 || node->node.nomgO > 0) {
        rect = QRectF(pt.x, pt.y, 0, 0);
        painter->drawText(rect, flags, QString("%1 %2").arg(node->node.nomP).arg(node->node.nomO), &rect);
        pt.y += rect.height()-1;
      }
    */

#if 0

    if ( m_bIsGidrInf ) {

        /*
          if ( m_bIsInfo[9] ) { // OPC
            if ( !node->strOpc1.IsEmpty() ) {
              opc_var.push_back(OPCvar(node->strOpc1, pt.x, pt.y));
              str = getOPC(node->strOpc1);
            }

            if ( !node->strOpc2.IsEmpty() ) {
              opc_var.push_back(OPCvar(node->strOpc2, pt.x, pt.y));
              str = getOPC(node->strOpc2);
            }

            if ( !node->strOpc2.IsEmpty() ) {
              opc_var.push_back(OPCvar(node->strOpc2, pt.x, pt.y));
              str = getOPC(node->strOpc3);
            }
          }
        */
    }

#endif
}


void GidWidget::drawNode(QPainter *painter, const CNode2 *node, int redr, bool is_text)
{
    if (isRezhim(node)) {
        return;
    }

    if (!IsFullDraw()) return;

//  if ( m_parent_kam && m_parent_kam->typ == TIP_PO ) {
//    if ( inc1(node) == 0) {
//      return;
//    }
//  }
//  m_dc->SetBkColor(m_bk_color);

    drawNode0(painter, node, redr);

    if (is_text && (node->node.isPodp || m_bIsPodpAll) && !redr) {
        drawPodpNode(painter, (CNode2*)node);
    }
}


void GidWidget::drawBorder(QPainter *painter)
{
    int width, height, x0, y0, x, y;

    //  if (m_rectBorder.left == LONG_MAX)
    //    SetBorder(CFRect(0.+40000., 0.-40000., 400.+40000., 400.-40000.));
    //  else
    //    SetBorder(m_rectBorder);

//    if (m_dc->IsPrinting()) {
//        hideU(m_dc, m_rect);
//    }

//    setline(painter, YELLOW, 3);
//    setline(painter, 0, 3);
    painter->setPen(QPen(Qt::black, 3));

    double w = painter->device()->width();
    double h = painter->device()->height();

    CFRect m_rect(0, 0, w, h);

    if (m_bIsRamka) {
        CFRect m_rectBorder0 = m_cxema.rect();
        CFRect rgeo;

        int x1 = m_rectBorder0.left / geom.masx - geom.bx - 40;
        int x2 = m_rectBorder0.right / geom.masx - geom.bx + 40;
        int y1 = m_rectBorder0.top / geom.masy - geom.by - 40;
        int y2 = m_rectBorder0.bottom / geom.masy - geom.by + 40;

//        rectangle(m_dc, x1, y1, x2, y2);

        int xx1 = fmax(-1, x1);
        int xx2 = fmin(x2, m_rect.Width()+1);
        int yy1 = fmax(-1, y1);
        int yy2 = fmin(y2, m_rect.Height()+1);

        if (0 <= x1 && x1 <= m_rect.Width()) {
            painter->drawLine(x1, yy1, x1, yy2);
        }
        if (0 <= y2 && y2 <= m_rect.Height()) {
            painter->drawLine(xx1, y2, xx2, y2);
        }
        if (0 <= x2 && x2 <= m_rect.Width()) {
            painter->drawLine(x2, yy1, x2, yy2);
        }
        if (0 <= y1 && y1 <= m_rect.Height()) {
            painter->drawLine(xx1, y1, xx2, y1);
        }
    }

    m_dX = 200;

//    if (m_bIsAstr /* && m_dX != 0 /*&& !m_dc->IsPrinting()*/ && m_dX / geom.masx >= 5 && m_parent_id != 0) {
    if (m_bIsAstr && m_parent_id != 0 && m_dX / geom.masx >= 5) {

        painter->setPen(QPen(Qt::black, 1));

        width = m_rect.Width() * geom.masx + m_dX;
        height = m_rect.Height() * geom.masy + m_dX;

        for (x = okr((geom.bx + m_rect.left) * geom.masx, m_dX), x0 = 0; x0 <= width; x += m_dX, x0 += m_dX)
            for (y = okr((geom.by + m_rect.top) * geom.masy, m_dX), y0 = 0; y0 <= height; y += m_dX, y0 += m_dX)
                painter->drawLine(x / geom.masx - geom.bx, y / geom.masy - geom.by, x / geom.masx - geom.bx, y / geom.masy - geom.by);
    }
}

void draw_b5(QPainter *painter, const QString &cx);


void GidWidget::draw(QPainter *painter, CCxema *cxema)
{
    if (!painter) return;

//    draw_b5(painter, "6.7");


    CGraph2 *graph = cxema->graph();
    if (graph == NULL) return;

    double w = painter->device()->width()/m_scale;
    double h = painter->device()->height()/m_scale;

    CFRect rr, rectScr = ScreenToCoord(CFRect(0, 0, w, h));
    CFRect rectScr2 = rectScr;

    init_draw(painter, getStdMas());

    if (!IsMain() || !m_cxema.isGeobaza())
        init_draw(painter, 50.);



    drawBorder(painter);

    for (auto& p : m_cxema.m_graph->map_node) {
        CNode2* pp = p.second;

//  while (p != graph->set_node.end()) {
        for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
            if (IsBegin(l)) {
                CNode2 *n1 = where(l);
                CNode2 *n2 = other(l);
  //              if (n1 && n2 && n1->node.coord.x != 0 && n2->node.coord.x != 0 && n1->node.internalNodeID == m_parent_id) {
                if (n1 && n2 && n1->node.internalNodeID == m_parent_id) {
                    CFRect rl(n1->node.coord, n2->node.coord);

                    rl = bline(l)->line.rect;

                    rl.NormalizeRect();
                    if (rr.IntersectRect(rectScr2, rl)) {
                        drawLine1(painter, l, 0, true);
                    }
                }
            }
        }
    }

    /*
      if (m_cxema.n_zn && rectScr.PtInRect(m_cxema.n_zn->node.coord)) {
        CFPoint pt = CoordToScreen(m_cxema.n_zn->node.coord);
    //    drawNodeTyp(pt.x, pt.y, 23, m_colors.pod(n));
        drawNodeTyp(pt.x, pt.y, 23, Qt:red);
      }
    */

//  p = graph->set_node.begin();

    for (auto& p : m_cxema.m_graph->map_node) {
        CNode2* pp = p.second;
        if (pp->node.internalNodeID == m_parent_id && rectScr2.PtInRect(pp->node.coord) ) {
            drawNode(painter, &(*pp), 0, true);
        }
    }


    painter->setPen(QPen(Qt::black, 0));

    int flags = Qt::AlignLeft|Qt::AlignBottom|Qt::TextDontClip;

//    for (auto & t : m_cxema.set_text) {
    for (auto it = m_cxema.set_text.begin(); it != m_cxema.set_text.end(); ++it) {
//    m_dc->SetTextAlign(TA_LEFT | TA_BOTTOM);

        if (it->internalNodeID == m_parent_id && rectScr2.PtInRect(it->coord) ) {
            CFPoint pt = CoordToScreen(it->coord);

            QString text = it->text;

            QRectF rect = QRectF(pt.x, pt.y, 0, 0);
            QRectF r = painter->boundingRect ( rect, flags, text);
            painter->fillRect(r, m_bk_color);
            painter->drawText(rect, flags, text, &rect);

            it->rect = r;

//            LOGFONT lf = pt->lf;

//            oldfont = initFont2(m_dc, lf);
//            m_dc->ExtTextOut(pt->coord.x / geom.masx - geom.m_bx, pt->coord.y / geom.masy - geom.m_by, ETO_CLIPPED, m_rect, pt->text, NULL);
//            closeFont(m_dc, oldfont);
        }
    }



//    drawBorder(painter);

//    void DrawB(QPainter *painter);
//    DrawB(painter);

}



#include <dxf/dxfi.h>

// Порог, с которого кадр считается дорогим и попадает в журнал.
// Перерисовка идёт на каждое движение мыши, поэтому писать про
// каждый кадр нельзя — журнал станет бесполезным.
static const int kSlowFrameMs = 150;

void drawDxf(cdxf* dxf, QPainter* pDC, CRect m_rect, double m_bx, double m_by, double masx, double masy);


void GidWidget::drawPoints(QPainter *painter)
{
    QPen thinPen(palette().window(), 0.5);
    painter->setPen(thinPen);
    painter->setBrush(palette().windowText());

    if (IsFullDraw() && m_dX != 0 && /*!m_dc->IsPrinting() && */m_dX/geom.masx >= 5) {
        int x0, y0, x, y;

        double w = painter->device()->width();
        double h = painter->device()->height();


        int ww = w*geom.masx+m_dX;
        int hh = h*geom.masy+m_dX;

        for ( x = okr(geom.bx*geom.masx, m_dX), x0 = 0; x0 <= ww; x += m_dX, x0 += m_dX) {
            for ( y = okr(geom.by*geom.masy, m_dX), y0 = 0; y0 <= hh; y += m_dX, y0 += m_dX) {
                CFPoint pt = CoordToScreen(CFPoint(x, y));
                painter->drawPoint(pt.x, pt.y);
            }
        }
    }
}

void GidWidget::draw(QPainter *painter, double w, double h, const ScrollGeom &geom1)
{
//    if (!IsMain() || !m_cxema.isGeobaza())
//        init_draw(painter, 50.);

    geom = geom1;

//    QColor niceBlue(150, 150, 200);
//    painter->setRenderHint(QPainter::Antialiasing, true);

/*
    QRect rr1 = painter->viewport();
    QRect rr2 = painter->window();

    double w = painter->device()->width()/m_scale;
    double h = painter->device()->height()/m_scale;

    m_wi = w;
    m_hi = h;
*/


    CRect m_rect(0, 0, w, h);
    CFRect r(0, 0, w, h);

    // Замер этапов отрисовки.
    //
    // Чтение схемы из БД замерено отдельно (tools/measure_fragments.py):
    // все 25 фрагментов, 167 321 объект — около 1,6 с. Заказчик говорит,
    // что открытие занимает больше двух секунд, значит остаток уходит
    // сюда, в рисование. Какой именно этап столько стоит, из журнала
    // было не видно: он молчал целиком.
    //
    // Пишем только когда кадр вышел заметно дорогим, иначе журнал
    // засорится: перерисовка происходит на каждое движение мыши.
    QElapsedTimer _t_frame, _t_step;
    _t_frame.start();
    _t_step.start();
    qint64 ms_map = 0, ms_dxf = 0, ms_geo1 = 0, ms_cxema = 0, ms_geo2 = 0;

    painter->fillRect(0, 0, w, h, m_bk_color);

    if (m_parent_id == 0) {
        redrawMap(painter, r, m_internetMap, m_reread);
        m_reread = false;
    }
    ms_map = _t_step.restart();

    if (m_bIsPicture && m_dxf && m_parent_id == 0) {
        drawDxf(m_dxf, painter, m_rect, geom.bx, geom.by, geom.masx, geom.masy);
    }
    ms_dxf = _t_step.restart();

    if (isGeo() && m_parent_id == 0) {
        if (!scaling)
        drawGeo(painter, false);
    }
    ms_geo1 = _t_step.restart();

    draw(painter, &m_cxema);
    ms_cxema = _t_step.restart();

    if (isGeo() && m_parent_id == 0) {
        if (!scaling)
        drawGeo(painter, true);
    }
    ms_geo2 = _t_step.restart();

    if (_t_frame.elapsed() >= kSlowFrameMs) {
        qInfo() << "кадр" << _t_frame.elapsed() << "мс:"
                << "карта" << ms_map
                << "подложка" << ms_dxf
                << "геобаза" << (ms_geo1 + ms_geo2)
                << "схема" << ms_cxema;
    }


    if (regim == R_BEGIN_CANAL_2) {
        CFPoint pt = CoordToScreen(m_pt1_canal);
        drawNodeTyp(painter, pt.x, pt.y, 29, Qt::red);
    }


//    painter->drawLine(w, 0, 0, h);

    GidWidget::drawPoints(painter);
}



void GidWidget::draw(QPainter *painter)
{
    if (!IsMain() || !m_cxema.isGeobaza())
        init_draw(painter, 50.);

//    QColor niceBlue(150, 150, 200);
//    painter->setRenderHint(QPainter::Antialiasing, true);

//    QRect rr1 = painter->viewport();
//    QRect rr2 = painter->window();

    double w = painter->device()->width()/m_scale;
    double h = painter->device()->height()/m_scale;

    m_wi = w;
    m_hi = h;

    CRect m_rect(0, 0, w, h);
    CFRect r(0, 0, w, h);

//    m_bk_color = Qt::yellow;

    painter->fillRect(0, 0, w, h, m_bk_color);

    if (m_parent_id == 0) {
        redrawMap(painter, r, m_internetMap, m_reread);
        m_reread = false;
    }

    if (m_bIsPicture && m_dxf && m_parent_id == 0) {
        drawDxf(m_dxf, painter, m_rect, geom.bx, geom.by, geom.masx, geom.masy);
    }

    if (isGeo() && m_parent_id == 0) {
        drawGeo(painter, false);
    }

    draw(painter, &m_cxema);

    if (isGeo() && m_parent_id == 0) {
        drawGeo(painter, true);
    }


    if (regim == R_BEGIN_CANAL_2) {
        CFPoint pt = CoordToScreen(m_pt1_canal);
        drawNodeTyp(painter, pt.x, pt.y, 29, Qt::red);
    }

//    painter->drawLine(w, 0, 0, h);

    GidWidget::drawPoints(painter);
}

void GidWidget::drawLine1(QPainter *painter, const CLINE2* ll, int redr, bool is_text)
{
    if (isRezhim(ll)) {
        return;
    }

    CNode2* n1, * n2;
    CLine2* l;
    int i;

    l = bline(ll);

//    if (l->line.mag != 0 && l->line.mag == m_mag || l->line.ms != 0 && l->line.ms == m_ms || l->line.rs != 0 && l->line.rs == m_rs) redr = 1;

    int podobr = l->line.nomP != -1 ? N_POD : N_OBR;
    if (l->line.nomP != -1 && l->line.nomO != -1) {
        podobr = N_OBA;
    }

    n1 = where(l);
    n2 = other(l);

//    if (IsRezhim(l)) {
//        return;
//    }

    if (n2->node.coord.x == 0 && n2->node.coord.y == 0) return;

    int x1, y1, x2, y2;

    int ris = 1;

    double len = hypot(n1->node.coord.x - n2->node.coord.x, n1->node.coord.y - n2->node.coord.y);

    //  if (IsMain() && m_cxema->isGeobaza()) {
/*
    if (IsMain()) {
        if (len < m_szY * 3) {
            ris = 0;
        }
    }
*/

    int t = (n2->node.coord.y > n1->node.coord.y || (n2->node.coord.y == n1->node.coord.y && n2->node.coord.x > n1->node.coord.x)) ? -1 : 1;

//    if (m_colors.style(l).m_style) {
//        len_dot = 0;
//    }
//    else {
//        len_dot = 40 * m_h;
//    }


    if (l->line.m_NP.size() == 0 || len / geom.masx < 10) {
        x1 = n1->node.coord.x;    y1 = n1->node.coord.y;
        x2 = n2->node.coord.x;    y2 = n2->node.coord.y;

        drawLine2(painter, ll, 0, x1, y1, x2, y2, podobr, redr, is_text, t);
        drawLine2(painter, ll, ris, x1, y1, x2, y2, podobr, redr, is_text, t);
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
                    drawLine2(painter, ll, 0, x, y, it->x, it->y, podobr, redr, is_text, t);
                    drawLine2(painter, ll, ris, x, y, it->x, it->y, podobr, redr, is_text, t);
                }
            }
            else {
                drawLine2(painter, ll, -1, x, y, it->x, it->y, podobr, redr, is_text, t);
            }
            x = it->x;
            y = it->y;
        }
        if (i_pic == l->line.m_NP.size()) {
            drawLine2(painter, ll, 0, x, y, n2->node.coord.x, n2->node.coord.y, podobr, redr, is_text, t);
            drawLine2(painter, ll, ris, x, y, n2->node.coord.x, n2->node.coord.y, podobr, redr, is_text, t);
        }
        else {
            drawLine2(painter, ll, -1, x, y, n2->node.coord.x, n2->node.coord.y, podobr, redr, is_text, t);
        }
    }
}


bool GidWidget::DR_otkr(CLine2* l)
{
//    if (!l || !l->getTableOut()) return false;

    double dru_home = 0;
    if (l->line.nomP > 0) {
        dru_home = l->line.pod.dru_home;
    }
    else if (l->line.nomO > 0) {
        dru_home = l->line.obr.dru_home;
    }

    return dru_home > 0;

//    return false;
}


bool GidWidget::ZD_otkr(CLine2* l)
{
    return true;
}

int m_kod_vyd = -1;
int m_linePTS_ID = -1234;
bool m_isOpressovaka = false;
bool m_isNeOpr = false;
int m_szY = 5;
static bool m_isPodpAll = false;

bool isPassport()
{
    return false;
}


void GidWidget::drawLine2(QPainter* painter, const CLINE2* ll, int ris, double xx1, double yy1, double xx2, double yy2, char podobr, int redr, bool is_text, int t)
{

    if (bline(ll)->line.nomP == 315888 || bline(ll)->line.nomO == 315888) {
        int qqq;
        qqq = 1;
    }
    
    
    if (ris == 1 && !isFull()) return;



    double x1, y1, x2, y2;

    bool strelka = m_bIsNaprGid && !m_bIsPts;

    x1 = xx1 / geom.masx - geom.bx, y1 = yy1 / geom.masy - geom.by,
    x2 = xx2 / geom.masx - geom.bx, y2 = yy2 / geom.masy - geom.by;

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
        strelka = false;
//        len_dot = 0;
        ris = 0;
    }


    //  bool isPjezo2 = where(ll)->node.node_name.kod == m_kod_vyd || other(ll)->node.node_name.kod == m_kod_vyd;
//    bool isPjezo2 = false;
    bool isPjezo2 = m_kod_vyd != -1 && (where(ll)->node.id_kod == m_kod_vyd || other(ll)->node.id_kod == m_kod_vyd);

    bool isY = false;

    if (l->line.pipeSectionID == m_linePTS_ID) { // Участок ПТС
        drawLine00(painter, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 4);
        isY = true;
    }   

    else if (l->line.isPjezoP && !l->line.isPjezoO) { // Выделенные желтым по подаче
        drawLine00(painter, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 8);
        isY = true;
    }
    else if (l->line.isPjezoP && l->line.isPjezoO) { // Выделенные желтым по подаче и по обратке
        drawLine00(painter, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 1);
        isY = true;
    }

    
    else if (l->line.isPjezo) { // Выделенные желтым
        drawLine00(painter, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 1);
        isY = true;
    }

    
    
    else if (isPjezo2) {
        drawLine00(painter, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 2);
        isY = true;
    }
    else if ((l->line.m_opressovka > 0 || m_isNeOpr) && m_isOpressovaka) {  // Опрессовка
        drawLine00(painter, ll, ris, x1, y1, x2, y2, podobr, false, t, typ, 3);
        isY = true;
    }

    if (!isY || mas_otn <= 2) {
        drawLine00(painter, ll, ris, x1, y1, x2, y2, podobr, redr, t, typ, 0);
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

    bool isOpc = !l->line.strOpc1.isEmpty() && l->line.typ == TIP_US;
//    bool isInfo = m_bIsInfo[1] || m_bIsInfo[2] || m_bIsInfo[3] || m_bIsInfo[4]
//        || m_bIsInfo[5] || m_bIsInfo[8] || m_bIsInfo[9] || m_bIsInfo[10] || m_bIsInfo[13];

    bool isInfo = true;


    if (m_szY > 4 && is_text && m_bIsGidrInf && (isInfo || isOpc) && !redr && ris == 1) {
        if (!isPassport()) drawLinePodp(painter, ll, x1, y1, x2, y2);
    }

    if (m_szY > 4 && is_text && m_bIsGidrInf && (!redr || isPassport()) && ris == 1) {

        int l3 = MagRasPar(l);

        if (isInfo && isPassport()) {
            CCoordList cl, m_NP = bline(ll)->line.m_NP;

            CCoordList::const_iterator it;
            it = m_NP.begin();
            cl.push_back(where(l)->node.coord);
            for (; it != m_NP.end(); ++it) {
                cl.push_back(*it);
            }
            cl.push_back(other(l)->node.coord);

//            if (podobr == N_POD) {
//                QString str;
//                if (m_bIsInfo[1]) {
//                    str.Format("%g", bline(ll)->line.pod.diam + bline(ll)->line.pod.tol * 2);
//                    DrawLabelMulty(m_dc, cl, str, 1);
//                }
//                if (m_bIsInfo[0]) {
//                    str.Format("%g", bline(ll)->line.pod.dl);
//                    DrawLabelMulty(painter, cl, str, 0);
//                }
//            }
        }
/*
        if (m_bIsInfo[14] && (l->line.isPodp || m_isPodpAll) && l3 == 0) { // Номер магистрали
            CCoordList cl, m_NP = bline(ll)->line.m_NP;

            CCoordList::const_iterator it;
            it = m_NP.begin();
            cl.push_back(where(l)->node.coord);
            for (; it != m_NP.end(); ++it) {
                cl.push_back(*it);
            }
            cl.push_back(other(l)->node.coord);

//            if (podobr == N_POD) DrawLabelMulty(painter, cl, where(ll)->node.node_name.kod, 1);
        }
*/
    }
}




void GidWidget::draw_pr(QPainter *painter, const QString & cxema, int w, int h)
{
    if (!painter) return;


//    double w = painter->device()->width()/m_scale;
//    double h = painter->device()->height()/m_scale;

    CFRect rr, rectScr = ScreenToCoord(CFRect(0, 0, w, h));
    CFRect rectScr2 = rectScr;

    init_draw(painter, getStdMas());

    for (auto& p : m_cxema.m_graph->map_node) {
        CNode2* pp = p.second;

        for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
            if (IsBegin(l)) {
                CNode2 *n1 = where(l);
                CNode2 *n2 = other(l);
                if (n1 && n2 && n1->node.coord.x != 0 && n2->node.coord.x != 0 && n1->node.internalNodeID == m_parent_id) {
                    CFRect rl(n1->node.coord, n2->node.coord);
                    rl.NormalizeRect();
                    if (rr.IntersectRect(rectScr2, rl)) {
                        drawLine1(painter, l, 0, true);
                    }
                }
            }
        }
    }


    for (auto& p : m_cxema.m_graph->map_node) {
        CNode2* pp = p.second;
        if (pp->node.internalNodeID == m_parent_id && rectScr2.PtInRect(pp->node.coord) ) {
            drawNode(painter, &(*pp), 0, true);
        }
    }
}

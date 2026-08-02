#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <gidview/GidWidget.h>
#include <gidview/colorgtd.h>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>


#include "Pjezo.h"
#include "db/db.h"

#include <any/MyMain.h>

//#include "mmenu.h"
#include <math.h>

//#include "excel.h"
//#include "win.h"
//#include "ado.h"

//#include "wm_user.h"

const double PIHNULL = -100000;

void drawText1(QPainter* painter, double x, double y, int flags, const QString & str, double angle)
{
    if (angle != 0) {
        painter->save();
        painter->translate(x, y);
        painter->rotate(angle);
        painter->translate(-x, -y);
    }

//    QRectF r2(x, y, 0, 0);
    QRectF r2(x, y, 0., 0.);
    painter->drawText(r2, flags,  str);

    if (angle) {
        painter->restore();
    }
}

CPjezo::CPjezo(CCxema *_cxema, QWidget *parent, GidWidget *_gid)
    : Scroll(parent)
//    , okno(this)
{

    gid = _gid;

    setFocusPolicy( Qt::StrongFocus );
    setFocus ( Qt::MouseFocusReason );

    //  m_gid_file = NULL;
    m_mas = 1;
    m_n = 0;
    m_napor = true;
    m_vysot = true;
    m_otmet = true;
    m_okno = true;
    m_stat = true;
    m_real = false;
    m_dbl = false;

    m_podacha = true;
    m_obratka = true;
    m_fact = true;

//    m_bIsFragment = true;

    m_calculationID = 0;
    m_fileID = 0;

    m_graph2 = nullptr;
    m_cxema = _cxema;

    for (int i = 0; i < sizeof(m_bIsInfo) / sizeof(m_bIsInfo[0]); i++) {
        QString sss;
//        sss = QString("IsInfoP%d").arg(i);
//        m_bIsInfo[i] = AfxGetApp()->GetProfileInt(szSection, sss, 1);
        m_bIsInfo[i] = true;
    }

    createActions();
    createMenu((MyMain*)parent);
    createToolBar((MyMain*)parent);


//    restoreFlags();

    QSettings settings;


    m_font = QFont("Arial", 8);

    m_font =      qvariant_cast<QFont>(settings.value("flags/font_pjezo", m_font));


    initPjezo();

    okno = new QMessageBox(this);
    okno->setWindowModality(Qt::NonModal);
//    okno->setParent(this);
//    okno->setAttribute(Qt::WA_DeleteOnClose);
    okno->setWindowFlags(Qt::WindowDoesNotAcceptFocus|Qt::Dialog|Qt::WindowStaysOnTopHint|Qt::MSWindowsFixedSizeDialogHint);
    okno->show();
}

CPjezo::~CPjezo()
{
    if (m_graph2) delete m_graph2;

    for (int i = 0; i < sizeof(m_bIsInfo) / sizeof(m_bIsInfo[0]); i++) {
        QString sss;
//        sss.Format("IsInfoP%d", i);
//        AfxGetApp()->WriteProfileInt(szSection, sss, m_bIsInfo[i]);
    }
}

void CPjezo::hideEvent(QHideEvent *event)
{    
//    save.save();

    okno->hide();

    QSettings settings;

    settings.setValue("flags/font_pjezo",  m_font);

    Scroll::hideEvent(event);
}


//void SaveFont(const TCHAR* szSection, const LOGFONT& lf, COLORREF color);
//bool LoadFont(const TCHAR* szSection, LOGFONT& lf, COLORREF& color);

void CPjezo::House(QPainter* painter, int x, int y, int hz)
{
    int col = 0xaFaF00;

    painter->setPen(QPen(QColor(col), 1));
    painter->setBrush(QBrush(QColor(col)));

    double et = 1.5;

//    painter->drawRect(x - 4 * m_h, y, x + 4 * m_h, y - hz * h / (max_pih - min_pih));
    painter->drawRect(x - 4 * m_h, y, 8 * m_h, - hz * h / (max_pih - min_pih));
    for (double hh = 0; hh < hz; hh += et * 2) {
        double hhh1 = y - hh * h / (max_pih - min_pih);
        double hhh2 = y - (hh + et) * h / (max_pih - min_pih);
        hhh2 = max(hhh2, y - hz * h / (max_pih - min_pih));
//        painter->drawRect(x - 4 * m_h, hhh1, x + 4 * m_h, hhh2);
        painter->drawRect(x - 4 * m_h, hhh1, 8 * m_h, hhh2-hhh1);
//        fillrectangle(painter, x - 4 * m_h, hhh1, x + 4 * m_h, hhh2);
        //    line(x-4*m_h, hhh, x+4*m_h, hhh);
    }

    int flags = Qt::AlignCenter|Qt::AlignBottom|Qt::TextDontClip;
    double hhh2 = y - hz * h / (max_pih - min_pih);
//    painter->drawText(x, hhh2,  QString("%1").arg(hz));
    drawText1(painter, x, hhh2, flags, QString("%1").arg(hz), 0);
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
    else
        pih = 0;

/*
    else if (typ == 7)
        pih = m_vnode[i].rn[nras].pP_old != 0 ? m_vnode[i].rn[nras].pP_old + m_vnode[i].rn[nras].geodz : 0;
    else if (typ == 8)
        pih = m_vnode[i].rn[nras].pO_old != 0 ? m_vnode[i].rn[nras].pO_old + m_vnode[i].rn[nras].geodz : 0;
*/
    return pih;
}

void CPjezo::drawGr(QPainter* painter, int color, int typ, int nras)
{
    int i;
    int len;
    int y;
    double pih, pih1, pih2;
    int xx1, yy1, xx2, yy2;

    if (typ == 3) {
        QVector<QPointF> xy;
        painter->setPen(QPen(QColor(color), 1));

//        CBrush brush1(HS_BDIAGONAL, 0x007F00), * oldbrush;
        painter->setBrush(QBrush(QColor(0x007F00), Qt::BDiagPattern));

        for (i = 0; i < m_vnode.size(); i++) {
            pih = m_vnode[i].rn[nras].h;
            len = m_vnode[i].rn[nras].len * w / m_len;
            y = (pih - min_pih) * h / (max_pih - min_pih);

            xy.push_back(QPointF(m_x1 - geom.bx + len, m_y2 - geom.by - y));
        }
        xy.push_back(QPointF(m_x2, m_y2));
        xy.push_back(QPointF(m_x1, m_y2));

        painter->drawPolygon(xy);
    }


    if (typ == 5 || typ == 6 || typ == 7 || typ == 8) {
        int f = 0;
        for (i = 0; i < m_vnode.size(); i++) {
            //      painter->setPen(QPen(QColor(color, 0), PS_DASH));
            painter->setPen(QPen(QColor(color), 3));
            len = m_vnode[i].rn[nras].len * w / m_len;

            xx2 = m_x1 - geom.bx + len;
            pih1 = getPih(i, typ, true, nras);

            if (pih1 != 0) {
                y = (pih1 - min_pih) * h / (max_pih - min_pih);

                yy2 = m_y2 - geom.by - y;

                if (f != 0) painter->drawLine(xx1, yy1, xx2, yy2);
                xx1 = xx2;
                yy1 = yy2;
                f = 1;
            }
        }
    }
    else {
        for (i = 0; i < m_vnode.size(); i++) {
            painter->setPen(QPen(QColor(color), 3));

            CPLine* l1;
            if (i > 0) {
                if (typ == 1) {
                    l1 = &m_vlineP[i - 1];
                    if (l1->rl[nras].g == 0) painter->setPen(QPen(QColor(GREEN), 3));
                }

                if (typ == 2) {
                    l1 = &m_vlineO[i - 1];
                    if (l1->rl[nras].g == 0) painter->setPen(QPen(QColor(GREEN), 3));
                }
            }


            len = m_vnode[i].rn[nras].len * w / m_len;

            xx2 = m_x1 - geom.bx + len;

            ///double pih11 = getPih(i, 1, true, 0);
            ///double pih21 = getPih(i, 1, false, 0);
            ///double pih12 = getPih(i, 1, true, 1);
            ///double pih22 = getPih(i, 1, false, 1);



            pih1 = getPih(i, typ, true, nras);
            pih2 = getPih(i, typ, false, nras);

            if (pih2 != PIHNULL) {
                y = (pih1 - min_pih) * h / (max_pih - min_pih);

                yy2 = m_y2 - geom.by - y;

                if (i != 0) painter->drawLine(xx1, yy1, xx2, yy2);
                xx1 = xx2;
                yy1 = yy2;

                y = (pih2 - min_pih) * h / (max_pih - min_pih);
                yy2 = m_y2 - geom.by - y;
                if (i != 0) painter->drawLine(xx1, yy1, xx2, yy2);

                if (typ == 4 && m_vnode[i].rn[nras].hz != 0 && m_vysot) {
                    painter->setPen(QPen(QColor(color), 1));
                    House(painter, xx2, yy2, m_vnode[i].rn[nras].hz);
                }
                xx1 = xx2;
                yy1 = yy2;
            }
        }
    }

    for (i = 0; i < m_vnode.size(); i++) {
        len = m_vnode[i].rn[nras].len * w / m_len;

        pih = getPih(i, typ, true, nras);

        y = (pih - min_pih) * h / (max_pih - min_pih);

        xx2 = m_x1 - geom.bx + len;
        yy2 = m_y2 - geom.by - y;

        if ((typ != 5 && typ != 6) || pih != 0) {
            painter->setPen(QPen(QColor(color), 1));
            painter->drawEllipse(QPointF(xx2, yy2), 3 * m_h, 3 * m_h);
        }

        if ((m_napor && (typ == 1 || typ == 2 || typ == 5 || typ == 6 || typ == 7 || typ == 8)) || (m_otmet && typ == 3)) {
            QString str = QString("%1").arg(((typ == 3 || typ == 4) ? pih : pih - m_vnode[i].rn[nras].geodz), 0, 'f', 0);
            painter->setFont(fontH);
            int flags = Qt::AlignLeft|Qt::AlignBottom|Qt::TextDontClip;

            QRect r1;
            QRectF r = painter->boundingRect ( r1, flags, str);

            double cy = (i == 0) ? 0 : m_vlineP[i - 1].rl[nras].len * w / m_len;

            if (i == 0 || i == m_vnode.size() - 1 || r.height() < cy) {
//                painter->drawText(xx2, yy2, str);
                drawText1(painter, xx2, yy2, flags, str, 0);
            }
        }
    }
}

QString Form_old(double v1, double v2, const QString & b, const QString & frm, const QString & izm)
{
    QString str = "";
    QString f;

    if (v1 != v2) {
//        f = QString("%%s=%s,%s %%s").arg(frm, frm);
        str = QString("%1=%2,%3 %4").arg(b).arg(v1).arg(v2).arg(izm);
    }
    else {
//        f = QString("%%s=%s %%s").arg(frm);
        str = QString("%1=%2 %3").arg(b).arg(v1).arg(izm);
//        str.Format(f, b, v1, izm);
    }
    return str;
}


QString Form(double v1, double v2, const QString & b, int frm, const QString & izm)
{
    QString str = "";
    QString f;

    if (v1 != v2) {
//        f = QString("%%s=%s,%s %%s").arg(frm, frm);
        str = QString("%1=%2,%3 %4")
            .arg(b)
            .arg(v1, 0, 'f', frm)
            .arg(v2, 0, 'f', frm)
            .arg(izm);
    }
    else {
//        f = QString("%%s=%s %%s").arg(frm);
        str = QString("%1=%2 %3")
            .arg(b)
            .arg(v1, 0, 'f', frm)
            .arg(izm);
//        str.Format(f, b, v1, izm);
    }
    return str;
}


void CPjezo::drawSetka(QPainter* painter)
{
//    CSize sz;
    int cy;
    int i;
    int len = 0;
    QString str;

    painter->setFont(fontV);

    int flags = Qt::AlignLeft|Qt::AlignBottom|Qt::TextDontClip;

    int l_podp = 60 * m_h;
    for (i = 0; i < m_vnode.size(); i++) {
        QRect r1;
        QRectF r = painter->boundingRect ( r1, flags, m_vnode[i].rn[0].n->getName());

//        sz = painter->GetTextExtent(m_vnode[i].rn[0].n->getName());
        l_podp = max(l_podp, r.width());
    }

    m_x2 -= 30 * m_h;
    m_y2 -= (l_podp + m_h);

    painter->setPen(QPen(Qt::black));
//    painter->drawRect(m_x1 - geom.bx, m_y1 - geom.by, m_x2 - geom.bx, m_y2 - geom.by);
    painter->drawRect(m_x1 - geom.bx, m_y1 - geom.by, m_x2-m_x1, m_y2-m_y1);

    w = (m_x2 - m_x1);
    h = (m_y2 - m_y1);

    if (m_stat) {
        painter->setPen(QPen(QColor(0x00007F), 2));
        int y = (h_max - min_pih + 5) * h / (max_pih - min_pih);
        y = m_y2 - geom.by - y;
        painter->drawLine(m_x1 - geom.bx, y, m_x2 - geom.bx, y);
    }

//    CPen pen1(PS_DOT, 1, (COLORREF)0), * oldpen;

    painter->setPen(QPen(Qt::black, 1, Qt::DotLine));

    for (i = 0; i < m_vnode.size() - 1; i++) {
        len = m_vnode[i].rn[0].len * w / m_len;
        painter->drawLine(m_x1 - geom.bx + len, m_y1 - geom.by, m_x1 - geom.bx + len, m_y2 - geom.by);
    }

    painter->setFont(fontH);
    flags = Qt::AlignRight|Qt::AlignBottom|Qt::TextDontClip;
    //painter->SetTextAlign(TA_RIGHT | TA_BASELINE);

    bool ch;
    double pih;

    for (pih = min_pih, ch = true; pih < max_pih; pih += 10., ch = !ch) {
        int y = (pih - min_pih) * h / (max_pih - min_pih);
        y = m_y2 - geom.by - y;
        painter->drawLine(m_x1 - geom.bx, y, m_x2 - geom.bx, y);

        if (ch) {
            str = QString("%1").arg(pih, 0, 'f', 0);
//            painter->drawText(m_x1 - geom.bx - 5, y, str);
            drawText1(painter, m_x1 - geom.bx - 5, y, flags, str, 0);
        }
    }

    flags = Qt::AlignCenter|Qt::AlignTop|Qt::TextDontClip;
    //painter->SetTextAlign(TA_CENTER | TA_TOP);


    QString  s2, s3, s4;
    QString s1 = QString("D=8888,8888 т/ч");
//    sz = painter->GetTextExtent(s1);
    QRect r1;
    QRectF r = painter->boundingRect ( r1, flags, s1);

    for (i = 0; i < m_vnode.size(); i++) {
        CPLine* l1, * l2;
        l1 = &m_vlineP[i];
        l2 = &m_vlineO[i];

        if ((m_vlineP[i].rl[0].len - 2) * w / m_len > r.width()) {
            int x = (m_vnode[i].rn[0].len + m_vlineP[i].rl[0].len / 2) * w / m_len + m_x1 - geom.bx;
            int yy2 = m_y2 + r.height() * 0.5;

            if (m_bIsInfo[0]) {
                s1 = Form(l1->rl[0].diam, l2->rl[0].diam, "D", 0, tr("мм"));
                drawText1(painter, x, yy2, flags, s1, 0);
                yy2 += r.height();
            }

            if (m_bIsInfo[1]) {
                if (fabs(l1->rl[0].g) < 10. && fabs(l2->rl[0].g) < 10.) {
                    s2 = Form(l1->rl[0].g, l2->rl[0].g, "G", 1, tr("т/ч"));
                }
                else {
                    s2 = Form(l1->rl[0].g, l2->rl[0].g, "G", 0, tr("т/ч"));
                }
                drawText1(painter, x, yy2, flags, s2, 0);
                yy2 += r.height();
            }

            if (m_bIsInfo[2]) {
                s3 = Form(l1->rl[0].len, l2->rl[0].len, "L", 0, tr("м"));
                drawText1(painter, x, yy2, flags, s3, 0);
                yy2 += r.height();
            }

            if (m_bIsInfo[3]) {
                s4 = Form(m_vnode[i].rn[0].len, m_vnode[i].rn[0].lenO, "R", 0, tr("м"));
                drawText1(painter, x, yy2, flags, s4, 0);
                yy2 += r.height();
            }

            if (m_bIsInfo[4]) {
                s4 = Form(l1->rl[0].a14, l2->rl[0].a14, "Pуд", 1, tr("мм/м"));
                drawText1(painter, x, yy2, flags, s4, 0);
                yy2 += r.height();
            }
        }
    }

    painter->setFont(fontV);

    flags = Qt::AlignRight|Qt::AlignTop|Qt::TextDontClip;

    // Вывод названий узлов

    for (i = 0; i < m_vnode.size(); i++) {
        len = m_vnode[i].rn[0].len * w / m_len;
        if (i != 0) cy = m_vlineP[i - 1].rl[0].len * w / m_len;
        str = m_vnode[i].rn[0].n->getName();
        QRect r1;
        QRectF r = painter->boundingRect ( r1, flags, str);

        if (i == m_vnode.size() - 1 || i == 0 || (r.height() < cy && !str.isEmpty() && str.left(1) != "#")) {
            drawText1(painter, m_x1 - geom.bx + len, m_y2 - geom.by + 5, flags, str, -90);
        }
    }

    painter->setFont(fontH);
    //painter->SetTextAlign(TA_LEFT);

    flags = Qt::AlignLeft|Qt::AlignTop|Qt::TextDontClip;

    str = QString(tr("Пьезометрический график, направление %1 - %2")).arg(m_vnode[0].rn[0].n->getName(), m_vnode[m_vnode.size() - 1].rn[0].n->getName());
//    painter->drawText(m_x1 - geom.bx + 40 * m_h, m_y1 - 20 * m_h, str);
//    drawText1(painter, m_x1 - geom.bx + 40 * m_h, m_y1 - 20 * m_h, flags, str, 0);
    drawText1(painter, m_x1 - geom.bx + 40 * m_h, m_y1 - 40 * m_h, flags, str, 0);

    flags = Qt::AlignCenter|Qt::AlignTop|Qt::TextDontClip;
    str = tr("Шкала полных напоров м.вод.ст");

//    drawText1(painter, m_x1 - 50 * m_h - geom.bx, (m_y1 + m_y2) / 2, flags, str, -90);
    drawText1(painter, m_x1 - 80 * m_h - geom.bx, (m_y1 + m_y2) / 2, flags, str, -90);
}

void CPjezo::drawOkno(QPainter* painter)
{
//    if (!okno->m_hWnd) return;

    QString str;
    CNode2* n1 = m_vnode[m_n].rn[0].n, * n2 = m_vnode[m_n + 1].rn[0].n;
    CPLine* l1, * l2;

    l1 = &m_vlineP[m_n];
    l2 = &m_vlineO[m_n];


    if (m_dbl) {
        str = QString(
            tr("%1\r\n"
                "%2\r\n"
                "D=%3,%4 мм\r\n"
                "G=%5,%6 т/ч\r\n"
                "L=%7,%8 м\r\n"
                "R=%9,%10 м\r\n\r\n"
                "Hп=%11 м.вод.ст.\r\n"
                "Hо=%12 м.вод.ст.\r\n\r\n"
                "Hп=%13 м.вод.ст.\r\n"
                "Hо=%14 м.вод.ст.\r\n\r\n"
                "Отм.мес=%15 м\r\n"
                "V=%16,%17 м/c\r\n"
                "hуд.л=%18,%19 м.вод.ст.\r\n"
            ))
            .arg(n1->getName())
            .arg(n2->getName())
            .arg(l1->rl[0].diam, l2->rl[0].diam)
            .arg(l1->rl[0].g, l2->rl[0].g)
            .arg(l1->rl[0].len, l2->rl[0].len)
            .arg(m_vnode[m_n].rn[0].len, m_vnode[m_n].rn[0].lenO)
            .arg(m_vnode[m_n + 1].rn[0].pihP1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihP1)
            .arg(m_vnode[m_n + 1].rn[0].pihO1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihO1)

            .arg(m_vnode[m_n + 1].rn[1].pihP1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[1].pihP1)
            .arg(m_vnode[m_n + 1].rn[1].pihO1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[1].pihO1)

            .arg(m_vnode[m_n + 1].rn[0].h)
            .arg(l1->rl[0].w)
            .arg(l2->rl[0].w)
            .arg(l1->rl[0].a14)
            .arg(l2->rl[0].a14)
        ;
    }
    else {
        if (m_vnode[m_n].rn[0].pP_fact || m_vnode[m_n].rn[0].pO_fact) {

            str = QString(
                tr("%1\r\n"
                    "%2\r\n"
                    "D=%3,%4 мм\r\n"
                    "G=%5,%6 т/ч\r\n"
                    "L=%7,%8 м\r\n"
                    "R=%9,%10 м\r\n\r\n"
                    "Hп=%11 м.вод.ст.\r\n"
                    "Hо=%12 м.вод.ст.\r\n\r\n"
                    "Hп факт=%13 м.вод.ст.\r\n"
                    "Hо факт=%14 м.вод.ст.\r\n\r\n"
                    "Отм.мес=%15 м\r\n"
                    "V=%16,%17 м/c\r\n"
                    "hуд.л=%18,%19 м.вод.ст.\r\n"
                ))
                .arg(n1->getName())   // 1
                .arg(n2->getName())   // 2
                .arg(l1->rl[0].diam)   // 3
                .arg(l2->rl[0].diam)   // 4
                .arg(l1->rl[0].g)   // 5
                .arg(l2->rl[0].g)   // 6
                .arg(l1->rl[0].len)   // 7
                .arg(l2->rl[0].len)   // 8
                .arg(m_vnode[m_n].rn[0].len)   // 9
                .arg(m_vnode[m_n].rn[0].lenO)   // 10
                .arg(m_vnode[m_n + 1].rn[0].pihP1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihP1)   // 11
                .arg(m_vnode[m_n + 1].rn[0].pihO1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihO1)   // 12
                .arg(m_vnode[m_n + 1].rn[0].pP_fact)   // 13
                .arg(m_vnode[m_n + 1].rn[0].pO_fact)   // 14
                .arg(m_vnode[m_n + 1].rn[0].h)   // 15
                .arg(l1->rl[0].w)   // 16
                .arg(l2->rl[0].w)   // 17
                .arg(l1->rl[0].a14) // 18
                .arg(l2->rl[0].a14) // 19
                    ;
        }
        else {
            str = QString(tr(
                "%1\r\n"
                "%2\r\n"
                "D=%3,%4 мм\r\n"
                "G=%5,%6 т/ч\r\n"
                "L=%7,%8 м\r\n"
                "R=%9,%10 м\r\n\r\n"
                "Hп=%11 м.вод.ст.\r\n"
                "Hо=%12 м.вод.ст.\r\n\r\n"
                "Отм.мес=%13 м\r\n"
                "V=%14,%15 м/c\r\n"
                "hуд.л=%16,%17 м.вод.ст.\r\n"
            ))
                .arg(n1->getName())   // 1
                .arg(n2->getName())   // 2
                .arg(l1->rl[0].diam)   // 3
                .arg(l2->rl[0].diam)   // 4
                .arg(l1->rl[0].g)   // 5
                .arg(l2->rl[0].g)   // 6
                .arg(l1->rl[0].len)   // 7
                .arg(l2->rl[0].len)   // 8
                .arg(m_vnode[m_n + 1].rn[0].len)   // 9
                .arg(m_vnode[m_n + 1].rn[0].lenO)   // 10
                .arg(m_vnode[m_n + 1].rn[0].pihP1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihP1)   // 11
                .arg(m_vnode[m_n + 1].rn[0].pihO1 == PIHNULL ? 0 : m_vnode[m_n + 1].rn[0].pihO1)   // 12
                .arg(m_vnode[m_n + 1].rn[0].h)   // 13
                .arg(l1->rl[0].w)   // 14
                .arg(l2->rl[0].w)   // 15
                .arg(l1->rl[0].a14) // 16
                .arg(l2->rl[0].a14) // 17
                    ;
        }
    }

    if (okno->isVisible()) {
        okno->setText(str);
//        okno->setInformativeText(str);
    }

    if (l1->rl[0].l) {
        okno->setWindowTitle(linef[l1->rl[0].l->line.typ].opis);
//        okno->SetText(str);
    }
    else if (l2->rl[0].l) {
        okno->setWindowTitle(linef[l2->rl[0].l->line.typ].opis);
//        okno->SetText(str);
    }
}

void CPjezo::draw(QPainter* painter)
{
    int mas = m_mas;
//    double bx = geom.bx, by = geom.by;

    if (painter->device()->devType() == QInternal::Printer) {
        width1 = painter->device()->width() * mas;
        height1 = painter->device()->height();
    }
    else {
        width1 = width()*mas;
        height1 = height();
    }
    draw_rect(painter, width1, height1, m_mas);

}

void CPjezo::draw_rect(QPainter* painter, int width, int height, int mas)
{
//    int mas = m_mas;
//    double bx = geom.bx, by = geom.by;

    width1 = width;
    height1 = height;

    /*
    if (painter->device()->devType() == QInternal::Printer) {
        width1 = painter->device()->width() * mas;
        height1 = painter->device()->height();
    }
    else {
        width1 = width()*mas;
        height1 = height();
    }
    */

    m_pt1 = CFPoint(0, 0);
    m_pt2 = ScreenToCoord(CFPoint(width1, height1));

//    fontH = QFont("Arial", 8);
//    fontV = QFont("Arial", 8);

    fontH = m_font;
    fontV = m_font;

    fontH.setPointSize(fontH.pointSize()*m_coef);
    fontV.setPointSize(fontV.pointSize()*m_coef);

    painter->setFont(fontV);
//    sz = painter->GetTextExtent("0");

    nkx = 60 * m_h;
    nky = 30 * m_h;

    int flags = Qt::AlignCenter|Qt::AlignBottom|Qt::TextDontClip;
    QRectF r1(0,0,0,0);

    QRectF r = painter->boundingRect ( r1, flags, "8888888");

    nkx = r.width()*2;
    nky = r.height()*4;

    m_x1 = nkx;  m_y1 = nky;  m_x2 = width1 - 20 * m_h;  m_y2 = height1 - 20 * m_h;

    int n_podp = 0;
    for (int i = 0; i < 5; i++) {
        n_podp += m_bIsInfo[i] ? 1 : 0;
    }

    double sz_cy = 10;

    m_y2 = height1 - max(20 * m_h, n_podp * sz_cy);

    QString str;

//    fontH = QFont("Arial", 8);
//    fontV = QFont("Arial", 8);

//    LOGFONT lf;
//    COLORREF color = 0;

//    fontH.CreatePointFont(100, _T("Arial"));
//    fontH.GetLogFont(&lf);

//    LoadFont(_T("Font Pjezo"), lf, color);

//    lf.lfHeight *= m_h;
//    lf.lfCharSet = DEFAULT_CHARSET;

//    fontH.CreateFontIndirect(&lf);

//    lf.lfEscapement = 900;
//    lf.lfOrientation = 900;

//    fontV.CreateFontIndirect(&lf);

    drawSetka(painter);

    drawGr(painter, BLACK, 3, 0);
    drawGr(painter, GREEN, 4, 0);

    if (m_podacha) drawGr(painter, RED, 1, 0);
    if (m_obratka) drawGr(painter, BLUE, 2, 0);

    if (m_real) {
        drawGr(painter, YELLOW, 5, 0);
        drawGr(painter, CYAN, 6, 0);
    }


    if (m_dbl) {
//        if (m_podacha) drawGr(painter, YELLOW, 7, 1);
//        if (m_obratka) drawGr(painter, CYAN, 8, 1);
        if (m_podacha) drawGr(painter, YELLOW, 1, 1);
        if (m_obratka) drawGr(painter, CYAN, 2, 1);
    }

    m_x1 = nkx;  m_y1 = nky;  m_x2 = width1 - 20 * m_h;
//    m_y2 = height1 - 20 * m_h;

//    m_y2 = height();
//    m_y2 = height() - max(20 * m_h, n_podp * sz_cy);
//    drawSetka(painter);

    painter->setPen(QPen(QColor(YELLOW), 3));
    //  painter->setPen(QPen(QColor(RED), 3));
    int xb, xe;
    xb = m_x1 + m_vnode[m_n].rn[0].len * w / m_len;
    xe = xb + m_vlineP[m_n].rl[0].len * w / m_len;

    painter->drawLine(xb - geom.bx, m_y2 - geom.by, xe - geom.bx, m_y2 - geom.by);

    if (m_vlineP[m_n].rl[0].len < 1) {
        painter->drawEllipse(QPointF(xb, m_y2), 2 * m_h, 2 * m_h);
    }

    drawOkno(painter);
}

int naprP(CGraph2* m_graph, const CNode2* n1, const CNode2* n2)
{
    CNode2* nn1 = m_graph->findPO(n1->id, CPOD);
    CNode2* nn2 = m_graph->findPO(n2->id, CPOD);

    CLINE2* LP = m_graph->find_line(nn1, nn2, CPOD);

    if (LP) return (bline(LP)->line.pod.q >= 0 && IsBegin(LP)) || (bline(LP)->line.pod.q < 0 && !IsBegin(LP));

    return 1;
}

int naprO(CGraph2* m_graph, const CNode2* n1, const CNode2* n2)
{
    CNode2* nn1 = m_graph->findPO(n1->id, COBR);
    CNode2* nn2 = m_graph->findPO(n2->id, COBR);

    CLINE2* LO = m_graph->find_line(nn1, nn2, COBR);

    if (LO) return (bline(LO)->line.obr.q >= 0 && IsBegin(LO)) || (bline(LO)->line.obr.q < 0 && !IsBegin(LO));

    return 0;
}

#if 0

void CPjezo::onInitialUpdate()
{
    CScroll::OnInitialUpdate();

    for (int i = 0; i < sizeof(m_bIsInfo) / sizeof(m_bIsInfo[0]); i++) {
        QString sss;
        sss = QString("IsInfoP%d").arg(i);
        m_bIsInfo[i] = AfxGetApp()->GetProfileInt(szSection, sss, 1);
    }

    GetParentFrame()->SetWindowText(tr("Пьезометр"));

    initPjezo();
    //  CRect rect(100, 100, 240, 360);
    CRect rect(100, 100, 280, 360);
    okno->Create(rect, this, 1999);
    okno->SetWindowText("");
}
#endif


void CPjezo::readUS_OUT(CGraph2 *m_graph, int num, int nras)
{
    int k;
    CNode2* n;

    for (k = 0; k < m_vnode.size(); k++) {

        //    LO = m_graph->find_line(m_graph->findPO(*n1, COBR), m_graph->findPO(*n2, COBR), COBR);

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

                auto pp = m_cxema->m_graph->map_node.begin();
                while (pp != m_cxema->m_graph->map_node.end()) {
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

                auto pp = m_cxema->m_graph->map_node.begin();
                while (pp != m_cxema->m_graph->map_node.end()) {
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
    m_graph = m_cxema->m_graph;

    {
        h_max = -1e10;
        auto p = m_graph->map_node.begin();

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
    QString str, ss;
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
        LP = m_graph->find_line(m_graph->findPO(n1->id, CPOD), m_graph->findPO(n2->id, CPOD), CPOD);
        LO = m_graph->find_line(m_graph->findPO(n1->id, COBR), m_graph->findPO(n2->id, COBR), COBR);


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

            m_vnode[k+1].rn[0].pihP1 = PIHNULL;
            m_vnode[k+1].rn[0].pihP2 = PIHNULL;

            m_vnode[k+1].rn[0].len = m_len;
            m_vnode[k+1].rn[0].lenP = lenP;
            m_vnode[k+1].rn[0].lenO = lenO;
            m_vnode[k+1].rn[0].vP = vP;
            m_vnode[k+1].rn[0].vO = vO;
        }

        if (m_dbl)  {
        if (m_graph2) {
            LP = m_graph2->find_line(m_graph2->findPO(n1_2->id, CPOD), m_graph2->findPO(n2_2->id, CPOD), CPOD);
            LO = m_graph2->find_line(m_graph2->findPO(n1_2->id, COBR), m_graph2->findPO(n2_2->id, COBR), COBR);
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

            m_vnode[k+1].rn[1].pihP1 = PIHNULL;
            m_vnode[k+1].rn[1].pihP2 = PIHNULL;

            m_vnode[k+1].rn[1].len = m_len2;
            m_vnode[k+1].rn[1].lenP = lenP2;
            m_vnode[k+1].rn[1].lenO = lenO2;
            m_vnode[k+1].rn[1].vP = vP2;
            m_vnode[k+1].rn[1].vO = vO2;
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
//      readUS_OUT(m_graph2, 1, 1);
    }
  //  readUS_OUT("US_OUT2", 2);

    max_pih = floor(max_pih / 10.) * 10. + 10.;
    min_pih = floor(min_pih / 10.) * 10.;
}

void CPjezo::onPlus()
{
    m_mas = min(10, m_mas + 1);
    repaint();
}

void CPjezo::onMinus()
{
    m_mas = max(1, m_mas - 1);
    repaint();
}

void CPjezo::onMasall()
{
    m_mas = 1;
    geom.bx = geom.by = 0;
    repaint();
}

void CPjezo::keyPressEvent(QKeyEvent *event)
{
    const int Dx = 160, Dy = 160;

    switch (event->key()) {
    case Qt::Key_Left:
        m_n = max(0, m_n - 1);
        geom.bx = min(geom.bx, m_vnode[m_n].rn[0].len * w / m_len);
        repaint();
        break;
    case Qt::Key_Right:
        m_n = min(m_vnode.size() - 2, m_n + 1);
        geom.bx = max(geom.bx, (m_vnode[m_n + 1].rn[0].len) * w / m_len - width() + 20 + nkx);
        repaint();
        break;
    case Qt::Key_Home:
        m_n = 0;
        geom.bx = 0;
        repaint();
        break;
    case Qt::Key_End:
        m_n = m_vnode.size() - 2, m_n + 1;
        geom.bx = max(geom.bx, (m_vnode[m_n + 1].rn[0].len) * w / m_len - width() + 20 + nkx);
        repaint();
        break;
    default :
        QWidget::keyPressEvent(event);
    }

}

//void CPjezo::onUpdateNapor(CCmdUI* pCmdUI)
//{
//    pCmdUI->SetCheck(m_napor);
//}



//void CPjezo::onUpdateOtmet(CCmdUI* pCmdUI)
//{
//    pCmdUI->SetCheck(m_otmet);
//}

void CPjezo::onOkno()
{
    if (okno->isVisible()) {
        okno->hide();
        m_okno = false;
    }
    else {
        okno->show();
        m_okno = true;
    }
}

//void CPjezo::onUpdateOkno(CCmdUI* pCmdUI)
//{
//    m_okno = okno->IsWindowVisible();
//    pCmdUI->SetCheck(m_okno);
//}


//void CPjezo::onUpdateVysot(CCmdUI* pCmdUI)
//{
//    pCmdUI->SetCheck(m_vysot);
//}

void CPjezo::onFilePrint() // Печать...\tCtrl+P
{
#ifndef QT_NO_PRINTER
    QPrinter pr(QPrinter::HighResolution);
    pr.setPageOrientation(QPageLayout::Landscape);

    QPrintDialog dlg(&pr, this);

    if (dlg.exec() == QDialog::Accepted) {
        QPainter painter(&pr);
        draw(&painter);
    }
#endif
}


void CPjezo::printPreview(QPrinter *printer)
{
#ifndef QT_NO_PRINTER
    QPainter painter(printer);
    draw(&painter);
#endif
}



void CPjezo::onFilePrintPreview() // Предварительный просмотр
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Landscape);
    QPrintPreviewDialog previewDialog(&printer, this);
    previewDialog.setWindowFlags ( Qt::Window );
    connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPreview(QPrinter *)));
    previewDialog.exec();
#endif
}



/*

*/
void CPjezo::CopyBlockP(CFRect rect)
{
}

void CPjezo::onEditCopy()
{
//    int w1 = 1000;
//    int h1 = 500;

//    29.7*96=2851.2
//    21*96=2016

//    int w1 = 2800;
//    int h1 = 2000;


//2325/300*2.54=19.685

//297*mas
//210/2.54*300=2480

//3150/300*2.54=26.67

//    int w1 = 3150*mas;
//    int h1 = 2325;
//    int h1 = 2325;

    int w1 = (297*m_mas-40)*300/25.4;
    int h1 = (210-40)*300/25.4;;
    h1 = 1500;

    CFRect rect(0, 0, w1, h1);

    if (rect.Width() == 0 && rect.Height() == 0) return;
    rect.NormalizeRect();

    double m_scale_old = m_scale;
    m_scale = 1.;
    m_coef = 300./96.;
    m_coef = 1.;

    double m_h_old = m_h;

    m_h = m_h * m_coef;

    double mx = geom.masx;
    double my = geom.masy;

    geom_old = geom;

    geom.masx = mx;
    geom.masy = my;
    geom.bx = rect.left/geom.masx;
    geom.by = rect.top/geom.masy;

    double w = rect.Width()/mx;
    double h = rect.Height()/my;

    QPixmap pixmap(QSize(w, h));
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

//    init_draw(&painter, getStdMas());
    init_draw(&painter, 1);

    painter.fillRect(QRect(0,0,w1, h1), m_bk_color);
    
    draw_rect(&painter, w1, h1, 1);

    painter.end();


    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(pixmap);    
    
    
    geom = geom_old;
    m_scale = m_scale_old;
    m_coef = 1;
    m_h = m_h_old;

}

//void CPjezo::onUpdateStat(CCmdUI* pCmdUI)
//{
//    pCmdUI->SetCheck(m_stat);
//}


//int getOutID(CAdoFile* ado, int fileID);

GidWidget *getView();

#include <table/MultiHeaderTable.h>

void view_my_table(MultiHeaderTable *view, const QString & title, QWidget *parent);
MultiHeaderTable* getMyTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title);


void CPjezo::onTechinfo()
{
#if 1
//    GidWidget* pView = getView();

    int i;

    QString s, sql;
    QString par = "";

    QString tn = "temp_pjezo";

    sql = QString("DROP TABLE IF EXISTS %1").arg(tn);
    bool ret = query_exec(gid->getCxema()->m_db, sql);

    sql = QString("CREATE TEMPORARY TABLE %1 (id int, id2 int,"
//    sql = QString("CREATE TABLE %1 (id int, id2 int,"
        "kod1 varchar(20), uzel1 varchar(70), pr1 varchar(20),"
        "kod2 varchar(20), uzel2 varchar(70), pr2 varchar(20),"
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
                  ")").arg(tn);


    ret = query_exec(gid->getCxema()->m_db, sql);


    long num = 1;

    double dlP = 0, dlO = 0, vP = 0, vO = 0;

    for (i = 0; i < m_vnode.size(); i++) {
        CPLine* l1, * l2;
        l1 = &m_vlineP[i];
        l2 = &m_vlineO[i];

        if (l1->rl[0].l && l2->rl[0].l) {
            CLINE2* lP = l1->rl[0].l;
            CLINE2* lO = l2->rl[0].l;

            double qP = bline(lP)->line.pod.q;
            double qO = bline(lO)->line.obr.q;


            if (bline(lP)->line.pod.q < 0) {
                lP = sym(lP);
                lO = sym(lO);
                qP = -qP;
                qO = -qO;
            }

            std::map<QString, QVariant> map_val;


            map_val["id"] = (int) bline(lP)->line.nomP;
            map_val["id2"] = (int) num++;
            map_val["kod1"] = where(lP)->getKod();
            map_val["uzel1"] = where(lP)->node.name;
            map_val["kod2"] = other(lP)->getKod();
            map_val["uzel2"] = other(lP)->node.name;

            map_val["pr1"] = tr(S_POD);
            map_val["pr2"] = tr(S_POD);

            map_val["q"] = qP;
            map_val["pih1"] = where(lP)->node.data.pihP;
            map_val["pih2"] = other(lP)->node.data.pihP;

            map_val["a14"] = bline(lP)->line.pod.a14;
            map_val["a15"] = bline(lP)->line.pod.a15;
            map_val["a16"] = bline(lP)->line.pod.a16;
            map_val["a17"] = bline(lP)->line.pod.a17;


            map_val["w"] = bline(lP)->line.pod.w;
            map_val["dl"] = bline(lP)->line.pod.dl;
            map_val["diam"] = bline(lP)->line.pod.diam;

            dlP += bline(lP)->line.pod.dl;
            vP += bline(lP)->line.pod.v;

            map_val["dl2"] = dlP;
            map_val["v2"] = vP;

//            int idv1 = execInsertQ(gid->getCxema()->m_db, tn, map_val, false, false);

            int idv1 = insertIntoDatabase(gid->getCxema()->m_db, tn, map_val);


//            ado->Update();


//            ado->AddNew(tn);
            map_val["id"] = (int) bline(lO)->line.nomO;
            map_val["id2"] = (int) num++;

            map_val["kod1"] = where(lO)->getKod();
            map_val["uzel1"] = where(lO)->node.name;
            map_val["kod2"] = other(lO)->getKod();
            map_val["uzel2"] = other(lO)->node.name;

            map_val["pr1"] = tr(S_OBR);
            map_val["pr2"] = tr(S_OBR);

            map_val["q"] = qO;
            map_val["pih1"] = where(lO)->node.data.pihO;
            map_val["pih2"] = other(lO)->node.data.pihO;

            map_val["a14"] = bline(lO)->line.obr.a14;
            map_val["a15"] = bline(lO)->line.obr.a15;
            map_val["a16"] = bline(lO)->line.obr.a16;
            map_val["a17"] = bline(lO)->line.obr.a17;


            map_val["w"] = bline(lO)->line.obr.w;
            map_val["dl"] = bline(lO)->line.obr.dl;
            map_val["diam"] = bline(lO)->line.obr.diam;

            dlO += bline(lO)->line.obr.dl;
            vO += bline(lO)->line.obr.v;

            map_val["dl2"] = dlP;
            map_val["v2"] = vO;

//            int idv2 = execInsertQ(gid->getCxema()->m_db, tn, map_val, false, false);
            int idv2 = insertIntoDatabase(gid->getCxema()->m_db, tn, map_val);


//            ado->Update();
        }

    }

    QString q = QString("SELECT * FROM %1").arg(tn);


    MultiHeaderTable* table = getMyTableView(gid->getCxema()->m_db, tn, q, "Тех.информация");
    if (table) {
        table->setGidWidget(this->gid);
        table->setDop("1");
        table->setEdit(false);
        table->checkCustomHeader();
        view_my_table(table, "Тех.информация", this);
    }
    else {
        QMessageBox::warning(nullptr, "", QString("Нет данных в таблице %1").arg(tn));
    }



#if 0
    if (pView) {
        QString q;
        q = "SELECT * FROM ##pjezo ORDER BY id2";
        pView->viewQ2(getGidAdoName(), q, tr("Техническая информация"), tr("Техническая информация"), virt_data_line_out);
    }
#endif
#endif
}



//void CPjezo::onUpdateRealP(CCmdUI* pCmdUI)
//{
//    pCmdUI->SetCheck(m_real);
//}


void CPjezo::onSavePjezo()
{
    this->gid->savePjezo();

/*
    CGidrView* pView = getView();

    if (pView) {
        pView->savePjezo();
    }
    else {
        AfxMessageBox("Ошибка", MB_OK|MB_ICONINFORMATION);
    }
*/
}

//void setView(CGidrView* pView0);


void CPjezo::onMoveTo()
{
    QString str;
    CNode2* n1 = m_vnode[m_n].rn[0].n, * n2 = m_vnode[m_n + 1].rn[0].n;
    CPLine* l1, * l2;

    l1 = &m_vlineP[m_n];
    l2 = &m_vlineO[m_n];

    int id = 0;

    if (l1 && l1->rl[0].l) id = l1->rl[0].l->line.nomP;
    if (id == 0 && l2 && l2->rl[0].l) id = l2->rl[0].l->line.nomP;

    this->gid->moveLine(id);

#if 0
    CGidrView* pView = getView();
    if (pView) {
        setView(pView);
        pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
    }
#endif
}

void CPjezo::onFont()
{

    QFontDialog dlg(this);
    dlg.setCurrentFont(m_font);

    if (!dlg.exec()) return;

    m_font = dlg.currentFont();
    repaint();


/*
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
        repaint();
    }
*/
}


void CPjezo::onSavePjezo2()
{
#if 0
    QString q;
    QString text = "";
    CInputDialog inD(this, tr("Название направления"), tr("Введите название направления"), text);
    if (inD.DoModal() == IDOK) {
        CAdoFile dbf(m_gid_file->m_dbf);

        q = QString("SELECT * FROM [Направления] WHERE [name]='%s'").arg(inD.m_strInput);

        if (dbf.openTable(q)) {
            int nr = dbf.NRecs();
            dbf.closeTable();

            if (nr) {
                q.Format(tr("Направление %s уже существует\nУдалить его?"), inD.m_strInput);
                if (AfxMessageBox(q) != IDOK) {
                    return;
                }
                q = QString("DELETE FROM [Направления] WHERE [name]='%s'").arg(inD.m_strInput);
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

QString get_Select(const QString & tn, const CNode2* parent = NULL);

//#include "CalcTree.h"
//#include "mmenu2.h"

void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);

#if 0

void CPjezo::onDouble()
{
#if 0
    if (m_dbl)  {
        m_dbl = false;
        if (m_graph2) delete m_graph2;
        m_graph2 = nullptr;
        initPjezo();
        repaint();
        return;
    }



    CAdoFile* ado = getAdo(getGidAdoName());

    CMMenu2 menu(AfxGetMainWnd(), IDD_MENU_DIAM, tr("Выбрать второй расчет"));


//  CCalcTree dlg(this, m_cxema->m_graph, ado);

//  dlg.DoModal();
    menu.AddColumn(tr("Расчет"));
    menu.AddColumn(tr("Время"));

    menu.setWidth(1, 400);
    menu.setWidth(2, 200);

    QString q;
    q = QString("SELECT TOP 10 * FROM CALCULATION WHERE fileID=%d ORDER BY id DESC").arg(m_fileID);

    bool first = true;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            if (!first) {
                int id = ado->read_long("id");
                QString name = ado->readStr("name");
                COleDateTime date1 = ado->read_date("date1");

                QString str = date1.Format(_T("%d.%m.%Y %H.%M"));

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
        AfxMessageBox("Нет других расчетов", MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (menu.DoModal() == IDOK) {
        QString par;
        par = QString("%d").arg((int)menu.getV());

        if (m_graph2) delete m_graph2;

        m_graph2 = new CGraph2(NULL);

        copyVydGraph_new(m_graph, m_graph2, true);

        CMapGraph::const_iterator p = m_graph2->map_node.begin();

        while (p != m_graph2->map_node.end()) {
            CNode2* n = p->second;
            n->node.data.pihP = 0;
            n->node.data.pihO = 0;
            p++;
        }

        m_cxema->openOut2(ado, m_graph2, par);
        m_dbl = true;
        initPjezo();
        repaint();
    }
#endif
}

#endif


#include <dialog/TableWindow.h>


void CPjezo::onDouble()
{
    if (m_dbl)  {
        m_dbl = false;
        if (m_graph2) delete m_graph2;
        m_graph2 = nullptr;
        initPjezo();
        repaint();
        return;
    }


//    QString q = QString("SELECT TOP 10 * FROM CALCULATION WHERE fileID=%1 LIMIT 10 ORDER BY id DESC").arg(m_fileID);
    QString q = QString("SELECT id, date1 AS \"Дата\", name AS \"Наименование\", user_gid AS \"Пользователь\" FROM CALCULATION WHERE fileID=%1 ORDER BY id DESC LIMIT 10").arg(m_fileID);

    TableWindow tab(m_cxema->m_db, "Double", "Выбрать второй расчет", q, this);


    if (!tab.exec()) return;


    int index = tab.value(0).toInt();
    QString text = tab.value(1).toString();

    QString par = QString("%1").arg(index);

    if (m_graph2) delete m_graph2;

    m_graph2 = new CGraph2(NULL);

    copyVydGraph_new(m_graph, m_graph2, true);


    for (auto& p : m_graph2->map_node) {
        CNode2* n = p.second;
        n->node.data.pihP = 0;
        n->node.data.pihO = 0;
    }

    QProgressDialog progress("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    m_cxema->m_graph_old = m_cxema->m_graph;
    m_cxema->m_graph = m_graph2;

    m_cxema->open_cxema_2(par, &progress);

    m_cxema->m_graph = m_cxema->m_graph_old;



//    m_cxema->openOut2(, m_graph2, par);
    m_dbl = true;
    initPjezo();
    repaint();




#if 0


    if (menu.m_rows.GetSize() == 0) {
        AfxMessageBox("Нет других расчетов", MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (menu.DoModal() == IDOK) {
        QString par;
        par = QString("%d").arg((int)menu.getV());

        if (m_graph2) delete m_graph2;

        m_graph2 = new CGraph2(NULL);

        copyVydGraph_new(m_graph, m_graph2, true);

        CMapGraph::const_iterator p = m_graph2->map_node.begin();

        while (p != m_graph2->map_node.end()) {
            CNode2* n = p->second;
            n->node.data.pihP = 0;
            n->node.data.pihO = 0;
            p++;
        }

        m_cxema->openOut2(ado, m_graph2, par);
        m_dbl = true;
        initPjezo();
        repaint();
    }
#endif
}



//void CPjezo::onUpdateDouble(CCmdUI* pCmdUI)
//{
//    pCmdUI->SetCheck(m_dbl);
//}


void CPjezo::onListPjezo()
{
/*
      Cgid6App* pApp = (Cgid6App*) AfxGetApp();
      Cgid6Doc* pDoc = (Cgid6Doc*) GetDocument();
    //  CGidrView *pView = pDoc->Kam(CNodeName("", "", ' '));
      CGidrView *pView = pDoc->Kam(NULL);

      if (pView) {
        pView->SendMessage(WM_COMMAND, ID_LIST_PJEZO);
        pView->SendMessage(WM_COMMAND, ID_PJEZO);
      }
    */
}

///////////////////////


double g_old(double g, double p1, double p2, double p1_old, double p2_old, double dh)
{
    if (p1 - p2 + dh == 0) return g;

    double g2 = g * sqrt(fabs((p1_old - p2_old + dh) / (p1 - p2 + dh)));

    return g2;
}


//-----------------------------------------------
// Отображаемая информация
//-----------------------------------------------

//#include "OtobrInf.h"
//#include "Inf2.h"

void CPjezo::onInf()
{
//    CInf2 oi(this, 2, sizeof(m_bIsInfo) / sizeof(m_bIsInfo[0]), m_bIsInfo);

//    if (oi.DoModal() == IDOK) {
//        InvalidateG();
//    }
}




/*
void CPjezo::closeEvent(QCloseEvent *event)
{

    QSettings settings;
    
    settings.setValue("flags/font_pjezo",  m_font);
}
*/


// geoView.cpp : implementation of the CGeoFile class
//

#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "geofile.h"

#include "win.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//void SaveFont(const TCHAR *szSection, const LOGFONT &lf, COLORREF color);
//void LoadFont(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);

//extern int is_no_mas;

//extern double m_MasPic;

bool log_draw();
bool LiangBar(int XS1, int YS1, int XS2, int YS2, double& x0, double& y0, double& x1, double& y1);


double set_no_mas_otn(const Klassif *kls, const ScrollGeom &geom)
{
    double mas = geom.masx*geom.dmas;
    double coef = 1;

    if (mas > kls->mas2 && kls->mas2 != 0) {
        coef = mas/kls->mas2;
    }

    return coef;

}


void CGeoFile::DrawObj1(CDC* m_dc, CScroll* scr, const Klassif* kls, int num, CCoordList& cl, bool isMark, bool redr, bool fill, bool label, bool no_mas)
{
    int loc, col, colb;

    loc = GetLoc(kls);
    col = GetColor(kls);
    colb = GetColor2(kls);

    double dm = m_dc->GetDeviceCaps(LOGPIXELSX) / 2.54;
    double mas_otn1 = scr->geom.masx * dm / (500);

    int tr = get_tr();

    double mas_otn_old = scr->mas_otn;

    if (no_mas) {
        //    mas_otn1 = scr->masx/100;
        mas_otn1 = 1;
        setTransp(255, get_bk_color());

        mas_otn1 = set_no_mas_otn(kls, scr->geom);

//        if (scr->mas_otn < mas_otn1) {
            scr->mas_otn = mas_otn1;
//        }
    }

    if (cl.is_color && colb != 0xFFFFFF) {
        colb = cl.color;
    }

    if (is_special_kls(kls->nazv)) {
        CPoint pt = scr->CoordToScreen(*cl.begin());

        int col1 = col;
        if (cl.is_color) {
            col1 = cl.color;
        }
        if (redr) {
//            col1 = 0x00FFFF;
        }

        if (kls->nazv == "shurfy") {
            scr->Draw11(m_dc, pt, isMark, col1, cl.color2, 1024, 1);
            return;
        }
        else if (kls->nazv == "defect") {
            scr->Draw11(m_dc, pt, isMark, col1, cl.color2, 1025, 1);
            return;
        }
        else if (kls->nazv == "indikator_korrozii") {
            scr->Draw11(m_dc, pt, isMark, col1, -1, 6, 1);
            return;
        }
    }

    if (loc == LOC1) {
        CPoint pt = scr->CoordToScreen(*cl.begin());
        if (cl.is_color) {
            col = cl.color;
        }
        if (redr) {
            col = 0x00FFFF;
        }
        scr->Draw1(m_dc, cl, isMark, redr, col, kls->typ, no_mas);
    }
    else if (loc == LOC11) {
        CPoint pt = scr->CoordToScreen(*cl.begin());

        if (cl.is_color) {
            col = cl.color;
        }
        if (redr) {
            col = 0x00FFFF;
        }
        //    scr->draw_tri(m_dc, pt, 10, 2, col);

        scr->Draw11(m_dc, pt, isMark, col, -1, kls->typ, cl.po);
    }
    else if (loc == LOC6) {
        if (cl.is_color) {
            col = cl.color;
        }
        if (redr) {
            col = 0x00FFFF;
        }

        scr->Draw6(m_dc, cl, isMark, redr, col, kls->typ, mas_otn1, no_mas);
    }
    else {
        if (kls->n_ObjectText == -1 || (!isLine(loc))) {
            int md = int(scr->geom.masx) * 5;
            CFRect rect = cl.GetFRect();
            if (rect.Width() < md && rect.Height() < md && loc != 12 && !kls->no_mas) {
                CPoint pt = scr->CoordToScreen(rect.TopLeft());
                scr->putpixel(m_dc, pt.x, pt.y, rgb_l(GetColorP(kls)));
            }
            else {
                if (fill) {
                    scr->Draw23(m_dc, cl, loc, isMark, redr, col, colb, kls->typ, kls->punct);
                }
            }
        }
    }

    if (!redr && label) {
        //    if (kls && kls->label && n_label >= 0) {
        if (kls && (kls->label && kls->n_label >= 0 || kls->n_text_string >= 0)) {
            CClientDC screenDC(NULL);
            LOGFONT lf = kls->lf;

            if (kls->label && kls->n_label >= 0 || kls->n_text_string >= 0) {
                //        read_label(num, text, x, y, alpha, coef, align);
                //        lf.lfHeight = 20;
            }


            // Исправлено 2015-02-18
            scr->m_lf_mag = kls->lf;
            //      int dpi = min(DPIPR, m_dc->GetDeviceCaps(LOGPIXELSX));
            //      double lh = lf.lfHeight*dpi/screenDC.GetDeviceCaps(LOGPIXELSX)/scr->mas_otn_geotext;
            //      double lh = lf.lfHeight/scr->mas_otn;
            double lh = lf.lfHeight / mas_otn1;

            if (scr->isColor()) {
                m_dc->SetTextColor(rgb_l(kls->color));
            }
            else {
                m_dc->SetTextColor(rgb_l(0));
            }

            //  scr->m_lf_color = 

            CString text, text_color;
            double x, y, alpha, coef;
            short align = 0;

            if (kls->n_ObjectText >= 0) {
                CFPoint p1 = cl.first();
                CFPoint p2 = cl.last();

                double h = hypot(p1.x - p2.x, p1.y - p2.y);
                lh = h / scr->geom.masx;

                x = y = alpha = 0; coef = 1;
                read_label(kls, num, text, text_color, x, y, alpha, coef, align);
                if (coef <= 0.) coef = 1.;
                scr->DrawLabel22(m_dc, cl, text, align, x, y, alpha, coef, kls);
            }
            else if (fabs(lh) > MIN_FONT || (isPoint(loc) && kls->label && kls->n_label >= 0 || kls->n_text_string >= 0)) {
                x = y = alpha = 0; coef = 1;
                read_label(kls, num, text, text_color, x, y, alpha, coef, align);
                if (coef <= 0.) coef = 1.;

                if (isPoint(loc)) {
                    if (kls->label && kls->n_label >= 0 || kls->n_text_string >= 0) {
                        m_dc->SetTextAlign(TA_LEFT | TA_BOTTOM);
                        //              scr->DrawLabel3_MI(m_dc, cl, text, x, y, alpha, coef, 1, kls);
                        scr->DrawLabel3(m_dc, cl, text, x, y, alpha, coef, 1, kls);
                    }
                    else {
                        m_dc->SetTextAlign(TA_CENTER | TA_BOTTOM);
                        scr->DrawLabel3(m_dc, cl, text, x, y, alpha, coef, 1, kls);
                    }
                }
                else if (isLine(loc)) {
                    if (kls->n_ObjectText >= 0) {
                        scr->DrawLabel22(m_dc, cl, text, align, x, y, alpha, coef, kls);
                    }
                    else {
                        // Исправления 2015-02-18
                        if (cl.size() > 2) {
                            scr->DrawLabelMulty(m_dc, cl, text, 1);
                        }
                        else {
                            scr->DrawLabel2(m_dc, cl, text, align, x, y, alpha, coef, kls);
                        }
                    }
                }
                else if (loc == LOC3) scr->DrawLabel3(m_dc, cl, text, x, y, alpha, coef, 3, kls);
            }
        }

        if (kls->n_text_string >= 0 && (!kls->label || kls->n_label < 0)) {
            //      if (loc == LOC1) DrawLabel1_MI(m_dc, scr, cl);
        }
    }
    if (no_mas) {
        setTransp(tr, get_bk_color());
        scr->mas_otn = mas_otn_old;
    }
}

void CGeoFile::DrawObj(CDC* m_dc, CScroll* scr, const Klassif* kls, int i, bool redr, bool fill, bool label, int isMarkOnly, bool no_mas)
{
    CCoordList cl;

    if (is_exit) return;

    if (isMarkOnly && !IsMark(kls, i)) return;
    if (!IsVisible(kls, i)) return;

    if (!GetCoord(kls, i, cl)) return;

    if (cl.is_date) {
        CGidrView* view = (CGidrView*)scr;

        if (view->m_sezon_korrozia > 0) {
            if (!(view->m_sezon_date1 <= cl.date && cl.date <= view->m_sezon_date2)) {
                return;
            }
        }
    }

    if (cl.text.IsEmpty()) {
        int isMark = IsMark(kls, i);
        if (isMarkOnly == 1) {
            isMark = 0;
        }

        DrawObj1(m_dc, scr, kls, i, cl, isMark, redr, fill, label, no_mas);
    }
    else {  /// Текст из Геомедии
        if (label) {
            m_dc->SetTextAlign(TA_LEFT | TA_BASELINE);
            m_dc->SetTextColor(kls->color);
            scr->DrawLabel2(m_dc, cl, cl.text, 0, 0, 0, 0, 1, kls);
        }
    }
}

#if 0
void CGeoFile::drawLayer(CDC* m_dc, CScroll* scr, const char* tn, const char* in, bool label, int isMarkOnly)
{
    CFRect r, rect;
    CFRect rectScr = scr->ScreenToCoord(scr->m_rect);
    int i, n;
    CPoint pt;

    if (!SetKls(tn)) return;

    int loc = GetLoc();

    //  if ((loc == LOC1 || loc == LOC6 || loc == LOC33) && scr->masx*scr->dmas > 5000) return;

    int md = int(scr->masx) * 5;

    bool isOpen = false;

    n = NObj();

    for (i = 1; i <= n; i++) {
        GetFRect(i, rect);

        if (rect.Width() == 0 && rect.Height() == 0 && !isPoint(loc)) continue;

        if (!r.IntersectRect(rect, rectScr)) continue;

        if (!isPoint(loc) && loc != 33 && loc != 12 && rect.Width() < md && rect.Height() < md && !kls->no_mas) {
            if (kls->n_ObjectText == -1 || (!isLine(loc))) {
                pt = scr->CoordToScreen(rect.TopLeft());
                if (!isMarkOnly || IsMark(i)) scr->putpixel(m_dc, pt.x, pt.y, rgb_l(GetColorP()));
            }
        }
        else if (1) {
            if (!isOpen && (!isRead(i) || !isReadLabel(i))) {

                if (!InitS(tn)) return;
                if (!kls->m_db->isCoord()) if (in[0] && !InitS(in)) return;
                isOpen = true;
            }
            //      isOpen = true;

#if 0
            CRect cr = scr->CoordToScreen(rect);
            scr->rectangle(m_dc, cr.left, cr.top, cr.right, cr.bottom);
#endif

            DrawObj(m_dc, scr, i, 0, label, isMarkOnly, kls->no_mas);
        }
    }
}
#endif


void CGeoFile::drawLayer(CDC* m_dc, CScroll* scr, const char* tn, const char* in, bool fill, bool label, int isMarkOnly)
{
    CFRect r, rect;
    CFRect rectScr1 = scr->ScreenToCoord(scr->m_rect);
    int i, n;
    CPoint pt;

    CFRect rectScr = rectScr1;
    rectScr.InflateRect(rectScr1.Width()/5., rectScr1.Height()/5.);

    if (!SetKls(tn)) return;

    //  Klassif *kls = m_kls;
    Klassif* kls = m_kl_list->findKlN(tn);

    double mas = scr->geom.masx*scr->geom.dmas;

//    if ((mas < kls->mas1 && kls->mas1 != 0) || (mas > kls->mas2 && kls->mas2 != 0)) {
//        double dm = dc->GetDeviceCaps(LOGPIXELSX) / 2.54;
//        mas_otn 
//        return;
//    }

    if (mas < kls->mas1 && kls->mas1 != 0) {
//        double dm = dc->GetDeviceCaps(LOGPIXELSX) / 2.54;
//        mas_otn 
        return;
    }


    double mas_otn_old = scr->mas_otn;

    
    if (mas > kls->mas2 && kls->mas2 != 0) {
        scr->mas_otn *= (mas/kls->mas2);
        label = false;
    }


    if (mas > kls->mas2*3 && kls->mas2 != 0) {
        return;
    }


    if ((mas < kls->mas_text1 && kls->mas_text1 != 0) || (mas > kls->mas_text2 &&  kls->mas_text2 != 0)) label = false;

    int loc = GetLoc(kls);

    //  if ((loc == LOC1 || loc == LOC6 || loc == LOC33) && scr->masx*scr->dmas > 5000) return;

    int md5 = int(scr->geom.masx) * 5;
    int md1 = int(scr->geom.masx) * 5;

    bool isOpen = false;

    n = kls->NObj();

    clock_t t1 = clock();

    for (i = 1; i <= n; i++) {
        GetFRect(kls, i, rect);

        if (rect.Width() == 0 && rect.Height() == 0 && !isPoint(loc)) continue;

        if (!r.IntersectRect(rect, rectScr)) continue;

        if (isPoint(loc) && !kls->no_mas) {
            rect.right = rect.left + 500;
            rect.bottom = rect.top + 500;
        }

        //    if (!isPoint(loc) && loc != 33 && loc != 12 && rect.Width() < md && rect.Height() < md && !kls->no_mas) {
        if (loc != 33 && loc != 12 && (rect.Width() < md5 && rect.Height() < md5) && !kls->no_mas) {
            if (!isPoint(loc)) {
                if (kls->n_ObjectText == -1 || (!isLine(loc))) {
                    pt = scr->CoordToScreen(rect.TopLeft());
                    if (!isMarkOnly || IsMark(kls, i)) {
                        if (rect.Height() > md1) {
                            scr->putpixel(m_dc, pt.x, pt.y, rgb_l(GetColorP(kls)));
                        }
                    }
                }
            }
        }

        else if (1) {
            DrawObj(m_dc, scr, kls, i, 0, fill, label, isMarkOnly, kls->no_mas);
        }
    }

    scr->mas_otn = mas_otn_old;


    clock_t t2 = clock();

    CString ss;
    if (log_draw()) {
        log1("%d %s - %g секунд  (1:%.0f)", loc, tn, (double)(t2-t1) / CLOCKS_PER_SEC, scr->geom.masx*scr->geom.dmas);
    }
}


#if 0
void CGeoFile::drawLayer_xy(CDC* m_dc, CScroll* scr, const char* tn, const char* in, bool label, int isMarkOnly)
{
    CString q, str;

    q.Format("Select [FID], [POINTS] From [%s]", in);
    q.Format("Select [ID], [Shape] From [%s]", tn);

    if (!InitS(in)) return;

    CFRect r, rect;
    CFRect rectScr = scr->ScreenToCoord(scr->m_rect);
    int i, n;
    int md = int(scr->masx) * 5;
    CPoint pt;
    int loc = GetLoc();

    n = NObj();

    for (i = 1; i <= n; i++) {
        GetFRect(i, rect);

        if (!r.IntersectRect(rect, rectScr)) continue;
        if (loc != 1 && loc != 33 && rect.Width() < md && rect.Height() < md) {
            pt = scr->CoordToScreen(rect.TopLeft());
            scr->putpixel(m_dc, pt.x, pt.y, rgb_l(GetColorP()));
        }
        else {
            DrawObj(m_dc, scr, i, 0, label, isMarkOnly, false);
        }
    }
}

void CGeoFile::drawLayer_shp_dbf(CDC* m_dc, CScroll* scr, const char* tn, bool label, int isMarkOnly)
{
    int i, n;
    int md = int(scr->masx) * 5;
    CFRect r, rect;
    CFRect rectScr = scr->ScreenToCoord(scr->m_rect);
    CFRect rectScr2;
    CPoint pt;

    rectScr2 = rectScr;
    rectScr2.InflateRect(1000 * scr->masx, 1000 * scr->masy);

    KlList::iterator it = m_kl_list->begin();

    for (; it != m_kl_list->end(); ++it) {

        Klassif* kl = &it->second;

        if (!strcmpi(kl->nazv, tn)) {
            m_kls = m_kls = kl;
            break;
        }
    }

    //  Cshp_dbf dbf(m_kls->nazv);

    InitS(m_kls->nazv);

    int loc = 3;

    int qq = m_kls->geo4.size();


    n = NObj();

    for (i = 1; i <= n; i++) {
        m_nCur = i;

        GetFRect(i, rect);

        if (rect.Width() == 0 && rect.Height() == 0) loc = LOC1;

        if (!r.IntersectRect(rect, rectScr) && loc != LOC1) continue;
        if (!r.IntersectRect(rect, rectScr2) && loc == LOC1) continue;

        if (loc != LOC1 && loc != LOC6 && rect.Width() < md && rect.Height() < md) {
            pt = scr->CoordToScreen(rect.TopLeft());
            scr->putpixel(m_dc, pt.x, pt.y, rgb_l(GetColorP()));
        }
        else {
            try {
                //        if (!m_shp->go_to_shp(m_kls->geo4[i-1].nom)) break;
                if (!m_shp->go_to(m_kls->geo4[i - 1]->nom)) break;

                double x1, y1, x2, y2;
                CCoordList cl;

                m_shp->read_coor2(x1, y1, x2, y2, cl);

                switch (m_shp->loc) {
                case 6: loc = 6; break;
                case 3: loc = 2; break;
                case 5: loc = 3; break;
                default:loc = 1; break;
                }

                DrawObj1(m_dc, scr, i, cl, false, IsMark(i), label, m_kls->no_mas);
            }
            catch (...) {
            }
        }
    }
    //  m_shp = NULL;
}
#endif

CString getPsAdoName();


void CGeoFile::DrawLoc(CDC* m_dc, CGidrView* scr, int loc, bool fill, bool label, int isMarkOnly, bool is_GID)
{
    int p;

    CString psname = getPsAdoName();

    if (loc == 99) {
    }
    else {
        for (p = 0; p < 11; p++) {
            for (auto& it : *m_kl_list) {
                Klassif* kl = &it.second;

                CString tn = kl->nazv;
                int colb = kl->colb;
                int loc0 = kl->loc;

                if (fill) {
                    if (kl->colb == 0xFFFFFF && loc0 == LOC3) {
                        loc0 = 2;
                    }
                }
                else {
                    if (kl->colb == 0xFFFFFF && loc0 == LOC3) {
                        continue;
                    }
                    kl->colb = 0xFFFFFF;
                }

                if (kl->prio == p && kl->ins && (loc0 == loc || isPoint(loc0) && loc == LOC1 || loc0 == 33 && loc == LOC1)) {
                    drawLayer(m_dc, scr, kl->nazv, kl->xy_table, fill, label, isMarkOnly);
                }
                kl->colb = colb;
            }
        }
    }
}



void draw_layer1(CDC* m_dc, CRect &rect_scr, ScrollGeom & geom, Klassif* kls);
void draw_layer2(CDC* m_dc, CRect &rect_scr, ScrollGeom & geom, Klassif* kls);
void draw_layer3(CDC* m_dc, CRect &rect_scr, ScrollGeom & geom, Klassif* kls);

void draw_layer3_labels(CDC* m_dc, CRect &rect_scr, ScrollGeom & geom, Klassif* kls);


void CGeoFile::Draw(CDC* m_dc, CGidrView* scr, int loc0, int isMarkOnly, int isText, bool is_GID)
{
    if (!scr) return;

    if (0) {
        log1("loc0=%d isMarkOnly=%d isText=%d is_GID=%d", loc0, isMarkOnly, isText, is_GID);

        std::list<Klassif*> list_1;
        std::list<Klassif*> list_2;
        std::list<Klassif*> list_3;

        for (int p = 0; p < 11; p++) {
            for (auto& it : *m_kl_list) {
                Klassif* kls = &it.second;

                if (kls->ins && kls->prio == p) {
                    if (isPoint(kls->loc)) {
                        list_1.push_back(kls);
                    }
                    else if (kls->loc == LOC2 || kls->colb == 0xFFFFFF) {
                        list_2.push_back(kls);
                    }
                    else if (kls->loc == LOC3 && kls->colb != 0xFFFFFF) {
                        list_3.push_back(kls);
                    }
                }
            }
        }


        if (loc0 != 1) {
            for (Klassif *kls : list_3) {
                draw_layer3(m_dc, scr->m_rect, scr->geom, kls);
                if (kls->n_label) {
                    draw_layer3_labels(m_dc, scr->m_rect, scr->geom, kls);
                }
            }

            for (Klassif *kls : list_2) {
                draw_layer2(m_dc, scr->m_rect, scr->geom, kls);
                if (kls->loc == LOC3 && kls->n_label) {
                    draw_layer3_labels(m_dc, scr->m_rect, scr->geom, kls);
                }
            }
        }
        return;
    }



    CFRect r, rect;
//    CFRect rectScr = scr->ScreenToCoord(scr->m_rect);

    double dm = m_dc->GetDeviceCaps(LOGPIXELSX) / 2.54;


    double mas_otn_old = scr->mas_otn;
    double mas_otn_geotext_old = scr->mas_otn_geotext;

    scr->mas_otn = scr->geom.masx * dm / (500);
    scr->mas_otn_geotext = scr->mas_otn;
    //  if (!m_bIsMas) scr->mas_otn = 1;

    try {
        clock_t t1 = clock();

//        log1("--------------------------------------------------------------");

        if (loc0 > -1) {
            DrawLoc(m_dc, scr, loc0, true, isText, isMarkOnly, is_GID);
        }
        else {
            DrawLoc(m_dc, scr, 3, true, false, isMarkOnly, is_GID);
            DrawLoc(m_dc, scr, 2, true, isText, isMarkOnly, is_GID);
            if (isText) {
                DrawLoc(m_dc, scr, 3, false, isText, isMarkOnly, is_GID);  // Подпись для площадных объектов
            }

            if (scr->geom.masx * dm < 5000 && loc0 != -2) {
                DrawLoc(m_dc, scr, 1, true, isText, isMarkOnly, is_GID);
            }

            if (loc0 != -2) {
                //        DrawLoc(m_dc, scr, 11, true, isText, isMarkOnly, is_GID);
            }
            //        DrawLoc(m_dc, scr, 2, true, isText, isMarkOnly, is_GID);
        }

        clock_t t2 = clock();


        CString ss;
        ss.Format("Итого - %g секунд", (double)(t2-t1) / CLOCKS_PER_SEC);
//        log1(ss);

//        log1("--------------------------------------------------------------");
    
    }
    catch (...) {
        //    Status(2, _TR("Испорченный файл"));
    }
    scr->mas_otn = mas_otn_old;
    scr->mas_otn_geotext = mas_otn_geotext_old;
}

static int m_disk = 0, m_memo = 0;;

void d_plus() {
    m_disk++;
}

void m_plus() {
    m_memo++;
}

void disk_memo(int& d, int& m)
{
    d = m_disk;
    m = m_memo;
}


void CGeoFile::Draw(CDC* dc, CGidrView* view, CRect rect, ScrollGeom &geom, int loc, int isMarkOnly, int isText, bool is_GID)
{
    is_exit = FALSE;
    m_disk = 0;
    m_memo = 0;

    CScroll scr;

    m_view = view;

    scr.geom.m_alpha = view->geom.m_alpha;
    scr.geom.dmas = view->geom.dmas;
    scr.geom.masx = geom.masx;
    scr.geom.masy = geom.masy;
    scr.geom.m_bx = geom.m_bx;
    scr.geom.m_by = geom.m_by;

    scr.m_rect = rect;
    //  scr.m_pDC = view->m_pDC;
    scr.mas_otn = view->mas_otn;
    scr.mas_otn_geotext = view->mas_otn_geotext;
    scr.m_isPrint = view->m_isPrint;

    view->EndPaint2(dc);

    /*
      scr.pen    = view->pen;
      scr.brush  = view->brush;
      scr.oldpen    = view->oldpen;
      scr.oldbrush  = view->oldbrush;
    */

    m_bIsGeoGeodez = view->m_bIsGeoGeodez;

    scr.SetDC(dc);

    //  Draw(dc, (CGidrView*) &scr, loc, isMarkOnly, isText, is_GID);
    Draw(dc, view, loc, isMarkOnly, isText, is_GID);


    scr.EndPaint2(dc);
}


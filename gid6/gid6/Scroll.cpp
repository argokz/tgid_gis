// Scroll.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Scroll.h"
#include "InputDia.h"
#include "PrintIni.h"
#include "win.h"
#include <math.h>
#include "MapDlg.h"
#include "geodez.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szMargin[] = _T("Margin");
static TCHAR BASED_CODE szScrollBar[] = _T("ScrollBar");

/////////////////////////////////////////////////////////////////////////////
// CScroll

void d2m(int zona, double B, double L, double& x, double& y);

extern double m_MasPic;


bool wheel = false;

bool isXdraw = true;


DEVMODE* m_devmode = NULL;


IMPLEMENT_DYNCREATE(CScroll, CBgi)

typedef BOOL(FAR PASCAL* PF) (
    HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION
    );

static PF lpfnAlphaBlend;



CScroll::CScroll() : CBgi()
{
    m_isRamka = TRUE;
    m_is_scroll = FALSE;
    m_is_print_fr = FALSE;
    m_isColor = TRUE;
    m_nTimerDbl = 0;
    m_color_uzel = WHITE;
    m_ring = 1.;
    regim = R_INIT;
    m_pt1 = CFPoint(0, 0);
    m_pt2 = CFPoint(0, 0);
    m_is_ob = false;
    m_isSavedImage = 0;
/**
    masx = masy = 1;
    m_bx = 0, m_by = 0;
    dmas = 1;
    m_alpha = 0.;
*/
    m_dX = 0;
    m_bk_color = 0xFFFFFF;

    m_bScrollBar = TRUE;
    scroll_type = 0;

    m_dbl = 0;

    m_isPrint = 0;
    m_dPrintMas = 1;
    //  m_nPrintMas = 500;
    m_rectPrint = CRect(0, 0, 0, 0);
    m_rectAllMargin = CRect(5000, 5000, 5000, 5000);
    m_rectAllMargin = CRect(0, 0, 0, 0);
    m_type_line = 0;
    m_bIsTic = 0;
    m_bIsFragment = FALSE;
    mdc = 0;
    is_google_map = GOOGLE_ALWAYS;
    m_dcSave = NULL;
    m_is_xdraw = false;

    m_noend = false;

    m_devmode = NULL;

    CString str = AfxGetApp()->GetProfileString(szSection, szMargin);
    if (str.IsEmpty()) {
        m_rectMargin = CRect(0, 0, 0, 0);
    }
    else {
        _stscanf(str, _T("%d %d %d %d"), &m_rectMargin.left, &m_rectMargin.top, &m_rectMargin.right, &m_rectMargin.bottom);
    }
    m_dX = AfxGetApp()->GetProfileInt(szSection, _T("dX"), m_dX);
    m_isRamka = AfxGetApp()->GetProfileInt(szSection, _T("Ramka"), m_isRamka);

    m_tecF = -1;


    m_nTransp = AfxGetApp()->GetProfileInt(szSection, _T("Transp"), 255);

    m_strText = "";

    HINSTANCE hInst;        // handle to MAPI32.DLL

    hInst = LoadLibraryA("Msimg32.DLL");
    lpfnAlphaBlend = (PF)GetProcAddress(hInst, "AlphaBlend");
}

CScroll::~CScroll()
{
    CString str;
    str.Format(_T("%d %d %d %d"), m_rectMargin.left, m_rectMargin.top, m_rectMargin.right, m_rectMargin.bottom);
    AfxGetApp()->WriteProfileString(szSection, szMargin, str);
    AfxGetApp()->WriteProfileInt(szSection, _T("dX"), m_dX);
    AfxGetApp()->WriteProfileInt(szSection, _T("Ramka"), m_isRamka);
    AfxGetApp()->WriteProfileInt(szSection, _T("Transp"), m_nTransp);
}


BEGIN_MESSAGE_MAP(CScroll, CBgi)
    //{{AFX_MSG_MAP(CScroll)
    ON_COMMAND(ID_PLUS, OnPlus)
    ON_COMMAND(ID_MINUS, OnMinus)
    ON_COMMAND(ID_KEY_MORE, OnPlus)
    ON_COMMAND(ID_KEY_LESS, OnMinus)
    ON_COMMAND(ID_MASALL, OnMasAll)
    ON_WM_KEYDOWN()
    ON_WM_HSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_VSCROLL()
    ON_COMMAND(ID_F5, OnF5)
    ON_COMMAND(ID_MASSHTAB, OnMasshtab)
    ON_COMMAND(ID_MOVEW, OnMoveW)
    ON_COMMAND(ID_FINDCOORD, OnFindcoord)
    ON_COMMAND(ID_FINDCOORD_DEG, OnFindcoordDeg)
    ON_WM_SIZE()
    ON_COMMAND(ID_VIEW_SCROLLBAR, OnViewScrollbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SCROLLBAR, OnUpdateViewScrollbar)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MBUTTONDOWN()
    ON_WM_MBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ID_ESC, OnEsc)
    ON_COMMAND(ID_DELETE, OnDelete)
    ON_COMMAND(ID_PAGE_SETUP, OnPageSetup)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_COPY_PNG, OnEditCopyPng)

    ON_COMMAND(ID_PRINT_FR, OnPrintFr)

    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_PNG, OnUpdateEditCopy)

    ON_COMMAND(ID_FRAGMENT, OnFragment)
    //  ON_COMMAND(ID_EDIT_COPYBMP, OnEditCopybmp)
    ON_COMMAND(ID_F4, OnF4)
    ON_COMMAND(ID_RETURN, OnReturn)
    ON_COMMAND(ID_CNTRL_RETURN, OnCtrlReturn)
    ON_COMMAND(ID_F2, OnF2)
    ON_COMMAND(ID_PRINT_INIT, OnPrintInit)
    //  ON_COMMAND(ID_ASTR, OnAstr)
    //  ON_UPDATE_COMMAND_UI(ID_ASTR, OnUpdateAstr)

    ON_COMMAND(ID_RAMKA, OnRamka)
    ON_UPDATE_COMMAND_UI(ID_RAMKA, OnUpdateRamka)
    ON_WM_TIMER()
    //  ON_WM_LBUTTONDBLCLK()
    ON_COMMAND(ID_TRANSP, OnTransp)
    ON_COMMAND(ID_FILE_PRINT_SETUP, OnPageSetup)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScroll drawing


void CScroll::DrawRect(CDC* m_dc, CFPoint pt, int w, int h)
{
    //  setline(m_dc, 0x7F7F7F, 1);
    setcolor(m_dc, 0x7F7F7F);
    CPoint p = CoordToScreen(pt);
    xrectangle(m_dc, p.x - w, p.y - h, p.x + w, p.y + h);
}

void CScroll::DrawLine(CDC* m_dc)
{
    if (regim == R_BEGIN_MOVE_PODP) {
        DrawPodp(m_dc);
        return;
    }

    if (m_NP.size() > 0) {
        setcolor(m_dc, 0xFF0000);

        //      CString ss;
        //      ss.Format("loc = %d", m_locNP);
        //      AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);

        int saveROP;
        saveROP = m_dc->SetROP2(R2_NOTXORPEN);
        DrawPolygon2(m_dc, m_NP, m_locNP);
        m_dc->SetROP2(saveROP);

        int n = 0;

        CFPoint p1 = m_NP.first();
        CFPoint p2 = m_NP.last();

        CCoordList::const_iterator it = m_NP.begin();
        for (; it != m_NP.end(); it++, n++) {
            if (n == m_NP.size() - 1 && *it == p1) break;
            DrawRect(m_dc, *it);
        }
    }
}

void rect_points(double x0, double y0, double x1, double y1, double m_otn, double& x3, double& y3, double& x4, double& y4)
{
    double d = hypot(x0 - x1, y0 - y1);
    double r1 = d / sqrt(1. + m_otn * m_otn);
    double r0 = r1 * m_otn;

    double a = (r0 * r0 - r1 * r1 + d * d) / (2. * d);
    double h = sqrt(r0 * r0 - a * a);

    double x2 = x0 + (x1 - x0) * a / d;
    double y2 = y0 + (y1 - y0) * a / d;

    x3 = x2 + h * (y1 - y0) / d;
    y3 = y2 - h * (x1 - x0) / d;

    x2 = x1 + (x0 - x1) * a / d;
    y2 = y1 + (y0 - y1) * a / d;

    x4 = x2 + h * (y0 - y1) / d;
    y4 = y2 - h * (x0 - x1) / d;
}

void CScroll::x_podp(CDC* m_dc, int _x0, int _y0, int _x1, int _y1, double otn)
{
    double x0 = _x0;
    double y0 = _y0;
    double x1 = _x1;
    double y1 = _y1;

    double x3, y3, x4, y4;

    rect_points(x0, y0, x1, y1, otn, x3, y3, x4, y4);

    xline(m_dc, x0, y0, x3, y3);
    xline(m_dc, x3, y3, x1, y1);
    xline(m_dc, x1, y1, x4, y4);
    xline(m_dc, x4, y4, x0, y0);
}

void CScroll::DrawPodp(CDC* m_dc)
{
    setcolor(m_dc, 0x7F7F7F);

    int saveROP;
    saveROP = m_dc->SetROP2(R2_NOTXORPEN);
    //  DrawPolygon2(m_dc, m_NP, m_locNP);
    CFPoint pt1 = m_NP.GetV(0);
    CFPoint pt2 = m_NP.GetV(1);

    CPoint p1 = CoordToScreen(pt1);
    CPoint p2 = CoordToScreen(pt2);

    x_podp(m_dc, p1.x, p1.y, p2.x, p2.y, m_otn);

    m_dc->SetROP2(saveROP);

    CCoordList::const_iterator it = m_NP.begin();

    for (; it != m_NP.end(); ++it) DrawRect(m_dc, *it);
}



void CScroll::OnDraw(CDC* pDC)
{
    if (m_off) return;

//    log1("Начали рисовать");

    CDocument* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    if (wheel) {
        wheel = false;
    }


    if (pDoc && !pDC->IsPrinting()) {

        //    pDC = GetDesktopWindow()->GetDC();
        SetDC(pDC);
        m_rect = ((CPaintDC*)pDC)->m_ps.rcPaint;
        //    GetClientRect(&m_rect);

        m_is_xdraw = false;


        if (m_isSavedImage) {
            pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_pDibDC, m_rect.left, m_rect.top, SRCCOPY);
        }
        else {
            SetDC(pDC);

            redrawPaint(pDC);
            DrawLine(pDC);

            if (!m_is_scroll) {
                XDraw(pDC, true);
            }
        }
    }
//    log1("Закончили рисовать");
}

CDC* CScroll::SaveImage()
{
    CDC* saveDC = NULL;

    m_isSavedImage++;

    CBitmap bitmap;

    CPaintDC m_dc(this);
    GetClientRect(&m_rect);

    saveDC = new CDC();

    saveDC->CreateCompatibleDC(saveDC);

    bitmap.CreateCompatibleBitmap(&m_dc, m_rect.right, m_rect.bottom);
    saveDC->SelectObject(&bitmap);
    saveDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), &m_dc, m_rect.left, m_rect.top, SRCCOPY);


    m_pDibDC = saveDC;

    return saveDC;
}



void CScroll::CloseImage(CDC* saveDC)
{
    //  log("closeImage", "1");
    if (m_isSavedImage) {
        //    CClientDC dc(this);
        CDC* m_dc;
        if ((m_dc = BeginPaint()) != NULL) {
            GetClientRect(&m_rect);
            m_dc->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), saveDC, m_rect.left, m_rect.top, SRCCOPY);
            delete saveDC;
            ValidateRect(m_rect);
            EndPaint(m_dc);
        }
        m_isSavedImage--;
    }
    //  log("closeImage", "2");
}

/////////////////////////////////////////////////////////////////////////////
// CScroll diagnostics

#ifdef _DEBUG
void CScroll::AssertValid() const
{
    CBgi::AssertValid();
}

void CScroll::Dump(CDumpContext& dc) const
{
    CBgi::Dump(dc);
}
#endif //_DEBUG


void CScroll::OnPlus()
{
    // TODO: Add your command handler code here
    MasPlus();
    InvalidateG();
}

void CScroll::OnMinus()
{
    // TODO: Add your command handler code here
    MasMinus();
    InvalidateG();
}

void CScroll::OnMasAll()
{
    MasAll();
    InvalidateG();
}



const int Dx = 160, Dy = 160;

void CScroll::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default

    CBgi::OnKeyDown(nChar, nRepCnt, nFlags);

    CPoint pt;
    CRect rc;

    int shift = GetKeyState(VK_SHIFT) & 0x80;
    int ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;

    GetCursorPos(&pt);
    ScreenToClient(&pt);

    if (shift) {
        if (nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN) {
            switch (nChar) {
            case VK_LEFT:  pt.x -= nRepCnt; break;
            case VK_RIGHT: pt.x += nRepCnt; break;
            case VK_UP:    pt.y -= nRepCnt; break;
            case VK_DOWN:  pt.y += nRepCnt; break;
            }
            GetClientRect(rc);

            if (pt.x >= rc.right)  pt.x = rc.right - 1;
            if (pt.x < rc.left)    pt.x = rc.left;
            if (pt.y >= rc.bottom) pt.y = rc.bottom - 1;
            if (pt.y < rc.top)     pt.y = rc.top;

            ClientToScreen(&pt);
            SetCursorPos(pt.x, pt.y);
        }
    }
    else {
        switch (nChar) {
        case VK_LEFT: ScrollWindow(Dx, 0);  break;
        case VK_RIGHT: ScrollWindow(-Dx, 0); break;
        case VK_UP: ScrollWindow(0, Dy);  break;
        case VK_DOWN: ScrollWindow(0, -Dy); break;
        }
    }
}


void CScroll::XDraw0(bool on)
{
    CDC* m_dc;

    if ((m_dc = BeginPaint()) != NULL) {
        XDraw(m_dc, on);
        EndPaint(m_dc);
    }
}


void CScroll::ScrollWindow(int dx, int dy)
{
    int or = regim;

    XDraw0(false);

    regim = R_INIT;

    CWnd::ScrollWindow(dx, dy);

    geom.m_bx -= dx; geom.m_by -= dy;
    m_pt2.x -= dx * geom.masx;
    m_pt2.y -= dy * geom.masy;
    regim = or ;

    bord();

    m_is_scroll = TRUE;

    UpdateWindow();

    m_is_scroll = FALSE;

    XDraw0(true);
}

/////////////////////////////////////////////////////////////////////////////
// CScroll message handlers


void CScroll::SetBar() {

    EnableScrollBarCtrl(SB_HORZ, m_bScrollBar);
    EnableScrollBarCtrl(SB_VERT, m_bScrollBar);

    if (!m_bScrollBar) return;

    SCROLLINFO info;
    info.fMask = SIF_PAGE | SIF_RANGE;

    CRect rect;
    GetClientRect(&rect);

    info.nPage = rect.Width() * 30000. * geom.masx / m_rectBorder.Width();
    info.nMin = 0;
    info.nMax = 30000;
    if (!SetScrollInfo(SB_HORZ, &info, TRUE))
        SetScrollRange(SB_HORZ, 0, 30000, FALSE);

    info.nPage = rect.Height() * 30000. * geom.masy / m_rectBorder.Height();
    info.nMin = 0;
    info.nMax = 30000;
    if (!SetScrollInfo(SB_VERT, &info, TRUE))
        SetScrollRange(SB_VERT, 0, 30000, FALSE);

    bord();
}


void CScroll::bord() {
    double x, y;

    x = m_rectBorder.left / geom.masx;
    y = m_rectBorder.top / geom.masy;

    double coefx = 30000. * geom.masx / m_rectBorder.Width();
    double coefy = 30000. * geom.masy / m_rectBorder.Height();

    SetScrollPos(SB_HORZ, (geom.m_bx - x) * coefx, TRUE);
    SetScrollPos(SB_VERT, (geom.m_by - y) * coefy, TRUE);

    setNavigator();

#ifdef COOLDIAL
    extern CMapDlg* pm_cMapDlg;
    if (pm_cMapDlg) {
        pm_cMapDlg->moveRect();
        pm_cMapDlg->curRect();
    }
#endif

}


void CScroll::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    double coefx = 30000. * geom.masx / m_rectBorder.Width();
    int dx = 0;

    switch (nSBCode) {
    case SB_LINELEFT: dx = Dx;   break;
    case SB_LINERIGHT: dx = -Dx;  break;
    case SB_PAGELEFT: dx = Dx;   break;
    case SB_PAGERIGHT: dx = -Dx;  break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        dx = geom.m_bx - (m_rectBorder.left / geom.masx + nPos / coefx);
        break;
    }


    if (dx != 0) ScrollWindow(dx, 0);

    CBgi::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CScroll::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    double coefy = 30000. * geom.masy / m_rectBorder.Height();
    int dy = 0;

    switch (nSBCode) {
    case SB_LINEUP: dy = Dy;  break;
    case SB_LINEDOWN: dy = -Dy; break;
    case SB_PAGEUP: dy = Dy;  break;
    case SB_PAGEDOWN: dy = -Dy; break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        dy = geom.m_by - (m_rectBorder.top / geom.masy + nPos / coefy);
        break;
    }

    if (dy != 0) ScrollWindow(0, dy);

    CBgi::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CScroll::OnF5()
{
    InvalidateG();
}

void CScroll::SetCur(int regim)
{
    //  switch ( m_nRegim ) {
    switch (regim) {
    case ID_ZOOM:
    case ID_EDIT_COPY:
    case ID_EDIT_COPYBMP:
        MySetCursor(0, IDC_CROSS);
        break;
    case ID_MOVEW:
        MySetCursor(AfxGetApp(), MAKEINTRESOURCE(16));
        break;

    default: MySetCursor(0, IDC_ARROW);
    }

    return;


    switch (regim) {
    case R_BEGIN_RECT:
    case R_BEGIN_DRAW_COPY1:
    case R_BEGIN_DRAW_COPY2:
    case R_BEGIN_DRAW_COPY_BMP1:
    case R_BEGIN_DRAW_COPY_BMP2:
        MySetCursor(0, IDC_CROSS);
        break;
    case R_BEGIN_MOVE_VIEW:
    case R_MOVE_VIEW:
    case R_MOVE_VIEW_MIDDLE:
    case R_MOVE_VIEW_VYD:
        MySetCursor(AfxGetApp(), MAKEINTRESOURCE(16));
        //      MySetCursor(AfxGetApp(), MAKEINTRESOURCE(ID_MOVEW));
        break;

    default: MySetCursor(0, IDC_ARROW);
    }
}

void CScroll::OnMasshtab()
{
    // TODO: Add your command handler code here
    double m;
    CString text;

    text.Format(_T("%g"), geom.masx * geom.dmas);


    if (0) {
        double x1, y1, x2, y2;
        CFPoint pt1 = ScreenToCoord(CPoint(0, 0));
        CFPoint pt2 = ScreenToCoord(CPoint(1, 0));
        d2m(-1, y2lat(-0.0000001 * pt1.y), x2lon(0.0000001 * pt1.x), x1, y1);
        d2m(-1, y2lat(-0.0000001 * pt2.y), x2lon(0.0000001 * pt2.x), x2, y2);

        double coef = hypot(x2 - x1, y2 - y1);

        text.Format(_T("%g"), geom.masx * geom.dmas / coef);
    }


    CInputDialog inD(this, _TR("Масштаб 1:N"), _TR("Введите масштаб N:"), text);
    if (inD.DoModal() == IDOK) {
        text = inD.m_strInput;
        if (text[0]) {
            if (_stscanf(text, _T("%lg"), &m) > 0) SetMas(m / geom.dmas, geom.m_bx, geom.m_by);
            InvalidateG();
        }
    }
}

void CScroll::OnMoveW()
{
    regim = R_BEGIN_MOVE_VIEW;
    SetCur(m_nRegim);
}

CDC* CScroll::BeginMoveW(CDC* m_dc)
{
    SetCapture();

    CDC* mdc = new CDC();

    mdc->CreateCompatibleDC(m_dc);

    CRect r;
    GetClientRect(&r);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(m_dc, r.right, r.bottom);
    mdc->SelectObject(bitmap);
    mdc->BitBlt(r.left, r.top, r.Width(), r.Height(), m_dc, 0, 0, SRCCOPY);
    return mdc;
}

void CScroll::EndMoveW(CDC* m_dc, CDC* mdc, int dx, int dy)
{
    ReleaseCapture();
    MoveW(m_dc, mdc, dx, dy);

    delete mdc;

    geom.m_bx -= dx;
    geom.m_by -= dy;
    InvalidateG();
    regim = ID_MOVEW;

    CRect r;
    GetClientRect(&r);
    ValidateRect(r + CSize(dx, dy));
    //  InvalidateG();
}

void CScroll::MoveW(CDC* m_dc, CDC* mdc, int dx, int dy)
{
    CRect r, rr;
    GetClientRect(&r);

    XDraw(m_dc, false);

    m_dc->BitBlt(r.left + dx, r.top + dy, r.Width(), r.Height(), mdc, 0, 0, SRCCOPY);

    CBrush brush, * pOldBrush;
    brush.CreateSolidBrush(m_bk_color);

    pOldBrush = m_dc->SelectObject(&brush);

    rr = r;
    if (dx > 0) rr.right = dx;
    else rr.left = r.right + dx;
    m_dc->PatBlt(rr.left, rr.top, rr.Width(), rr.Height(), PATCOPY);
    rr = r;
    if (dy > 0) rr.bottom = dy;
    else rr.top = r.bottom + dy;
    m_dc->PatBlt(rr.left, rr.top, rr.Width(), rr.Height(), PATCOPY);

    m_dc->SelectObject(pOldBrush);
    brush.DeleteObject();

    XDraw(m_dc, true);

}


static CRgn rgnA;


void CScroll::MoveW(CDC* m_dc, CDC* mdc, int dx, int dy, CCoordList& cl)
{
    CRect r, rr;
    GetClientRect(&r);

    CBitmap bitmap;

    GetClientRect(&m_rect);

    CDC saveDC;

    saveDC.CreateCompatibleDC(mdc);
    bitmap.CreateCompatibleBitmap(mdc, m_rect.right, m_rect.bottom);
    saveDC.SelectObject(&bitmap);
    saveDC.BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), mdc, m_rect.left - dx, m_rect.top - dy, SRCCOPY);

    CPoint* ptVertex = new CPoint[cl.size()];

    CCoordList::const_iterator it = cl.begin();

    int n = 0;
    for (; it != m_NP.end(); ++it) {
        ptVertex[n++] = CoordToScreen(*it);
    }

    CBrush brush(&bitmap);
    CBrush brushWhite(RGB(255, 255, 255));
    CPen penWhite(PS_SOLID, 0, RGB(255, 255, 255));

    CDC saveDC1;

    saveDC1.CreateCompatibleDC(m_dc);

    CBitmap bitmap2;
    bitmap2.CreateCompatibleBitmap(m_dc, r.right, r.bottom);
    saveDC1.SelectObject(bitmap2);

    saveDC1.BitBlt(r.left, r.top, r.Width(), r.Height(), mdc, 0, 0, SRCCOPY);

    CBrush* pOldBrush = saveDC1.SelectObject(&brushWhite);
    CPen* pOldPen = saveDC1.SelectObject(&penWhite);

    saveDC1.Polygon(ptVertex, n);

    pOldBrush = saveDC1.SelectObject(&brush);

    for (int i = 0; i < n; i++) {
        ptVertex[i].x += dx;
        ptVertex[i].y += dy;
    }

    saveDC1.Polygon(ptVertex, n);
    saveDC1.SelectObject(pOldBrush);
    saveDC1.SelectObject(pOldPen);

    m_dc->BitBlt(r.left, r.top, r.Width(), r.Height(), &saveDC1, 0, 0, SRCCOPY);
}


CDC* CScroll::BeginMoveW(CDC* m_dc, CCoordList& cl)
{
    SetCapture();

    CDC* mdc = new CDC();

    mdc->CreateCompatibleDC(m_dc);

    CRect r;
    GetClientRect(&r);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(m_dc, r.right, r.bottom);
    mdc->SelectObject(bitmap);
    mdc->BitBlt(r.left, r.top, r.Width(), r.Height(), m_dc, 0, 0, SRCCOPY);

    /*
      CPoint *ptVertex = new CPoint[cl.size()];

      CCoordList::const_iterator it = cl.begin();

      int i = 0;
      for (; it != m_NP.end(); ++it) {
        ptVertex[i++] = CoordToScreen(*it);
      }

      rgnA.CreatePolygonRgn( ptVertex, i, ALTERNATE);
      m_dc->SelectClipRgn(&rgnA);
    */

    return mdc;
}


void CScroll::toOb(const CFPoint& pt)
{
    m_pt_ob = pt;
    m_is_ob = true;
}

bool CScroll::moveXYmas(const CFPoint& pt)
{
    double bx0 = geom.m_bx;
    double by0 = geom.m_by;
    double masx0 = geom.masx;
    double masy0 = geom.masy;

    double mas0 = 1000;
    double mas = geom.masx * geom.dmas;

    if (mas0 < mas) {
        SetMas(mas0 / geom.dmas, 0, 0);
    }

    moveXY(pt);

    if (bx0 != geom.m_bx || by0 != geom.m_by || masx0 != geom.masx) {
        InvalidateG();
    }

    return true;
}

bool CScroll::moveXY(const CFPoint& pt)
{
    //  if (!m_rectBorder.PtInRect(pt)) return false;

    bool is_ob = m_is_ob;

    CRect rect;
    CFRect srect;
    GetClientRect(&rect);

    srect.SetRect((geom.m_bx + rect.left) * geom.masx, (geom.m_by + rect.top) * geom.masy,
        (geom.m_bx + rect.right) * geom.masx, (geom.m_by + rect.bottom) * geom.masy);

    if (!srect.PtInRect(pt)) {
        geom.m_bx = pt.x / geom.masx - rect.Width() / 2;
        geom.m_by = pt.y / geom.masy - rect.Height() / 2;
        InvalidateG();
    }
    CPoint p = CoordToScreen(pt);
    Mxy(p.x, p.y);


    m_is_ob = is_ob;
    return true;
}

void CScroll::OnFindcoord() {
    double fx, fy;
    CString text;

    CPoint point;
    CFPoint pt;

    GetCursorPos(&point);
    ScreenToClient(&point);

    pt = ScreenToCoord(point);
    text.Format(_T("%.2f %.2f"), -pt.y / 100., pt.x / 100.);

    CInputDialog inD(this, _TR("Координаты"), _TR("Введите новые координаты"), text);
    if (inD.DoModal() == IDOK) {
        text = inD.m_strInput;
        text.Replace(",", ".");
        if (_stscanf(text, _T("%lg %lg"), &fy, &fx) == 2)
            if (moveXY(CFPoint(fx * 100, -fy * 100)))
                toOb(CFPoint(fx * 100, -fy * 100));
            else {
                text.Format(_TR("Точка %.0lf %.0lf за пределами карты"), fy, fx);
                AfxMessageBox(text, MB_OK | MB_ICONINFORMATION);
            }
    }
}

void CScroll::OnFindcoordDeg() {
    CString text;

    CPoint point;
    CFPoint pt;

    GetCursorPos(&point);
    ScreenToClient(&point);

    pt = ScreenToCoord(point);

    double x, y;

    void get_google_coord(CFPoint pt, double& x, double& y);
    CFPoint get_google_to_coord(double x, double y);

    get_google_coord(pt, x, y);

    text.Format(_T("%.12f %.12f"), y, x);

    CInputDialog inD(this, _TR("Координаты"), _TR("Введите новые координаты"), text);
    if (inD.DoModal() == IDOK) {
        text = inD.m_strInput;
        text.Replace(",", ".");
        if (_stscanf(text, _T("%lg %lg"), &y, &x) == 2) {
            CFPoint pt2 = get_google_to_coord(x, y);

            if (moveXY(pt2)) {
                toOb(pt2);
            }
            else {
                text.Format(_TR("Точка %.0lf %.0lf за пределами карты"), pt2.y, pt2.x);
                AfxMessageBox(text, MB_OK | MB_ICONINFORMATION);
            }
        }
    }
}

void CScroll::OnSize(UINT nType, int cx, int cy)
{
    CBgi::OnSize(nType, cx, cy);
    SetBar();
}

void CScroll::OnViewScrollbar()
{
    m_bScrollBar = !m_bScrollBar;
    SetBar();
}

void CScroll::OnUpdateViewScrollbar(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bScrollBar);
}

void CScroll::XDraw(CDC* m_dc, bool on)
{
    if (!isXdraw) return;

    if (m_is_xdraw == on) {
        return;
    }

    //  m_is_xdraw = on;

    CPoint pt1, pt2;

    pt1 = CoordToScreen(m_pt1);
    pt2 = CoordToScreen(m_pt2);

    if (m_bIsFragment) {
        CPoint pt3 = CoordToScreen(m_pt1_2);
        setline(m_dc, 0x7F7F7F, 3);
        xrectangle(m_dc, pt1.x, pt1.y, pt3.x, pt3.y);
        m_is_xdraw = on;
        return;
    }

    setcolor(m_dc, DARKGRAY);
    switch (regim) {
    case R_BEGIN_RECT:
    case R_BEGIN_DRAW_COPY2:
    case R_BEGIN_DRAW_COPY_BMP2:
        setline(m_dc, 0x7F7F7F, 3);
        xrectangle(m_dc, pt1.x, pt1.y, pt2.x, pt2.y);
        m_is_xdraw = on;
        break;

    case R_BEGIN_RECT_1:
    {
        setline(m_dc, 0x7F7F7F, 3);

        double mas = m_print_mas;
        int dx = m_print_size.cx * mas / geom.masx / 10;
        int dy = m_print_size.cy * mas / geom.masy / 10;

        xrectangle(m_dc, pt2.x, pt2.y, pt2.x + dx, pt2.y + dy);
        m_is_xdraw = on;
    }
    break;

    case R_BEGIN_LEN2:
    case R_BEGIN_LEN_OKR2:
    case R_BEGIN_LEN_OKR_VYD_2:
    case R_BEGIN_LEN_OKR_VYD_GEO_2:
    case R_BEGIN_DRAW2:
    case R_BEGIN_DRAW_LINE:
    case R_BEGIN_POVOROT_2:
    case R_BEGIN_POVOROT_3:
        xline(m_dc, pt1.x, pt1.y, pt2.x, pt2.y);
        m_is_xdraw = on;
        break;

    case R_BEGIN_DRAW_TEXT2:
        //      is_paint = 0;
        //      if (m_dcSave) CloseImage(m_dcSave);
        //      m_dcSave = SaveImage();
        //      is_paint = 1;

        //      DrawLabelMultyZoom(m_dc, m_pt1, m_pt2, m_strText, 1);
        xline(m_dc, pt1.x, pt1.y, pt2.x, pt2.y);
        m_is_xdraw = on;
        break;

    case R_BEGIN_DRAW_TYPE:
    case R_BEGIN_MOVE_POINT6_2:
    {
        int saveROP;
        saveROP = m_dc->SetROP2(R2_NOTXORPEN);
        double thickness = 1.;
        double mas_otn_old = mas_otn;
        double dm = m_dc->GetDeviceCaps(LOGPIXELSX) / 2.54;
        mas_otn = geom.masx * dm / 500;

        drawLineTyp(m_dc, thickness, pt1.x, pt1.y, pt2.x, pt2.y, m_type_line, 0, 0, TRUE, TRUE, FALSE, TRUE, false);
        mas_otn = mas_otn_old;

        m_dc->SetROP2(saveROP);
    }
    m_is_xdraw = on;
    break;


    case R_BEGIN_MOVE_LINE:
    {
        CPoint p1, p2;
        int n = m_NP.size();

        if (n >= 2 && m_tecF >= 0) {
            p1 = CoordToScreen(m_NP.GetV((m_tecF + n - 1) % n));
            p2 = CoordToScreen(m_NP.GetV((m_tecF) % n));

            setline(m_dc, 0x7F7F7F, 3);
            xline(m_dc, p1.x, p1.y, p2.x, p2.y);
        }
        //        DrawRect(m_dc, m_pt2);
    }
    m_is_xdraw = on;
    break;

    case R_MOVE_LINE:
    {
        CPoint p1, p2;
        int n = m_NP.size();

        if (n >= 2 && m_tecF >= 0) {
            p1 = CoordToScreen(m_NP.GetV((m_tecF + n - 1) % n));
            p2 = CoordToScreen(m_NP.GetV((m_tecF + 1) % n));


            setline(m_dc, 0x7F7F7F, 3);

            if (m_locNP == 3 || m_tecF != 0) xline(m_dc, p1.x, p1.y, pt2.x, pt2.y);
            if (m_locNP == 3 || m_tecF != n - 1) xline(m_dc, p2.x, p2.y, pt2.x, pt2.y);
            DrawRect(m_dc, m_pt2);
        }
    }
    m_is_xdraw = on;
    break;

    case R_MOVE_PODP:
    {
        CPoint p1, p2;
        int n = m_NP.size();

        p1 = CoordToScreen(m_NP.GetV((m_tecF + n - 1) % n));
        p2 = CoordToScreen(m_NP.GetV((m_tecF + 1) % n));
        //        xline(m_dc,p1.x, p1.y, pt2.x, pt2.y);
        x_podp(m_dc, p1.x, p1.y, pt2.x, pt2.y, m_otn);
        setline(m_dc, 0x7F7F7F, 3);
        DrawRect(m_dc, m_pt2);
    }
    m_is_xdraw = on;
    break;
    }
}

void CScroll::OnMouseMove(UINT nFlags, CPoint point)
{
    //  CBgi::OnMouseMove(nFlags, point);
    //  return;

    CTic tic(this);

    CDC* m_dc;

    if ((m_dc = BeginPaint()) != NULL) {
        int shift = nFlags & MK_SHIFT;
        int ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;

        CFPoint pt = ScreenToCoord(point);

        if (m_is_ob && CoordToScreen(m_pt_ob) != point) {
            m_is_ob = false;
        }

        XDraw(m_dc, false);

        if (ctrl) pt = to90(pt);


        if (regim == R_MOVE_VIEW) MoveW(m_dc, mdc, (m_pt2.x - m_pt1.x) / geom.masx, (m_pt2.y - m_pt1.y) / geom.masy);
        if (regim == R_MOVE_VIEW_MIDDLE) MoveW(m_dc, mdc, (m_move_pt2.x - m_move_pt1.x) / geom.masx, (m_move_pt2.y - m_move_pt1.y) / geom.masy);
        if (regim == R_MOVE_VIEW_VYD) MoveW(m_dc, mdc, (m_pt2.x - m_pt1.x) / geom.masx, (m_pt2.y - m_pt1.y) / geom.masy, m_NP);

        if (regim == R_MOVE_VIEW_MIDDLE) {
            m_move_pt2 = pt;
        }
        else {
            m_pt2 = pt;
        }


        XDraw(m_dc, true);

        CString str;
        str.Format("%-8.2lf, %-8.2lf", -0.01 * pt.y, 0.01 * pt.x);
        //  str.Format(_T(" %lg %lg "), pt.y, pt.x);

        if (is_google_map && 0) {
            double y = y2lat(-0.0000001 * pt.y);
            double x = x2lon(0.0000001 * pt.x);

            CString sx, sy;

            if (x > 0) {
                sx.Format("%.4lf°ВД", x);
            }
            else {
                sx.Format("%.4lf°ЗД", -x);
            }

            if (y > 0) {
                sy.Format("%.4lf°СШ", y);
            }
            else {
                sy.Format("%.4lf°ЮШ", -y);
            }

            void d2m(int zona, double B, double L, double& x, double& y);

            double xx, yy;
            d2m(-1, y, x, xx, yy);

            str.Format("%-8.2lf, %-8.2lf", yy, xx);
            str.Format("%s, %s", sy, sx);

            Status(3, str);
        }

        if (regim == R_BEGIN_LEN2 || regim == R_BEGIN_LEN_OKR2 || regim == R_BEGIN_DRAW2 || regim == R_BEGIN_DRAW_LINE) {
            double len1, len2;


            len1 = LengthG(m_pt1, pt, is_google_map & GOOGLE_LEN);
            len2 = m_NP.LengthG(is_google_map & GOOGLE_LEN);

            double uu;

            uu = (m_pt1.x == pt.x && m_pt1.y == pt.y) ? 0 :
                -atan2(m_pt1.x - pt.x, m_pt1.y - pt.y) * 180 / M_PI;

            //      str.Format(_T("  Полная длина %.2lf, Длина %.2lf, Угол %5.1f"), len2+len1, len1, uu);
            if (len2 <= 0) {
                str.Format(_TR("Длина отрезка %.1lf м"), len1);
            }
            else {
                str.Format(_TR("Суммарная длина выделенного пути %.1lf м, Длина последнего отрезка %.1lf м"), len2, len1);
                str.Format(_TR("Общая длина %.1lf м, последний отрезок %.1lf м"), len2, len1);
            }

            Status(0, str);
        }


        EndPaint(m_dc);
    }

    CBgi::OnMouseMove(nFlags, point);
}

void CScroll::OnMButtonDown(UINT nFlags, CPoint point)
{
    CTic tic(this);
    oldregim = regim;

    CFPoint pt = ScreenToCoordOb(point);

    SetCur(ID_MOVEW);

    //   XDraw0(false);


    pt.x = okr3(pt.x, m_dX, false);
    pt.y = okr3(pt.y, m_dX, false);
    m_move_pt1 = m_move_pt2 = pt;
    {

        CClientDC dc(this);


        mdc = new CDC();

        mdc->CreateCompatibleDC(&dc);

        CRect r;
        GetClientRect(&r);
        CBitmap bitmap;

        bitmap.CreateCompatibleBitmap(&dc, r.right, r.bottom);
        mdc->SelectObject(bitmap);

        CRect rect1;
        SetDC(mdc);

        mdc->SetBkColor(m_bk_color);
        GetClientRect(rect1);
        CBrush brush(m_bk_color);
        mdc->FillRect(rect1, &brush);

        redrawPaint(mdc);
        DrawLine(mdc);
        XDraw(mdc, false);
        XDraw(mdc, true);



        SetCapture();

    }

    //   XDraw(true);


    regim = R_MOVE_VIEW_MIDDLE;

    CBgi::OnMButtonDown(nFlags, point);
}

void CScroll::OnMButtonUp(UINT nFlags, CPoint point)
{
    CTic tic(this);
    CFPoint pt = ScreenToCoordOb(point);

    if (regim == R_MOVE_VIEW_MIDDLE) {
        CDC* m_dc;
        if ((m_dc = BeginPaint()) != NULL) {
            EndMoveW(m_dc, mdc, (pt.x - m_move_pt1.x) / geom.masx, (pt.y - m_move_pt1.y) / geom.masy);
            EndPaint(m_dc);
            XDraw(mdc, false);
        }
        regim = oldregim;
    }
    SetCur(m_nRegim);

    CBgi::OnMButtonUp(nFlags, point);
}


//static int m_dbl = 0;

void CScroll::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    int shift = GetKeyState(VK_SHIFT) & 0x80;
    int ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;

    if (shift) {   // отметка
        CFPoint pt = ScreenToCoordOb(point);
        sfind(pt);
        return;
    }


    if (regim == R_INIT && m_nRegim == ID_INFO) {
        if (!m_dbl) {
            m_dbl = 1;
            m_nTimerDbl = SetTimer(4, ::GetDoubleClickTime(), 0);
            m_nFlagsDbl = nFlags;
            m_pointDbl = point;
        }
    }
    else {
        OnLButtonDown2(nFlags, point);
    }
}


void CScroll::OnLButtonDown2(UINT nFlags, CPoint point)
{
    CTic tic(this);

    CFPoint pt = ScreenToCoordOb(point);

    int ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;


    switch (regim) {

        //    case R_INIT :
    case R_ALSECO_PICK:
    case R_NODE_PICK:
    case R_NODE_PICK1:
    case R_NODE_PICK2:
    case R_NODE_ADD:
    case R_BEGIN_DRAW_LINE:
    case R_BEGIN_DRAW_TYPE:
    case R_BEGIN_DRAW_TEXT1:
    case R_BEGIN_DRAW_TEXT2:
    case R_BEGIN_MOVE_POINT1:  // Установит точку
    case R_BEGIN_MOVE_POINT6:  // Установит точку
    case R_BEGIN_MOVE_POINT6_2:  // Установит точку

        if (ctrl) pt = to90(pt);

        find(pt);
        break;

        /*
            case R_BEGIN_DRAW_TEXT2:
              if (m_dcSave) {
                CloseImage(m_dcSave);
                m_dcSave = NULL;
              }
              find(pt);
              break;
        */


    case R_BEGIN_DRAW_COPY1:
    case R_BEGIN_DRAW_COPY_BMP1:
        oldregim = regim;
        switch (regim) {
        case R_BEGIN_DRAW_COPY1:
            regim = R_BEGIN_DRAW_COPY2;
            m_bIsFragment = FALSE;
            break;
            //        case R_BEGIN_DRAW_COPY_BMP1:
            //          regim = R_BEGIN_DRAW_COPY_BMP2;
            //          break;
        }
        m_pt2 = m_pt1 = pt;
        MySetCursor(0, IDC_CROSS);
        break;

    case R_BEGIN_DRAW1:
    case R_BEGIN_DRAW2:

        if (ctrl) pt = to90(pt);

        pt.x = okr3(pt.x, m_dX, false);
        pt.y = okr3(pt.y, m_dX, false);
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);
        CDC* m_dc;
        if ((m_dc = BeginPaint()) != NULL) {
            DrawRect(m_dc, pt);
            EndPaint(m_dc);
        }

        regim = R_BEGIN_DRAW2;
        break;

    case R_BEGIN_LEN1:
    case R_BEGIN_LEN2:
        //      pt.x = okr(pt.x, m_dX);
        //      pt.y = okr(pt.y, m_dX);
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);
        if ((m_dc = BeginPaint()) != NULL) {
            DrawRect(m_dc, pt);
            EndPaint(m_dc);
        }

        regim = R_BEGIN_LEN2;
        break;
        /*
            case  R_BEGIN_DRAW_TEXT1 :
              regim = R_BEGIN_DRAW_TEXT2;
              m_pt1 = m_pt2 = pt;
              break;
            case R_BEGIN_DRAW_TEXT2 :
              regim = R_INIT;
              m_nRegim = ID_INFO;
              m_pt2 = pt;
              createText(m_pt1, m_pt2, m_strText);
              break;
        */
    case R_BEGIN_LEN_OKR1:
    case R_BEGIN_LEN_OKR2:
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);
        if ((m_dc = BeginPaint()) != NULL) {
            DrawRect(m_dc, pt);
            EndPaint(m_dc);
        }

        regim = R_BEGIN_LEN_OKR2;
        break;


    case R_BEGIN_LEN_OKR_VYD_1:
    case R_BEGIN_LEN_OKR_VYD_2:
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);
        if ((m_dc = BeginPaint()) != NULL) {
            DrawRect(m_dc, pt);
            EndPaint(m_dc);
        }

        regim = R_BEGIN_LEN_OKR_VYD_2;
        break;

    case R_BEGIN_LEN_OKR_VYD_GEO_1:
    case R_BEGIN_LEN_OKR_VYD_GEO_2:
        m_pt1 = m_pt2 = pt;
        m_NP.push_back(pt);
        if ((m_dc = BeginPaint()) != NULL) {
            DrawRect(m_dc, pt);
            EndPaint(m_dc);
        }

        regim = R_BEGIN_LEN_OKR_VYD_GEO_2;
        break;



    case R_BEGIN_MOVE_LINE:
        if (m_NP.size()) find_point2(pt);
        break;

    case R_BEGIN_MOVE_PODP:
        if (m_NP.size()) find_podp2(pt);
        break;

    case R_MOVE_LINE:
        pt.x = okr3(pt.x, m_dX, false);
        pt.y = okr3(pt.y, m_dX, false);

        if ((m_dc = BeginPaint()) != NULL) {
            XDraw(m_dc, false);
            m_pt2 = pt;
            m_NP.SetV(m_tecF, pt);
            XDraw(m_dc, true);
            EndPaint(m_dc);
        }

        regim = R_BEGIN_MOVE_LINE;
        break;

    case R_MOVE_PODP:
        pt.x = okr3(pt.x, m_dX, false);
        pt.y = okr3(pt.y, m_dX, false);

        if ((m_dc = BeginPaint()) != NULL) {
            XDraw(m_dc, false);
            m_pt2 = pt;
            m_NP.SetV(m_tecF, pt);
            XDraw(m_dc, true);
            EndPaint(m_dc);
        }

        regim = R_BEGIN_MOVE_PODP;
        break;


    case R_BEGIN_MOVE_VIEW:
        regim = R_MOVE_VIEW;
        pt.x = okr3(pt.x, m_dX, false);
        pt.y = okr3(pt.y, m_dX, false);
        m_pt1 = m_pt2 = pt;
        {
            CClientDC dc(this);

            mdc = new CDC();

            mdc->CreateCompatibleDC(&dc);

            CRect r;
            GetClientRect(&r);
            CBitmap bitmap;

            bitmap.CreateCompatibleBitmap(&dc, r.right, r.bottom);
            mdc->SelectObject(bitmap);

            CRect rect1;
            SetDC(mdc);

            mdc->SetBkColor(m_bk_color);
            GetClientRect(rect1);
            CBrush brush(m_bk_color);
            mdc->FillRect(rect1, &brush);

            redrawPaint(mdc);

            SetCapture();
        }
        break;
        //-----------------------------------------------
        //-----------------------------------------------

    case R_BEGIN_MOVE_FRAGMENT_1:
        regim = R_MOVE_VIEW_VYD;
        pt.x = okr3(pt.x, m_dX, false);
        pt.y = okr3(pt.y, m_dX, false);
        m_pt1 = m_pt2 = pt;
        if ((m_dc = BeginPaint()) != NULL) {
            mdc = BeginMoveW(m_dc, m_NP);
            EndPaint(m_dc);
        }
        break;

    case R_BEGIN_POVOROT_1:
        m_pt1 = m_pt2 = pt;
        regim = R_BEGIN_POVOROT_2;
        break;

    case R_BEGIN_POVOROT_2:
        if ((m_dc = BeginPaint()) != NULL) {
            CPoint pt1, pt2;
            pt1 = CoordToScreen(m_pt1);
            pt2 = CoordToScreen(m_pt2);
            line(m_dc, pt1.x, pt1.y, pt2.x, pt2.y);
            EndPaint(m_dc);
        }
        m_pt3 = pt;

        regim = R_BEGIN_POVOROT_3;
        break;

    case R_BEGIN_POVOROT_3:
        regim = R_INIT;

        m_pt_alpha = m_pt1;

        double ugol(double x1, double y1, double x2, double y2, double x3, double y3);

        geom.m_alpha += ugol(m_pt1.x, m_pt1.y, m_pt3.x, m_pt3.y, m_pt2.x, m_pt2.y);

        set_coord_gid(m_pt_alpha.x, m_pt_alpha.y, geom.m_alpha);

        m_nRegim = ID_INFO;

        InvalidateG();

        break;

    case R_BEGIN_RECT_1:
    {
        regim = R_INIT;
        m_bIsFragment = TRUE;
        m_is_print_fr = TRUE;
        m_pt1 = pt;
        double mas = m_print_mas;
        double dx = m_print_size.cx * mas / 10;
        double dy = m_print_size.cy * mas / 10;
        m_pt1_2 = m_pt1 + CFPoint(dx, dy);
    }
    break;

    case R_INIT:
        if (m_nRegim == ID_ZOOM || m_nRegim == ID_EDIT_COPY || m_nRegim == ID_EDIT_COPYBMP) {
            oldregim = regim;
            regim = R_BEGIN_RECT;
            m_pt2 = m_pt1 = pt;
            MySetCursor(0, IDC_CROSS);
        }
        else {
            find(pt);
        }
        break;

        //-----------------------------------------------

    default:break;
    }

    CBgi::OnLButtonDown(nFlags, point);
}

void CScroll::OnLButtonUp(UINT nFlags, CPoint point)
{
    CTic tic(this);

    CDC* m_dc;

    CFPoint pt = ScreenToCoordOb(point);

    switch (regim) {

    case R_INIT:
    case R_BEGIN_DRAW_LINE:
        findUP(pt);
        break;

    case R_MOVE_VIEW:
        if ((m_dc = BeginPaint()) != NULL) {
            EndMoveW(m_dc, mdc, (pt.x - m_pt1.x) / geom.masx, (pt.y - m_pt1.y) / geom.masy);
            EndPaint(m_dc);
        }
        regim = R_BEGIN_MOVE_VIEW;
        break;

    case R_MOVE_VIEW_VYD:
        if ((m_dc = BeginPaint()) != NULL) {
            //        EndMoveW(m_dc, mdc, (pt.x-m_pt1.x)/masx, (pt.y-m_pt1.y)/masy);
            EndPaint(m_dc);
            if (AfxMessageBox(_TR("Передвинуть фрагмент?"), MB_YESNO) == IDYES) {
                MoveFragm(pt.x - m_pt1.x, pt.y - m_pt1.y);
            }
        }
        m_NP.clear();
        regim = R_INIT;
        InvalidateG();
        break;


        //-----------------------------------------------
  //-----------------------------------------------
    case R_BEGIN_RECT:
    case R_BEGIN_DRAW_COPY2:
    case R_BEGIN_DRAW_COPY_BMP2:
        switch (regim) {
        case R_BEGIN_RECT:
            ChMas(CFRect(m_pt1, m_pt2));
            InvalidateG();
            break;
        case R_BEGIN_DRAW_COPY2:
            //          CopyBlock(CFRect(m_pt1, m_pt2));
            m_pt1_2 = m_pt2;
            m_bIsFragment = TRUE;
            break;
        case R_BEGIN_DRAW_COPY_BMP2:
            //          CopyBlockBMP(CFRect(m_pt1, m_pt2));
            break;
        }
        regim = R_INIT;
        SetCur(m_nRegim);
        MySetCursor(0, IDC_ARROW);
        break;
        //-----------------------------------------------


    }

    //  if (regim == 0) //SetCursor(0,IDC_ARROW);
    SetCur(m_nRegim);

    CBgi::OnLButtonUp(nFlags, point);
}



void CScroll::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CTic tic(this);

    CFPoint pt = ScreenToCoordOb(point);

    XDraw0(false);

    switch (regim) {
    case R_BEGIN_DRAW2:
        createObj();
        m_NP.clear();
        regim = R_INIT;
        InvalidateG();
        SetCur(m_nRegim);
        break;

    case R_BEGIN_LEN1:
    case R_BEGIN_LEN2:
    {
        CDC* m_dc = BeginPaint();
        if (m_dc != NULL) {
            DrawLine(m_dc);
            EndPaint(m_dc);
        }
        m_NP.clear();
        regim = R_INIT;
        SetCur(m_nRegim);
    }
    break;

    case R_BEGIN_LEN_OKR1:
    case R_BEGIN_LEN_OKR2:
        //      regim = R_BEGIN_LEN_OKR3;
    {
        CDC* m_dc = BeginPaint();
        if (m_dc != NULL) {
            DrawLine(m_dc);
            EndPaint(m_dc);
        }
        mark_vyd();
        InvalidateG();
        regim = R_BEGIN_LEN_OKR1;
        SetCur(m_nRegim);
    }
    break;

    case R_BEGIN_LEN_OKR_VYD_2:
    {
        m_nRegim = ID_FRAGMENT_POLY;
        CCoordList::iterator it = m_NP.begin();
        m_NP.push_back(*it);
        regim = R_BEGIN_MOVE_LINE;
    }

    break;

    case R_BEGIN_LEN_OKR_VYD_GEO_2:
    {
        m_nRegim = ID_FRAGMENT_POLY_GEO;
        CCoordList::iterator it = m_NP.begin();
        m_NP.push_back(*it);
        regim = R_BEGIN_MOVE_LINE;
    }

    break;


    case R_BEGIN_MOVE_LINE:
        if (m_NP.size() > 0 && m_nRegim != ID_FRAGMENT_POLY) {
            regim = R_INIT;
            updateObj();
            SetCur(m_nRegim);
        }
        break;

    case R_BEGIN_MOVE_PODP:
        if (m_NP.size() > 0 && m_nRegim != ID_FRAGMENT_POLY) {
            regim = R_INIT;
            updateObj();
            SetCur(m_nRegim);
        }
        break;

    }

    XDraw0(true);

    CBgi::OnLButtonDblClk(nFlags, point);
}


void CScroll::OnRButtonDown(UINT nFlags, CPoint point)
{
    CTic tic(this);

    CMenu menubar;
    if (menubar.LoadMenu(IDR_POPUPMENUS))
    {
        // debug menu is the first popup
        CMenu* pPopup = menubar.GetSubMenu(0);
        ASSERT(pPopup != NULL);

        // route commands through this main frame
        pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
    }

    CBgi::OnRButtonDown(nFlags, point);
}

#if 0


void CScroll::CopyBlock(CFRect rect)
{
#if 0

        // Размеры в сантиметрах
        const int sideInCentimeters = 10;

        // Перевод сантиметров в сотые доли миллиметра (HIMETRIC)
        const int sideInHIMETRIC = static_cast<int>(sideInCentimeters * 1000);

        // Создание прямоугольника для метафайла
        RECT rect = { 0, 0, sideInHIMETRIC, sideInHIMETRIC };

        // Создание расширенного метафайла
        HDC hDC = ::CreateEnhMetaFile(NULL, NULL, &rect, NULL);
        if (hDC == NULL) {
            AfxMessageBox(_T("Не удалось создать метафайл."));
            return;
        }

        // Создание контекста устройства
        CDC dc;
        dc.Attach(hDC);

        // Настройка масштаба логических единиц на 1000 единиц на метр (то есть 10 единиц на миллиметр)
        dc.SetMapMode(MM_HIMETRIC);

        // Заполнение белым фоном
        CBrush whiteBrush(RGB(255, 255, 255));
        dc.FillRect(&rect, &whiteBrush);

        // Рисование черного квадрата
        CBrush blackBrush(RGB(0, 0, 0));
        dc.SelectObject(&blackBrush);
        dc.Rectangle(0, 0, sideInHIMETRIC, sideInHIMETRIC);

        // Завершение создания метафайла
        HENHMETAFILE hEnhMetaFile = ::CloseEnhMetaFile(hDC);
        dc.Detach();

        if (hEnhMetaFile == NULL) {
            AfxMessageBox(_T("Не удалось закрыть метафайл."));
            return;
        }

        // Открытие буфера обмена
        if (OpenClipboard()) {
            // Очистка буфера обмена
            EmptyClipboard();

            // Установка метафайла в буфер обмена
            SetClipboardData(CF_ENHMETAFILE, hEnhMetaFile);

            // Закрытие буфера обмена
            CloseClipboard();
        }
        else {
            AfxMessageBox(_T("Не удалось открыть буфер обмена."));
        }

        // Удаление метафайла, если он не был перемещен в буфер обмена
        if (hEnhMetaFile) {
            DeleteEnhMetaFile(hEnhMetaFile);
        }

#else

    double masx_old = masx, masy_old = masy, bx_old = m_bx, by_old = m_by;
    int m_nTransp_old = m_nTransp;
    int dx, dy;
    int dx1, dy1;


    if (!OpenClipboard()) {
        AfxMessageBox("Не удается открыть буфер обмена", MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if (!EmptyClipboard()) {
        AfxMessageBox("Не удается очистить буфер обмена", MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    CClientDC screenDC(NULL);
    double dmas = screenDC.GetDeviceCaps(LOGPIXELSX) / 2.54;

    double mas = masx * dmas;

//    double lx0 = screenDC.GetDeviceCaps(LOGPIXELSX);
//    double dm = screenDC.GetDeviceCaps(LOGPIXELSX) / 2.54;

    CClientDC odc(this);

    double lx = odc.GetDeviceCaps(LOGPIXELSX);
    double ly = odc.GetDeviceCaps(LOGPIXELSY);

//    masx = masx * lx / 96;
//    masy = masy * lx / 96;


//    masx = masx * 96 / lx;
//    masy = masy * 96 / ly;

//    masx /= dmas;

//    masy = masx;


    //////////////

    m_bx = rect.left / masx;
    m_by = rect.top / masy;

    dx = rect.Width() / masx;
    dy = rect.Height() / masy;

/**

//    double ddd = 40 / 2.54;
//    double ddd = 27.47*lx0/lx;
    double ddd = 27.59 * lx0 / lx;

    dx1 = dx * ddd;
    dy1 = dy * ddd;
*/
    dx1 = dx*10;
    dy1 = dy*10;

    CRect rectView(0, 0, dx1, dy1);

    CMetaFileDC dcMeta;

    dcMeta.CreateEnhMetaFile(&odc, NULL, rectView, "GeneDoc\0MetaFile\0\0");
    dcMeta.Rectangle(0, 0, dx, dy);

    dcMeta.SetAttribDC(odc.m_hAttribDC);

    OnPrepareDC(&dcMeta);

    dcMeta.SetMapMode(MM_TEXT);
    SetRect(CRect(0, 0, dx, dy));

    m_isPrint = 1;
    redrawPaint2(&dcMeta);
    m_isPrint = 0;

    HENHMETAFILE hMeta = dcMeta.CloseEnhanced();

    if (SetClipboardData(CF_ENHMETAFILE, hMeta) == NULL) {
        AfxMessageBox("Select Failed in SetClipBoardData", MB_OK | MB_ICONEXCLAMATION);
    }

    // CBitmap
    //
    if (!CloseClipboard()) {
        AfxMessageBox("CloseClipboard Failed", MB_OK | MB_ICONEXCLAMATION);
    }

    //    ReleaseDC(pDC);   // Moved this down here so nt4.0 would work.

    masx = masx_old; masy = masy_old;
    m_bx = bx_old; m_by = by_old;
    m_nTransp = m_nTransp_old;

    return;
#endif 
}

#endif

//void CScroll::CopyBlock(CFRect rect)
//{
//    CopyBlockBMP(rect);
//}



void CScroll::CopyBlock(CFRect rect)
{
    double masx_old = geom.masx, masy_old = geom.masy, bx_old = geom.m_bx, by_old = geom.m_by;
    int m_nTransp_old = m_nTransp;
    int dx, dy;
    int dx1, dy1;

    rect.NormalizeRect();




    if (!OpenClipboard()) {
        AfxMessageBox("Не удается открыть буфер обмена", MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if (!EmptyClipboard()) {
        AfxMessageBox("Не удается очистить буфер обмена", MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    CClientDC screenDC(NULL);

    double hx = screenDC.GetDeviceCaps(HORZSIZE);
    double hy = screenDC.GetDeviceCaps(VERTSIZE);

    double hrx = screenDC.GetDeviceCaps(HORZRES);
    double hry = screenDC.GetDeviceCaps(VERTRES);


    double lx0 = screenDC.GetDeviceCaps(LOGPIXELSX);
    double dm = screenDC.GetDeviceCaps(LOGPIXELSX) / 2.54;

    CClientDC odc(this);

    int lx = odc.GetDeviceCaps(LOGPIXELSX);
    int ly = odc.GetDeviceCaps(LOGPIXELSY);

    geom.masx = geom.masx * lx / 96;
    geom.masy = geom.masy * lx / 96;
    //////////////

    geom.m_bx = rect.left / geom.masx;
    geom.m_by = rect.top / geom.masy;

    dx = rect.Width() / geom.masx;
    dy = rect.Height() / geom.masy;

    //    double ddd = 40 / 2.54;
    //    double ddd = 27.47*lx0/lx;
//    double ddd = 27.59 * lx0 / lx;
//    double ddd = 17.5;
//    double ddd = 15.7;
//    double ddd = 15.65;
    double ddd = 40 / 2.54 * 3840./hrx;

    dx1 = dx * ddd;
    dy1 = dy * ddd;

    CRect rectView(0, 0, dx1, dy1);

    CMetaFileDC dcMeta;

    dcMeta.CreateEnhanced(&odc, NULL, rectView, "GeneDoc\0MetaFile\0\0");

    dcMeta.SetAttribDC(odc.m_hAttribDC);

    OnPrepareDC(&dcMeta);

    dcMeta.SetMapMode(MM_TEXT);
    SetRect(CRect(0, 0, dx, dy));

    m_isPrint = 1;
    redrawPaint2(&dcMeta);

    CPoint fil[10];

//    dx /= 2;
//    dy /= 2;


    fil[0] = CPoint(0, 0);
    fil[1] = CPoint(0, dy);
    fil[2] = CPoint(dx, dy);
    fil[3] = CPoint(dx, 0);
    fil[4] = CPoint(0, 0);
    fil[5] = CPoint(dx, dy);
    fil[6] = CPoint(dx, 0);
    fil[7] = CPoint(0, dy);

//    dcMeta.Polyline(fil, 8);

    m_isPrint = 0;

    HENHMETAFILE hMeta = dcMeta.CloseEnhanced();

    if (SetClipboardData(CF_ENHMETAFILE, hMeta) == NULL) {
        AfxMessageBox("Select Failed in SetClipBoardData", MB_OK | MB_ICONEXCLAMATION);
    }

    // CBitmap
    //
    if (!CloseClipboard()) {
        AfxMessageBox("CloseClipboard Failed", MB_OK | MB_ICONEXCLAMATION);
    }

    //    ReleaseDC(pDC);   // Moved this down here so nt4.0 would work.

    geom.masx = masx_old; geom.masy = masy_old;
    geom.m_bx = bx_old; geom.m_by = by_old;

    m_nTransp = m_nTransp_old;

    return;
}



void savePNG(HDC& hdc, int nWidth, int nHeight, const char* fn);

// Для HTTP

void CScroll::CopyBlockPng2(CDC& dc, double mas, double bx, double by, int dx, int dy, const char* fn, int xx, int yy, int zz)
{
    double masx_old = geom.masx, masy_old = geom.masy, bx_old = geom.m_bx, by_old = geom.m_by;
    geom.masx = geom.masy = mas / geom.dmas;

    try {
        geom.m_bx = bx / geom.masx;
        geom.m_by = by / geom.masy;

        SetRect(CRect(0, 0, dx, dy));

        CRect rect1;
        dc.SetBkColor(0xFFFFFF);
        CBrush brush(0xFFFFFF);
        dc.FillRect(m_rect, &brush);

        m_isPrint = 1;

        redrawPaint2(&dc);

        if (0) {
            CFont font;
            font.CreatePointFont(80, _T("Arial"));
            CFont* oldfont = dc.SelectObject(&font);
            dc.SetTextAlign(TA_LEFT | TA_TOP);
            dc.SetTextColor(0);
            CString ss;
            ss.Format("%d %d %d", zz, xx, yy);

            dc.TextOut(0, 0, ss);
            rectangle(&dc, 0, 0, dx, dy);
            dc.SelectObject(oldfont);
        }

        m_isPrint = 0;

        ::savePNG(dc.m_hDC, dx, dy, fn);
    }
    catch (CException* e) {
        //    throw e;

        TCHAR s[256];
        e->GetErrorMessage(s, 255);
        AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
        e->Delete();
    }
    catch (...) {
    }


    geom.masx = masx_old; geom.masy = masy_old;
    geom.m_bx = bx_old; geom.m_by = by_old;
}


void draw_ramka(CDC &dc, const CString &txt, int dx, int dy)
{
    return;
    CFont font;
//    font.CreatePointFont(80, _T("Arial"));
    font.CreatePointFont(80, _T("courier"));
    CFont* oldfont = dc.SelectObject(&font);
    dc.SetTextAlign(TA_LEFT | TA_TOP);
    dc.SetTextColor(0);

    dc.TextOut(0, 0, txt);
//    rectangle(&dc, 0, 0, DD-1, DD-1);

    CPoint fil[5];

    fil[0] = CPoint(0, 0);
    fil[1] = CPoint(0, dy-0);
    fil[2] = CPoint(dx-0, dy-0);
    fil[3] = CPoint(dx-0, 0);
    fil[4] = CPoint(0, 0);

    dc.Polyline(fil, 5);

    dc.SelectObject(oldfont);
}


void CScroll::CopyBlockPng4(CDC& dc, double masx, double masy, double bx0, double by0, int dx, int dy)
{
    ScrollGeom geom_old = geom;

    geom.masx = masx;
    geom.masy = masy;

    try {
        geom.m_bx = bx0 / geom.masx;
        geom.m_by = by0 / geom.masy;

        SetRect(CRect(0, 0, dx, dy));

        dc.SetBkColor(0xFFFFFF);
        CBrush brush(0xFFFFFF);
        dc.FillRect(m_rect, &brush);

        m_isPrint = 1;
        redrawPaint2(&dc);
//        draw_ramka(dc, txt, dx, dy);
        m_isPrint = 0;
    }
    catch (CException* e) {
        //    throw e;

        TCHAR s[256];
        e->GetErrorMessage(s, 255);
        AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
        e->Delete();
    }
    catch (...) {
    }

    geom = geom_old;
}






// Для сохранения

void CScroll::CopyBlockPng(double mas, CFRect rect, const char* fn)
{
    double masx_old = geom.masx, masy_old = geom.masy, bx_old = geom.m_bx, by_old = geom.m_by;
    geom.masx = geom.masy = mas / geom.dmas;

    try {
        int dx = rect.Width() / geom.masx;
        int dy = rect.Height() / geom.masy;
        dx = (dx + 7) / 8 * 8;

        geom.m_bx = rect.left / geom.masx;
        geom.m_by = rect.top / geom.masy;

        CClientDC oDC(this);

        CDC dc;
        dc.CreateCompatibleDC(&oDC);

        CBitmap bitmap;
        bitmap.CreateCompatibleBitmap(&oDC, dx, dy);
        dc.SelectObject(bitmap);

        SetRect(CRect(0, 0, dx, dy));

        CRect rect1;
        dc.SetBkColor(0xFFFFFF);
        CBrush brush(0xFFFFFF);
        dc.FillRect(m_rect, &brush);

        m_isPrint = 1;
        redrawPaint2(&dc);
        m_isPrint = 0;

        ::savePNG(dc.m_hDC, dx, dy, fn);
    }
    catch (CException* e) {
        //    throw e;
        TCHAR s[256];
        e->GetErrorMessage(s, 255);
        AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
        e->Delete();
    }
    catch (...) {
    }

    geom.masx = masx_old; geom.masy = masy_old;
    geom.m_bx = bx_old; geom.m_by = by_old;
}

void CScroll::CopyBlockPng(CFRect rect)
{
    double masx_old = geom.masx, masy_old = geom.masy, bx_old = geom.m_bx, by_old = geom.m_by;

    CFileDialog fd(FALSE, _T("PNG"), "", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST, _T("PNG-файл (*.png)|*.png|"), AfxGetMainWnd());

    CString text;
    text.Format(_T("%g"), geom.masx * geom.dmas);
    double m;

    CInputDialog inD(this, _TR("Масштаб 1:N"), _TR("Введите масштаб N:"), text);
    if (inD.DoModal() == IDOK) {
        text = inD.m_strInput;
        if (text[0]) {
            if (_stscanf(text, _T("%lg"), &m) > 0) {
                if (fd.DoModal() == IDOK) {
                    try {
                        CWaitCursor wait;
                        CopyBlockPng(m, rect, fd.GetPathName());
                    }
                    catch (CException* e) {
                        AfxMessageBox(_TR("Ошибка"), MB_OK | MB_ICONINFORMATION);
                        e->Delete();
                    }
                    catch (...) {
                    }
                }
            }
        }
    }
    geom.masx = masx_old; geom.masy = masy_old;
    geom.m_bx = bx_old; geom.m_by = by_old;
}

void CScroll::CopyBlockBMP(CFRect rect)
{
    double masx_old = geom.masx, masy_old = geom.masy, bx_old = geom.m_bx, by_old = geom.m_by;
    int m_nTransp_old = m_nTransp;
    int dx, dy;
    double inch = 2.54;

    m_nTransp = 0;

    rect.NormalizeRect();

    OpenClipboard();

    if (::EmptyClipboard()) {

        CWaitCursor cur;

        dx = rect.Width() / geom.masx;
        dy = rect.Height() / geom.masy;

        //    masx = (masx*dmas)*inch/96;
        //    masy = (masy*dmas)*inch/96;
        geom.m_bx = rect.left / geom.masx;
        geom.m_by = rect.top / geom.masy;

        CClientDC odc(this);

        CDC pDibDC;
        pDibDC.CreateCompatibleDC(&odc);

        pDibDC.SetWindowExt(CSize(dx, dy));
        pDibDC.SetWindowOrg(0, 0);

        pDibDC.m_hAttribDC = pDibDC.m_hDC;
        SetDC(&pDibDC);

        CBitmap bitmap;
        bitmap.CreateCompatibleBitmap(&odc, dx, dy);

        pDibDC.SelectObject(bitmap);

        CBrush brush, * pOldBrush;
        brush.CreateSolidBrush(RGB(255, 255, 255));
        pOldBrush = pDibDC.SelectObject(&brush);

        pDibDC.PatBlt(0, 0, dx, dy, PATCOPY);

        SetRect(CRect(0, 0, dx, dy));

        m_isPrint = 1;
        redrawPaint2(&pDibDC);
        m_isPrint = 0;

        pDibDC.SelectObject(pOldBrush);
        brush.DeleteObject();

        //    if (isPl) redrawPlan();

        ::SetClipboardData(CF_BITMAP, HBITMAP(bitmap));

        geom.masx = masx_old; geom.masy = masy_old;
        geom.m_bx = bx_old; geom.m_by = by_old;
        m_nTransp = m_nTransp_old;
    }

    CloseClipboard();
}


#include <math.h>

double CScroll::mas1() { return 0.1; };
double CScroll::mas2() { return 1e8; };


void CScroll::SetMas(double m, double bx0, double by0)
{
    //  XDraw0(false);

    double mm = m * geom.dmas;

    double m1 = mas1(), m2 = mas2();

    if (mm < m1) {
        mm = m1;
        m = mm / geom.dmas;
    }
    if (mm > m2) {
        mm = m2;
        m = mm / geom.dmas;
    }


    //  if (mm < M0 || mm > M255) {
    //  if (mm < M0) {
    if (mm <= 0) {
        AfxMessageBox(_TR("Масштаб 1: недопустим"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        //    nd =  (int) min(255.0, 255.0 * log(mm/M0) / log(M255/M0));
        geom.m_bx = bx0 * geom.masx / m;
        geom.m_by = by0 * geom.masy / m;
        geom.masx = geom.masy = m;
    }
    //  XDraw0(true);

    CString str;
    str.Format(_T("1:%.0f"), m * geom.dmas);
    Status(2, str);
    SetBar();
}

void CScroll::MoveRect(CFRect rect)
{
    if (rect.left == LONG_MAX) return;

    int cx = (rect.left + rect.right) / 2;
    int cy = (rect.top + rect.bottom) / 2;
    int w = rect.Width();
    int h = rect.Height();

    if (w <= 0) w = 10;
    if (h <= 0) h = 10;

    int ww = m_rect.Width();
    int hh = m_rect.Height();

    if (ww <= 0) ww = 10;
    if (hh <= 0) hh = 10;


    if (rect.Width() * hh < ww * rect.Height()) {
        w = h * ww / hh;
    }
    else {
        h = w * hh / ww;
    }

    rect.left = cx - w / 2;
    rect.right = cx + w / 2;
    rect.top = cy - h / 2;
    rect.bottom = cy + h / 2;


    rect.InflateRect(rect.Width() / 4, rect.Height() / 4);
    if (rect.Width() == 0 && rect.Height() == 0) {
        SetMas(500. / geom.dmas, cx / geom.masx / geom.dmas, cy / geom.masx / geom.dmas);
        moveXY(CFPoint(cx, cy));
    }

    ChMas(rect);

    if (geom.masx * geom.dmas < 500.) {
        SetMas(500. / geom.dmas, cx / geom.masx / geom.dmas, cy / geom.masx / geom.dmas);
        moveXY(CFPoint(cx, cy));
    }

    CPoint p = CoordToScreen(CFPoint(cx, cy));
    Mxy(p.x, p.y);


    Invalidate();
}



void CScroll::ChMas(CFRect rect) {
    double m;

    if (rect.Width() == 0 && rect.Height() == 0) return;

    CRect r;
    GetClientRect(&r);

    rect.NormalizeRect();

    m = max((double)rect.Width() / r.Width(), (double)rect.Height() / r.Height());

    SetMas(m, rect.left / geom.masx, rect.top / geom.masy);
}


/*
void CScroll::ChMas2(CFRect rect) {
  double m;

  if (rect.Width() == 0 && rect.Height() == 0) return;

  CRect r;
  GetClientRect(&r);

  rect.NormalizeRect();

  m = max((double)rect.Width()/r.Width(), (double)rect.Height()/r.Height());

  double mm = m*dmas;

  if (mm > 0) {
    m_bx = rect.left/m;
    m_by = rect.top/m;
    masx = masy = m;
  }

  CString str;
  str.Format(_T("1:%g"), m*dmas);
  Status(2, str);
  SetBar();
}

void CScroll::MasPlus() {
  SetMas(masx/1.5, m_bx, m_by);
}

void CScroll::MasMinus() {
  SetMas(masx*1.5, m_bx, m_by);
}

*/

void CScroll::moveXY2(double bx0, double by0) {
    geom.m_bx = bx0 / geom.masx;
    geom.m_by = by0 / geom.masy;
    SetBar();
}


void CScroll::MasAll() {
    if (m_clExpl.size() > 0) {
        ChMas(m_clExpl.GetFRect());
    }
    else {
        ChMas(m_rectBorder);
    }
}



void CScroll::MasPlus() {
    CRect rect;
    GetClientRect(&rect);


    CPoint pt;

    GetCursorPos(&pt);
    ScreenToClient(&pt);

    geom.m_bx += pt.x;
    geom.m_by += pt.y;


    SetMas(geom.masx / 1.5, geom.m_bx, geom.m_by);
    geom.m_bx -= pt.x;
    geom.m_by -= pt.y;
}

void CScroll::MasMinus() {
    CRect rect;
    GetClientRect(&rect);

    CPoint pt;

    GetCursorPos(&pt);
    ScreenToClient(&pt);


    geom.m_bx += pt.x;
    geom.m_by += pt.y;
    SetMas(geom.masx * 1.5, geom.m_bx, geom.m_by);
    geom.m_bx -= pt.x;
    geom.m_by -= pt.y;
}


void CScroll::SetBorder(const CFRect& rect)
{
    m_rectBorder = rect;
    m_rectBorder.NormalizeRect();
    if (!m_isPrint) SetBar();
}


void CScroll::OnEsc()
{
    CFPoint p0;
    CDC* m_dc;

    if ((m_dc = BeginPaint()) != NULL) {
        XDraw(m_dc, false);

        m_bIsFragment = FALSE;

        switch (regim) {
        case R_BEGIN_LEN2:
        case R_BEGIN_LEN_OKR2:
        case R_BEGIN_LEN_OKR_VYD_2:
        case R_BEGIN_LEN_OKR_VYD_GEO_2:
        case R_BEGIN_DRAW2:
        case R_BEGIN_DRAW_LINE:

            if (m_nRegim == ID_F4) {
                m_NP.clear();
            }
            else {
                p0 = m_NP.back();
                DrawRect(m_dc, p0);
                m_NP.pop_back();
            }


            if (m_NP.empty()) {
                regim = R_INIT;
            }
            else {
                m_pt1 = m_NP.back();
                setcolor(m_dc, DARKGRAY);
                CPoint p1 = CoordToScreen(m_pt1);
                CPoint p2 = CoordToScreen(p0);
                xline(m_dc, p1.x, p1.y, p2.x, p2.y);
            }
            break;

        case R_MOVE_LINE:
            //    xt = fNew[m_tecF].x;
            //    yt = fNew[m_tecF].y;

            //// 2010.01.20
            m_NP.clear();
            /////

            regim = R_BEGIN_MOVE_LINE;
            break;

        case R_MOVE_PODP:
            m_NP.clear();

            regim = R_BEGIN_MOVE_PODP;
            break;


        case R_BEGIN_MOVE_LINE:
            DrawLine(m_dc);
            m_NP.clear();
            regim = R_INIT;
            SetCur(m_nRegim);
            break;

        case R_BEGIN_MOVE_PODP:
            DrawPodp(m_dc);
            m_NP.clear();
            regim = R_INIT;
            SetCur(m_nRegim);
            break;

        default:
            //// 2010.01.20
            m_NP.clear();
            /////

            regim = R_INIT;
            break;
        }

        XDraw(m_dc, true);

        EndPaint(m_dc);
    }

    m_is_print_fr = FALSE;
}

void CScroll::OnDelete()
{
    if (regim == R_MOVE_LINE && m_NP.size() > 2) {
        CDC* m_dc;
        if ((m_dc = BeginPaint()) != NULL) {
            XDraw(m_dc, false);

            m_NP.Del(m_tecF);
            regim = R_BEGIN_MOVE_LINE;
            XDraw(m_dc, true);

            EndPaint(m_dc);
            m_tecF = -1;
        }
    }
}

void CScroll::OnEditCopy() {
    if (m_bIsFragment) {
        CopyBlock(CFRect(m_pt1, m_pt1_2));
        m_bIsFragment = FALSE;
        InvalidateG();
    }
}

void CScroll::OnEditCopyPng() {
    if (m_bIsFragment) {
        CopyBlockPng(CFRect(m_pt1, m_pt1_2));
        m_bIsFragment = FALSE;
        InvalidateG();
    }
}


void CScroll::OnUpdateEditCopy(CCmdUI* pCmdUI) {
    pCmdUI->Enable(m_bIsFragment);
}

void CScroll::OnFragment()
{
    regim = R_BEGIN_DRAW_COPY1;
}


void CScroll::OnEditCopybmp()
{
    regim = R_BEGIN_DRAW_COPY_BMP1;
}

int chline(double x1, double y1, double x2, double y2, double xx, double yy, double delta) {
    double xmin, ymin, xmax, ymax, xxx, yyy;

    if (x1 == x2 && y1 == y2) return 0;

    if (x2 > x1) { xmin = x1; xmax = x2; }
    else { xmin = x2; xmax = x1; }
    if (y2 > y1) { ymin = y1; ymax = y2; }
    else { ymin = y2; ymax = y1; }

    if (xmax - xmin >= ymax - ymin) {
        if (xmin <= xx && xx <= xmax) {
            yyy = y1 + (xx - x1) * (y2 - y1) / (x2 - x1);
            if (yyy - delta < yy && yy < yyy + delta) return TRUE;
        }
    }
    else {
        if (ymin <= yy && yy <= ymax) {
            xxx = x1 + (yy - y1) * (x2 - x1) / (y2 - y1);
            if (xxx - delta < xx && xx < xxx + delta) return TRUE;
        }
    }
    return 0;
}


int chline2(long x1, long y1, long x2, long y2, int xx, int yy) {
    long xmin, ymin, xmax, ymax;

    if (x1 == x2 && y1 == y2) return 0;

    if (x2 > x1) { xmin = x1; xmax = x2; }
    else { xmin = x2; xmax = x1; }
    if (y2 > y1) { ymin = y1; ymax = y2; }
    else { ymin = y2; ymax = y1; }

    if (xmin == xmax && x1 - 7 < xx && xx < x1 + 7 && ymin < yy && yy < ymax) return TRUE;
    if (ymin == ymax && y1 - 7 < yy && yy < y1 + 7 && xmin < xx && xx < xmax) return TRUE;
    return chline(x1, y1, x2, y2, xx, yy, 9);
}


void CScroll::find_point2(CFPoint pt) {
    int i, n;
    unsigned nom;

    if (m_NP.size() == 0) return;

    double d, mind = LONG_MAX;

    m_tecF = -1;
    nom = 0;


    CCoordList::iterator it, it0;

    it = m_NP.begin();

    for (i = 0; it != m_NP.end(); ++it, i++) {
        d = Length(pt, *it);
        if (d < mind) {
            mind = d;
            if (d < geom.masx * 8 / geom.dmas) {

                if (m_noend && (i == 0 || i == m_NP.size() - 1)) {
                    return;
                }

                m_pt2 = *it;
                m_tecF = i;

                nom = 1;
            }
        }
    }

    if (nom) {
        regim = R_MOVE_LINE;
        XDraw0(false);
        m_pt2 = pt;
        XDraw0(true);
    }

    if (m_locNP != 6) {
        if (nom == 0) {
            n = m_NP.size();
            if (m_locNP == 3) n++;

            it = m_NP.begin();

            CFPoint p0, p1, p2;

            p0 = p1 = *it;

            ++it;

            for (i = 0; i < n - 1; it++, i++) {
                p2 = (it == m_NP.end()) ? p0 : *it;

                if (chline(p1.x, p1.y, p2.x, p2.y, pt.x, pt.y, 9 * geom.masx)) {

                    m_tecF = i + 1;

                    XDraw0(false);

                    if (it != m_NP.end()) {
                        m_NP.insert(it, pt);
                    }
                    else {
                        m_NP.push_back(pt);
                    }

                    regim = R_MOVE_LINE;

                    XDraw0(true);

                    break;
                }
                p1 = p2;
            }
        }
    }
}

void CScroll::find_podp2(CFPoint pt)
{
    unsigned nom = 0;

    if (m_NP.size() != 2) return;

    CFPoint p1 = m_NP.GetV(0);
    CFPoint p2 = m_NP.GetV(1);

    if (Length(pt, p1) < geom.masx * 8 / geom.dmas) {
        m_pt2 = p1;
        m_tecF = 0;
        nom = 1;
    }
    else if (Length(pt, p2) < geom.masx * 8 / geom.dmas) {
        m_pt2 = p2;
        m_tecF = 1;
        nom = 1;
    }

    if (nom) {
        regim = R_MOVE_PODP;
        CDC* m_dc;
        if ((m_dc = BeginPaint()) != NULL) {
            XDraw(m_dc, false);
            m_pt2 = pt;

            XDraw(m_dc, true);
            EndPaint(m_dc);
        }
    }
}



// Начало длины

void CScroll::OnF4()
{
    m_nRegim = ID_F4;
    regim = R_BEGIN_LEN1;
}

void CScroll::OnReturn()
{
    CPoint pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);
    SendMessage(WM_LBUTTONDOWN, 0, MAKELONG(pt.x, pt.y));
}

void CScroll::OnCtrlReturn()
{
    CPoint pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);
    SendMessage(WM_LBUTTONDOWN, 0, MAKELONG(pt.x, pt.y));
}


void CScroll::OnF2()
{
    CPoint pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);
    SendMessage(WM_RBUTTONDOWN, 0, MAKELONG(pt.x, pt.y));
}

/*
void CScroll::OnAstr()
{
  m_dX = m_dX == 0 ? 10*20 : 0;
  InvalidateG();
}

void CScroll::OnUpdateAstr(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_dX != 0);
}
*/


void CScroll::OnRamka()
{
    m_isRamka = !m_isRamka;

    Invalidate();
}

void CScroll::OnUpdateRamka(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_isRamka);
}



void CScroll::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default

    if (nIDEvent == m_nTimerDbl)
    {
        KillTimer(m_nTimerDbl);
        m_nTimerDbl = NULL;
        OnLButtonDown2(m_nFlagsDbl, m_pointDbl);
        m_dbl = 0;
    }

    CBgi::OnTimer(nIDEvent);
}

static long vn(long n, long round = 10000)
{
    if (n > round) return round;
    if (n < -round) return -round;
    return n;
}


CFRect CScroll::ScreenToCoord(const CRect& rect)
{
    return CFRect(
        ScreenToCoord(rect.TopLeft()),
        ScreenToCoord(rect.BottomRight()));
}

CRect CScroll::CoordToScreen(const CFRect& rect)
{
    return CRect(
        CoordToScreen(rect.TopLeft()),
        CoordToScreen(rect.BottomRight()));
}

BOOL CScroll::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    ScreenToClient(&pt);

    pt.x %= 0x10000;
    pt.y %= 0x10000;

    if (regim != R_MOVE_VIEW && regim != R_MOVE_VIEW_MIDDLE && regim != R_MOVE_VIEW_VYD) {
        if (zDelta != 0) {
            double mas = zDelta < 0 ? -geom.masx * (1.5 * zDelta / 120.) : geom.masx / (1.5 * zDelta / 120.);

            CString str;

            double m1 = mas1(), m2 = mas2();

            if (m1 / geom.dmas <= mas && mas <= m2 / geom.dmas) {
                geom.m_bx += pt.x;
                geom.m_by += pt.y;

                SetMas(mas, geom.m_bx, geom.m_by);
                geom.m_bx -= pt.x;
                geom.m_by -= pt.y;

                InvalidateG();

                wheel = true;
            }
        }
    }
    return CBgi::OnMouseWheel(nFlags, zDelta, pt);
}

static CDC* pDC;
static CDC* pDibDCAlpha = NULL;
static CBitmap* pOldBitmap;
static CBitmap* pBitmap;

CDC* CScroll::BeginAlpha(CDC* m_dc)
{
    /////////////////////////////////

    int m_nTransp = 255;
    m_nTransp2 = m_nTransp;

    CDC* pDibDCAlpha = m_dc;

    if (0 <= m_nTransp && m_nTransp <= 255 && !m_dc->IsPrinting()) {
        pDibDCAlpha = new CDC();
        pBitmap = new CBitmap();

        pDibDCAlpha->CreateCompatibleDC(m_dc);

        pBitmap->CreateCompatibleBitmap(m_dc, m_rect.right, m_rect.bottom);
        pOldBitmap = (CBitmap*)pDibDCAlpha->SelectObject(*pBitmap);
        pDibDCAlpha->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_dc, m_rect.left, m_rect.top, SRCCOPY);
    }
    return pDibDCAlpha;
    /////////////////////////////////
}

void CScroll::EndAlpha(CDC* m_dc, CDC* pDibDCAlpha)
{
    //////////////////////////////////

    int m_nTransp = 255;
    m_nTransp = m_nTransp2;

    if (m_dc == pDibDCAlpha) return;

    if (0 <= m_nTransp && m_nTransp <= 255 && pDibDCAlpha) {
        BLENDFUNCTION blendFunction = { 0, 0, m_nTransp, 0 };

        BOOL ret = lpfnAlphaBlend(
            m_dc->m_hDC,     // handle to destination DC
            m_rect.left,    // x-coord of upper-left corner
            m_rect.top,     // y-coord of upper-left corner
            m_rect.Width(), // destination width
            m_rect.Height(),// destination height
            pDibDCAlpha->m_hDC,    // handle to source DC
            m_rect.left,    // x-coord of upper-left corner
            m_rect.top,     // y-coord of upper-left corner
            m_rect.Width(), // source width
            m_rect.Height(),// source height
            blendFunction   // alpha-blending function
        );
        delete pBitmap;
        delete pDibDCAlpha;
    }
    ////////////////////////////////////
}


CDC* CScroll::BeginAlpha2(CDC* m_dc)
{
    /////////////////////////////////
    CDC* pDibDCAlpha = m_dc;

    pDibDCAlpha = new CDC();
    pBitmap = new CBitmap();

    pDibDCAlpha->CreateCompatibleDC(m_dc);

    pBitmap->CreateCompatibleBitmap(m_dc, m_rect.right, m_rect.bottom);
    pOldBitmap = (CBitmap*)pDibDCAlpha->SelectObject(*pBitmap);
    pDibDCAlpha->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_dc, m_rect.left, m_rect.top, SRCCOPY);

    return pDibDCAlpha;
    /////////////////////////////////
}



void CScroll::EndAlpha2(CDC* m_dc, CDC* pDibDCAlpha)
{
    delete pBitmap;
    delete pDibDCAlpha;
}


void CScroll::DrawAlpha2(CDC* m_dc, CDC* pDibDCAlpha, int nTransp)
{
    if (m_dc == pDibDCAlpha) return;

    //  if (nTransp > 0) {
    //    m_nTransp = nTransp;
    //  }


    if (0 <= m_nTransp && nTransp <= 255 && pDibDCAlpha) {
        BLENDFUNCTION bf = { 0, 0, nTransp, 0 };

        BOOL ret = lpfnAlphaBlend(
            m_dc->m_hDC,     // handle to destination DC
            m_rect.left,    // x-coord of upper-left corner
            m_rect.top,     // y-coord of upper-left corner
            m_rect.Width(), // destination width
            m_rect.Height(),// destination height
            pDibDCAlpha->m_hDC,    // handle to source DC
            m_rect.left,    // x-coord of upper-left corner
            m_rect.top,     // y-coord of upper-left corner
            m_rect.Width(), // source width
            m_rect.Height(),// source height
            bf   // alpha-blending function
        );
    }
}

#include "TranspDialog.h"

void CScroll::OnTransp()
{
    CTranspDialog td(this);

    td.m_slider = m_nTransp * 100 / 255;

    if (td.DoModal() == IDOK) {
        m_nTransp = td.m_slider * 255 / 100;
        InvalidateG();
    }
}

void CScroll::set_coord_gid(double x0, double y0, double alpha, bool convert)
{
    set_coord(x0, y0, alpha);
}


void CScroll::InvalidateG(BOOL bErase)
{
    if (m_dcSave) {
        CloseImage(m_dcSave);
        m_dcSave = NULL;
    }
    Invalidate(bErase);
}


#include "PrintFrDial.h"


void CScroll::OnPrintFr()
{
    CPrintFrDial dial(this);

    if (dial.DoModal() == IDOK) {
        CSize l_mas[] = {
          CSize(210, 297),  // A4
          CSize(297, 420),  // A3
          CSize(420, 594),  // A2
          CSize(594, 840),  // A1
          CSize(840, 1188), // A0
        };

        if (dial.m_paper >= 0 && dial.m_paper < sizeof(l_mas) / sizeof(CSize)) {

            m_print_mas = atof(dial.m_mas);
            m_print_size = l_mas[dial.m_paper];

            if (dial.m_orient == 1) {
                int x = m_print_size.cx;
                m_print_size.cx = m_print_size.cy;
                m_print_size.cy = x;
            }

            regim = R_BEGIN_RECT_1;

            CPoint point;

            GetCursorPos(&point);
            ScreenToClient(&point);

            CFPoint pt = ScreenToCoord(point);

            m_pt1 = m_pt2 = pt;

            CDC* m_dc;

            if ((m_dc = BeginPaint()) != NULL) {
                XDraw(m_dc, false);
                XDraw(m_dc, true);
                EndPaint(m_dc);
            }
        }
    }
}

void ug90(double x1, double y1, double x2, double y2, double& xt, double& yt);


CFPoint CScroll::to90(CFPoint pt)
{
    double xt, yt, x1, y1, x2, y2;
    CFPoint p1, p2;

    if (m_NP.size() > 1) {
        xt = pt.x;
        yt = pt.y;

        p2 = m_NP.back();    m_NP.pop_back();
        p1 = m_NP.back();    m_NP.pop_back();

        x1 = p1.x;    y1 = p1.y;
        x2 = p2.x;    y2 = p2.y;

        m_NP.push_back(p1);
        m_NP.push_back(p2);

        ug90(x1, y1, x2, y2, xt, yt);
        pt = CFPoint(xt, yt);
    }
    return pt;
}

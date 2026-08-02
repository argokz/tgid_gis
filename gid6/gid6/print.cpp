// Scroll.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "gid6.h"
#include "Scroll.h"
#include "InputDia.h"
#include "PrintIni.h"
#include "win.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szMargin[] = _T("Margin");
static TCHAR BASED_CODE szScrollBar[] = _T("ScrollBar");

bool GetPaperSize(DEVMODE *devmode, int &lx, int &ly);

void pApp_SetOrientation(int Orientation) 
{
    Cgid6App* pApp = (Cgid6App*) AfxGetApp();
    pApp->SetPrintOrientation(Orientation);
}


void CScroll::hideU(CDC *m_dc, const CCoordList &cl) {
  int i, n, imin;
  int xmin = INT_MAX, ymin, xmax, ymax;
  int *fil;

  n = cl.size();

  if (n == 0) return;

  fil = new int[n*2+14];

  CCoordList::const_iterator it;
  CPoint pt;

  for (it = cl.begin(), i = 0; it != cl.end(); ++it ) {
    pt = CoordToScreen(*it);
    fil[i++] = pt.x;
    fil[i++] = pt.y;
  }

  CRect r;
  GetClientRect(&r);

  r = m_rect;
  r.InflateRect(geom.masx, geom.masx);

  for ( i = 0; i < n; i++ ) if (fil[i*2] < xmin) {
    imin = i;
    xmin = fil[i*2];
  }

  xmin = r.left-1;
  xmax = r.right+1;
  ymin = r.top-1;
  ymax = r.bottom+1;


  for ( i = n-1; i >= imin; i-- ) {
    fil[(i+7)*2] = fil[i*2];
    fil[(i+7)*2+1] = fil[i*2+1];
  }
  fil[(imin+1)*2]   = xmin;
  fil[(imin+1)*2+1] = fil[imin*2+1];
  fil[(imin+2)*2]   = xmin;
  fil[(imin+2)*2+1] = ymax;
  fil[(imin+3)*2]   = xmax;
  fil[(imin+3)*2+1] = ymax;
  fil[(imin+4)*2]   = xmax;
  fil[(imin+4)*2+1] = ymin;
  fil[(imin+5)*2]   = xmin;
  fil[(imin+5)*2+1] = ymin;
  fil[(imin+6)*2]   = xmin;
  fil[(imin+6)*2+1] = fil[imin*2+1];

  n += 7;

  setfillstyle(m_dc, 0, 0xFFFFFF);
  setcolor(m_dc, 0xFFFFFF);
  

  fillpoly(m_dc, n, fil);
  setcolor(m_dc, 0);
//  drawpoly(n-7, fil);

  delete [] fil;

}

void CScroll::hideU(CDC *m_dc, CRect rect) 
{
  int xmin = INT_MAX, ymin, xmax, ymax;
  int *fil;

 
  fil = new int[30];

  CCoordList::const_iterator it;
  CPoint pt;

  CRect r;

  r = m_rect;

  xmin = r.left;
  xmax = r.right;
  ymin = r.top;
  ymax = r.bottom;

  int n = 0;

  fil[n++] = xmin;
  fil[n++] = ymin;
  fil[n++] = xmin;
  fil[n++] = ymax;
  fil[n++] = xmax;
  fil[n++] = ymax;
  fil[n++] = xmax;
  fil[n++] = ymin;
  fil[n++] = xmin;
  fil[n++] = ymin;

//  GetClientRect(&r);
//  r.InflateRect(geom.masx, geom.masx);

  xmin = -1;
  ymin = -1;
  xmax = rect.right+1000;
  ymax = rect.bottom+1000;


  fil[n++] = xmin;
  fil[n++] = ymin;
  fil[n++] = xmax;
  fil[n++] = ymin;
  fil[n++] = xmax;
  fil[n++] = ymax;
  fil[n++] = xmin;
  fil[n++] = ymax;
  fil[n++] = xmin;
  fil[n++] = ymin;


  setfillstyle(m_dc, 0, 0xFFFFFF);
  setcolor(m_dc, 0xFFFFFF);
//  setcolor(m_dc, 0x00FFFF);
//  setfillstyle(m_dc, 0, 0x00FFFF);

  fillpoly(m_dc, n/2, fil);
//  setcolor(m_dc, RED);
//  drawpoly(m_dc, n/2, fil);

  delete [] fil;

}

/*
void CScroll::BorderRect(CDC *m_dc, CRect rect, CRect rect1) {
  int i, n, imin;
  int fil[256], xmin = INT_MAX, ymin, xmax, ymax;
  double x1, y1, x2, y2;

  x1 = rect.left;
  y1 = rect.top;
  x2 = rect.right;
  y2 = rect.bottom;

  n = 4;
  fil[0] = x1;  fil[1] = y1;
  fil[2] = x1;  fil[3] = y2;
  fil[4] = x2;  fil[5] = y2;
  fil[6] = x2;  fil[7] = y1;

  for ( i = 0; i < n; i++ ) if (fil[i*2] < xmin) {
    imin = i;
    xmin = fil[i*2];
  }

  xmin = rect1.left-10;
  xmax = rect1.right+10;
  ymin = rect1.top-10;
  ymax = rect1.bottom+10;

  for ( i = n-1; i >= imin; i-- ) {
    fil[(i+7)*2] = fil[i*2];
    fil[(i+7)*2+1] = fil[i*2+1];
  }
  fil[(imin+1)*2]   = xmin;  fil[(imin+1)*2+1] = fil[imin*2+1];
  fil[(imin+2)*2]   = xmin;  fil[(imin+2)*2+1] = ymax;
  fil[(imin+3)*2]   = xmax;  fil[(imin+3)*2+1] = ymax;
  fil[(imin+4)*2]   = xmax;  fil[(imin+4)*2+1] = ymin;
  fil[(imin+5)*2]   = xmin;  fil[(imin+5)*2+1] = ymin;
  fil[(imin+6)*2]   = xmin;  fil[(imin+6)*2+1] = fil[imin*2+1];

  n += 7;

  setfillstyle(m_dc, 0, 0xFFFFFF);
  setcolor(m_dc, 0xFFFFFF);

  fillpoly(m_dc, n, fil);
}

*/
bool CScroll::GetPrintSize(int &m_npx, int &m_npy, const char *fN) 
{
  int lx, ly;

  m_rectPrint = ScreenToCoord(m_rect);

  if (m_bIsFragment) {
    m_rectPrint = CFRect(m_pt1, m_pt1_2);
  }
  m_rectPrint.NormalizeRect();

  int Orientation = m_rectPrint.Width() > m_rectPrint.Height() ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;

  Cgid6App* pApp = (Cgid6App*) AfxGetApp();

  if (!m_devmode) {
    CPrintDialog dlg(TRUE);
    dlg.GetDefaults();

    m_devmode = dlg.GetDevMode();
  }

  if (m_devmode) {
    m_devmode->dmOrientation = Orientation;
  }
  pApp_SetOrientation(Orientation);

  GetPaperSize(m_devmode, lx, ly);
  m_rectAllMargin = CRect(0, 0, 0, 0);

  int p_x = lx*10-(m_rectMargin.left+m_rectMargin.right);
  int p_y = ly*10-(m_rectMargin.top+m_rectMargin.bottom);

  int wmx = m_rectAllMargin.left+m_rectAllMargin.right;
  int wmy = m_rectAllMargin.top+m_rectAllMargin.bottom;

  m_dPrintMas = (long) (geom.masx*geom.dmas+0.9);

  m_dPrintMas = max(m_rectPrint.Width()*1000/p_x, m_rectPrint.Height()*1000/p_y);

  if (m_is_print_fr) {
//    m_is_print_fr = FALSE;
    m_dPrintMas = m_print_mas;
  }


  CPrintIni pi(this, &m_rectPrint, &m_rectBorder, &m_dPrintMas);

  if (pi.DoModal() != IDOK) return false;

  double d105 = 1.05;

  m_dPrintMas *= d105;

  m_rectPrint.NormalizeRect();

  wmx = m_rectAllMargin.left+m_rectAllMargin.right;
  wmy = m_rectAllMargin.top+m_rectAllMargin.bottom;

  int lenx = (m_rectPrint.Width())*1000.*d105/m_dPrintMas+wmx;
  int leny = (m_rectPrint.Height())*1000.*d105/m_dPrintMas+wmy;

//  int lenx = (m_rectBorder.Width())*1000./m_dPrintMas+wmx;
//  int leny = (m_rectBorder.Height())*1000./m_dPrintMas+wmy;


  m_npx = (lenx-5)/p_x+1;
  m_npy = (leny-5)/p_y+1;

  return true;
}

void CScroll::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
  double masx_old = geom.masx, masy_old = geom.masy, bx_old = geom.m_bx, by_old = geom.m_by;
  double dmas_old = geom.dmas;
  int lx, ly;

  CRect rect;

  int dpix = pDC->GetDeviceCaps(LOGPIXELSX);
  int dpiy = pDC->GetDeviceCaps(LOGPIXELSY);

  int ddx = pDC->GetDeviceCaps(PHYSICALOFFSETX);
  int ddy = pDC->GetDeviceCaps(PHYSICALOFFSETY);

  int dpi = min(dpix, dpiy);
  int PrevMode;


  CSize OldVExt, OldWExt;

  rect = pInfo->m_rectDraw;

  int CheckForColorPrinter(CString printer);
  m_isColor = CheckForColorPrinter(CString(m_devmode->dmDeviceName));


int n_DRIVERVERSION = pDC->GetDeviceCaps(DRIVERVERSION);    /* Device driver version                    */
int n_TECHNOLOGY = pDC->GetDeviceCaps(TECHNOLOGY);    /* Device classification                    */
int n_HORZSIZE = pDC->GetDeviceCaps(HORZSIZE);    /* Horizontal size in millimeters           */
int n_VERTSIZE = pDC->GetDeviceCaps(VERTSIZE);    /* Vertical size in millimeters             */
int n_HORZRES = pDC->GetDeviceCaps(HORZRES);    /* Horizontal width in pixels               */
int n_VERTRES = pDC->GetDeviceCaps(VERTRES);   /* Vertical height in pixels                */
int n_BITSPIXEL = pDC->GetDeviceCaps(BITSPIXEL);   /* Number of bits per pixel                 */
int n_PLANES = pDC->GetDeviceCaps(PLANES);   /* Number of planes                         */
int n_NUMBRUSHES = pDC->GetDeviceCaps(NUMBRUSHES);   /* Number of brushes the device has         */
int n_NUMPENS = pDC->GetDeviceCaps(NUMPENS);   /* Number of pens the device has            */
int n_NUMMARKERS = pDC->GetDeviceCaps(NUMMARKERS);   /* Number of markers the device has         */
int n_NUMFONTS = pDC->GetDeviceCaps(NUMFONTS);   /* Number of fonts the device has           */
int n_NUMCOLORS = pDC->GetDeviceCaps(NUMCOLORS);   /* Number of colors the device supports     */
int n_PDEVICESIZE = pDC->GetDeviceCaps(PDEVICESIZE);   /* Size required for device descriptor      */
int n_CURVECAPS = pDC->GetDeviceCaps(CURVECAPS);   /* Curve capabilities                       */
int n_LINECAPS = pDC->GetDeviceCaps(LINECAPS);   /* Line capabilities                        */
int n_POLYGONALCAPS = pDC->GetDeviceCaps(POLYGONALCAPS);   /* Polygonal capabilities                   */
int n_TEXTCAPS = pDC->GetDeviceCaps(TEXTCAPS);   /* Text capabilities                        */
int n_CLIPCAPS = pDC->GetDeviceCaps(CLIPCAPS);   /* Clipping capabilities                    */
int n_RASTERCAPS = pDC->GetDeviceCaps(RASTERCAPS);   /* Bitblt capabilities                      */
int n_ASPECTX = pDC->GetDeviceCaps(ASPECTX);   /* Length of the X leg                      */
int n_ASPECTY = pDC->GetDeviceCaps(ASPECTY);   /* Length of the Y leg                      */
int n_ASPECTXY = pDC->GetDeviceCaps(ASPECTXY);   /* Length of the hypotenuse                 */



  if (dpix != dpiy) {
    PrevMode = pDC->SetMapMode(MM_ANISOTROPIC);

    OldVExt = pDC->SetViewportExt(
          pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES)
        );
    OldWExt = pDC->SetWindowExt(
          MulDiv(pDC->GetDeviceCaps(HORZRES), dpi, dpix),
          MulDiv(pDC->GetDeviceCaps(VERTRES), dpi, dpiy)
          );
    pDC->SetWindowOrg(0, 0);

  }

  int dlx, dly, dlx1, dly1;
  CRect rectMarginPoint;

  GetPaperSize(m_devmode, lx, ly);


  double lx1 = rect.Width() * 254 / dpi;
  double ly1 = rect.Height() * 254 / dpi;

  lx = rect.Width() * 254 / dpi;
  ly = rect.Height() * 254 / dpi;

  dlx = (lx*dpi/254-rect.Width()*dpi/dpix)/2;
  dly = (ly*dpi/254-rect.Height()*dpi/dpiy)/2;

  double dlx2 = ((double)lx*dpi/254-rect.Width()*dpi/dpix)/2;
  double dly2 = ((double)ly*dpi/254-rect.Height()*dpi/dpiy)/2;


  dlx1 = min(m_rectMargin.left*dpi/2540, dlx);
  dly1 = min(m_rectMargin.top*dpi/2540, dly);

  rectMarginPoint = CRect(
    m_rectMargin.left*dpi/2540-dlx1,
    m_rectMargin.top*dpi/2540-dly1,
    m_rectMargin.right*dpi/2540-dlx1,
    m_rectMargin.bottom*dpi/2540-dly1
  );


  m_rect = CRect(
    MulDiv(rect.left, dpi, dpix)+rectMarginPoint.left,
    MulDiv(rect.top, dpi, dpiy)+rectMarginPoint.top,
    MulDiv(rect.right, dpi, dpix)-rectMarginPoint.right,
    MulDiv(rect.bottom, dpi, dpiy)-rectMarginPoint.bottom
  );

  int ix, iy;
  ix = (pInfo->m_nCurPage-1)%m_npx;
  iy = (pInfo->m_nCurPage-1)/m_npx;

  CClientDC screenDC(NULL);
//  geom.masx = geom.masy = m_dPrintMas*screenDC.GetDeviceCaps(LOGPIXELSX)/pDC->GetDeviceCaps(LOGPIXELSX);
  geom.masx = geom.masy = m_dPrintMas*2.54/dpi;
  geom.dmas = m_dPrintMas/geom.masx;

//  int mx = 3000*dpi/2540, my = 3000*dpi/2540;
//  int mx = 0, my = 0;

  geom.m_bx = -m_rectAllMargin.left*dpi/2540 + (double)m_rectPrint.left/geom.masx-rectMarginPoint.left+ix*m_rect.Width();
  geom.m_by = -m_rectAllMargin.top*dpi/2540 + (double)m_rectPrint.top/geom.masy-rectMarginPoint.top+iy*m_rect.Height();

  SetDC(pDC);

  m_isPrint = 1;
  redrawPaint(pDC);
  m_isPrint = 0;

//  rectangle(m_rect.left+1, m_rect.top+1, m_rect.right-1, m_rect.bottom-1);

/*
  CRect rect1 = m_rect;

  rect = CRect(
    m_rectPrint.left/geom.masx-bx,
    m_rectPrint.top/geom.masy-geom.m_by,
    m_rectPrint.right/geom.masx-bx,
    m_rectPrint.bottom/geom.masy-geom.m_by
  );

  setline(0, 1);
  rectangle(rect.left, rect.top, rect.right, rect.bottom);

  rect.InflateRect(1, 1);
//  BorderRect(rect, rect1);

  geom.masx = geom.masy = 2.54/dpi;
  bx = -rectMarginPoint.left+ix*m_rect.Width();
  geom.m_by = -rectMarginPoint.top+iy*m_rect.Height();

//  PrintFrame(pDC);

  setline(0, 1);
  rectangle(rect1.left, rect1.top, rect1.right, rect1.bottom);

  rect1.InflateRect(1, 1);
//  BorderRect(rect1, pInfo->m_rectDraw);
*/

  m_isColor = TRUE;
  geom.masx = masx_old; geom.masy = masy_old; geom.m_bx = bx_old; geom.m_by = by_old;
  geom.dmas = dmas_old;

  if (dpix != dpiy) {
    pDC->SetViewportExt(OldVExt);
    pDC->SetWindowExt(OldWExt);

    pDC->SetMapMode(PrevMode);
  }

  CBgi::OnPrint(pDC, pInfo);
}

BOOL CScroll::OnPreparePrinting(CPrintInfo* pInfo)
{
  if (!GetPrintSize(m_npx, m_npy)) return FALSE;

  Cgid6App* pApp = (Cgid6App*) AfxGetApp();

//  pApp->SetOrientation(m_npx > m_npy ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT);

  pInfo->SetMaxPage(m_npx*m_npy);

  // TODO: call DoPreparePrinting to invoke the Print dialog box
  return DoPreparePrinting(pInfo);
}

void CScroll::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  // TODO: Add your specialized code here and/or call the base class

  CBgi::OnBeginPrinting(pDC, pInfo);
}

void CScroll::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  // TODO: Add your specialized code here and/or call the base class

  m_rectPrint = CFRect(0, 0, 0, 0);

  CBgi::OnEndPrinting(pDC, pInfo);
}


bool save_DEVMODE(DEVMODE *m_devmode)
{
    AfxGetApp()->WriteProfileInt(_T("PageSetup"), _T("dmPaperSize"), m_devmode->dmPaperSize);
    AfxGetApp()->WriteProfileInt(_T("PageSetup"), _T("dmPaperLength"), m_devmode->dmPaperLength);
    AfxGetApp()->WriteProfileInt(_T("PageSetup"), _T("dmPaperWidth"), m_devmode->dmPaperWidth);
    AfxGetApp()->WriteProfileInt(_T("PageSetup"), _T("dmDisplayOrientation"), m_devmode->dmDisplayOrientation);


/*

//BYTE  dmDeviceName[CCHDEVICENAME];
  WORD  dmSpecVersion;
  WORD  dmDriverVersion;
  WORD  dmSize;
  WORD  dmDriverExtra;
  DWORD dmFields;
  union {
    struct {
      short dmOrientation;
      short dmPaperSize;
      short dmPaperLength;
      short dmPaperWidth;
      short dmScale;
      short dmCopies;
      short dmDefaultSource;
      short dmPrintQuality;
    } DUMMYSTRUCTNAME;
    POINTL dmPosition;
    struct {
      POINTL dmPosition;
      DWORD  dmDisplayOrientation;
      DWORD  dmDisplayFixedOutput;
    } DUMMYSTRUCTNAME2;
  } DUMMYUNIONNAME;
  short dmColor;
  short dmDuplex;
  short dmYResolution;
  short dmTTOption;
  short dmCollate;
  BYTE  dmFormName[CCHFORMNAME];
  WORD  dmLogPixels;
  DWORD dmBitsPerPel;
  DWORD dmPelsWidth;
  DWORD dmPelsHeight;
  union {
    DWORD dmDisplayFlags;
    DWORD dmNup;
  } DUMMYUNIONNAME2;
  DWORD dmDisplayFrequency;
  DWORD dmICMMethod;
  DWORD dmICMIntent;
  DWORD dmMediaType;
  DWORD dmDitherType;
  DWORD dmReserved1;
  DWORD dmReserved2;
  DWORD dmPanningWidth;
  DWORD dmPanningHeight;
*/

    return true;
}

int GetProfileIntIfExists(CString s1, CString s2, int val)
{
    int v = AfxGetApp()->WriteProfileInt(s1, s2, 0);

    if (v) val = v;

    return v;
}



bool load_DEVMODE(DEVMODE *m_devmode)
{
    m_devmode->dmPaperSize = GetProfileIntIfExists(_T("PageSetup"), _T("dmPaperSize"), m_devmode->dmPaperSize);
    m_devmode->dmPaperLength = GetProfileIntIfExists(_T("PageSetup"), _T("dmPaperLength"), m_devmode->dmPaperLength);
    m_devmode->dmPaperWidth = GetProfileIntIfExists(_T("PageSetup"), _T("dmPaperWidth"), m_devmode->dmPaperWidth);
    m_devmode->dmDisplayOrientation = GetProfileIntIfExists(_T("PageSetup"), _T("dmDisplayOrientation"), m_devmode->dmDisplayOrientation);
    
    return true;
}




void CScroll::OnPageSetup()
{

//    pApp->SetPrintOrientation(Orientation);

    
    CPageSetupDialog psd;

  PRINTDLG pd;
  pd.hDevNames = NULL;
  pd.hDevMode = NULL;



  AfxGetApp()->GetPrinterDeviceDefaults(&pd);

  const CString szSection1 = "PrinterSetup";

///  psd.

  psd.m_psd.hDevNames = pd.hDevNames;
  psd.m_psd.hDevMode = pd.hDevMode;
  psd.m_psd.rtMargin = m_rectMargin;

//  psd.m_psd.rtMargin = m_rectMargin;
//  psd.m_psd.Flags |= PSD_INHUNDREDTHSOFMILLIMETERS;
//  psd.m_psd.Flags |= PSD_DEFAULTMINMARGINS;

  Cgid6App* pApp = (Cgid6App*)AfxGetApp();

  int Orientation = AfxGetApp()->GetProfileInt(_T("PageSetup"), _T("dmOrientation"), DMORIENT_PORTRAIT);

  psd.m_psd.hDevMode = pApp->SetPrintOrientation(Orientation    );

  if (psd.DoModal() == IDOK) {
    psd.GetMargins(&m_rectMargin, NULL);
    m_devmode = psd.GetDevMode();

    if (m_devmode->dmPaperSize > DMPAPER_LAST && m_devmode->dmPaperWidth <= 0) {
      CSize sz = psd.GetPaperSize();
      m_devmode->dmPaperWidth = sz.cx/10;
      m_devmode->dmPaperLength = sz.cy/10;
    }
    AfxGetApp()->SelectPrinter(psd.m_psd.hDevNames, psd.m_psd.hDevMode, FALSE);

    AfxGetApp()->WriteProfileInt(_T("PageSetup"), _T("dmOrientation"), m_devmode->dmOrientation);
  }
}

void CScroll::OnPrintInit()
{
  CPrintIni pi(this, &m_rectPrint, &m_rectBorder, &m_dPrintMas);

  pi.DoModal();
}

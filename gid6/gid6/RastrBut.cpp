// RastrBut.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "RastrBut.h"

#include "gid6Doc.h"
#include "gidrView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern Prim *rs;


void put_ras(CDC &dc, int x, int y, int typ);


void put_ras(CDC &dc, int x, int y, int typ) {

  if (typ >= 256)  return;

  int i, xx, yy;
  unsigned short k;
  unsigned short *str = (unsigned short *) rs[typ-1].s;

  x -= rs[typ-1].x; y -= rs[typ-1].y;
  for (i = 0, yy = y; i < 16; i++, yy++) {
    for (k = 0x8000, xx = x; k; k >>= 1, xx++) {
      if (str[i] & k) dc.SetPixel(xx, yy, 0L);
    }
  }
}



/////////////////////////////////////////////////////////////////////////////
// CRastrBut

CRastrBut::CRastrBut()
{
  typ = 0;
}

CRastrBut::~CRastrBut()
{
}


BEGIN_MESSAGE_MAP(CRastrBut, CButton)
  //{{AFX_MSG_MAP(CRastrBut)
  ON_WM_DRAWITEM()
  ON_WM_ERASEBKGND()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CRastrBut message handlers

const int *get_pics(int typ, double coef);
const int *get_pics_map(int typ);

/*

void CRastrBut::OnPaint() 
{
  CPaintDC dc(this); // device context for painting


  if (typ > 0) {
    CRect rect;
  
    GetClientRect(rect);
  
    int x = (rect.right-rect.left)/2;
    int y = (rect.bottom-rect.top)/2;

    CBgi scr;

    scr.m_rect = rect;
//    scr.m_pDC = &dc;
    scr.SetDC(&dc);

//    put_ras(dc, x, y, typ);
    const int *pic = get_pics(typ, 1);

    scr.drawLinePic(&dc, 1, x, y, x+100, y, 0, 0, 0, pic, 1);
  }
  else {

    CRect rect;
 
    GetClientRect(rect);
  
    CBgi scr;

    scr.m_rect = rect;
    scr.SetDC(&dc);

  
  }
}
*/


BOOL CRastrBut::OnEraseBkgnd(CDC*)
{
  return TRUE;    // we don't do any erasing...
}

#define CY_SHADOW   4
#define CX_SHADOW   4

//static int typ0 = 0;


void CRastrBut::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
  ASSERT(pDC != NULL);

  CRect rect;
  GetClientRect(rect);
  int cxClient = rect.Width();
  int cyClient = rect.Height();

  // draw border around icon
  CPen pen;
  pen.CreateStockObject(BLACK_PEN);
  CPen* pPenOld = pDC->SelectObject(&pen);
  pDC->Rectangle(0, 0, cxClient-CX_SHADOW, cyClient-CY_SHADOW);
  if (pPenOld)
    pDC->SelectObject(pPenOld);

  // draw shadows around icon
  CBrush br;
  br.CreateStockObject(DKGRAY_BRUSH);
  rect.SetRect(cxClient-CX_SHADOW, CY_SHADOW, cxClient, cyClient);
  pDC->FillRect(rect, &br);
  rect.SetRect(CX_SHADOW, cyClient-CY_SHADOW, cxClient, cyClient);
  pDC->FillRect(rect, &br);

  // draw the icon contents
//  pDC->StretchBlt(2, 2, cxClient-CX_SHADOW-4, cyClient-CY_SHADOW-4,
//    &dcMem, 0, 0, cxIcon, cyIcon, SRCCOPY);

  if (typ > 0) {
    rect = lpDrawItemStruct->rcItem;
    int x = (rect.right-rect.left)/2;
    int y = (rect.bottom-rect.top)/2;

    CBgi scr;

    scr.m_rect = rect;
//    scr.m_pDC = &dc;
    scr.SetDC(pDC);

//    put_ras(dc, x, y, typ);
//    const int *pic = get_pics(typ, 1);

//    typ = typ0;
//    typ0++;


    const int *pics = get_pics_map(typ);

    if (!pics) {
      pics = get_pics(typ, 1);
    }


    scr.drawLinePic(pDC, 1, x, y, x+100, y, 0, 0, WHITE, pics, 1, false);
  }


}

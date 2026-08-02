// gidrView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "cxema.h"

#include "scroll.h"

#include "win.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool LiangBar(int XS1, int YS1, int XS2, int YS2, double& x0, double& y0, double& x1, double& y1);


#define X1 9998000
#define X2 9997000
#define Y1 9996000
#define Y2 9995000

void chXY(double &x, double &y, double x1, double y1, double x2, double y2, double mas, double dx, double dy)
{
  double xx, yy, x0, y0;;

  if (X1-1000 < x && x < X1+1000) {
    x0 = x1;
    xx = x-X1;
  }
  else if (X2-1000 < x && x < X2+1000) {
    x0 = x2;
    xx = x-X2;
  }
  else {
    x0 = (x1+x2)/2;
    xx = x;
  }

  if (Y1-1000 < y && y < Y1+1000) {
    y0 = y1;
    yy = y-Y1;
  }
  else if (Y2-1000 < y && y < Y2+1000) {
    y0 = y2;
    yy = y-Y2;
  }
  else {
    y0 = (y1+y2)/2;
    yy = y;
  }

  xx /= mas;
  yy /= mas;

  x = x0 + dy*yy + dx*xx;
  y = y0 - dx*yy + dy*xx;
}

int rgb_l(int color, int tr, int bk);


void CBgi::drawLinePic(CDC *m_dc, int thickness, double x1, double y1, double x2, double y2, int color, int colorF, int bk_color,  const int *pics, double mas_otn, bool no_mas)
{
  double dd, dx, dy, x, y, x01, y01, y02, x02;
  double xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4;
  double xx, yy, r;
  int i, t, ip, c1, c2;
  int fill[256];
//  double thickness3 = 0.5*m_h;
  double thickness3 = 1;

  if (!pics) return;

  
  int c_color = color;
  double mas_otn2 = mas_otn/m_h;
  double mas = 1.;

  if (m_ring != 0) mas = 1. / m_ring;



//  double mas = 1./m_h/m_ring;
//  double mas = 1./m_h/m_ring;

  x01 = x1, y01 = y1, y02 = y2, x02 = x2;

  x = (x01+x02)/2;
  y = (y01+y02)/2;
  dd = hypot(x02-x01, y02-y01);  
  
  if (no_mas && dd <= 0. || !no_mas && dd < 1.) return;

  dx = (x02-x01)/dd/mas;
  dy = (y02-y01)/dd/mas;

  dx = (x02-x01)/dd;
  dy = (y02-y01)/dd;

  ip = 0;

  while (1) {
    t = pics[ip++];

    if (t == 'q') break;

    switch (t)
    {
    case 'f' :
    case 'l' :
      i = 0;
      while (true) {
        xx = pics[ip++]; if (xx == 9999) break;
        yy = pics[ip++];
        chXY(xx, yy, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

        fill[i++] = xx;
        fill[i++] = yy;
      }

//      setline(m_dc, c_color, 1./mas_otn+0);
      setline(m_dc, c_color, thickness3+0);

      if (t == 'f')
          setline(m_dc, c_color, 0);
      else
///          setline(m_dc, c_color, 1./mas_otn+0);
          setline(m_dc, c_color, thickness3+0);

      if (t == 'f')
          fillpoly(m_dc, i/2, fill);
      else
          drawpoly(m_dc, i/2, fill);
      break;

    case 'a' :
      xx = pics[ip++]/mas;
      yy = pics[ip++]/mas;
        xx /= mas_otn2;
        yy /= mas_otn2;
      setline(m_dc, c_color, thickness/mas_otn);
      line (m_dc, x1, y1, x + dy*yy + dx*xx, y - dx*yy + dy*xx);
      break;

    case 'b' :
      xx = pics[ip++]/mas;
      yy = pics[ip++]/mas;

        xx /= mas_otn2;
        yy /= mas_otn2;
      setline(m_dc, c_color, thickness/mas_otn);
      line (m_dc, x2, y2, x + dy*yy + dx*xx, y - dx*yy + dy*xx);
      break;

    case 'k' :  // Круг
      xx = pics[ip++];
      yy = pics[ip++];

      chXY(xx, yy, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

      r = pics[ip++];
      r /= mas_otn2;

//      setline(m_dc, c_color, 1./mas_otn+0);
      setline(m_dc, c_color, thickness3+0);
      fillellipse(m_dc, xx, yy, r/mas, r/mas);
      break;

    case 'r' : //  Окружность
      xx = pics[ip++];
      yy = pics[ip++];

      chXY(xx, yy, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

      r = pics[ip++];
        r /= mas_otn2;
//      setline(m_dc, c_color, 1./mas_otn+0);
      setline(m_dc, c_color, thickness3+0);
      ellipse(m_dc, xx, yy, r/mas, r/mas);
      break;
    case 'c' :   // Цвет
      c1 = pics[ip++];
      c2 = pics[ip++];
      switch (c1) {
      case 'C' :  {
        c_color = rgb_l(color, 30, 0xFFFFFF);  break;
      }

      case 'c' :  c_color = color;  break;
      case 'b' :  c_color = BLACK;  break;
      case 'w' :  c_color = bk_color;  break;
      case 'x' :  setfillstyle(m_dc, 1, colorF);  break;
      }
      switch (c2) {
      case 'C' :  
        {
          setfillstyle(m_dc, 1, rgb_l(color, 30, 0xFFFFFF));  
        }
        break;
      case 'c' :  setfillstyle(m_dc, 1, color);  break;
      case 'b' :  setfillstyle(m_dc, 1, BLACK);  break;
      case 'w' :  setfillstyle(m_dc, 1, bk_color);  break;
      case 'x' :  setfillstyle(m_dc, 1, colorF);  break;
      }
      break;

    case 'd' :   // Дуга
      xx = pics[ip++];
      yy = pics[ip++];

      chXY(xx, yy, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

      r = pics[ip++];
        r /= (mas_otn2/m_ring);

      xx1 = xx-r;
      yy1 = yy-r;
      xx2 = xx+r;
      yy2 = yy+r;


      xx3 = pics[ip++];
      yy3 = pics[ip++];

      chXY(xx3, yy3, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

      xx4 = pics[ip++];
      yy4 = pics[ip++];

      chXY(xx4, yy4, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

//      setline(m_dc, c_color, 1./mas_otn+0);
      setline(m_dc, c_color, thickness3+0);

      m_dc->Arc(xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4);

      break;
    }
  }
}

int sizeLineTyp(const int *pics)
{
  double xx, yy, r;
  double x1 = 100000, x2 = -100000;
  int i, t, ip;
  double len = 0;

  ip = 0;

  while (1) {
    t = pics[ip++];

    if (t == 'q') break;

    switch (t)
    {
    case 'f' :
    case 'l' :
      i = 0;
      while (true) {
        xx = pics[ip++]; if (xx == 9999) break;
        yy = pics[ip++];
        chXY(xx, yy, 0, 0, 0, 0, 1, 1, 0);
        x1 = min(x1, xx);
        x2 = max(x2, xx);
      }
      break;

    case 'a' :
    case 'b' :
    case 'c' :   // Цвет
      ip +=2;
      break;

    case 'k' :  // Круг
    case 'r' : //  Окружность
      xx = pics[ip++];
      yy = pics[ip++];

      chXY(xx, yy, 0, 0, 0, 0, 1, 1, 0);
      r = pics[ip++];
      x1 = min(x1, xx-r);
      x2 = max(x2, xx+r);
      break;
    }
  }

  return x2-0;
//  return x2-x1;
}

#include "primdrawline.h"

const int *get_pics_map(int typ);

//const int *get_pics(int typ, GID_STYLE style, double coef)
const int *get_pics(int typ, double coef)
{
  int *pics;

  pics = picUT;

  switch (typ) {

  case TIP_V_UT:   pics = picUT;    break;
  
  case TIP_UT:   pics = picUT;    break;
  case TIP_HC:   pics = picNS;    break;
  case TIP_RD:   pics = picRD;    break;
  case TIP_RR:   pics = picRR;    break;
  case TIP_RP:   pics = picRD;    break;
  case TIP_OK:   pics = picOK;    break;
  case TIP_ZD:   pics = picZD;    break;
  case TIP_ZD2:  pics = picZD2;    break;
  case TIP_BP:   pics = picVENT;  break;
  case TIP_DR:   pics = picDR;    break;
  case TIP_EL:   pics = picEL;    break;
  case TIP_TO:   pics = picTO;    break;
  case TIP_KU:   pics = picKU;    break;
  case TIP_RO:   pics = picRO;    break;

  case 55:   pics = picDR_zakr;   break;
  case 56:   pics = picZDZAKR;    break;
  case 156:  pics = picZD2;    break;
  case 57:   pics = picZD57;      break;

  case 58:   pics = picDREN;      break;
  case 59:   pics = picZD1;       break;

  case 60:   pics = picVOZD;      break;

  case 61:   pics = picOPORA;     break;
  case 62:   pics = picKompVert;  break;

//  case 63:   pics = picRING1;      break; ////

  case 64:   pics = picKompSaln;  break;
  case 65:   pics = picKompSilf;  break;
  case 66:   pics = picPER;       break;
  case 67:   pics = picCIRCLE;    break;
  case 68:   pics = picZD0;       break;
  case 69:   pics = picSPUSK;      break;


  case 70:   pics = picRING1;      break;
  case 71:   pics = picRING2;      break;
  case 72:   pics = picROMB;      break;

  case 73:   pics = picFLAG;      break;


//  case 74:   pics = picFLAG;      break;
//  case 75:   pics = picFLAG;      break;



  case 99:   pics = picCIRCLE0;   break;

  default :  pics = NULL;         break;
  }
  return pics;
}

void CScroll::drawLineTyp(CDC *m_dc, double thickness, double x1, double y1, double x2, double y2, int typ, int color, int colorF, BOOL ris, BOOL napr, BOOL strelka, BOOL isQ, bool no_map) 
{
  if (!napr && (typ == TIP_UT || typ == TIP_V_UT)) {
    double x, y;
    x = x1; x1 = x2; x2 = x;
    y = y1; y1 = y2; y2 = y;
  }

  if (ris == 1 && typ == TIP_UT && !isQ) {
    return;
  }

  double x01 = x1, y01 = y1, y02 = y2, x02 = x2;

  if (!LiangBar(m_rect.left-20/ geom.masx, m_rect.top-20/ geom.masy,
                m_rect.right+20/ geom.masx, m_rect.bottom+20/ geom.masy,
          x1, y1, x2, y2)) return;

  if (x1 == x2 && y1 == y2) return;


  double main_thickness = thickness/mas_otn;

  if (main_thickness < thickness && thickness < 4) main_thickness = thickness;

  if (typ < 0) {
    setline(m_dc, LIGHTGRAY, main_thickness);
    line(m_dc, x1, y1, x2, y2);
    return;
  }

  setline(m_dc, color, main_thickness);
  setfillstyle(m_dc, 1, color);

  if (ris == -1 && typ != TIP_UT && typ != TIP_V_UT) {
    line(m_dc, x1, y1, x2, y2);
    return;
  }
  if (ris == 0 && typ != TIP_UT) {
//    line(x1, y1, x2, y2);
  }


  if (ris != 1 && (typ == TIP_UT || typ == TIP_V_UT)) {
    int i, n, x, y, xx, yy;
    double len = hypot(x2-x1, y2-y1)*mas_otn;

    if (len_dot <= 0.) {
      n = 0;
    }
    else {
      n = len/len_dot;
    }

    if (n <= 1) {
      line(m_dc, x1, y1, x2, y2);
    }
    else {
      for ( i = 0; i < n; i++) {
        x = x1 + (x2-x1)*i/n;
        y = y1 + (y2-y1)*i/n;

        if (i == n-1) {
          xx = x2;
          yy = y2;
        }
        else {
          xx = x1 + (x2-x1)*(i+0.7)/n;
          yy = y1 + (y2-y1)*(i+0.7)/n;
        }
        line(m_dc, x, y, xx, yy);
      }
    }
    return;
  }

  int d = 4;
  double coef = 1;

  const int *pics = get_pics_map(typ);
  
  if (pics == NULL) {
    pics = get_pics(typ, coef);
  }

  if (pics == NULL) {
    pics = get_pics(0, coef);
  }

  if (pics == NULL) {
    return;
  }


  int dd = max(d*4, d*thickness);

  int picUT_NAPR[] = 
  {
    'f', -d+dd, d/2+thickness/2, 0+dd, 0, -d+dd, -d/2-thickness/2,  9999,
    'q'
  };

  double len = hypot(x1-x2, y1-y2), minlen = 30*m_h;
  double mas_otn_old = mas_otn;

  minlen = sizeLineTyp(pics);

  if (typ == TIP_UT) minlen = sizeLineTyp(picUT_NAPR);

  if (!no_map && len*mas_otn < minlen ) {
    mas_otn = minlen/len;
  }

  if (typ != TIP_UT || strelka) {
    drawLinePic(m_dc, thickness, x01, y01, x02, y02, color, colorF, m_bk_color, pics, mas_otn, no_map);
  }
  if (typ != TIP_UT && strelka && isQ) {
    if (!napr) {
      double x, y;
      x = x01; x01 = x02; x02 = x;
      y = y01; y01 = y02; y02 = y;
    }
    drawLinePic(m_dc, thickness, x01, y01, x02, y02, color, colorF, m_bk_color, picUT_NAPR, mas_otn, no_map);
  }
  mas_otn = mas_otn_old;
}

void CScroll::drawNodeNull(CDC*m_dc, double x, double y, int color)
{
  double thickness3 = 1;

  setcolor(m_dc, BLACK, thickness3);
  if (color == YELLOW) 
    setfillstyle(m_dc, 1, color);
  else
    setfillstyle(m_dc, 1, LIGHTGRAY);
  fillellipse(m_dc, x, y, 4, 4);
}



void CScroll::drawNodeTyp(CDC*m_dc, double x, double y, int typ, int color, double markerPx)
{
//  double thickness3 = 0.5*m_h;
  double thickness3 = 1;

  if (typ < 0 || typ >= 1024) {
    drawNodeNull(m_dc, x, y, color);

/*
//    setcolor(BLACK, m_colors.m_thickness/mas_otn);
    setcolor(m_dc, BLACK, thickness3);
    if (color == YELLOW) 
      setfillstyle(m_dc, 1, color);
    else
      setfillstyle(m_dc, 1, LIGHTGRAY);
    fillellipse(m_dc, x, y, 4, 4);
*/
    return;
  }

//  setcolor(color, m_colors.m_thickness/mas_otn);
  setcolor(m_dc, color, thickness3);
  setfillstyle(m_dc, 1, color);

  int fill[256];

  int d = 4;
  int i;

  int *pics[1024];

  for (i = 0; i < 1024; i++) pics[i] = NULL;

  if (m_ring == 0) m_ring = 1;


  double mas;
  if (markerPx > 0.0) {
      double new_mas_otn = mas_otn / 3;
      if (new_mas_otn < 1.0)
          new_mas_otn = 1.0;
      mas = 1.0 / (markerPx * new_mas_otn * m_h * m_ring);
  }
  else {
      mas = 1. / m_h / m_ring;
  }

#include "primdrawnode.h"

  pics[TIP_US] = picUS;
  pics[TIP_PR] = picPR;
  pics[TIP_PO] = picPT;
  pics[TIP_UP] = picUP;
  pics[TIP_IS] = picIS;
  pics[TIP_HS] = picNS;
  pics[TIP_C3] = picC3;
//  pics[TIP_ZP] = picZP;
  pics[TIP_US2] = picUS2;


///////////
  pics[TIP_PODP] = picPODP;
//  pics[TIP_PODP] = picKRAN;


//  pics[TIP_VODOR] = picVODOR;
  pics[TIP_VODOR] = picKRAN;

  
  pics[TIP_V_PT] = picPR;
  pics[TIP_V_IS] = picIS;

  
  
  pics[22] = picMN; // С внутренностями
  pics[TIP3_ZN] = picZN; // Узел с заданным напором
  pics[24] = picEL; // С элеватором
  pics[25] = picKAM; // Камера
  pics[26] = picINVALID; // 
  pics[27] = picPR_El; // 
  pics[28] = picPR_Nez; // 

  pics[TIP3_ORP] = picORP; // Объекты с измеряемыми параметрами

  pics[30] = picPT_C;

  pics[31] = picPRIBOR; // 

  pics[32] = pic3; // Трехходовой


  pics[35] = picKvadrat;
  pics[36] = picKrug;
  pics[37] = picTri;
  pics[38] = picPT_C2;




  if (!pics[typ]) {
//    drawNodeNull(m_dc, x, y, color);
//    return;
    typ = TIP_US;
  }


  double mas_otn2 = mas_otn;

  double xx, yy, r, xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4, xx0, yy0;

  int t, ip, c1, c2;

  ip = 0;

  while (1) {
    t = pics[typ][ip++];

    if (t == 'q') break;

    switch (t)
    {
    case 'f' :
    case 'l' :
      i = 0;
      while (true) {
        xx = pics[typ][ip++]; if (xx == 9999) break;
        yy = pics[typ][ip++];
        xx /= mas_otn2;
        yy /= mas_otn2;

        fill[i++] = x + xx/mas;
        fill[i++] = y + yy/mas;
      }

      if (t == 'f')
          fillpoly(m_dc, i/2, fill);
      else
          drawpoly(m_dc, i/2, fill);
      break;

    case 'k' :  // Круг
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];

      xx /= mas_otn2;
      yy /= mas_otn2;
      
      r = pics[typ][ip++];
      r /= mas_otn2;
      fillellipse(m_dc, x + xx/mas, y + yy/mas, r/mas, r/mas);
      break;
    case 'r' : //  Окружность
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];
      r = pics[typ][ip++];
      r /= mas_otn2;
      ellipse(m_dc, x + xx/mas, y + yy/mas, r/mas, r/mas);
      break;
    case 'c' :   // Цвет
      c1 = pics[typ][ip++];
      c2 = pics[typ][ip++];
      switch (c1) {
      case 'C' :  setcolor(m_dc, rgb_l(color, 30, 0xFFFFFF), thickness3);  break;
      case 'c' :  setcolor(m_dc, color, thickness3);  break;
      case 'b' :  setcolor(m_dc, BLACK, thickness3);  break;
      case 'w' :  setcolor(m_dc, m_bk_color, thickness3);  break;
      case 'x' :  setcolor(m_dc, 1, LIGHTCYAN);  break;
      case 'y' :  setcolor(m_dc, 1, YELLOW);  break;
//      case 'x' :  setfillstyle(1, color*2);  break;
      }
      switch (c2) {
      case 'C' :  setfillstyle(m_dc, 1, rgb_l(color, 30, 0xFFFFFF));  break;
      case 'c' :  setfillstyle(m_dc, 1, color);  break;
      case 'b' :  setfillstyle(m_dc, 1, BLACK);  break;
      case 'w' :  setfillstyle(m_dc, 1, m_bk_color);  break;
//      case 'x' :  setfillstyle(m_dc, 1, LIGHTCYAN);  break;
      case 'x' :  setfillstyle(m_dc, 1, m_color_uzel);  break;
      case 'y' :  setfillstyle(m_dc, 1, YELLOW);  break;
//      case 'x' :  setfillstyle(1, color*2);  break;
      }
      break;

    case 'd' :   // Дуга
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];
      r = pics[typ][ip++]/mas;
      r /= mas_otn2/mas;

      xx0 = x + xx/mas;
      yy0 = y + yy/mas;

      xx1 = xx0-r;
      yy1 = yy0-r;
      xx2 = xx0+r;
      yy2 = yy0+r;

      xx3 = pics[typ][ip++]/mas;
      yy3 = pics[typ][ip++]/mas;
      xx4 = pics[typ][ip++]/mas;
      yy4 = pics[typ][ip++]/mas;

      m_dc->Arc(xx1, yy1, xx2, yy2,
        x + xx3, y + yy3, x + xx4, y + yy4);
      break;
    }
  }
}


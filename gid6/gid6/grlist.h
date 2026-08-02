#pragma once

#include "Scroll.h"
#include "coordlis.h"

class CGrObject
{
public:
  CGrObject() : rect(0., 0., 0., 0.), color(0)
  {};
  virtual void Draw(CDC *m_dc, CScroll *scr) {};

  int color;
  CFRect rect;
};

class CLineObject : public CGrObject
{
public:
  CLineObject(double _x1, double _y1, double _x2, double _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2)
  {
    rect = CFRect(x1, y1, x2, y2);
    rect.NormalizeRect();
  }
  void Draw(CDC *m_dc, CScroll *scr)
  {
    scr->setline(m_dc, color, width/scr->geom.masx);
    scr->line(m_dc, x1/scr->geom.masx-scr->geom.m_bx, y1/scr->geom.masy-scr->geom.m_by, x2/scr->geom.masx-scr->geom.m_bx, y2/scr->geom.masy-scr->geom.m_by);
  }
  double x1, y1, x2, y2, width;
};

class CRectObject : public CGrObject
{
public:
  CRectObject(double _x1, double _y1, double _x2, double _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2)
  {
    rect = CFRect(x1, y1, x2, y2);
    rect.NormalizeRect();
  }

  void Draw(CDC *m_dc, CScroll *scr)
  {
    scr->setline(m_dc, color, width/scr->geom.masx);
    scr->rectangle(m_dc, x1/scr->geom.masx-scr->geom.m_bx, y1/scr->geom.masy-scr->geom.m_by, x2/scr->geom.masx-scr->geom.m_bx, y2/scr->geom.masy-scr->geom.m_by);
  }

  double x1, y1, x2, y2, width;
};


class CTextObject : public CGrObject
{
public:
  CTextObject(double _x, double _y, const char *_txt, const char *_fnt, double _size, UINT _align, double _napr);
  void Draw(CDC *m_dc, CScroll *scr);

  double x, y;
  double size;
  UINT align;
  double napr;
  CString txt;
  CString fnt;
};


class CPolyObject : public CGrObject
{
public:
  CPolyObject(CCoordList & _cl) : cl(_cl)
  {
    rect = cl.GetFRect();
  }
  CCoordList cl;
};


//class CGrList : public list<CGrObject *>, public CGrObject
class CGrList : public CGrObject
{
public:
  CGrList() : 
    size(8), width(0), font("Arial"), align(0), napr(0)
   {};
  ~CGrList() { clear(); };

  void line(double x1, double x2, double y1, double y2);
  void bmp(const char *fN, double x1, double y1, double w, double h = 0.);
  void rectangle(double x1, double x2, double y1, double y2);
  void textout(double x, double y, const char *txt);
  void setcolor(int _color) {color = _color;};
  void setalign(int _align) {align = _align;};
  void setfontsize(double _size) {size = _size;};
  void setwidth(double _width) {width = _width;};
  void setnapr(double _napr) {napr = _napr;};
  void setfont(const char *_font) {font = _font;};
  void setrect(const CFRect & _rect) {rect = _rect;};
  void clear();

  void Draw(CDC *m_dc, CScroll *scr);

//  CFRect rect;
//  int color;
  UINT align;
  double width, size, napr;
  CString font;

private:
  list<CGrObject *> _list;
};



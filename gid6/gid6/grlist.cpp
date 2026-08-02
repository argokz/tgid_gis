#include "stdafx.h"
#include "Bmp.h"
#include "grlist.h"

CTextObject::CTextObject(double _x, double _y, const char *_txt,
  const char *_fnt, double _size, UINT _align, double _napr)

  : x(_x), y(_y), txt(_txt), size(_size), fnt(_fnt), align(_align), napr(_napr)
{
//  rect = CFRect(x-size*strlen(txt), y-size, x+size*strlen(txt), y+size);
  rect = CFRect(x-size*strlen(txt), y-size*strlen(txt), x+size*strlen(txt), y+size*strlen(txt));
  rect.NormalizeRect();
}

void CTextObject::Draw(CDC *m_dc, CScroll *scr)
{
  LOGFONT lf;
  CFont *oldfont, font;

  double masx = scr->geom.masx, masy = scr->geom.masy;
  int m_bx = scr->geom.m_bx;
  int m_by = scr->geom.m_by;
//  CDC *m_dc = scr->m_dc;

  font.CreatePointFont(100, fnt);
  font.GetLogFont(&lf);
  lf.lfHeight = size/masx;

  lf.lfEscapement = 1;
  lf.lfOrientation = 1;
  lf.lfEscapement = napr*10;
  lf.lfOrientation = napr*10;
  font.DeleteObject();
  font.CreateFontIndirect(&lf);

  oldfont = m_dc->SelectObject(&font);

  m_dc->SetTextAlign(align);

  m_dc->SetBkMode(TRANSPARENT);
  m_dc->SetTextColor(color);
  m_dc->TextOut(x/masx-m_bx, y/masy-m_by, txt);

  m_dc->SelectObject(oldfont);
}


void CGrList::Draw(CDC *m_dc, CScroll *scr)
{
  CFRect r, rect;
  CFRect rectScr(
    (scr->geom.m_bx+scr->m_rect.left-10)*scr->geom.masx,
    (scr->geom.m_by+scr->m_rect.top-10)*scr->geom.masy,
    (scr->geom.m_bx+scr->m_rect.right+10)*scr->geom.masx,
    (scr->geom.m_by+scr->m_rect.bottom+10)*scr->geom.masy);

  list<CGrObject *>::const_iterator it = _list.begin();

  for ( ; it != _list.end(); ++it) {
    if (!r.IntersectRect((*it)->rect, rectScr)) continue;
//    scr->rectangle((*it)->rect.left/scr->masx-scr->m_bx, (*it)->rect.top/scr->masy-scr->m_by, (*it)->rect.right/scr->masx-scr->m_bx, (*it)->rect.bottom/scr->masy-scr->m_by);
//    DrawObject(*it);
    (*it)->Draw(m_dc, scr);
  }
}

void CGrList::rectangle(double x1, double y1, double x2, double y2)
{
  CRectObject *go = new CRectObject(x1, y1, x2, y2);
  go->color = color;
  go->width = width;
  _list.push_back(go);
  rect.UnionRect(&rect, &go->rect);
}

void CGrList::textout(double x, double y, const char *txt)
{
  CTextObject *go = new CTextObject(x, y, txt, font, size, align, napr);
  go->color = color;
  _list.push_back(go);
  rect.UnionRect(&rect, &go->rect);
}

void CGrList::line(double x1, double y1, double x2, double y2)
{
  CLineObject *go = new CLineObject(x1, y1, x2, y2);
  go->color = color;
  go->width = width;
  _list.push_back(go);
  rect.UnionRect(&rect, &go->rect);
}

void CGrList::bmp(const char *fN, double x0, double y0, double w, double h)
{
  CBmp *go = new CBmp(fN);    

  go->m_x0 = x0, go->m_y0 = y0, go->m_dx0 = w, go->m_dy0 = h;
  go->rect = CFRect(x0, y0, x0+max(w, h), y0+max(h, w));
  go->rect.NormalizeRect();

  _list.push_back(go);
  rect.UnionRect(&rect, &go->rect);
}

void CGrList::clear()
{
  list<CGrObject *>::const_iterator it;
  for (it = _list.begin(); it != _list.end(); ++it) delete (*it);
}

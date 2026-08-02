// PodlList.cpp : implementation file
//

/*
      Список подложек
*/

#include "stdafx.h"
#include "Resource.h"
#include "bbb.h"
#include "win.h"

static FILE *ff = nullptr;

CString GetName(const char *path);

void CPodlList::OpenQQQ(const char *fN) {
  FILE *f;
  PODL p;
  PodlObj qqq;

  f = fopen(fN, "rb");
  if (f) {
    fread((void*) &p, sizeof(PODL), 1, f);
    fclose(f);


    qqq.fN = fN;
    qqq.mx0 = p.x;
    qqq.my0 = p.y;
    qqq.x0 = p.x0;
    qqq.y0 = p.y0;
    qqq.dx = p.dx0;
    qqq.dy = p.dy0;
    qqq.typ = 2;

    insert(qqq);

/*
    if (ff) {
      CString str;
      str = GetName(qqq.fN);
      fwrite(&qqq.x0, 4*sizeof(double)+3*sizeof(int), 1, ff);
      fprintf(ff, "%s\n", (LPCTSTR)str);
    }
*/
  }
}

void CPodlList::OpenBBB(const char *fN, int typ) 
{
  FILE *f;
  PCXHEADER h;
  PodlObj qqq;

  f = fopen(fN, "rb");
  if (f) {
    fread((void*) &h,128,1,f);
    fclose(f);

    qqq.fN = fN;
    qqq.mx0 = (h.x2 - h.x1 + 1);
    qqq.my0 = (h.y2 - h.y1 + 1);
    qqq.x0 = h.x0;
    qqq.y0 = h.y0;
    qqq.dx = h.dx;
    qqq.dy = h.dy;
    qqq.typ = typ;

    insert(qqq);

    if (ff) {
      fwrite(&qqq.x0, 4*sizeof(double)+3*sizeof(int), 1, ff);
      CString str;
      str = GetName(qqq.fN);
      fprintf(ff, "%s\n", (LPCTSTR)str);
    }
  }
}

void CPodlList::openFile(const char *fn) 
{
  clear();
  OpenQQQ(fn);
}


void CPodlList::openDir(const char *dir) 
{
  CFileFind finder;
  BOOL bWorking;
  CString str;
  PodlObj qqq;
  char s[256];
  int n;

  str = CString(dir)+CString("\\filelist.lst");

  ff = fopen(str, "rb");

  if (ff) {
    do {
      fread(&qqq.x0, 4*sizeof(double)+3*sizeof(int), 1, ff);
      n = fgetstr(s, 255, ff);

      if (n == 0) break;

      qqq.fN = CString(dir) + CString("\\")+s;

      insert(qqq);
    } while (n != 0);
    fclose(ff);
    ff = nullptr;
  }
  else {
    ff = fopen(str, "wb");
    ChDir cd(dir);

    bWorking = finder.FindFile(_T("*.grf"));
    while (bWorking) {
      bWorking = finder.FindNextFile();
      OpenQQQ(finder.GetFilePath());
    } 

    bWorking = finder.FindFile(_T("*.ggg"));
    while (bWorking) {
      bWorking = finder.FindNextFile();
      OpenBBB(finder.GetFilePath(), 1);
    }

    bWorking = finder.FindFile(_T("*.bbb"));
    while (bWorking) {
      bWorking = finder.FindNextFile();
      OpenBBB(finder.GetFilePath(), 0);
    }

    finder.Close();
    if (ff) fclose(ff);
    ff = nullptr;
  }
}


CPodlList::CPodlList() : set<PodlObj>() {
}

#include "Podl.h"

#include "mmenu.h"

void rectangle(CDC *pDC, int x1, int y1, int x2, int y2);

CFPoint CPodlList::find()
{
  CMMenu menu(AfxGetMainWnd());

  const_iterator it;

  for ( it = begin(); it != end(); ++it ) {
    menu.lst.Add(GetName(it->fN));
  }

  if (menu.DoModal() == IDOK) {
    for ( it = begin(); it != end(); ++it ) {
      if (GetName(it->fN) == menu.m_strText) {
        return CFPoint(it->x0+0.5*it->dx, -it->y0-0.5*it->dy);
      }
    }
  }
  return CFPoint(0, 0);
}


void CPodlList::Draw(CDC *dc, CRect rect, double m_bx, double m_by, double masx, double masy)
{
  double bbx, bby, mx, my;
  double mas;
  const_iterator it;

  CRect m_rect = rect;

  for ( it = begin(); it != end(); ++it ) {

    mas = it->dx/it->mx0/masx;

    bbx = m_bx-it->x0/masx+m_rect.left;
    bby = m_by+(it->y0+it->dy)/masy+m_rect.top;

    mx = it->mx0*mas;  my = it->my0*mas;

    if (mx > bbx && my > bby && bbx > m_rect.left-m_rect.right && bby > m_rect.top-m_rect.bottom) {
      if (mas >= 1./16.) {
        CPodl podl(it->fN);
        podl.Draw(dc, rect, m_bx, m_by, masx, masy);
      }
//      else {
        CPen penBlack;
        penBlack.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
        CPen* pOldPen = dc->SelectObject(&penBlack);

        rectangle(dc, it->x0/masx-m_bx, -(it->y0+it->dy)/masy-m_by,
                  (it->x0+it->dx)/masx-m_bx, -it->y0/masy-m_by);

        dc->SelectObject(pOldPen);
//      }
    }
  }
}

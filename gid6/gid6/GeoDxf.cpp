#include "stdafx.h"

#include "geofile.h"

#include "win.h"

#include "math.h"


#include "dxf/DXF.H"

CString GetTitle(CString path);
CString GetTitleB(CString path);

void write_pline(FILE *dxf, int flags, double width1, double width2, const char *layer, const CCoordList &cl, double m_coef) 
{
  write_pline(dxf, 0, 0., 0., layer);
  CCoordList::const_iterator it;

  for ( it = cl.begin(); it != cl.end(); ++it) {
    if (!isBreakCoord(it->x)) {
      write_vertex(dxf, it->x/m_coef, -it->y/m_coef, 0., 0., 0., layer);
    }
    else {
      write_seqend(dxf, layer);
      write_pline(dxf, 0, 0., 0., layer);
    }
  }
  write_seqend(dxf, layer);
}


void CGeoFile::DxfLayer_shp_dbf(FILE *dxf, const char *tn, CFRect & rectBorder)
{
  int i, n;
  CFRect rect, rr;

  KlList::iterator it = m_kl_list->begin();

/*
  for ( ; it != m_kl_list->end(); ++it) {
    Klassif *kl = &it->second;
    if (!_strcmpi(kl->nazv, tn)) {
      m_kls000 = kl;
      break;
    }
  }
*/

  Klassif* kls = m_kl_list->findKlN(tn);

  if (!kls) return;

//  Cshp_dbf dbf(m_kls->nazv.c_str());

//  InitS(m_kls->nazv);

  int loc = 3;

//  int qq = m_kls->geo4.size();

  n = kls->NObj();

  for ( i = 1; i <= n; i++ ) {
    m_nCur = i;

    GetFRect(kls, i, rect);

    if (!rr.IntersectRect(rect, rectBorder)) continue;


    if (rect.Width() == 0 && rect.Height() == 0) loc = 1;

      try {
        if (!m_shp->go_to(kls->geo4[i-1]->nom)) break;


        double x1, y1, x2, y2;
        CCoordList cl;

        m_shp->read_coor2(x1, y1, x2, y2, cl);

        switch(m_shp->loc) {
         case 6: loc = 6; break;
         case 3: loc = 2; break;
         case 5: loc = 3; break;
         default:loc = 1; break;
        }

        double m_coef = 100;

        if (loc == LOC1) {
          char s[256];

          if (kls->n_text_string >= 0) {
            strcpy(s, read_textstring());

            if (s[0]) {
              double angle = read_angle();
              double sz = read_size();

              int nl  = 1;
              for (int j = 0; s[j]; j++) {
                if (s[j] == '\n') nl++;
              }

              if (sz != 0) {
                sz /= nl;
              }

              sz *= 0.5;

              CCoordList::const_iterator it = cl.begin();

              if (it != cl.end()) {
                if (nl == 1) {
  //                  m_dc->TextOut(pt.x, pt.y, s);
                  write_text(dxf, it->x/m_coef, -it->y/m_coef, s, angle, sz, 0);
                }
                else {
                  for (int j = 0; s[j]; j++) {
                    if (s[j] == '\n') s[j] = 0;
                  }
  //                  m_dc->TextOut(pt.x, pt.y, s);
                  write_text(dxf, it->x/m_coef, -it->y/m_coef, s, angle, sz, 0);
                }
              }
            }
          }
        }
        else if (cl.size() > 1) {
          write_pline(dxf, 0, 0., 0., GetTitleB(tn), cl, m_coef);
        }
      }
      catch (...) {
      }
    }
}

void CGeoFile::DxfLayer(FILE *dxf, const char *tn, const char *in, CFRect & rectBorder)
{
  CFRect r, rect;
  int i, n;
  CPoint pt;

  Klassif* kls = m_kl_list->findKlN(tn);

  if (!kls) return;


  int loc = GetLoc(kls);

  n = kls->NObj();

  double m_coef = 100;

  for (i = 1; i <= n; i++) {
      GetFRect(kls, i, rect);

      if (rect.Width() == 0 && rect.Height() == 0 && !isPoint(loc)) continue;

      //    if (!r.IntersectRect(rect, rectBorder)) continue;

      //    if (GoToN(i)) {
      CCoordList cl;
      if (GetCoord(kls, i, cl)) {
          if (!isPoint(loc) && cl.size() > 1) {
              write_pline(dxf, 0, 0., 0., GetTitleB(tn), cl, m_coef);
          }
      }
  }
}


COLORREF rgb_color(int c);


int rgb2dxf(unsigned long rgb)
{
  int mincol = 16;
  double dx, mindx = 1e20;
  unsigned int col, kol;


  for (int c = 1; c < 256; c++) {
    col = dxf_color(c);
    kol = rgb;

    double r1, r2, g1, g2, b1, b2;

    r1 = col&0xFF; g1 = (col&0xFF00)>>8; b1 = (col&0xFF0000)>>16;
    r2 = kol&0xFF; g2 = (kol&0xFF00)>>8; b2 = (kol&0xFF0000)>>16;

    dx = (r1-r2)*(r1-r2)+(g1-g2)*(g1-g2)+(b1-b2)*(b1-b2);

    if (dx < mindx) {
      mindx = dx;
      mincol = c;
    }
  }

  unsigned long rgb1 = dxf_color(mincol);

  return mincol;
}

void CGeoFile::DxfKlass(FILE *dxf)
{
  KlList::iterator it;

  for ( it = m_kl_list->begin(); it != m_kl_list->end(); ++it) {
    Klassif *kl = &it->second;
    if (kl->ins) {
      generate_tables(dxf, GetTitleB(kl->FileName()), rgb2dxf(kl->coli));  // Правильно
    }
  }
}

int write_block_record(FILE* dxf, const char* name, int handle_parent);



void CGeoFile::DxfKlass1(FILE *dxf)
{
  KlList::iterator it;

  for ( it = m_kl_list->begin(); it != m_kl_list->end(); ++it) {
    Klassif *kl = &it->second;
    if (kl->ins) {
        write_block_record(dxf, GetTitleB(kl->FileName()), 0);
    }
  }
}

void CGeoFile::DxfKlass2(FILE *dxf)
{
  KlList::iterator it;

  for ( it = m_kl_list->begin(); it != m_kl_list->end(); ++it) {
    Klassif *kl = &it->second;
    if (kl->ins) {
      generate_tables(dxf, GetTitleB(kl->FileName()), rgb2dxf(kl->coli));  // Правильно
    }
  }
}


void CGeoFile::Dxf(FILE *dxf, CFRect & rectBorder)
{
  KlList::iterator it;

  int aloc[] = {3, 2, 6, 1, 4, 0};
  int p;

  for (int loc = 0; aloc[loc]; loc++) {

    for (p = 0; p < 11; p++) {
      for ( it = m_kl_list->begin(); it != m_kl_list->end(); ++it) {
        Klassif *kl = &it->second;
        if (kl->prio == p && kl->ins && kl->loc == aloc[loc]) {
          if ( is_odbc ) {
//              drawLayer_xy(scr, it->nazv.c_str(), it->xy_table.c_str());
          }
          else if ( kl->is_shpdbf ) {
            DxfLayer_shp_dbf(dxf, kl->nazv, rectBorder);
          }
          else {
            DxfLayer(dxf, kl->nazv, kl->xy_table, rectBorder);
          }
        }
      }
    }
  }
}

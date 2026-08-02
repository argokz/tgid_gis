#if !defined(__Podl_h)              // Sentry, use file only if it's not already included.
#define __Podl_h

#include "bbb.h"

/////////////////////////////////////////////////////////////////////////////
// CPodl view

class CPodl
{
protected:

// Attributes
public:
  CPodl(const char *fN);           // protected constructor used by dynamic creation
  virtual ~CPodl();

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPodl)
  protected:
//  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
protected:
//  CPodlList qqq_list;
  double m_x0, m_y0, m_dx0, m_dy0;
  int m_x, m_y;
  int m_typ;

  PODL p;
  CString m_fN;
  CRect m_rect;
//  CDC *m_pDC;
  PCXHEADER hdr;
  RGBQUAD *Palette;
  double mas;
  long *off;
  unsigned char *image;
  FILE *f;
  int DD, BTS, typ;

  int readBBB(int x = 0, int y = 0);
  HANDLE readBBB1(int x1, int y1, int x2, int y2);
  HANDLE readBBB4(int x1, int y1, int x2, int y2);
  int readBBBhead(const char *filename, int &x, int &y);

  void SetPalette(RGBQUAD *palette);
  int readQQQ(int x, int y);
  HANDLE readQQQ1(int x1, int y1, int x2, int y2);
  void PaintQQQ(CDC *dc, int tsx, int tsy);
  void PaintQQQ_P(CDC *dc, int tsx, int tsy);
  void PaintBBB(CDC *dc, int tsx, int tsy, int t);

  void ClosePodl();
  int OpenPodl(const char *filename);

public:
  void Draw(CDC *dc, CRect rect, double m_bx, double m_by, double masx, double masy);
};

/////////////////////////////////////////////////////////////////////////////
#endif                                      // __Podl_h sentry.

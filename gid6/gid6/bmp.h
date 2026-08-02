#if !defined(__Bmp_h)              // Sentry, use file only if it's not already included.
#define __Bmp_h

#include "grlist.h"

/////////////////////////////////////////////////////////////////////////////
// CBmp view

class CBmp : public CGrObject
{
protected:

// Attributes
public:
  CBmp(const char *fN);           // protected constructor used by dynamic creation
  virtual ~CBmp();

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CBmp)
  protected:
//  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
protected:
  int m_x, m_y;

  CString m_fN;
  CRect m_rect;
//  CDC *m_pDC;

  unsigned char *image;

  void PaintBMP(CDC *m_pDC, int tsx, int tsy, int mx, int my);

  void CloseBmp();
  int OpenBmp(const char *filename);

public:
  void Draw(CDC *dc, CRect rect, long m_bx, long m_by, double masx, double masy);
  void Draw(CScroll *scr, CDC *m_dc);
  double m_x0, m_y0, m_dx0, m_dy0;
};

/////////////////////////////////////////////////////////////////////////////
#endif                                      // __Bmp_h sentry.

#if !defined(AFX_COLORSV_H__11564FC6_347A_11D3_B153_8334196DC269__INCLUDED_)
#define AFX_COLORSV_H__11564FC6_347A_11D3_B153_8334196DC269__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Coor.h : header file
//

#include "Bgi.h"
#include "colorgtd.h"

/*
struct GID_COLORS
{
  GID_COLORS() {
   set_def();
  }
  void set_def() {
    CUR_COLOR = 0x00FF00;
    PODOTKL = 0x9f9fFF;
    OBROTKL = 0xFF9f9f;
    COLOR_POD = RED;
    COLOR_OBR = BLUE;
    COLOR_OTKL = GREEN;
    COLOR_NEOTR = LIGHTRED;
    m_thickness = 1.;
    m_distance = 1.5;
  }

  void save();
  void load();

  unsigned long CUR_COLOR;
  unsigned long PODOTKL;
  unsigned long OBROTKL;
  unsigned long COLOR_POD;
  unsigned long COLOR_OBR;
  unsigned long COLOR_OTKL;
  unsigned long COLOR_NEOTR;
  double m_thickness;
  double m_distance;
};

*/

class CColorsWindow : public CStatic {
private:
  DECLARE_DYNCREATE(CColorsWindow)

public:
  CColorsWindow();

  void set_ring(double ring) {
    m_ring = ring;
    Invalidate();
  };

  void set_color_uzel(unsigned long color_uzel) {
    m_color_uzel = color_uzel;
    Invalidate();
  };
  
  void set_colors(int _mag_ras, int _open_close, int _typ) {
//    m_colors = colors;
    m_mag_ras = _mag_ras;
    m_open_close = _open_close;
    m_typ = _typ;
    Invalidate();
  };
//  virtual ~CColorsWindow();
  void Draw (int tec);

protected:
//  GID_COLORS *m_colors;
  int m_mag_ras, m_open_close, m_typ;
  unsigned long m_color_uzel;

  int x_old, y_old, first;
  double coef, xmin, ymin, xmax, ymax, m_ring;
  //{{AFX_MSG(CColorsWindow)
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSV_H__11564FC6_347A_11D3_B153_8334196DC269__INCLUDED_)

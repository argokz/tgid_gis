#include <QColor>


static int m_nTransp = 100;
static int m_bk_color;

void setTransp(int tr, int bk_color)
{
    m_nTransp = tr;
    m_bk_color = bk_color;
}


//int rgb_l(int color, int tr, int bk_color)
QColor rgb_l(QColor _color, int tr, QColor _bk_color)
{
  int r, g, b;
  int rb, gb, bb;

  unsigned int color = _color.rgb();
  unsigned int bk_color = _bk_color.rgb();

//  int tr = m_nTransp;

  if (0 < tr && tr < 255) 
  {
    r = (color&0xFF0000) >> 16;
    g = (color&0x00FF00) >> 8;
    b = (color&0x0000FF);

    rb = (bk_color&0xFF0000) >> 16;
    gb = (bk_color&0x00FF00) >> 8;
    bb = (bk_color&0x0000FF);

/*
  rgbtohls(r, g, b, h, l, s);
  l *= 3;
  shift_int(l, 0);
  hlstorgb(h, l, s, r, g, b);
*/

    r = (rb*(255-tr)+r*tr)/255;
    g = (gb*(255-tr)+g*tr)/255;
    b = (bb*(255-tr)+b*tr)/255;

    _color = QColor(r<<16|g<<8|b);
  }


  return _color;
}

QColor rgb_l(QColor _color)
{
    return rgb_l(_color, m_nTransp * 256/100, m_bk_color);
}

/*

int rgb_l(int color)
{
  return rgb_l(color, m_nTransp, m_bk_color);
}

*/


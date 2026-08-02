#if !defined(AFX_COLORGTD_H__11564FC6_347A_11D3_B153_8334196DC269__INCLUDED_)
#define AFX_COLORGTD_H__11564FC6_347A_11D3_B153_8334196DC269__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Coor.h : header file
//

#include "Bgi.h"
#include "Graph.h"
#include "Graph2.h"
#include "win.h"


#define COLOR_NEOTR GREEN
#define CUR_COLOR 0x00FF00
#define CUR_OTKL GREEN

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

struct GID_STYLE
{
  int m_style;
  double m_thickness;
  double m_distance;
  double m_thickness_save;
  double m_distance_save;
};

inline int MagRasPar(const CLine *l)
{
//  return 1;

//  int dm = 426;
  int dm = 400;

//  CNode *n = &where(l)->node;
//  if (n && !strcmp(n->node_name.kod, "пар")) return 2;

  return (l->pod.diam >= dm || l->obr.diam >= dm) ? 0 : 1;
}


struct GID_COLORS
{
  GID_COLORS() {
   set_def();
//   set_def2();
  }
/*
  1 - Магистраль Распредсеть
  2 - Надземные Бесканальные Канальные Подвальные
*/

  void set_def();
  void set_def2();
  void set_defPS();
  void set_distance_0();
  void distance_save();
  void distance_load();


  GID_STYLE &style(int l1 = 1, int l2 = 0) {
    return m_style[l1][l2];
  }


  GID_STYLE &style(CNode *n) {
    return m_style[0][1];
  }

  GID_STYLE &style(CLine *l) {
    int l1 = 1, l2 = 0;

//    l1 = l->isMag ? 0 : 1;
//    l1 = isMagPar(l) ? 0 : 1;
    l1 = MagRasPar(l);

//    int dm = 426;
//    l1 = l->diamP >= dm || l->diamO >= dm ? 0 : 1;

/**
    switch(l->name_typ) {
      case 'Н' : l2 = 0; break;
      case 'Б' : l2 = 1; break;
      case 'К' : l2 = 2; break;
      case 'П' : l2 = 3; break;
    }
*/

    switch (l->tubingTypeID) {
    case 4 /*'Н'*/: l2 = 0; break;
    case 2 /*'Б'*/: l2 = 1; break;
    case 1 /*'К'*/: l2 = 2; break;
    case 3 /*'П'*/: l2 = 3; break;
    case 5 /*'П'*/: l2 = 3; break;
    }


    return m_style[l1][l2];
  }

  void save(int regim);
  void load(int regim);


/*
  1 - Магистраль Распредсеть
  2 - Надземные Бесканальные Канальные Подвальные
*/


/*
  1 - Подача Обратка
  2 - Нагруженная Не нагруженная Закрытая Яркая
  3 - Магистраль Распредсеть Паропрвод
  4 - Надземные Бесканальные Канальные Подвальные
  5 - Невыделенная Выделенная
*/

  unsigned long &pod(int l2, int l3, int l5) {
    return m_color[0][l2][l3][l5];
  }

  unsigned long &obr(int l2, int l3, int l5) {
    return m_color[1][l2][l3][l5];
  }

  unsigned long &color(const CNode *n, int m_bIsGidrInf) {
    int l1 = 0, l2 = 0, l3 = 0, l5 = 0;

    l1 = n->nomP != -1 ? 0 : 1;
    if (n->nomgP == -1 && n->nomgO == -1 && m_bIsGidrInf) l2 = 1;

    if ( n->isPjezo ) l2 = 2;

    l3 = 0;

    return m_color[l1][l2][l3][l5];
  }

  unsigned long pod(const CLine *l, int m_bIsGidrInf) {
    int l2 = 0, l5 = 0;

    l2 = l->pod.isOtkl || l->pod.isOtklZD ? 2 : (!isNul(l->pod.q) || !m_bIsGidrInf) ? 0 : 1;

//    if ( l->isPjezo ) l2 = 2;

    if (!l->isPjezo && !l->obr.isOtkl && !l->obr.isOtklZD && l->colorP != 0xFFFFFFFF) return l->colorP;

//    int l3 = isMag(l) ? 0 : 1;
    int l3 = MagRasPar(l);

    return pod(l2, l3, l5);
  }

  unsigned long obr(const CLine *l, int m_bIsGidrInf) {
    int l2 = 0, l5 = 0;

    l2 = l->obr.isOtkl || l->obr.isOtklZD ? 2 : (!isNul(l->obr.q) || !m_bIsGidrInf) ? 0 : 1;

//    if ( l->isPjezo ) l2 = 2;

    if (!l->isPjezo && !l->obr.isOtkl && !l->obr.isOtklZD && l->colorO != 0xFFFFFFFF) return l->colorO;

//    int l3 = isMag(l) ? 0 : 1;
    int l3 = MagRasPar(l);

    return obr(l2, l3, l5);
  }







  unsigned long pod(const CLine *l, int l3, int m_bIsGidrInf) {
    int l2 = 0, l5 = 0;

    l2 = l->pod.isOtkl || l->pod.isOtklZD ? 2 : (!isNul(l->pod.q) || !m_bIsGidrInf) ? 0 : 1;
    if (!l->isPjezo && !l->obr.isOtkl && !l->obr.isOtklZD && l->colorP != 0xFFFFFFFF) return l->colorP;
    return pod(l2, l3, l5);
  }

  unsigned long obr(const CLine *l, int l3, int m_bIsGidrInf) {
    int l2 = 0, l5 = 0;
    l2 = l->obr.isOtkl || l->obr.isOtklZD ? 2 : (!isNul(l->obr.q) || !m_bIsGidrInf) ? 0 : 1;
    if (!l->isPjezo && !l->obr.isOtkl && !l->obr.isOtklZD && l->colorO != 0xFFFFFFFF) return l->colorO;
    return obr(l2, l3, l5);
  }






  unsigned long podF(const CLine *l, int m_bIsGidrInf) {
    int l2 = 0, l5 = 0;

    l2 = l->pod.isOtkl || l->pod.isOtklZD ? 2 : (!isNul(l->pod.q) || m_bIsGidrInf) ? 0 : 1;

//    if ( l->isPjezo ) l2 = 2;

//    int l3 = isMag(l) ? 0 : 1;
    int l3 = MagRasPar(l);

    return pod(3, l3, l5);
  }

  unsigned long obrF(const CLine *l, int m_bIsGidrInf) {
    int l2 = 0, l5 = 0;

    l2 = l->obr.isOtkl || l->obr.isOtklZD ? 2 : (!isNul(l->obr.q) || m_bIsGidrInf) ? 0 : 1;

//    if ( l->isPjezo ) l2 = 2;

//    int l3 = isMag(l) ? 0 : 1;
    int l3 = MagRasPar(l);

    return obr(3, l3, l5);
  }

  void save1();
  void load1();


  unsigned long m_color[2][4][3][2];
  GID_STYLE m_style[3][4];

  unsigned long m_color_save[2][4][3][2];
  GID_STYLE m_style_save[3][4];

};

extern GID_COLORS m_colors;


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORGTD_H__11564FC6_347A_11D3_B153_8334196DC269__INCLUDED_)

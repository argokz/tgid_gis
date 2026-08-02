#include "stdafx.h"
#include "colorgtd.h"

GID_COLORS m_colors;

/*
  1 - Магистраль Распредсеть
  2 - Надземные Бесканальные Канальные Подвальные
*/

/*
  1 - Подача Обратка
  2 - Нагруженная Не нагруженная Закрытая Яркая
  3 - Магистраль Распредсеть Паропровод
  4 - Надземные Бесканальные Канальные Подвальные
  5 - Невыделенная Выделенная
*/

void GID_COLORS::set_distance_0()
{
  int i, j;

//  distance_save();

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      m_style[i][j].m_distance = 0;
    }
  }
} 

void GID_COLORS::distance_save()
{
  int i, j;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      m_style[i][j].m_distance_save = m_style[i][j].m_distance;
    }
  }
}

void GID_COLORS::distance_load()
{
  int i, j;
  
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      m_style[i][j].m_distance = m_style[i][j].m_distance_save;
    }
  }
}



void GID_COLORS::set_def() 
{
  int i, j, k, m;

  double th[3] = {3, 1, 3};

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      m_style[i][j].m_thickness = th[i];
      m_style[i][j].m_distance = 1.5;

      if (th[i] == 3) m_style[i][j].m_distance = 2;
      
      m_style[i][j].m_style = 0;
    }
    m_style[i][0].m_style = 1;
  }

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 3; k++) {
        for (m = 0; m < 2; m++) {
          m_color[i][j][k][m] = 0;
        }
      }
    }
  }


//  Выделенная

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 3; k++) {
        m_color[i][j][k][1] = 0x00FF00;  // Выделенная
      }
    }
  }

/*
  1 - Подача Обратка
  2 - Нагруженная Не нагруженная Закрытая Яркая
  3 - Магистраль Распредсеть Паропровод  //   4 - Надземные Бесканальные Канальные Подвальные
  4 - Невыделенная Выделенная
*/



  for (k = 0; k < 3; k++) {
    m_color[0][0][k][0] = RED; 
    m_color[1][0][k][0] = BLUE;

    m_color[0][1][k][0] = 0x9f9fFF;
    m_color[1][1][k][0] = 0xFF9f9f;

    m_color[0][2][k][0] = GREEN;
    m_color[1][2][k][0] = GREEN;

    m_color[0][2][k][0] = 
    m_color[1][2][k][0] = 0x808000;   // Закрытый

    m_color[0][3][k][0] = LIGHTRED; 
    m_color[1][3][k][0] = LIGHTBLUE;
  }
}

void GID_COLORS::set_def2() 
{
  int i, j, k, m;
  double th[3] = {3, 3, 5};
//  int color[3] = {DARKGREEN, RED, BLUE};
  int color[3] = {0x003F00, RED, BROWN};
  double dist[4] = {1.5, 0, 0, 0};


  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      m_style[i][j].m_thickness = th[i];
//      m_style[i][j].m_distance = 0;
      m_style[i][j].m_distance = dist[j];

      if (dist[j] > 0) m_style[i][j].m_thickness -= 1;

      m_style[i][j].m_style = 1;
    }
//    m_style[i][0].m_style = 1;
  }

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 3; k++) {
        for (m = 0; m < 2; m++) {
          m_color[i][j][k][m] = color[k];
          m_color[i][2][k][m] = CYAN;
          m_color[i][2][k][m] = CYAN;
        }
      }
    }
  }
}


void GID_COLORS::set_defPS() 
{
  int i, j, k, m;
  double th[3] = {3, 2, 3};
//  int color[3] = {DARKGREEN, RED, BLUE};
//  int color[3] = {0x00FF00, 0x0000FF, BROWN};
  int color[3] = {0x008000, 0x0000FF, BROWN};
  double dist[4] = {1.5, 0, 0, 0};


  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      m_style[i][j].m_thickness = th[i];
//      m_style[i][j].m_distance = 0;
      m_style[i][j].m_distance = dist[j];

      if (dist[j] > 0) m_style[i][j].m_thickness -= 1;

      m_style[i][j].m_style = 1;
    }
//    m_style[i][0].m_style = 1;
  }

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 3; k++) {
        for (m = 0; m < 2; m++) {
          m_color[i][j][k][m] = color[k];
          m_color[i][2][k][m] = CYAN;
          m_color[i][2][k][m] = CYAN;
        }
      }
    }
  }
}


void GID_COLORS::save(int regim)
{
  int i, j, k, m;
  CString str1, str2;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      str1.Format("%d_%d_%d", regim, i, j);
      str2.Format("%d %.2g %.2g", m_style[i][j].m_style,  m_style[i][j].m_thickness, m_style[i][j].m_distance);
      AfxGetApp()->WriteProfileString(CString(szSection)+"\\Style2", str1, str2);
    }
  }

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 3; k++) {
       for (m = 0; m < 2; m++) {
         str1.Format("%d_%d_%d_%d_%d", regim, i, j, k, m);
         AfxGetApp()->WriteProfileInt(CString(szSection)+"\\Color2", str1, m_color[i][j][k][m]);
       }
      }
    }
  }
}

void GID_COLORS::save1()
{
  int i, j, k, m;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      m_style_save[i][j].m_style     =  m_style[i][j].m_style;      
      m_style_save[i][j].m_thickness =  m_style[i][j].m_thickness;  
      m_style_save[i][j].m_distance  =  m_style[i][j].m_distance;   
    }
  }

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 3; k++) {
         for (m = 0; m < 2; m++) {
           m_color_save[i][j][k][m] = m_color[i][j][k][m];
         }
      }
    }
  }
}

void GID_COLORS::load1()
{
  int i, j, k, m;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      m_style[i][j].m_style     =  m_style_save[i][j].m_style;      
      m_style[i][j].m_thickness =  m_style_save[i][j].m_thickness;  
      m_style[i][j].m_distance  =  m_style_save[i][j].m_distance;   
    }
  }

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 3; k++) {
        for (m = 0; m < 2; m++) {
          m_color[i][j][k][m] = m_color_save[i][j][k][m];
        }
      }
    }
  }
}



void GID_COLORS::load(int regim)
{
  int i, j, k, m, n;
  CString str1, str2;
  int l1;
  double l2, l3;

  if (regim) {
    set_defPS();
  }
  else {
    set_def();
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      str1.Format("%d_%d_%d", regim, i, j);
      str2.Format("%d %g %g",  m_style[i][j].m_style,  m_style[i][j].m_thickness, m_style[i][j].m_distance);
      str2 = AfxGetApp()->GetProfileString(CString(szSection)+"\\Style2", str1, str2);
      n = sscanf((const char*)str2, "%ld %lg %lg", &l1, &l2, &l3);
      m_style[i][j].m_style = l1;
      m_style[i][j].m_thickness = l2;
      m_style[i][j].m_distance = l3;
    }
  }

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 3; k++) {
       for (m = 0; m < 2; m++) {
         str1.Format("%d_%d_%d_%d_%d", regim, i, j, k, m);
         m_color[i][j][k][m] = AfxGetApp()->GetProfileInt(CString(szSection)+"\\Color2", str1, m_color[i][j][k][m]);
       }
      }
    }
  }
}


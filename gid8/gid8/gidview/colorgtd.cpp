#include "colorgtd.h"
#include <QSettings>


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

    m_color[0][1][k][0] = 0xFF9f9f;
    m_color[1][1][k][0] = 0x9f9fFF;

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
//  int color[3] = {0x008000, 0x0000FF, BROWN};
  int color[3] = {0x008000, 0xFF0000, 0x007F7F};
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
    QSettings settings;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            QString str1 = QString("%1_%2_%3").arg(regim).arg(i).arg(j);
            QString str2 = QString("%1 %2 %3").arg(m_style[i][j].m_style).arg(m_style[i][j].m_thickness).arg(m_style[i][j].m_distance);
            settings.setValue(QString("Style2/%1").arg(str1), str2);
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3; k++) {
                for (int m = 0; m < 2; m++) {
                    QString str1 = QString("%1_%2_%3_%4_%5").arg(regim).arg(i).arg(j).arg(k).arg(m);
                    settings.setValue(QString("Color2/%1").arg(str1), QVariant((unsigned int)m_color[i][j][k][m]));
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

    if (regim) {
        set_defPS();
    }
    else {
        set_def();
    }

    QSettings settings;
    
    int i, j, k, m, n;
    int l1;
    double l2, l3;

//    dlg.m_cmbMas = settings.value("print_form/mas", 0).toInt();;


    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            QString str1 = QString("%1_%2_%3").arg(regim).arg(i).arg(j);
            QString str2 = QString("%1 %2 %3").arg(m_style[i][j].m_style).arg(m_style[i][j].m_thickness).arg(m_style[i][j].m_distance);

            str2 = settings.value(QString("Style2/%1").arg(str1), str2).toString();

//            AfxGetApp()->GetProfileString(CString(szSection)+"\\Style2", str1, str2);

            auto l = str2.split(' ');

            if (l.size() == 3) {
                bool ok;
                l1 = l[0].toDouble(&ok);
                l2 = l[1].toDouble(&ok);
                l3 = l[2].toDouble(&ok);

                m_style[i][j].m_style = l1;
                m_style[i][j].m_thickness = l2;
                m_style[i][j].m_distance = l3;
            }
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 3; k++) {
               for (m = 0; m < 2; m++) {
                   QString str1 = QString("%1_%2_%3_%4_%5").arg(regim).arg(i).arg(j).arg(k).arg(m);
                   m_color[i][j][k][m] = settings.value(str1, (unsigned int)m_color[i][j][k][m]).toInt();
               }
            }
        }
    }
}


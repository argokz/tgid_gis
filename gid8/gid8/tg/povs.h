#pragma once

#pragma hdrstop

/////////////////////////////////////////////////////////////////////////////
struct POV
{
  double KOL;    // Количество точек расчета

  double THOR;   // Расчетная температура наружного воздуха
  double TVR;    // Расчетная температура воздуха внутри помещения
  double TAURP;  // Расчетная темп-ра сетевой воды в подающем трубопроводе
  double TAURO;  //     -#-                        в обратном
  double TAURS;  //     -#-                        после узла смешения
  double QOR;    // Расчетная нагрузка отопления, Гкал/ч
  double QGW;    // Среднечасовая нагрузка горячего водоснабжения, Гкал/ч
  double THK;    // Температура наружного воздуха конца отопительного сезона
  double TSMIN;  // Температура нижней срезки  сетевой воды
  double TSMAX;  // Температура верхней срезки сетевой воды
  double TVRO;   // Температура горячей воды в точке водоразбора
  double TV;     // Температура холодной воды
  double TB;     // Требуемая температура воздуха внутри отапливаемых помещени
  double NEDOG;  // Величина недогрева водопроводной воды в первой ступени по-
                 // догревателя горячего водоснабжения, Град. C
  double QMAX;   // Отношение максимального количества тепла отпускаемого ТЭЦ
                 // к расчетному теплу на отопление магистрали
  double KSR;    // Поправка коэффициента смешения
  double V;      // Скорость ветра
  char  TEC[15]; // Наименование ТЭЦ
  char  MAG[15]; // Наименование магистали
  char  GDN[5];  // Начало отопительного сезона
  char  GDK[5];  // Конец отопительного сезона

  POV() { memset( this, 0, sizeof(POV) ); }
};

struct POVDATA
{
  int n;
  double    *tn;      // Температура наружного воздуха
  double *tau01;   // В Подающем
  double *tau02;   // В Обратном
  double *t01;
  double *t02;
  double *tau01v;  // Ветра
  double *tb;      //
  double *tg;      //
  double defm;     // Дефицит мощности

  POVDATA() { memset( this, 0, sizeof(POVDATA));  }

  ~POVDATA()
  {
    delete[] tn;
    delete[] tau01;  delete[] tau02;
    delete[] t01;    delete[] t02;   
    delete[] tau01v; 
    delete[] tb;     delete[] tg;
  }

  void Init( int diap )
  {
    n=diap;
    tn     = new    double[ diap+10 ];
    tau01  = new double[ diap+10 ];
    tau02  = new double[ diap+10 ];
    t01    = new double[ diap+10 ];
    t02    = new double[ diap+10 ];
    tau01v = new double[ diap+10 ];
    tb     = new double[ diap+10 ];
    tg     = new double[ diap+10 ];
  }
};



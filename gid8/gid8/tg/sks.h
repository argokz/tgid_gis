#pragma once
#pragma hdrstop

/////////////////////////////////////////////////////////////////////////////
struct SKK
{
  int    KOL;    // Количество точек расчета
  bool   IsPov;  // 1=Повышенный / 0=Пониженный
  int    iw;     // Точка излома

  double THOR;   // Расчетная температура наружного воздуха
  double TVR;    // Расчетная температура воздуха внутри помещения
  double TAURP;  // Расчетная темп-ра сетевой воды в подающем трубопроводе
  double TAURO;  //      -"-                       в обратном
  double TAURS;  //      -"-                       после узла смешения

//  double QR;     // Расчетная тепловая нагрузка на отопление
  double QMAX;   // Отношение максимального количества тепла отпускаемого ТЭЦ
                 //   к расчетному теплу на отопление магистрали
  double QOR;    // Расчетная нагрузка отопления, Гкал/ч
  double QGW;    // Среднечасовая нагрузка горячего водоснабжения, Гкал/ч

  double TSMIN;  // Температура нижней срезки  сетевой воды
  double TSMAX;  // Температура верхней срезки сетевой воды
  double T2MIN;  // Температура нижней срезки обратной воды
  double THK;    // Температура наружного воздуха конца отопительного сезона
  char   TEC[15];// Наименование ТЭЦ
  char   MAG[15];// Наименование магистали
  char   GDN[5]; // Начало отопительного сезона
  char   GDK[5]; // Конец отопительного сезона

  double KSR;    // Поправка коэффициента смешения
  double V;      // Скорость ветра

  double KGUP;   // Коэффициент гидравлической устойчивости подающего труб-да
  double KGUO;   // Коэффициент гидравлической устойчивости обратного труб-да

  int    PSN;    // Признак водоразбора воды на ГВ с переключением
  int    PSY;    // Признак водоразбора воды на ГВ с узлом смешения
  int    PSP;    // Признак водоразбора воды на ГВ только из подающего
  int    PSO;    // Признак водоразбора воды на ГВ только из обратного

  double T2GW;   // Темпер-ра сетевой воды начала водоразбора из обрат-го тр-да
  double TV;     // Температура холодной воды
  double TB;     // Требуемая температура воздуха внутри отап-х помещениях
  double TVRO;   // Температура горячей воды в точке водоразбора

  SKK() { memset( this, 0, sizeof(SKK) ); }
};

struct SKDATA
{
  int n;
  double    *tn;      // Температура наружного воздуха
  double *tau01;   // В Подающем
  double *tau02;   // В Обратном
  double *tau03;   // Смешанная
  double *tau01v;  // Ветра
  double *t01;     //
  double *t02;     //
  double defm;     // Дефицит мощности
  double *tb;      //
  double *tgw;     //

  SKDATA()  { memset( this, 0, sizeof(SKDATA)); }

  ~SKDATA()
  {
    delete[] tn;
    delete[] tau01; delete[] tau02;  delete[] tau03;
    delete[] t01;   delete[] t02;
    delete[] tau01v;
    delete[] tb;    delete[] tgw;
  }

  void Init( int diap )
  {
    n=diap;
    tn     = new    double[ diap+10 ];
    tau01  = new double[ diap+10 ];
    tau02  = new double[ diap+10 ];
    tau03  = new double[ diap+10 ];
    tau01v = new double[ diap+10 ];
    t01    = new double[ diap+10 ];
    t02    = new double[ diap+10 ];
    tb     = new double[ diap+10 ];
    tgw    = new double[ diap+10 ];
  }
};

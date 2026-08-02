#pragma once
#pragma hdrstop

/////////////////////////////////////////////////////////////////////////////
struct OTOP
{
  int    KOL;    // Количество точек расчета

  double THOR;   // Расчетная температура наружного воздуха
  double TVR;    // Расчетная температура воздуха внутри помещения
  double TAURP;  // Расчетная темп-ра сетевой воды в подающем трубопроводе
  double TAURO;  //      -"-                       в обратном
  double TAURS;  //      -"-                       после узла смешения
  double QR;     // Расчетная тепловая нагузка на отопление
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
  double TB;     // Требуемая температура воздуха внутри отап-х помещениях
  double QMAX;   // Отношение максимального количества тепла отпускаемого ТЭЦ
                 // к расчетному теплу на отопление магистрали

  OTOP() { memset( this, 0, sizeof(OTOP) ); }
};

struct OTDATA
{
  int n;
  double *tn;      // Температура наружного воздуха
  double *tau01;   // В Подающем
  double *tau02;   // В Обратном
  double *tau03;   // Смешанная
  double *QO;      //
  double *tau01v;  // Ветра
  double defm;     // Дефицит мощности
  double *tb;      //

  OTDATA()  { memset( this, 0, sizeof(OTDATA)); }

  ~OTDATA()
  {
    delete[] tn;
    delete[] tau01; delete[] tau02;  delete[] tau03;
    delete[] QO;    delete[] tau01v; delete[] tb;
  }

  void Init( int diap )
  {
    n = diap;
    tn     = new double[ diap+10 ];
    tau01  = new double[ diap+10 ];
    tau02  = new double[ diap+10 ];
    tau03  = new double[ diap+10 ];
    QO     = new double[ diap+10 ];
    tau01v = new double[ diap+10 ];
    tb     = new double[ diap+10 ];
  }
};

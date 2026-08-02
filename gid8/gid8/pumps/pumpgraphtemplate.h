#ifndef PUMPGRAPHTEMPLATE_H
#define PUMPGRAPHTEMPLATE_H

#include <QPainter>
#include <QWidget>
#include <QString>
#include <QRect>
#include <QPen>
#include <QBrush>
#include <QSqlQuery>
#include <QSqlError>

struct GRAPH_PARAM {
    double H[10]; // Напор
    double Q[10]; // Расход
    double N[10]; // Мощность
    double KPD[10]; // КПД
    double Q_min; // Расход (начало рабочей зоны)
    double Q_max; // Расход (конец рабочей зоны)
    QString Tip; // Тип насоса
    QString Name; // Название насоса
    int k_kol; // Количество насосов
};

struct GRAPHR_PARAM {
    GRAPH_PARAM gr; // Данные графика
    double c_vr, diam, S; // Частота, диаметр, гидравлическое сопротивление
    bool Spr; // Признак учета сопротивления
    double diam1, c_vr1; // Номинальные параметры
    double Nmin, Nmax; // Мощности
    bool IsDnom, IsFnom; // Использовать номинальные значения
};

struct APPROX_PARAM
{
    double pog; //погрешность аппроксимации
    double r0; //коэффиниент аппроксимации r0
    double r1; //коэффиниент аппроксимации r1
    double r2; //коэффиниент аппроксимации r2
    void SetToNullApprox()
    {
        pog=0;
        r0=0;
        r1=0;
        r2=0;
    }
};
enum TTypeChart{H_chart,N_chart,KPD_chart};
class PumpGraphTemplate {
public:
    void Draw(QPainter &painter);
    void DrawCalc(QPainter &painter);
    void DrawCalc2(QPainter &painter);
    void DrawContent(QPainter &painter); //отчет для графика аппроксимации
    void DrawContent2(QPainter &painter); //отчет для графика обточки диаметра колеса
    void prdim(QPoint p, QPainter *drawdc);
    void DrawChart(QPainter &painter, int type, APPROX_PARAM *apr_par, bool prepared);
    void DrawEdge(QPainter &painter);
    void clearGuidelines(QWidget *widget);
    void drawGuidelines(QWidget *widget);
    void ChangeDrawParams(int type, APPROX_PARAM *apr_par, QPen* &pen);
    void ChangeDrawParams2(int type, APPROX_PARAM *apr_par, QPen* &pen);
    void setStandardPumps(QSqlQuery &query, GRAPHR_PARAM &grp, int id);
    void initData(GRAPH_PARAM * GR);
    void initData2(GRAPH_PARAM * GR); //для графика среза

private:
    double FindMax(double *arr);
    void inihq(double *n, double *mas, short kol );
    void DrawGraph(double r0, double r1, double r2, QPainter* painter, int col, int row);
    void MasSh(double min, double max, short kol, double *mas);
    void Appr( double &r0,double &r1,double &r2,double *arr1, double *arr2, double* pog );
protected:
    int llx,lly;
    int gr_rap; // График (0) - отчет (1)
    short stepX, stepY;
    short nGridX, nGridY;

    double minX, maxX, minY, maxY;
    double masX, masY;

    //для графика среза
    double r0, r1, r2;
    char legX[5], legY[5];

    double qq2, hh2, //Точка на номинальной характеристике
        del_p, //Полученная относительна величина обточки
        del_d;//Допустимая относительна величина обточки
    double kb;    // Коэффициент быстроходности
    double _rr2;  // Переменная аппроксимации (для пересчета диаметра обточки)




public:
    short lX, lY, rX, rY;

    APPROX_PARAM apH, //параметры аппроксимации для H=f(Q)
        apHr,//параметры аппроксимации для H_zad=f(Q)
        apN, ////параметры аппроксимации для N=f(Q)
        apNr,//параметры аппроксимации для N_zad=f(Q)
        apKPD,//параметры аппроксимации для KPD=f(Q)
        apKPDr;//параметры аппроксимации для KPD_zad=f(Q)


    bool Show_H_chart,
        Show_Hr_chart,
        Show_N_chart,
        Show_Nr_chart,
        Show_KPD_chart,
        Show_KPDr_chart;

    GRAPH_PARAM DiamGR;
    GRAPHR_PARAM* GrApr;
    bool calcMode = false;

    //для графика среза
    double d_obt; // Диаметр обточки
    // При нажатии Правой кнопки мыши нужны они
    int frst;
    int rButtonDown;
    bool showGuidelines = false;
    QPoint guidelinePoint; // Текущая позиция для линий
    QPoint p, pmode;
};

#endif // PUMPGRAPHTEMPLATE_H



#include "stdafx.h"
#include <math.h>


const double Pi = 3.14159265358979; // Число Пи
const double ro = 206264.8062; // Число угловых секунд в радиане

// Эллипсоид Красовского
const double aP = 6378245; // Большая полуось
const double alP = 1 / 298.3; // Сжатие
const double e2P = 2 * alP - pow(alP, 2); // Квадрат эксцентриситета

// Эллипсоид WGS84 (GRS80, эти два эллипсоида сходны по большинству параметров)
const double aW = 6378137; // Большая полуось
const double alW = 1 / 298.257223563; // Сжатие
const double e2W = 2 * alW - pow(alW, 2); // Квадрат эксцентриситета

// Вспомогательные значения для преобразования эллипсоидов
const double a = (aP + aW) / 2;
const double e2 = (e2P + e2W) / 2;
const double da = aW - aP;
const double de2 = e2W - e2P;

// Линейные элементы трансформирования, в метрах
/*
const double dx = 23.92;
const double dy = -141.27;
const double dz = -80.9;
*/

//const double dx = 23.92;
//const double dy = -141.27;
//const double dz = -80.9;

/*
msk51
Обвід
Extent not known
Proj4
+proj=tmerc +lat_0=0 +lon_0=30 +k=1 +x_0=300000 +y_0=0 +ellps=krass +towgs84=24.376,-121.321,-75.895,0,0,0,0 +units=m +no_defs
*/


static double dx = 24.376;
static double dy = -121.321;
static double dz = -75.895;

// Угловые элементы трансформирования, в секундах
static double wx = 0;
static double wy = 0;
static double wz = 0;


//static double wx = -0;
//static double wy = -0.35;
//static double wz = -0.82;

// Дифференциальное различие масштабов
//static double ms = 0;
//static double ms = 0.12*1e-6;
static double ms = 0;



double dB(double Bd, double Ld, double H);
double dL(double Bd, double Ld, double H);


double WGS84_SK42_Lat(double Bd, double Ld, double H) {
    return Bd - dB(Bd, Ld, H) / 3600;
}

double SK42_WGS84_Lat(double Bd, double Ld, double H) {
    return Bd + dB(Bd, Ld, H) / 3600;
}

double WGS84_SK42_Long(double Bd, double Ld, double H) {
    return Ld - dL(Bd, Ld, H) / 3600;
}

double SK42_WGS84_Long(double Bd, double Ld, double H) {
    return Ld + dL(Bd, Ld, H) / 3600;
}

double dB(double Bd, double Ld, double H) {
    double B, L, M, N;
    B = Bd * Pi / 180;
    L = Ld * Pi / 180;
    M = a * (1 - e2) / pow((1 - e2 * pow(sin(B), 2)), 1.5);
    N = a * pow((1 - e2 * pow(sin(B), 2)), -0.5);
    return ro / (M + H) * (N / a * e2 * sin(B) * cos(B) * da + (pow(N, 2) / pow(a, 2) + 1) * N * sin(B) * cos(B) * de2 / 2 - (dx * cos(L) + dy * sin(L)) * sin(B) + dz * cos(B)) - wx * sin(L) * (1 + e2 * cos(2 * B)) + wy * cos(L) * (1 + e2 * cos(2 * B)) - ro * ms * e2 * sin(B) * cos(B);
}

double dL(double Bd, double Ld, double H) {
    double B, L, N;
    B = Bd * Pi / 180;
    L = Ld * Pi / 180;
    N = a * pow((1 - e2 * pow(sin(B), 2)), -0.5);
    return ro / ((N + H) * cos(B)) * (-dx * sin(L) + dy * cos(L)) + tan(B) * (1 - e2) * (wx * cos(L) + wy * sin(L)) - wz;
}

double WGS84Alt(double Bd, double Ld, double H) {
    double B, L, N, dH;
    B = Bd * Pi / 180;
    L = Ld * Pi / 180;
    N = a * pow((1 - e2 * pow(sin(B), 2)), -0.5);
    dH = -a / N * da + N * pow(sin(B), 2) * de2 / 2 + (dx * cos(L) + dy * sin(L)) * cos(B) + dz * sin(B) - N * e2 * sin(B) * cos(B) * (wx / ro * sin(L) - wy / ro * cos(L)) + (pow(a, 2) / N + H) * ms;
    return H + dH;
}


void SK42_WGS84(double Bd, double Ld, double H, double &Bo, double &Lo, double &Ho) {
    double B, L, N, M;
    B = Bd * Pi / 180;
    L = Ld * Pi / 180;

    N = a * pow((1 - e2 * pow(sin(B), 2)), -0.5);
    M = a * (1 - e2) / pow((1 - e2 * pow(sin(B), 2)), 1.5);

    double dB = ro / (M + H) * (N / a * e2 * sin(B) * cos(B) * da + (pow(N, 2) / pow(a, 2) + 1) * N * sin(B) * cos(B) * de2 / 2 - (dx * cos(L) + dy * sin(L)) * sin(B) + dz * cos(B)) - wx * sin(L) * (1 + e2 * cos(2 * B)) + wy * cos(L) * (1 + e2 * cos(2 * B)) - ro * ms * e2 * sin(B) * cos(B);
    double dL = ro / ((N + H) * cos(B)) * (-dx * sin(L) + dy * cos(L)) + tan(B) * (1 - e2) * (wx * cos(L) + wy * sin(L)) - wz;
    double dH = -a / N * da + N * pow(sin(B), 2) * de2 / 2 + (dx * cos(L) + dy * sin(L)) * cos(B) + dz * sin(B) - N * e2 * sin(B) * cos(B) * (wx / ro * sin(L) - wy / ro * cos(L)) + (pow(a, 2) / N + H) * ms;

    Bo =  Bd + dB / 3600;
    Lo =  Ld - dL / 3600;
    Ho =  H + dH;
}

void toWGS84params(double _dx, double _dy, double _dz, double _wx = 0, double _wy = 0, double _wz = 0, double _ms = 0)
{
    dx = _dx;
    dy = _dy;
    dz = _dz;

    wx = _wx;
    wy = _wy;
    wz = _wz;
    ms = _ms*1e-6;
}
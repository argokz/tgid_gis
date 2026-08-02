#ifndef PUMPREPORT_H
#define PUMPREPORT_H

#include "pumpgraphtemplate.h"


struct PUMP_REPORT
{
    APPROX_PARAM apH, //параметры аппроксимации для H=f(Q)
        apHr,//параметры аппроксимации для H_zad=f(Q)
        apN, ////параметры аппроксимации для N=f(Q)
        apNr,//параметры аппроксимации для N_zad=f(Q)
        apKPD,//параметры аппроксимации для KPD=f(Q)
        apKPDr;//параметры аппроксимации для KPD_zad=f(Q)

    int k_kol, driveTypeID, rotorDiameterTypeID;
    double d, f, d_obt;
    int drive_id, wheel_id;
    int pump_id;
    QString typePump;
    bool calc = false;

};

#endif // PUMPREPORT_H

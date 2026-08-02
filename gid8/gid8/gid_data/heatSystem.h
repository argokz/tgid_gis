#pragma once

struct C_heatSystem
{
   QString name;
   QString nasel_point;
   int seasonID;
   QString year;
   double t_or;
   double t_vr;
   double t_vnew;
   double tx;
   double tx_leto;
   int tip_Qgvs;
   QDateTime begin_year;
   QDateTime end_year;
   double tn_god;
   double tg_god;
   double tn_god_leto;
   double tg_god_leto;
   double a;
   QDateTime year_audit;
   double tvn[5];
   double G1n_sys[5];
   double G2n_sys[5];
   double Gnz_sys[5];
   double Qn_pot[5];
   double Qn_gv_pot[5];
   double Q1n_tp[5];
   double Q2n_tp[5];
   double t1n_pot[5];
   double t2n_pot[5];
   double t1n_sys[5];
   double t2n_sys[5];
   double tn_long[12];
   double tgr_long[12];
   double tpod_long[12];
   double tn_fakt[12];
   double tgr_fakt[12];
   double tpod_fakt[12];
   QString phone_manager;
   int removed;
   int idRemoved;
   double Central_Meridian;
   double Latitude_Of_Origin;
   int False_Easting;
   int False_Northing;
   double Scale_Factor;
   double Angle;
   double dx;
   double dy;

};


bool getCT(QSqlDatabase &db, C_heatSystem &d);

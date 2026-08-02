#pragma once

struct SystemCoord
{
    SystemCoord() :  False_Easting(0),  False_Northing(0), Central_Meridian(0), Latitude_Of_Origin(0), Scale_Factor(1), Angle(0) {};

    bool isMercator() 
    {
        return Central_Meridian == 0;
    }

    double False_Easting = 0;
    double False_Northing = 0;
    double Central_Meridian = 0;
    double Latitude_Of_Origin = 0;
    double Scale_Factor = 1;
    double Angle = 0;
    double dx = 0;
    double dy = 0;
};



void set_system_coord(double _False_Easting, double _False_Northing, double _Central_Meridian, double _Latitude_Of_Origin, double _Scale_Factor, double _Angle, double _dx, double _dy);
void set_system_coord(SystemCoord sc);


void d2m3(SystemCoord& system_coord, double B, double L, double &x, double &y);

void m2m(SystemCoord& system_coord, double X1, double Y1, double& xx, double& yy, bool shift);
void m2m2(SystemCoord& system_coord, double X1, double Y1, double& xx, double& yy, bool shift);

double lat2y(double y);
double y2lat(double y);
double lon2x(double x);
double x2lon(double H);


double WGS84_SK42_Lat(double Bd, double Ld, double H);
double WGS84_SK42_Long(double Bd, double Ld, double H);
double SK42_WGS84_Lat(double Bd, double Ld, double H);
double SK42_WGS84_Long(double Bd, double Ld, double H);

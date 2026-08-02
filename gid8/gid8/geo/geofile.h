#pragma once

#include <QSqlDatabase>
#include <geo/Klassif.h>

class QProgressDialog;
class GidWidget;

class GeoFile
{
public:
    bool open_geo(GidWidget *view, const QString & name, QSqlDatabase &db, QProgressDialog *percent);
    bool delGeoObj(const QString & tn0, int id);

    int createObj(GidWidget* view, const QString & tn, CCoordList& cl, std::map<QString, QVariant>& m_map_geo_value1);
    int createObj2(GidWidget* view, const QString & tn, CCoordList& cl, std::map<QString, QVariant>& m_map_geo_value1);

      int NMark(const QString & fn, list<int> &list_id);
      int NMark(Klassif* kl, list<int> &list_id);

      int NMark(const QString & fn);
      int NMark(Klassif* kl);
      int NMark();

      bool IsMark(const Klassif *kls, int n);

      void clearMark();


};



struct KlGeo
{
    Klassif *kls;
    CGeoObject *gl;
};

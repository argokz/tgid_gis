#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QWidget>
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

    // Поиск по слою геобазы (SQL ILIKE по id и текстовым колонкам).
    // Возвращает true и заполняет outTn/outId при удачном совпадении.
    bool find(QWidget *parent, QString *outTn = nullptr, int *outId = nullptr);
    bool findNext(QWidget *parent, QString *outTn = nullptr, int *outId = nullptr);
    bool isFindNext() const { return !m_findIds.isEmpty() && m_findIndex >= 0; }

private:
    bool runFindQuery(Klassif *kls, const QString &pattern, QList<int> *ids);
    bool emitFindHit(QWidget *parent, QString *outTn, int *outId);

    QString m_findTn;
    QString m_findPattern;
    QList<int> m_findIds;
    int m_findIndex = -1;
};



struct KlGeo
{
    Klassif *kls;
    CGeoObject *gl;
};

#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"
#include "Pjezo.h"
#include "win.h"
#include "mmenu.h"
#include "NagrTree.h"


// Нагрузка для Алматы

static int nom_neiz;

void getNagr(CNode2 *n1, list<int> & list_n, set<int> & set_z, Klassif *klsZ, map<CNode2*, int>& map_z)
{
    if (n1->node.typ == TIP_PR || n1->node.typ == TIP_PO) {
        list_n.push_back(n1->node.nomP);

        int nom = nom_neiz++;

        if (klsZ) {
            int nn = m_geofile->findXY3(n1->node.coord, 5, 3, klsZ->nazv);
            if (nn) nom = nn;
        }

        map_z[n1] = nom;
        set_z.insert(nom);
    }

    for (CLINE2 *l = n1->lines; l; l = next(l)) {
        CLine2 *ll = bline(l);
        double naprP = ll->line.pod.q*napr(l);
        if (ll->line.nomP != -1 && naprP > 0) {
            CNode2 *o = other(l);
            getNagr(o, list_n, set_z, klsZ, map_z);
        }
    }
}




void CGidrView::OnPopupNagr()
{
  CNode2 *node = m_menu_node;
  if (!node) return;

  int nom_neiz0 = 1000000;
  nom_neiz = nom_neiz0;
  map<CNode2*, int> map_z;

  Klassif *klsZ = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));

  CString str;

  CNagrTree dial(this, node);

  int ret = dial.DoModal();


#if 0

  if (ret == 2005) {
    set<int> set_z;
    list<int> list_n;
    getNagr(node, list_n, set_z, klsZ, map_z);

    CString tn = _TR("Нагрузки");
    CString sql, mdb = getVyd();
    CDbFile dbf;
    
    double ot_d = 0., gvs_d = 0., vent_d = 0., par_d = 0.;
    double ot_r = 0., gvs_r = 0., vent_r = 0., par_r = 0.;

    if (dbf.openMDB(mdb)) {
      try {
        sql.Format("DROP TABLE [%s]", tn);
        dbf.db->Execute(sql, dbSQLPassThrough );
      }
      catch (CDaoException *e) {
        e->Delete();
      }

      sql.Format("CREATE TABLE [%s] "
        "(nom int,[Район эксплуатации] char(50), [Участок эксплуатации] char(50), [Узел присоединения] char(50), [Микрорайон] char(255), [Улица] char(255), [Дом] char(50),"
        " [Договорное Отопление] double, [Договорная ГВС] double, [Договорная Вентиляция] double, [Договорная Суммарная] double, "
        " [Расчетное Отопление] double, [Расчетная ГВС] double, [Расчетная Вентиляция] double, [Расчетная Суммарная] double"
        ")"
        ,tn);
      dbf.db->Execute(sql, dbSQLPassThrough );

      CDbFile *db = klsZ->m_db;

      if (db->openTable(klsZ->nazv.c_str())) {
        if (dbf.openTable(tn)) {
          set<int>::const_iterator it = set_z.begin();
          for ( ; it != set_z.end(); it++ ) {
            dbf.AddNew();

            int nn = *it;

            dbf.write(1, nn);

            double ot = 0., gvs = 0., vent = 0., par = 0.;

            map<CNode*, int>::const_iterator it2 = map_z.begin();
            for (; it2 != map_z.end(); it2++) {
              if (it2->second == *it) {
                CNode *n = it2->first;
                if (n) {
                  ot += n->Qot;
                  gvs += n->Qgvs;
                  vent += n->Qvent;
                }
              }
            }
            dbf.write(_TR("Расчетное Отопление"), ot);
            dbf.write(_TR("Расчетная ГВС"), gvs);
            dbf.write(_TR("Расчетная Вентиляция"), vent);
            dbf.write(_TR("Расчетная Суммарная"), ot+gvs+vent);

            ot_r += ot;
            gvs_r += gvs;
            vent_r += vent;

            if (db->GotoKey(nn)) {
              CString s1 = db->read(_TR("Микрорайон"));
              CString s2 = db->read(_TR("Улица"));
              CString s3 = db->read(_TR("Дом"));
              CString s4 = db->read(_TR("Узел присоединения"));

              dbf.write(_TR("Микрорайон"), s1);
              dbf.write(_TR("Улица"), s2);
              dbf.write(_TR("Дом"), s3);
              dbf.write(_TR("Узел присоединения"), s4);


        CString q, q1, q2;

        q1.Format("SELECT "
"[Нагрузка \_TR("Отопление\")],"
"[Нагрузка \_TR("Вентиляция\")],"
"[Нагрузка \_TR("ГВС\")],"
"[Нагрузка \_TR("Пар\")],"
"[Эксплуатационный район по объекту],"
"[Эксплуатационный участок по объекту]"
        
        " FROM [Организации] WHERE [Здание] = %d", nn);


        q2.Format("SELECT "
"[Нагрузка Отопление] AS [Нагрузка \_TR("Отопление\")],"
"0 AS [Нагрузка \_TR("Вентиляция\")],"
"[Нагрузка ГВС] AS [Нагрузка \_TR("ГВС\")],"
"0 AS [Нагрузка \_TR("Пар\")],"
"[Эксплуатационный район] AS [Эксплуатационный район по объекту],"
"'' AS [Эксплуатационный участок по объекту] "

          " FROM [Жилье] WHERE [Здание] = %d", nn);

        q.Format("%s\nUNION\n%s", q2, q1);

        CDbFile db2(db);

        CString er = "", eu = "";
        
        ot = 0., gvs = 0., vent = 0., par = 0.;

        if (db2.openTable(q, TRUE)) {
          if (!db2.rs->IsEOF()) {
            while ( !db2.rs->IsEOF() ) {
              double ot1 = db2.read_double(1);
              double vent1 = db2.read_double(2);
              double gvs1 = db2.read_double(3);
              double par1 = db2.read_double(4);

              ot += ot1;
              gvs += gvs1;
              vent += vent1;
              par += par1;

              CString s1 = db2.read(5);
              CString s2 = db2.read(6);
              if (s1 != "") {
                er = s1;
              }
              if (s2 != "") {
                eu = s2;
              }
              db2.rs->MoveNext();
            }
          }
          db2.closeTable();
        }

        ot /= 1e6;
        gvs /= 1e6;
        vent /= 1e6;

        dbf.write(_TR("Договорное Отопление"), ot);
        dbf.write(_TR("Договорная ГВС"), gvs);
        dbf.write(_TR("Договорная Вентиляция"), vent);
        dbf.write(_TR("Договорная Суммарная"), ot+gvs+vent);

        ot_d += ot;
        gvs_d += gvs;
        vent_d += vent;

        dbf.write(_TR("Район эксплуатации"), er);
        dbf.write(_TR("Участок эксплуатации"), eu);

            }

            dbf.Update();
          }


        dbf.AddNew();

        dbf.write(_TR("Микрорайон"), _TR("Итого"));

        dbf.write(_TR("Расчетное Отопление"), ot_r);
        dbf.write(_TR("Расчетная ГВС"), gvs_r);
        dbf.write(_TR("Расчетная Вентиляция"), vent_r);
        dbf.write(_TR("Расчетная Суммарная"), ot_r+gvs_r+vent_r);

        dbf.write(_TR("Договорное Отопление"), ot_d);
        dbf.write(_TR("Договорная ГВС"), gvs_d);
        dbf.write(_TR("Договорная Вентиляция"), vent_d);
        dbf.write(_TR("Договорная Суммарная"), ot_d+gvs_d+vent_d);
        dbf.Update();




          dbf.closeTable();
        }
        db->closeTable();
      }

      dbf.closeMDB();



      CString q;
      q.Format("SELECT * FROM [%s]", tn);
      RunTable(mdb, tn, q, NULL, -1, 0, FALSE, FALSE, TRUE, FALSE);
    }
  }

#endif

}

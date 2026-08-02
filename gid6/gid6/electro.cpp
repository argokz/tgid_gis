#include "stdafx.h"
#include "gid6.h"

#include "gidrView.h"

#include "electro.h"


void CGidrView::CreateElectro(const char *tn)
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(tn));
  if (kls) {
    if (addGeo(kls)) {
       CString str;
       str.Format(_TR("Начинайте создавать %s"), kls->RusName());
       AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    }
  }
}


void CGidrView::OnElectroRemont()
{
#if 0
  CString tmpName;
  tmpName.Format("%s\\index2.html", getenv("TMP"));

  CString tn = EL_LINE;

  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(tn));
  if (!kls || !kls->ins) {
    return;
  }

  FILE *f = fopen(tmpName, "w");
  if (!f) return;

  setlocale(LC_NUMERIC, "");

  print_index1(f, "");

  FILE *g = print_doc(f, "remont\\Линия электропередач.html", _TR("Линии электропередач"));

  if (g) {
    CString q;

    q.Format("SELECT [Наименование ЛЭП], [Напряжение, кВ], [Фидера], [Марка кабеля линии], [Протяженность, м] FROM [%s]", tn);

    CDbFile dbf;

    if (dbf.openMDB(kls->m_db->m_strFileName)) {
      if (dbf.openTable(q)) {

        InitTab(kls->m_db->m_strFileName, tn, false);


        if (!dbf.rs->IsEOF()) {

          dbf.rs->MoveFirst();

          while ( !dbf.rs->IsEOF() ) {
            CString s;

            fprintf(g, "<tr>", s);

            s = dbf.read(_TR("Наименование ЛЭП"));
            fprintf(g, "<td class=xl67>%s</td>", s);
            s = dbf.read(_TR("Напряжение, кВ"));
            fprintf(g, "<td class=xl67>%s</td>", s);
            s = dbf.read(_TR("Фидера"));
            fprintf(g, "<td class=xl67>%s</td>", s);

            s = dbf.read(_TR("Марка кабеля линии"));
            s = getReal(tn, _TR("Марка кабеля линии"), s);
            fprintf(g, "<td class=xl67>%s</td>", s);
            
            s = dbf.read(_TR("Протяженность, м"));
            fprintf(g, "<td class=xl67>%s</td>", s);
            fprintf(g, "</tr>", s);

            dbf.rs->MoveNext();
          }
        }
        dbf.closeTable();
      }
      dbf.closeMDB();
    }


    print_doc_end(g);
  }

  print_index2(f);
  fclose(f);
  setlocale(LC_NUMERIC, "eng");

  tmpName.Format("%s\\index2.html", getenv("TMP"));

  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), "open", "excel", "\""+tmpName+"\"", NULL, SW_SHOWMAXIMIZED);
#endif
}

void CGidrView::OnElectroIst()
{
  CreateElectro(EL_IST);
}

void CGidrView::OnElectroPriem()
{
  CreateElectro(EL_PRIEM);
}
void CGidrView::OnElectroLine()
{
  CreateElectro(EL_LINE);
}
void CGidrView::OnElectroMufta()
{
  CreateElectro(_TR("Муфта"));
}
void CGidrView::OnElectroOpora()
{
  CreateElectro(_TR("Опора ЭС"));
}
void CGidrView::OnElectroGil()
{
  CreateElectro(_TR("Гильза ЭС"));
}
void CGidrView::OnElectroKanal()
{
  CreateElectro(_TR("Кабельный канал ЭС"));
}

void CGidrView::OnUpdateElectro(CCmdUI* pCmdUI)
{
  bool isElectro = false;
  
  if (IsMain() && m_isGeobaza && m_cxema->isGeobaza() && m_geofile->isOpen()) {
    Klassif *kls = m_geofile->m_kl_list->findKlN(EL_LINE);
    if (kls) {
      isElectro = true;
    }
  }
  pCmdUI->Enable(isElectro);
}


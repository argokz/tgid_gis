// gidrView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
//#include "DbTable.h"
#include "MainFrm.h"
#include "gidrView.h"
#include "win.h"
#include "mmenu.h"
//#include "TeplopotrBox.h"
#include "InputDia.h"

#include "ado.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 0

void Run(CWnd *win, const char *cmd, const char *prtn);

CString GetFName(const char *path);


bool addFile(CDbFile &dbf1, const char *fn)
{
  CDaoTableDefInfo tabInfo;
  CString str;
  COleVariant var;
  COleVariant var2;
  CDaoFieldInfo fieldInfo, fieldInfo1;
  CDbFile dbf2;
  int j, nind;

  CDaoDatabase *db = dbf1.db;

  if (!dbf2.openMDB(fn)) return false;

  int nTableCount = db->GetTableDefCount();

  for (j=0; j < nTableCount; j++)
  {
    db->GetTableDefInfo(j, tabInfo);
    if (tabInfo.m_lAttributes & dbSystemObject) continue;

    if (tabInfo.m_strName.Find("_OUT") != -1) continue;
    if (!strcmp(tabInfo.m_strName, "TG_Развернутый температурный график")) continue;
    if (!strcmp(tabInfo.m_strName, "TK_График температур качественного регулирования")) continue;
    if (!strcmp(tabInfo.m_strName, "TN_График нормативных температур")) continue;
    if (!strcmp(tabInfo.m_strName, _TR("Месячный график работы источника"))) continue;
    if (!strcmp(tabInfo.m_strName, _TR("Температуры воздуха и грунта"))) continue;
    if (!strcmp(tabInfo.m_strName, _TR("Район эксплуатации"))) continue;

    if (!strcmp(tabInfo.m_strName, _TR("Система теплоснабжения"))) continue;
    if (!strcmp(tabInfo.m_strName, _TR("Направления"))) continue;
    if (!strcmp(tabInfo.m_strName, _TR("Организация"))) continue;
    if (!strcmp(tabInfo.m_strName, _TR("Объекты с измеряемыми параметрами"))) continue;



//    if (!strcmp(tabInfo.m_strName, )) continue;

    {
      CDaoTableDef td(dbf1.db);
      td.Open(tabInfo.m_strName);
      try {
        nind  = td.GetIndexCount();
        for (int ii = nind-1; ii >= 0; ii--) {
          CDaoIndexInfo indexinfo;
          td.GetIndexInfo( ii, indexinfo);
          td.DeleteIndex(ii);
        }
      }
      catch (CDaoException *e) {
        e->Delete();
      }
      td.Close();
    }

    CDaoIndexInfo indexinfo;
    CDaoFieldInfo fi;

    if (nind) {
      CDaoTableDef td(dbf1.db);
      td.Open(tabInfo.m_strName);
      td.GetFieldInfo(0, fieldInfo);

      try {
        indexinfo.m_strName = "PrimaryKey";
        indexinfo.m_pFieldInfos = new CDaoIndexFieldInfo[1];
        indexinfo.m_pFieldInfos[0].m_strName = fieldInfo.m_strName;
        indexinfo.m_pFieldInfos[0].m_bDescending = FALSE;
        indexinfo.m_nFields = 1;
        indexinfo.m_bPrimary = TRUE;
        indexinfo.m_bUnique = TRUE;
        indexinfo.m_bForeign = TRUE;

        td.CreateIndex(indexinfo);
      }
      catch (CDaoException *e) {
        AfxMessageBox(_TR("Ошибка 3 :\n")+tabInfo.m_strName+ "\n"+e->m_pErrorInfo->m_strDescription, MB_OK|MB_ICONINFORMATION);
        e->Delete();
      }
      td.Close();
    }

    if (dbf1.openTable(tabInfo.m_strName)) {

      dbf1.rs->GetFieldInfo(0, fieldInfo);

      if (dbf2.openTable(tabInfo.m_strName)) {
        dbf2.rs->GetFieldInfo(0, fieldInfo1);
        int i0 = !strcmp(fieldInfo.m_strName, "nomer") ? 1 : 0;
        if (i0 == 0) i0 = !strcmp(fieldInfo.m_strName, "nomer_building") ? 1 : 0;

        if (!dbf2.rs->IsEOF()) {
          dbf2.rs->MoveFirst();

          while ( !dbf2.rs->IsEOF() ) {
            dbf1.AddNew();
            dbf2.rs->GetFieldValue(0, var2);

            for ( int i = i0; i < dbf2.NFlds(); i++ ) {
              try {
                dbf2.rs->GetFieldInfo(i, fieldInfo);
                dbf2.rs->GetFieldValue(i, var);
                dbf1.rs->SetFieldValue(fieldInfo.m_strName, var);
              }
              catch (CDaoException *e) {
                AfxMessageBox(_TR("Ошибка 1 :\n")+tabInfo.m_strName+ "\n"+e->m_pErrorInfo->m_strDescription, MB_OK|MB_ICONINFORMATION);
                e->Delete();
              }
            }
            try {
              dbf1.Update();
            }
            catch (CDaoException *e) {
              AfxMessageBox(_TR("Ошибка 2 :\n")+tabInfo.m_strName+ "\n"+e->m_pErrorInfo->m_strDescription, MB_OK|MB_ICONINFORMATION);
              e->Delete();
            }
            dbf2.rs->MoveNext();
          }
        }

        dbf2.closeTable();
      }
      dbf1.closeTable();
    }
  }
  
  return true;
}

void DelTable(CDaoDatabase *db, const char *tn)
{
  try {
    CString str;
    str.Format("DROP TABLE [%s]", tn);

    db->Execute(str, dbSeeChanges);
  }
  catch (CDaoException *e) {
//    AfxMessageBox("Error!", MB_OK|MB_ICONINFORMATION);
    e->Delete();
  }
}


void CGidrView::OnTeplopoteri()
{
//  CWaitCursor wait;
//  m_cxema->ExcelPoteri();

  CMMenu menu1(AfxGetMainWnd(), IDD_MENU6, _TR("Выберите сезон"));
  menu1.m_Index = 0;
  menu1.lst.Add(_TR("Отопительный"));
  menu1.lst.Add(_TR("Летний"));

  if (menu1.DoModal() == IDOK) {
//    return addNode(TIP_US, CNodeName(m_kod, new_name, menu.m_Index == 0 ? CPOD : COBR), pt);
  }
  if (menu1.m_Index == -1) {
    AfxMessageBox(_TR("Выберите сезон"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  Cgid6Doc * pDoc = (Cgid6Doc*) GetDocument();

  CString tmp_n;
  tmp_n.Format("%s\\fileforteplopoter.tmp", getenv("TMP"));


  ::CopyFile(pDoc->m_strMDB_real, tmp_n, FALSE);


  CDbFile dbf;

  if (dbf.openMDB(tmp_n)) {

  //  dbf = m_cxema->dbf;

    CMMenu menu(AfxGetMainWnd(), IDD_MENU6, _TR("Выберите источник"));
    menu.m_Index = 0;

    if (dbf.openTable("SELECT [kod_ist] FROM [Источник тепла]")) {
      if (!dbf.rs->IsEOF()) {
        dbf.rs->MoveFirst();

        while ( !dbf.rs->IsEOF() ) {
          char str[256];
          dbf.read(1, str);
          menu.lst.Add(str);
          dbf.rs->MoveNext();
        }
      }
      dbf.closeTable();
    }

    if (menu.lst.GetSize() == 0) {
      AfxMessageBox(_TR("Отсутствуют источники"), MB_OK|MB_ICONINFORMATION);
      return;
    }


    if (menu.DoModal() == IDOK) {
      if (menu.m_Index == -1) {
        AfxMessageBox(_TR("Выберите источник"), MB_OK|MB_ICONINFORMATION);
        return;
      }

      DelTable(dbf.db, "POT_Материальная характеристика");
      DelTable(dbf.db, "POT_Месячные потери через изоляцию");
      DelTable(dbf.db, "POT_Нагрузки");
      DelTable(dbf.db, "POT_Нормируемые месячные тепловые потери с утечкой сетевой воды");
      DelTable(dbf.db, "POT_Нормы потерь надземными трубопроводами");
      DelTable(dbf.db, "POT_Нормы потерь подземными трубопроводами");
      DelTable(dbf.db, "POT_ПСВ с нормативной утечкой");
      DelTable(dbf.db, "POT_Среднегодовые потери через изоляцию");
      DelTable(dbf.db, "POT_Температуры");
      DelTable(dbf.db, "POT_ЧАСОВЫЕ СРЕДНЕМЕСЯЧНЫЕ ТЕПЛОВЫЕ ПОТЕРИ");
      DelTable(dbf.db, "TEKPAR_OUT");
      DelTable(dbf.db, "TEMP_IST_OUT");
      DelTable(dbf.db, "TEMP_PROD_OUT");

      Cgid6Doc * pDoc = (Cgid6Doc*) GetDocument();
      CString fn = pDoc->GetPathName();
      if ( fn == "" ) fn = "Gidr1.mdb";

      CString str, par = "", qq;

      str.Format("\"%ssety4\" \"%s\" -f \"%s\" -poteri %s -sezon %d",
          argpath(), tmp_n, fn, menu.m_strText, menu1.m_Index);

//      if (m_parent_id != 0) m_nn = m_parent_kam->node_name;

      m_CurLine = NULL;
      m_CurNode = NULL;

      m_regim_ras = 256;  
      m_istName = menu.m_strText;
    
//      m_cxema->create_vyd_table(&dbf);

      Run(this, str, GetFName(fn)+".pro");
    }
    dbf.closeMDB();
  }
//  DeleteFile(tmp_n);
}

#include "FaktDial.h"

#endif




void CGidrView::OnTeplopoteriF()
{
#if 0
  CFaktDial dial(this);

  CDbFile *m_dbf;

  m_dbf = m_cxema->m_cur_file->m_dbf;

  if (m_dbf->openTable("SELECT [kod_ist] FROM [Источник тепла]")) {
    if (!m_dbf->rs->IsEOF()) {
      m_dbf->rs->MoveFirst();

      while ( !m_dbf->rs->IsEOF() ) {
        char str[256];
        m_dbf->read(1, str);
        dial.list.Add(str);
        m_dbf->rs->MoveNext();
      }
    }
    m_dbf->closeTable();
    m_regim_ras = 257;  

    CString str, par = "", qq;

    Cgid6Doc * pDoc = (Cgid6Doc*) GetDocument();
    CString fn = pDoc->GetPathName();
    if ( fn == "" ) fn = "Gidr1.mdb";

    int ret = dial.DoModal();

    if (ret == IDOK) {

      if (dial.m_sez_mon == 0) {
        str.Format("\"%ssety4\" \"%s\" -f \"%s\" -poteri %s -fakt -sezon %d",
            argpath(), pDoc->m_lpszTmpName, fn, _TR("ТЭЦ1"), dial.m_otop ? 1 : 0);
      }
      else {
        str.Format("\"%ssety4\" \"%s\" -f \"%s\" -poteri %s -fakt -month %d",
            argpath(), pDoc->m_lpszTmpName, fn, _TR("ТЭЦ1"), dial.m_month);
      }

      DelTable(m_dbf->db, "POT_Материальная характеристика");
      DelTable(m_dbf->db, "POT_Месячные потери через изоляцию");
      DelTable(m_dbf->db, "POT_Нагрузки");
      DelTable(m_dbf->db, "POT_Нормируемые месячные тепловые потери с утечкой сетевой воды");
      DelTable(m_dbf->db, "POT_Нормы потерь надземными трубопроводами");
      DelTable(m_dbf->db, "POT_Нормы потерь подземными трубопроводами");
      DelTable(m_dbf->db, "POT_ПСВ с нормативной утечкой");
      DelTable(m_dbf->db, "POT_Среднегодовые потери через изоляцию");
      DelTable(m_dbf->db, "POT_Температуры");
      DelTable(m_dbf->db, "POT_ЧАСОВЫЕ СРЕДНЕМЕСЯЧНЫЕ ТЕПЛОВЫЕ ПОТЕРИ");
      DelTable(m_dbf->db, "TEKPAR_OUT");
      DelTable(m_dbf->db, "TEMP_IST_OUT");
      DelTable(m_dbf->db, "TEMP_PROD_OUT");

      Run(this, str, GetFName(fn)+".pro");
    }
  }
#endif
}

//#include "UnvisualTree2.h"
//#include "TeplopoteriD.h"
//#include "TeplopoteriDFact.h"
#include "CTeplopoteriGeneral.h"


static map<int, CString> map_ist;

CString getExcelQ(const char *path, const char *fn);

//int py(const char *path, const char *modul, const char *fun, const char *arg1);
int py(const char *path, const char *modul, const char *fun, const char *arg1 = NULL, const char *arg2 = NULL, const char *arg3 = NULL, const char *arg4 = NULL, const char *arg5 = NULL, const char *arg6 = NULL, const char *arg7 = NULL, const char *arg8 = NULL );


#include "CTeploPotMain.h"
#include "json.h"

void teplopoteri(CWnd *wnd)
{
}

void mk_config_json(Cgid6Doc * pDoc)
{
    CString path_temp = getenv("TMP");
    FILE *f = fopen(path_temp + "\\config.json", "w");

    if (f) {
        JSON json;
        json.add("{");

        json.add("\"mssql\": {");
        json.add("host", pDoc->m_IP);
        json.add("user", pDoc->m_user);
        json.add("password", pDoc->m_password);
        json.add("port", pDoc->m_port);
        json.add("db", pDoc->m_bd_gid);
        json.add("ps", pDoc->m_bd_ps);
        //        json.add("geo", pDoc->m_bd_geo);
        json.add("}");
        json.add("}");
        fputs(json.str(), f);
        fclose(f);
    }
}


void CGidrView::OnUpdateTeplopoteri(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
//    pCmdUI->Enable(getGidAdoName() == "AlmatyGID" || getGidAdoName() == "SemeyGID");
//    pCmdUI->Enable(FALSE);
}

void CGidrView::OnUpdateTU(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(getGidAdoName().Find("AlmatyGID") == 0 || getGidAdoName() == "SemeyGID");
}


void CGidrView::OnUpdateIznos(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(getGidAdoName().Find("Astana") != -1);
}



void CGidrView::OnTeplopoteri()
{
    CAdoFile *ado = m_cxema->m_ado;
    CGraph2 *graph = m_cxema->m_graph;
    
    CTeploPotMain dlg(this, m_cxema->m_city);

    dlg.m_check_fragment = graph->isMark();

    if (dlg.DoModal() == IDOK) {
        CString path = argpath()+"\\main\\";

        Cgid6Doc * pDoc = (Cgid6Doc*)GetDocument();

        mk_config_json(pDoc);


        bool create_vyd_line_table0(CAdoFile *ado, CGraph2 *graph, CString tn);
        bool create_vyd_node_table0(CAdoFile *ado, CGraph2 *graph, CString tn);

        if (dlg.m_check_fragment) {
            graph->save_pjezo();

            //        if (!graph->isMark()) graph->vyd_all();

            create_vyd_node_table0(ado, graph, "TEMP_NODE");
            create_vyd_line_table0(ado, graph, "TEMP_LINE");
            graph->restore_pjezo();
        }
        CString yesno = dlg.m_check_fragment ? "yes" : "no";
        CString city = m_cxema->m_city;

        if (city == "") city = _TR("Алматы");


//        py(path, "main", "main", dlg.m_norm_fact == 0 ? "norm" : "fact", dlg.m_s1, dlg.m_s2, city, yesno);

        CString str;

        path = argpath() + "poteriNew";
        str.Format("tp_main.py %s %s %s %s %s %s", dlg.m_norm_fact == 0 ? "norm" : "fact", dlg.m_s1, dlg.m_s2, city, yesno, m_cxema->m_par);

        log1(str);

//        CInputDialog inD(AfxGetMainWnd( ), _T(""), "", str, IDD_INPUTDIALOG2);
//        if (inD.DoModal() != IDOK) return;


        HINSTANCE hRun = ShellExecute (AfxGetMainWnd()->GetSafeHwnd (), "open", pythonw_exe(), str, path, SW_SHOW);

    }
/*
    CAdoFile *ado = getAdo(getGidAdoName());
    CTeplopoteriGeneral dlg(this, false);
    dlg.DoModal();
*/
}

void CGidrView::OnTeplopoteriFakt()
{
    teplopoteri(this);
  
//  py("D:\\vc_gid_mysql2\\poteriNew", "main", "main", "fact");


/*
  CAdoFile *ado = getAdo(getGidAdoName());

//  CFileDialog fd(TRUE, "xls", NULL, OFN_HIDEREADONLY,
//    _TR("Схема (*.xls)|*.xls|"), this);

//  if (fd.DoModal() == IDOK) {
      CTeplopoteriGeneral dlg(this, true);
      dlg.DoModal();
//  }
*/
}

#include "Ultimate Grid/TPView.h"

void CGidrView::viewTP(const char *db, const char *q, const char *tn, VIRT_DATA typ)
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

//    CAdoFile *ado = getAdo(getGidAdoName());
    CAdoFile *ado = getAdo(db);
    if (!ado) return;

    pDoc->m_view_typ = typ;

    pDoc->m_view_db = db;
    pDoc->m_view_tn = tn;
    pDoc->m_view_caption = tn;
    pDoc->m_view_sql = q;
    pDoc->m_view_all = false;
    pDoc->m_view_connect = ado->getConnect();

/*
    pDoc->m_view_is_edit = true;
    pDoc->m_view_is_add = true;
    pDoc->m_view_is_del = true;
*/

    pDoc->m_view_map_ist = map_ist;

    CFrameWnd     *pWndSecondChild;
    CMultiDocTemplate *pDocTemplate;

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL)
    {
        CTPView *pView = (CTPView*)pDoc->GetNextView(pos);
        if (pView->IsKindOf(RUNTIME_CLASS(CTPView))) {

            CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
            mf->setActiveView(pView);


//            CMDIFrameWnd *pFrame = (CMDIFrameWnd*)theApp->m_pMainWnd;
//            pFrame->MDIActivate(pView->GetParent());
//            pView->init(pDoc->m_data);
            return;
        }
    }

    pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_TABLE_TP);
    if (pDocTemplate != NULL)
    {
        pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
        ASSERT(pWndSecondChild != NULL);
        pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
    }
}

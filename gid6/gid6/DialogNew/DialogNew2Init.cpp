#include "stdafx.h"
#include "gid6.h"
#include "DialogNew2.h"
#include "FormDialog2.h"

#include "gidrDoc.h"
#include "gidrView.h"
#include "TableDialog.h"


#include "dbi.h"
#include "win.h"

extern CGeoFile *m_geofile;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "maptabs.h"

CString getVal(map<CString, COleVariant> &map_val, const char *fn)
{
  CString val = "";
  map<CString, COleVariant>::const_iterator it;

  it = map_val.find(CString(fn)); 
  if (it != map_val.end()) {
    val = CCrack::strVARIANT(it->second);
  }
  
  return val;
}

static map<CString, CString> map_help;
static map<CString, int> map_help_yes;

bool GotoKey(CAdoFile *dbf, int n1) 
{
  if (n1 > 0) {
     return dbf->GotoKey(n1);
  }
  
  if (n1 == -1) {
    dbf->rs->MoveFirst();
  }
  return TRUE;
}

CString read_db(const char *mdb, const char *tn, const char *q, int nom, const char *fn)
{
  CString text = "";

  CAdoFile dbf;
  if (dbf.openMDB(mdb)) {
    if (dbf.openTable(q)) {
      if (GotoKey(&dbf, nom)) {
        text = dbf.read(fn);
      }
    }  
    dbf.closeMDB();
  }
  return text;
}



BOOL init_help(const char *help, const char *tn)
{
  BOOL yes = FALSE;
  CAdoFile dbf_help;

  if (dbf_help.openMDB(getHelpOsnovn())) {
    CString q;
    q.Format("SELECT English, Help, HelpShiftF1 FROM [Help_%s]", tn);

    if (dbf_help.openTable(q, TRUE)) {
      yes = TRUE;
      if (!dbf_help.rs->IsEOF()) {

//        dbf_help.rs->MoveFirst();

        while ( !dbf_help.rs->IsEOF() ) {
          CString s1 = dbf_help.read(1);

          s1.MakeLower();

          CString s2 = dbf_help.read(2);
          CString s3 = dbf_help.read(3);
          map_help[s1] = s2;

          dbf_help.rs->MoveNext();
        }
      }
      dbf_help.closeTable();
    }
    dbf_help.closeMDB();
  }
  return yes;
}


void init_help(const char *tn)
{
  if (init_help(getHelpOsnovn(), tn)) return;
//  if (init_help(getHelpOsnovn(), tn)) return;
}


CString getHelp(const char *fn)
{
  map<CString, CString>::const_iterator it;

  CString s1 = CString(fn);
  s1.MakeLower();

  it = map_help.find(s1);
  if (it != map_help.end()) {
    return it->second;
  }
  return CString(fn);
}

CString getMdb(CString mdb);


CString getViewValue(const char *val, const char *mdb1, const char *table, const char *id, const char *fn)
{
  CString mdb = getMdb(mdb1);

  CAdoFile dbf;

  try {

    if (dbf.openMDB(mdb)) {
      if (dbf.openTable(table)) {

        CString str;

        str.Format("%s=%s", id, val);

        int ret = dbf.rs->Find(AFX_DAO_FIRST, str);
        if ( ret ) {
          CString s = dbf.read(fn);
          return s;
        }
      }
    }
  }
  catch (...) {
  }
  return val;
}


BOOL CDialogNew2::viewTable(const char *tn, const char *title, bool isEdit)
{
  CControl *cnt, *cnt2;

  CFormDialog2 *view = (CFormDialog2 *) m_pFrames[m_n_tab]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
  if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
  {
    m_tab.InsertItem(m_n_tab, title);
   
//        m_geo_nom = atoi(dbf->read("nomer_geo"));

        int row = 0;

        for (int i = 0; i < db_row.nFlds; i++) {

          FIELD fld;

          CString tn = db_row.getTable(i);
          CString fn = db_row.getName(i);

          if (fn == "X_TEXT") continue;
          if (fn == "Y_TEXT") continue;
          if (fn == "COEF_TEXT") continue;
          if (fn == "A_TEXT") continue;
          if (fn == "SHAPE") continue;
          if (fn == "SHAPE_AREA") continue;
          if (fn == "SHAPE_LENGTH") continue;
          if (i == 0 && (fn == "ID" || fn == "nomer")) continue;

          if (!db_row.getField(tn, fn, fld, 0)) continue;

          cnt = view->addControl(TIP_PODPIS, getHelp(fn), row, 0, 1);
  //        cnt = view->addControl(TIP_COMBO, str, typ, len, false, i);

          int is_comb = 0;

          MFIELDS map1;
          if (getMap(tn, fn, map1)) {
            cnt = view->addControl(TIP_COMBO, fn, row, 1, 1);
            if (cnt) {
              vector <CString> tok2;
              list<pair<CString, CString> >::const_iterator it2;
         
              tok2.push_back("1");
              tok2.push_back("2");
         
              for (it2 = map1.list2.begin(); it2 != map1.list2.end(); it2++) {
                tok2.push_back(it2->first);
                tok2.push_back(it2->second);
              }
         
              cnt->tok = tok2;
              cnt->m_value = fld.val;
              cnt->m_new_value = cnt->m_value;
              cnt->m_tn = tn;
              cnt->m_num = 1;
            }
            is_comb = 1;
          }
          
          if (!is_comb) {
            if (fld.typ == dbBoolean) {
              cnt = view->addControl(TIP_CHECKBOX, fn, row, 1, 1);
              if (cnt) {
                cnt->m_value = fld.val;
                cnt->m_new_value = cnt->m_value;
                cnt->m_tn = tn;
                cnt->m_num = 1;
              }
            }
            else {
              if (0 && fld.typ == dbDate) {
                cnt = view->addControl(TIP_DATE, fn, row, 1, 1);
                if (cnt) {
                  cnt->m_value = fld.val;
                  cnt->m_new_value = cnt->m_value;
                  cnt->m_typ = fld.typ;
                  cnt->m_len = fld.size;
                  cnt->m_tn = tn;
                  cnt->m_num = 1;
                  cnt->m_is_edit = isEdit;
                }
              }
              else  {
                cnt = view->addControl(TIP_EDIT, fn, row, 1, 1);
                if (cnt) {
                  cnt->m_value = fld.val;
                  cnt->m_new_value = cnt->m_value;
                  cnt->m_typ = fld.typ;
                  cnt->m_len = fld.size;
                  cnt->m_tn = tn;
                  cnt->m_num = 1;
                  cnt->m_is_edit = isEdit;

                  if (fld.typ == dbDate) {
                    cnt2 = view->addControl(TIP_BUTTON, fn, row, 1, 1);
                    if (cnt2) {
                      cnt2->m_main = cnt;
                    }
                  }
                }
              }
            }
          }
  //        view->addControl(TIP_BUTTON, "..", -1, -1, i);
          view->nextRow();
          row ++;
        }
//        dbf->closeTable();
    //  m_pFrames[m_n_tab]->InitialUpdateFrame( m_pDocument, TRUE );
        m_pFrames[m_n_tab]->InitialUpdateFrame( NULL, TRUE );
        m_n_tab++;
      
        return TRUE;
      }
  return FALSE;
}


BOOL CDialogNew2::viewTableOut(CAdoFile *dbf, const char *tn, int n1, int n2, const char *title)
{
  CFormDialog2 *view = (CFormDialog2 *) m_pFrames[m_n_tab]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
  if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
  {
    m_tab.InsertItem(m_n_tab, title);
    
    CString q;

//    n1 = 4138;

//    q.Format("SELECT * FROM [%s] WHERE nomer_m=%d", tn, n1);
    q.Format("SELECT * FROM [%s] WHERE nomer=%d OR nomer=%d", tn, n1, n2);

//    if (dbf->openTableQ(tn)) {
    if (dbf->openTable(q, TRUE)) {

//      dbf->rs->MoveFirst();

      map<CString, COleVariant> map_val1, map_val2;

//      dbf->rs->MoveFirst();
      if (!dbf->rs->IsEOF()) {
        dbf->read(map_val2);
        map_val1 = map_val2;

        dbf->rs->MoveNext();
        if (!dbf->rs->IsEOF()) {
          dbf->read(map_val1);
        }
      }

      
      int row = 0;
      
      for (int i = 1; i < dbf->NFlds(); i++) {
        CDaoFieldInfo fieldInfo;
        dbf->rs->GetFieldInfo(i, fieldInfo);
        CString str = fieldInfo.m_strName;

        if ( !strcmp(str, "kod_p") ) continue;
        if ( !strcmp(str, "uzel_p") ) continue;
        if ( !strcmp(str, "pr_p") ) continue;
        if ( !strcmp(str, "nomer_m") ) continue;
        if ( !strcmp(str, "pr_videlen") ) continue;

//        CString val = dbf->read(i+1);

        CControl *cnt;
        CString val1, val2;
        
        val1 = getVal(map_val1, str);
        val2 = getVal(map_val2, str);

//        val1 = "1";
//        val2 = "2";

        cnt = view->addControl(TIP_PODPIS, getHelp(str), row, 0, 1);
        cnt = view->addControl(TIP_EDIT, str, row, 1, 1);
        if (cnt) {
          cnt->m_value = val1;
          cnt->m_new_value = cnt->m_value;
          cnt->m_typ = fieldInfo.m_nType;
          cnt->m_len = fieldInfo.m_lSize;
          cnt->m_is_edit = FALSE;
          cnt->m_tn = tn;
          cnt->m_num = n1;
          cnt->m_base = BASE_OUT;
          cnt->m_rjad = 0;

          if (n2 != -1 && val2 != val1) cnt->m_width = 0.5;
        }

        if (n2 != -1 && val2 != val1) {
          cnt = view->addControl(TIP_EDIT, str, row, 1, 1);
          if (cnt) {
            cnt->m_value = val2;
            cnt->m_new_value = cnt->m_value;
            cnt->m_typ = fieldInfo.m_nType;
            cnt->m_len = fieldInfo.m_lSize;
            cnt->m_is_edit = FALSE;
            cnt->m_tn = tn;
            cnt->m_num = n2;
            cnt->m_base = BASE_OUT;
            cnt->m_width = 0.5;
            cnt->m_rjad = 1;
          }
        }
        view->nextRow();
        row ++;
      }
      dbf->closeTable();
      m_pFrames[m_n_tab]->InitialUpdateFrame( NULL, TRUE );
      m_n_tab++;
      return TRUE;
    }
  }
  return FALSE;
}

bool CDialogNew2::setTok(const char *t2, const char *t3, const char *t4, const char *t5, vector <CString> &tok2)
{
  
  CAdoFile *dbf = NULL;

  tok2.clear();
  tok2.push_back("1");
  tok2.push_back("2");
  
  if (!strcmp(t2, "MAIN")) {
    dbf = new CAdoFile(m_dbf);    
  }
  else if (!strcmp(t2, "СтандОбор")) {
    dbf = new CAdoFile();
    dbf->openMDB(getStandObor());
  }
  else {
    dbf = new CAdoFile();

    CString t;
    t.Format("%s%s.mdb", argpath(), t2);
    dbf->openMDB(t);
  }

  if (dbf) {
    CString q;

    q.Format("SELECT DISTINCT [%s], [%s] FROM [%s]", t4, t5, t3);

    if (dbf->openTable(q, TRUE)) {
      if (!dbf->rs->IsEOF()) {

//        dbf->rs->MoveFirst();

        while ( !dbf->rs->IsEOF() ) {
          CString v1 = dbf->read(1);
          CString v2 = dbf->read(2);

          tok2.push_back(v1);
          tok2.push_back(v2);

          dbf->rs->MoveNext();
        }
      }
      dbf->closeTable();
    }
    delete dbf;
  }

  return false;
}

BOOL CDialogNew2::isChange() 
{
  if (m_is_change) return TRUE;

  CString str = "";
  UpdateData(TRUE);

  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      map <CControl, CString>::const_iterator it1;

      for (it1 = view->map_field.begin(); it1 != view->map_field.end(); it1++) {
        return TRUE;
      }
    }
  }
  return FALSE;
}


/*
bool CDialogNew2::saveDialog() 
{
  CString str = "";
  UpdateData(TRUE);

  if (isChange()) {
    if (AfxMessageBox("Сохранить изменения?",  MB_YESNO) != IDYES ) return false;
  }

  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      map <CControl , CString>::const_iterator it1;

      for (it1 = view->map_field.begin(); it1 != view->map_field.end(); it1++) {
        CString s;

        CControl cn = it1->first;

        CString tn = cn.m_tn;

        int q1 = cn.m_rjad;
        CString q2 = cn.m_fieldName;
        CString v = it1->second;

        map<CString, CString>::iterator itf = m_map_f_t.find(q2);

        if (itf != m_map_f_t.end()) {
          q2 = itf->second;
        }

        m_map_val[tn][q1][q2] = v;
        str += s;
      }
    }
  }
  closeDialog();
  return true;
}

  */

bool CDialogNew2::saveFormDialog2(CFormDialog2 *view) 
{
  map <CControl , CString>::const_iterator it1;

  CString str = "";

  for (it1 = view->map_field.begin(); it1 != view->map_field.end(); it1++) {
    CString s;

    CControl cn = it1->first;

    CString tn = cn.m_tn;

    int ID = cn.m_rjad;
    CString q2 = cn.m_fieldName;
    CString v = it1->second;

    map<CString, CString>::iterator itf = m_map_f_t.find(q2);

    if (itf != m_map_f_t.end()) {
      q2 = itf->second;
    }

    m_map_val[tn][ID][q2] = v;
    str += s;
  }
  return true;
}


bool CDialogNew2::saveDialog() 
{
  UpdateData(TRUE);

  if (isChange()) {
    if (AfxMessageBox("Сохранить изменения?",  MB_YESNO) != IDYES ) return false;
  }

  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      saveFormDialog2(view);
    }
  }
  closeDialog();
  return true;
}

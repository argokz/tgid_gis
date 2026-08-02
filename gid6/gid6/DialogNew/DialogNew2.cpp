// DialogNew.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "DialogNew2.h"
#include "FormDialog2.h"

#include "gidrDoc.h"
#include "gidrView.h"
#include "TableDialog.h"

#include "mmenu.h"
#include "mmenu2.h"

#include "MyEdit.h"

#include "../TeploCalc/ForTeploCalc.h"

#include "dbi.h"
#include "win.h"

extern CGeoFile *m_geofile;

void read_dfm(FILE *f, list<OBJ> &l_ob);
void read_html(FILE *f, list<OBJ> &l_ob);
void initDialog();
bool CopyBuf(const char *fn);
bool IsBufDib();


#define fopenexe wsqweretr

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "maptabs.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogNew2 dialog

//void fgetstr(char *s, int n, FILE *f);

int editQ_1(CWnd *wnd, CAdoFile &dbf, const char *tn, int nom1, int readOnly, const char *cap, int idd);

bool GotoKey(CAdoFile *dbf, int n1);


bool readArr(COleVariant &var, const char *fname)
{
//  COleVariant var = fld.var;

  bool is = false;

  if (var.vt != VT_NULL) {
    COleSafeArray sa, sa1;

    sa = COleSafeArray((VARIANT) var);

    long r, r1, r2;

    DWORD dim = sa.GetDim();
    DWORD es = sa.GetElemSize();

    unsigned char *sh;

    sa.GetLBound(1, &r1);
    sa.GetUBound(1, &r2);
    sa.AccessData((LPVOID *) & sh);

    FILE *f = fopen(fname, "wb");
    if (f) {
      for (r = r1; r <= r2; r++) {
        fputc(sh[r], f);
        is = true;
      }
      fclose(f);
    }
    sa.UnaccessData();
  }
  return is;
}

/*
void readWord(ROW2 &db_row, const char *tn, const char *fn, CString & m_doc_path)
{
  TCHAR szTempFileName[MAX_PATH];  
  GetTempFileName(getenv("TMP"),  TEXT("Akt"), 0, szTempFileName); 
  m_doc_path = szTempFileName;
  m_doc_path += ".docx";

  try {
    FIELD fld;
    if (db_row.getField(tn, fn, fld, 0)) {
      readArr(fld.var, m_doc_path);
    }
  }
  catch(CMemoryException *e1) {
    e1->Delete();
  }
  catch(COleException *e2) {
    e2->Delete();
  }
  catch (...) {
  }
}
*/

void read_txt(FILE *f, vector <CString> &tabs, vector <vector <CString> > &fields);

void reads_field2(const char *field, CString &tn, CString &fn, vector <CString> &tok)
{
  char str[1024];
  char seps[]   = "\t";
  char *token;
  int n = 0;

  fn = field;

  for (int i = 0, k = 0; field[k]; i++, k++) {
    str[i] = field[k];
    if (field[k] == ' ' && field[k+1] == ' ') {
      str[i] = '\t';
      for (; field[k] == ' '; k++);
      k--;
    }
  }
  str[i] = 0;


  token = strtok( str, seps );

  while( token != NULL )
  {
    if (!strcmp(token, "@")) {
      tok.push_back(CString(" "));
    }
    else {
      tok.push_back(CString(token));
    }
    token = strtok( NULL, seps );
    n++;
  }

  if (tok.size() >= 1) {
    tn = tok[0];
  }

  if (tok.size() >= 2) {
    fn = tok[1];
  }
}

void CDialogNew2::addQ(const char *tn, const char *q, int nom)
{
  CString qq = q;

  if (!q || !q[0]) {
    qq.Format("SELECT * FROM [%s]", tn);
  }

  m_q[m_n_q] = qq;
  m_tn[m_n_q] = tn;
  m_nom[m_n_q] = nom;
  m_n_q ++;     
}


CDialogNew2::CDialogNew2(CWnd* pParent, CAdoFile *dbf, const char *name, int readOnly, const char *cap, int idd)
  : CDialog(idd, pParent)
{
  //{{AFX_DATA_INIT(CDialogNew2)
  //}}AFX_DATA_INIT

//  m_cur = -1;

  initDialog();


  m_nTimerWord = 0;

  m_pFrame = NULL;
  m_n_tab = 0;

  m_parent = pParent;

  m_dbf = dbf;
  m_name = name;

  m_font_save_name = CString("Dialog New\\")+m_name;
//  szSection1 = CString("T\\")+name;
  szSection1 = m_font_save_name;

  if (cap) {
    m_cap = cap;
  }
  else {
    m_cap = m_name;
  }

  m_nagr = 0;
  m_tu = 0;

  m_n_q = 0;

  m_readOnly = readOnly;

  m_cx0 = 0; m_cy0 = 0;

  check_data = NULL;

  ngP = -1;
  ngO = -1;
  tnOut = "";

  m_len = 0;

  m_doc_name = "";
  m_doc_path1 = "";
  m_doc_path2 = "";
  m_doc_path3 = "";
  m_doc_path4 = "";
  m_doc_path5 = "";
  m_doc_path6 = "";
  m_doc_path7 = "";

  for (int i = 0; i < 256; i++) {
    m_nom[i] = -1;
  }

  m_is_change = false;

  m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_DBTYPE));
}

void CDialogNew2::addOut(const char *tn, int nP, int nO)
{
  ngP = nP;
  ngO = nO;
  if (nP == nO) ngO = -1;

  tnOut = tn;
}

int CDialogNew2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;
  
  // Если нет необходимости в использовании документов в созданном фрейме,
  // не создавайте его, а в CCreateContext поместите NULL
//  m_pDocument = new(CDocument);

    // Инициализируем контекст представления
    CCreateContext ccc;
    ccc.m_pNewViewClass   = RUNTIME_CLASS(CFormDialog2);
//    ccc.m_pCurrentDoc     = m_pDocument;
    ccc.m_pCurrentDoc     = NULL;
    ccc.m_pNewDocTemplate = NULL;
    ccc.m_pLastView       = NULL;
    ccc.m_pCurrentFrame   = NULL;
  
  // Т.к. CFrameWnd нуждается в оконном классе, создадим его.
  // Код взят из примера в MSDN к функции “AfxRegisterWndClass”
    CString strMyClass = AfxRegisterWndClass(CS_VREDRAW |
                         CS_HREDRAW,
                         ::LoadCursor(NULL, IDC_ARROW),
                         (HBRUSH) ::GetStockObject(WHITE_BRUSH),
                         ::LoadIcon(NULL, IDI_APPLICATION));

  // Создаём фрейм, указывая в качестве родительского окна наш диалог (this)

    for (int i = 0; i < 20; i++) {
      m_pFrames[i] = new CFrameWnd;
      m_pFrames[i]->Create(strMyClass,"", WS_CHILD, CRect(0,0,1,1), this, NULL,  0, &ccc );
      m_pFrames[i]->ShowWindow(SW_HIDE);
      m_pFrames[i]->EnableWindow(FALSE);
    }

    m_pFrames[0]->ShowWindow(SW_SHOW);
    m_pFrames[0]->EnableWindow(TRUE);

    m_pFrame = m_pFrames[0];
    m_pFrame->MoveWindow(0,0,600,600);

  return 0;
}
  
void CDialogNew2::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDialogNew2)
//  ON_WM_KEYDOWN()
//  DDX_Control(pDX, IDOK, m_ok);
//  DDX_Control(pDX, IDCANCEL, m_cancel);
//  DDX_Control(pDX, IDC_TAB, m_table);
//  DDX_Control(pDX, IDC_SAVE, m_save);
//  DDX_Control(pDX, IDC_COPY_GEO, m_copy_geo);

  DDX_Control(pDX, IDC_TAB1, m_tab);
  
  
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogNew2, CDialog)
  //{{AFX_MSG_MAP(CDialogNew2)
  ON_WM_TIMER()
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_WM_SYSCOMMAND()
  ON_BN_CLICKED(IDC_FONT, OnFont)
  ON_BN_CLICKED(IDC_TAB, OnTab)
  ON_BN_CLICKED(IDC_DEL, OnDel)
  ON_BN_CLICKED(IDC_SAVE, OnSave)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_BN_CLICKED(IDC_ANNUL, OnAnnul)
  ON_BN_CLICKED(IDC_MON, OnMon)
  ON_BN_CLICKED(IDC_DEISTV, OnDeistv)
  ON_BN_CLICKED(IDC_ZAV, OnZav)
  ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
  ON_NOTIFY(TCN_SELCHANGING, IDC_TAB1, OnSelchangingTab1)
  ON_COMMAND(ID_SHIFT_F1, OnShiftF1)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogNew2 message handlers



void CDialogNew2::setDialog(int cx, int cy) 
{
  int dpi = CClientDC(NULL).GetDeviceCaps(LOGPIXELSX);

  if (::IsWindow(m_tab.m_hWnd) ) {
//    m_tab.MoveWindow(10,10,cx-100-10,40,TRUE); 
    m_tab.MoveWindow(10,10,cx-100*dpi/96-5,cy-20,TRUE); 
  }
  if (m_pFrame && ::IsWindow(m_pFrame->m_hWnd) ) {
    m_pFrame->MoveWindow(20,40,cx-100*dpi/96-10-10,cy-10-10-40,TRUE); 
  }

  CRect rect;
//  static int cx0 = 0, cy0 = 0;

  for (CWnd *wnd = GetTopWindow(); wnd; wnd = wnd->GetNextWindow()) {
    CString ss;
    wnd->GetWindowText(ss);
   
    int id = wnd->GetDlgCtrlID();
    if (id != 0 && id != IDC_TAB1) {
      
      wnd->GetWindowRect(rect);
      ScreenToClient(&rect);
      if (m_cx0 != 0 || m_cy0 != 0) {
        wnd->MoveWindow(rect.left+cx-m_cx0, rect.top, rect.Width(), rect.Height());
      }
    }
  }
  m_cx0 = cx;
  m_cy0 = cy;
}


static map<CString, CString> map_help;
static map<CString, int> map_help_yes;

BOOL init_help(const char *help, const char *tn);
void init_help(const char *tn);
CString getHelp(const char *fn);



CGidrView *getView();
CString getViewValue(const char *val, const char *mdb1, const char *table, const char *id, const char *fn);

BOOL CDialogNew2::viewTableDFM(const char *tn, const char *title, list<OBJ> l_ob, map<CString, CString> &map_ob, map<CString, vector <CString> > & map_lst2)
{
  CFormDialog2 *view = (CFormDialog2 *) m_pFrames[m_n_tab]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
  if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
  {
    int n2 = 1;

    m_tab.InsertItem(m_n_tab, title);

    bool isEdit = !m_readOnly;
  
    CControl *cnt, *cnt2;

    list<OBJ>::const_iterator it = l_ob.begin();

    int row = 0;

    for (; it != l_ob.end(); it++) {
      OBJ obj = *it;
      cnt = view->addControlObj(TIP_PODPIS, obj);
      if (cnt) {
        cnt->m_value = obj.name; 
        cnt->m_new_value = cnt->m_value;
       
        map<CString, CString>::const_iterator it = map_ob.find(obj.name);
        if (it != map_ob.end()) {
          CString field_name = it->second;

          FIELD fld;
         
          cnt->m_fn = field_name;
          cnt->m_fieldName = field_name;
          cnt->m_podp = obj.Caption;
          cnt->m_is_edit = false;

          map<CString, vector <CString> >::const_iterator it = map_lst2.find(field_name);
          if (it != map_lst2.end()) {
//            isButton = true;
            cnt->m_is_file = true;

            vector <CString> tok2 = it->second;
            m_map_dial[cnt->m_id] = tok2;
          }

      
      if (!db_row.getField(tn, field_name, fld, 0)) continue;
          cnt->m_value = fld.val;
          cnt->m_new_value = cnt->m_value;
          cnt->m_real_value = cnt->m_value;

          cnt->m_typ = fld.typ;
          cnt->m_len = fld.size;
          cnt->m_is_edit = isEdit;
          cnt->m_tn = tn;
          cnt->m_num = n2;

            MFIELDS map1;
            
            int is_e = FALSE;

            int ret = getMap(tn, field_name, map1);

            if (ret) {
                bool isComboEdit = obj.ID == ID_TComboEdit;;

                int typ = 0;
                if (isComboEdit) {
                  typ = TIP_COMBOEDIT;
                }
                else {
                  typ = TIP_COMBO;
                }

                cnt->m_ctrl = typ;

                vector <CString> tok2;
                list<pair<CString, CString> >::const_iterator it2;
       
                tok2.push_back("1");
                tok2.push_back("2");
       
                for (it2 = map1.list2.begin(); it2 != map1.list2.end(); it2++) {
                  tok2.push_back(it2->first);
                  tok2.push_back(it2->second);
                }
        
                cnt->m_is_edit = isEdit;
                cnt->tok = tok2;
                cnt->m_value = fld.val;
                cnt->m_new_value = cnt->m_value;
              }

              bool isButton = false;
              bool isButton2 = false;
              if (fld.typ == dbDate) isButton = true;

              map<CString, vector <CString> >::const_iterator it2 = map_lst2.find(field_name);
              if (it2 != map_lst2.end()) {
                isButton = true;
                cnt->m_is_file = true;

                vector <CString> tok2 = it2->second;
                m_map_dial[cnt->m_id] = tok2;
              }
             
              if (isButton) {
                cnt2 = view->addControlObj(TIP_BUTTON, obj);
                if (cnt2) {
                  cnt2->m_fieldName = cnt->m_fieldName;
                  cnt2->m_left = cnt->m_left+cnt->m_width;
                  cnt2->m_width = 20;

                  cnt2->m_ctrl = TIP_BUTTON;
                  cnt2->m_main = cnt;
                  cnt2->m_is_edit = isEdit;
                }
              }
              if (isButton2) {
                cnt2 = view->addControlObj(TIP_BUTTON, obj);
                if (cnt2) {
                  cnt2->m_buf_copy = true;
                  cnt2->m_fieldName = cnt->m_fieldName;
                  cnt2->m_left = cnt->m_left+cnt->m_width+20;
                  cnt2->m_width = 20;

                  cnt2->m_ctrl = TIP_BUTTON;
                  cnt2->m_main = cnt;
                  cnt2->m_is_edit = isEdit;
                }
              }
        }
      }
    }
    m_pFrames[m_n_tab]->InitialUpdateFrame( NULL, TRUE );
    m_n_tab++;
  }
  
  return TRUE;
}

BOOL CDialogNew2::viewTableFile(const char *tn, const char *title0, const vector <CString> &m_fields, int nC)
{
  int n2 = 1;
  int m_xy = 0;

  CFormDialog2 *view = (CFormDialog2 *) m_pFrames[m_n_tab]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
  if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
  {
    m_tab.InsertItem(m_n_tab, title0);

/*
    if (!strcmp(title0, "Изменения") && !strcmp(tn, "Технические условия")) {

      for (int k = 0; k < m_fields.size(); k++) {
        CString tnU;
        CString fn;
        vector <CString> tok;
        reads_field2(m_fields[k], tnU, fn, tok);

        char *ss = "Дата изменения продления";
        int k = strlen(ss);

        if (!strncmp(fn, ss, k)) {
          int qq;
          qq = 1;
        }
      }
    }

*/

    int m_cols = 1;
    int m_row = 0;
    int m_col = 0;

    if (1) {
        CControl *cnt, *cnt2;

        CString mdb, table, id, fn_id, progr;
        CString fn;
        CString typ_str;
        int n_dial = 0;
        vector <CString> m_fn_dial;

        long itter=1;

        CString tnU;

        m_n_tab++;


        for (int k = 0; k < m_fields.size(); k++) {
          vector <CString> tok;
          reads_field2(m_fields[k], tnU, fn, tok);

          int it = itter;
          int toksize = tok.size();
          int tok_i = 1;

          CString title = fn;
          CString field_name = fn;

          bool isEdit = !m_readOnly;
          bool isComboEdit = false;

          
          if (tnU == "Next") {
            m_col = -1;
            continue;
          }

//          tok_i++;

          if (tok.size() > tok_i) {
            typ_str = tok[tok_i];
            tok_i++;
          }

/*
          if (tok.size() > tok_i && tok[tok_i] == "ComboTable") {
            isComboEdit = true;
            tok_i ++;
          }
*/

          if (tok.size() > tok_i && tok[tok_i] == "ComboEdit") {
            isComboEdit = true;
            tok_i ++;
          }

          if (tok.size() > tok_i && tok[tok_i] == "ReadOnly") {
            isEdit = false;
            tok_i ++;
          }
          
          if (tok.size() > tok_i && tok[tok_i] == "Write") {
            isEdit = true;
            tok_i ++;
          }
          if (tok.size() > tok_i && tok[tok_i] == "Title") {
            title = tok[tok_i+1];
            tok_i ++;
            tok_i ++;
          }
          if (tok.size() > tok_i && tok[tok_i] == "FieldName") {
            field_name = tok[tok_i+1];
            tok_i ++;
            tok_i ++;
          }

          if (tok.size() > tok_i && tok[tok_i] == "XY") {

            CString srow = tok[tok_i+1];
            CString scol = tok[tok_i+2];

            if (srow == "N") {
              if (m_xy) m_row ++;
            }
            else if (srow == "A") {
            }
            else {
              m_row = atoi(srow);
            }

            if (scol == "N") {
//              m_col ++;
            }
            else if (scol == "A") {
//              m_col
            }
            else {
              m_col = atoi(scol);
            }

            m_cols = atoi(tok[tok_i+3]);

            m_xy = 1;
            tok_i ++;
            tok_i ++;
            tok_i ++;
            tok_i ++;
          }

          if (tnU == "Text") {
            cnt = view->addControl(TIP_PODPIS, fn, m_row, m_col, m_cols);
            m_col += m_cols;
            continue;
          }

          m_map_f_t[fn] = field_name;

          FIELD fld;

          if (!db_row.getField(tnU, field_name, fld, 0)) continue;

          CString val[256];


          for (int j = 0; j < nC; j++ ) {
            if (db_row.getField(tnU, field_name, fld, j)) {
              val[j] = fld.val;
            }
          }

          if (!m_xy) {
            cnt = view->addControl(TIP_PODPIS, getHelp(title), m_row , m_col, m_cols);
            m_col += m_cols;
          }

          typ_str = "Edit";
          if (tok.size() > tok_i) {
            typ_str = tok[tok_i];
            tok_i++;
          }

          bool isTable = false;
          bool isButton = false;
          bool isButton2 = false;
          if (fld.typ == dbDate) isButton = true;

          bool is_file = false;

          if (typ_str == "File" || typ_str == "PicBuf" || typ_str == "Dir") {
            is_file = true;
          }


          if (typ_str == "Table" || typ_str == "Table2" || typ_str == "EditTable" || typ_str == "Dialog_Call" || typ_str == "Dialog_Call_Table" || typ_str == "EditCALL" || typ_str == "File" || typ_str == "Dir" || typ_str == "PicBuf") {
            m_fn_dial.push_back(fn);

            vector <CString> tok2;
            tok2.push_back(fn);
            tok2.push_back(typ_str);
            for (int jj = tok_i; jj < tok.size(); jj++) tok2.push_back(tok[jj]);

            m_map_dial[cnt->m_id+2] = tok2;
            if (typ_str == "PicBuf" && IsBufDib()) {

              bool b = IsBufDib();

              isButton2 = true;
              m_map_dial[cnt->m_id+3] = tok2;
            }
            
            isButton = true;
            if (typ_str == "Table2" || typ_str == "Dialog_Call_Table") {
              isTable = true;

              if (typ_str == "Dialog_Call_Table") {
                progr = tok[tok_i++];
              }
              mdb = tok[tok_i++];
              table  = tok[tok_i++];
              id  = tok[tok_i++];
              fn_id = tok[tok_i++];
            }
            
            if (typ_str == "ReadOnly" || typ_str == "Table"  || typ_str == "Table2" || typ_str == "Dialog_Call" || typ_str == "Dialog_Call_Table") {
              isEdit = false;
            }
          }
          else if (typ_str == "DbTable") {
            vector <CString> tok2;
//            CString tn = tok[1+tok_i];
//            setTok(tok[tok_i], tok[1+tok_i], tok[2+tok_i], tok[3+tok_i], tok2);

            for (int j = 0; j < nC; j++ ) {
              cnt = view->addControl(TIP_TABLE, fn, m_row , m_cols, m_cols);
              if (cnt) {
                m_col += m_cols;
                cnt->m_is_edit = isEdit;
                cnt->tok = tok2;
                cnt->m_value = val[j];
                cnt->m_new_value = cnt->m_value;
                cnt->m_new_value = cnt->m_value;
                cnt->m_real_value = cnt->m_value;

                cnt->m_tn = tnU;
//                cnt->m_fn = field_name;
                cnt->m_num = n2;
                cnt->m_q = tok[tok_i];
                cnt->m_rjad = j;
              }
            }
            itter++;
          }
          else if (typ_str == "PzGraph") {
            vector <CString> tok2;

            for (int j = 0; j < nC; j++ ) {
              cnt = view->addControl(TIP_PZGRAPH, fn, m_row , m_col, m_cols);
              if (cnt) {
                m_col += m_cols;
                cnt->m_is_edit = isEdit;
                cnt->tok = tok2;
                cnt->m_value = val[j];
                cnt->m_new_value = cnt->m_value;
                cnt->m_new_value = cnt->m_value;
                cnt->m_real_value = cnt->m_value;
                cnt->m_tn = tnU;
//                cnt->m_fn = field_name;
                cnt->m_num = n2;
                cnt->m_q = tok[tok_i];
                cnt->m_rjad = j;
              }
            }
            itter++;
          }
          else if (typ_str == "MEROPR") {
            vector <CString> tok2;

            for (int j = 0; j < nC; j++ ) {
              cnt = view->addControl(TIP_MEROPR, fn, m_row , m_col, m_cols);
              if (cnt) {
                m_col += m_cols;
                cnt->m_is_edit = isEdit;
                cnt->tok = tok2;
                cnt->m_value = val[j];
                cnt->m_new_value = cnt->m_value;
                cnt->m_real_value = cnt->m_value;
                cnt->m_tn = tnU;
//                cnt->m_fn = field_name;
                cnt->m_num = n2;
                cnt->m_q = tok[tok_i];
                cnt->m_rjad = j;
              }
            }
            itter++;
          }
          else if (typ_str == "ComboTable") {
            vector <CString> tok2;
//            CString tn = tok[1+tok_i];
            setTok(tok[tok_i], tok[1+tok_i], tok[2+tok_i], tok[3+tok_i], tok2);


            for (int j = 0; j < nC; j++ ) {
              cnt = view->addControl(TIP_COMBO, fn, m_row , m_col, m_cols);
              if (cnt) {
                m_col += m_cols;
                cnt->m_is_edit = isEdit;
                cnt->tok = tok2;
                cnt->m_value = val[j];
                cnt->m_new_value = cnt->m_value;
                cnt->m_real_value = cnt->m_value;
                cnt->m_tn = tnU;
//                cnt->m_fn = field_name;
                cnt->m_num = n2;
                cnt->m_rjad = j;
              }
            }
            itter++;
          }
          else if (typ_str == "Combo") {
            for (int j = 0; j < nC; j++ ) {
              cnt = view->addControl(TIP_COMBO, fn, m_row , m_col, m_cols);
              if (cnt) {
                m_col += m_cols;
//                vector <CString> tok2;
//              
                cnt->tok.push_back("1");
                cnt->tok.push_back("2");

                for (int jj = tok_i+1; jj < tok.size(); jj++) {
                  cnt->tok.push_back(tok[jj]);
                }

                cnt->m_is_edit = isEdit;
                cnt->m_value = val[j];
                cnt->m_new_value = cnt->m_value;
                cnt->m_tn = tnU;
//                cnt->m_fn = field_name;
                cnt->m_num = n2;
                cnt->m_rjad = j;
              }
            }
            itter++;
          }
          else if (typ_str == "Edit" && fld.typ == dbBoolean) {
            for (int j = 0; j < nC; j++ ) {
              cnt = view->addControl(TIP_CHECKBOX, fn, m_row , m_col, m_cols);
              if (cnt) {
                m_col += m_cols;
                cnt->m_is_edit = isEdit;
                cnt->m_value = val[j];
                cnt->m_new_value = cnt->m_value;
                cnt->m_real_value = cnt->m_value;
                cnt->m_tn = tnU;
//                cnt->m_fn = field_name;
                cnt->m_num = n2;
                cnt->m_rjad = j;
              }
            }
            itter++;
          }
          else {
            MFIELDS map1;
            
            int is_e = FALSE;

            int ret = getMap(tnU, title, map1);

            if (!ret) {
              ret = getMap(tnU, fn, map1);
            }

            if (ret) {
              for (int j = 0; j < nC; j++) {
                if (isComboEdit) {
                  cnt = view->addControl(TIP_COMBOEDIT, fn, m_row , m_col, m_cols);
                  m_col += m_cols;
                }
                else {
                  cnt = view->addControl(TIP_COMBO, fn, m_row , m_col, m_cols);
                  m_col += m_cols;
                }
                if (cnt) {
                  vector <CString> tok2;
                  list<pair<CString, CString> >::const_iterator it2;
         
                  tok2.push_back("1");
                  tok2.push_back("2");
         
                  for (it2 = map1.list2.begin(); it2 != map1.list2.end(); it2++) {
                    tok2.push_back(it2->first);
                    tok2.push_back(it2->second);
                  }
          
                  cnt->m_is_edit = isEdit;
                  cnt->tok = tok2;
                  cnt->m_value = val[j];
                  cnt->m_new_value = cnt->m_value;
                  cnt->m_tn = tnU;
//                  cnt->m_fn = field_name;
                  cnt->m_num = n2;
                  cnt->m_rjad = j;
                }
              }
              itter++;
            }
          }

          if (it == itter)  {
            for (int j = 0; j < nC; j++ ) {
              cnt = view->addControl(TIP_EDIT, fn, m_row , m_col, m_cols);
              if (cnt) {
                cnt->m_is_file = is_file;
                m_col += m_cols;
                cnt->m_value = val[j];
                cnt->m_new_value = cnt->m_value;
                cnt->m_real_value = cnt->m_value;

                if (isTable) {
                  cnt->m_real_value = val[j];

                  cnt->mdb = mdb;
                  cnt->table = table;
                  cnt->id = id;
                  cnt->fn_id = fn_id;

                  cnt->m_value = getViewValue(val[j], mdb, table, id, fn_id);
                  cnt->m_new_value = cnt->m_value;
                  cnt->m_real_value = cnt->m_value;
                }
                cnt->m_typ = fld.typ;
                cnt->m_len = fld.size;
                cnt->m_is_edit = isEdit;
                cnt->m_tn = tnU;
//                cnt->m_fn = field_name;
                cnt->m_num = n2;
                cnt->m_rjad = j;


                if (is_file) {
                  if (tok.size() > 0+tok_i) cnt->m_file_ext = tok[0+tok_i];
                  if (tok.size() > 1+tok_i) cnt->m_file_Filter = tok[1+tok_i];
                  if (tok.size() > 2+tok_i) cnt->m_file_path = tok[2+tok_i];
                }


                if (isButton) {
                  m_col -= m_cols;
                  cnt2 = view->addControl(TIP_BUTTON, fn, m_row, m_col, m_cols);
                  if (cnt2) {
                    m_col += m_cols;
                    cnt2->m_main = cnt;
                    cnt2->m_is_edit = isEdit;
                  }
                }

                if (isButton2) {
                  cnt2 = view->addControl(TIP_BUTTON, fn, m_row, m_col, m_cols);
                  if (cnt2) {
                    cnt2->m_buf_copy = true;
                    cnt2->m_fieldName = cnt->m_fieldName;
                    cnt2->m_left = cnt->m_left+cnt->m_width+20;
                    cnt2->m_width = 20;

                    cnt2->m_ctrl = TIP_BUTTON;
                    cnt2->m_main = cnt;
                    cnt2->m_is_edit = isEdit;
                  }
                }
              }
            }
          }
          view->nextRow();

          if (!m_xy) {
            m_row ++;
            m_col = 0;
          }
        }
        m_n_tab--;
        m_pFrames[m_n_tab]->InitialUpdateFrame( NULL, TRUE );
        m_n_tab++;
        return TRUE;
      }
    }
  return FALSE;
}


CString get_val(CDialogNew2 *dlg,  const char *fn, const char *val, const char *old_val, CAdoFile *m_dbf, CString m_tn, CString m_q, int m_nom)
{
  if (m_tn == "Проверка знаний") {
    if (!strcmp(fn, "ФИО")) {
      CString ss;

      ss.Format("SELECT * FROM [Начальники участков] WHERE [ФИО]='%s'", val);

      if (m_dbf->openTable(ss)) {
        if ( !m_dbf->rs->IsEOF() ) {
          CString s1 = m_dbf->read("Должность");
          CString s2 = m_dbf->read("Дата приказа о назначении");
          CString s3 = m_dbf->read("Номер приказа о назначении");

          dlg->setValue(m_tn, "Должность", s1);
          dlg->setValue(m_tn, "Дата назначения", s2);
          dlg->setValue(m_tn, "Номер приказа о назначении", s3);

        }
        m_dbf->closeTable();
      }

      ss.Format("SELECT * FROM [Участки эксплуатации] U, [Район эксплуатации] R, [Начальники участков] M WHERE R.[Код]=U.[Район эксплуатации] AND M.[Код]=U.[Начальник участка] AND M.[ФИО]='%s'", val);

      if (m_dbf->openTable(ss)) {
        if ( !m_dbf->rs->IsEOF() ) {
          CString s1 = m_dbf->read(1);
          CString s2 = m_dbf->read(2);
          CString s3 = m_dbf->read(3);
          CString s18 = m_dbf->read("Наименование района эксплуатации источника тепла");
          CString s19 = m_dbf->read("Вид сети");

          dlg->setValue(m_tn, "Участок", s3);
          dlg->setValue(m_tn, "Район", s18);
          dlg->setValue(m_tn, "Ответственный РС", s19 == "р" ? "TRUE" : "FALSE");
          dlg->setValue(m_tn, "Ответственный МС", s19 == "м" ? "TRUE" : "FALSE");
        }
        m_dbf->closeTable();
      }
    }
  }
  return "";
}



void setEdit(CDialogNew2 *dlg, const char *tn)
{
  CControl *cntrl = dlg->getControl(tn);
  if (cntrl) {
    cntrl->m_is_edit = true;
    CEdit *edit = (CEdit *) cntrl->m_wnd;
    edit->SetReadOnly(FALSE);

    if (1) {
      CMyEdit *edit1 = (CMyEdit *) cntrl->m_wnd;
      edit1->SetBkColor(0xFFFFFF);
    }
  }
}

void copyVal(CDialogNew2 *dlg, const char *tn, const char *pic, int n)
{
  CString fn1 = pic, fn2;
  if (n > 1) {
    fn1.Format("%s %d", pic, n-1);
  }
  fn2.Format("%s %d", pic, n);

  setEdit(dlg, fn2);

  CString val1 = dlg->getValue(fn1);
  CString val2 = dlg->getValue(fn2);
  
  if (val2 == "" ) {
    dlg->setValue(tn, fn2, val1);
  }
}

bool copyVal3(CDialogNew2 *dlg, const char *fn, const char *fn1, const char *pic, int n)
{
  CString fn2, s;
  fn2.Format("%s %d", pic, n);

  if (fn != fn2) return false;

  s.Format("Дата изменения продления %d", n);
  CString d = dlg->getValue(s);

//  CString val1 = dlg->getValue(fn1);
  CString val = dlg->getValue(fn2);
 
  dlg->setValue(fn1, val);

  return (d != "");
}

void copyVal2(CDialogNew2 *dlg, const char *fn1, const char *fn2)
{
  CString val1 = dlg->getValue(fn1);
  CString val2 = dlg->getValue(fn2);
  
  if (val2 == "" ) {
    dlg->setValue("", fn2, val1);
  }
}


BOOL CDialogNew2::chValue(CControl *control, const char *val, const char *old_val)
{
  char fn[256];

  strcpy(fn, control->m_fieldName);

  CString s = get_val(this, fn, val, old_val, m_dbf, control->m_tn, control->m_q, control->m_nom);

  char *ss = "Дата изменения продления";

  int k = strlen(ss);

  if (!strncmp(fn, ss, k)) {
    CString tn = "Изменения";

    int n = atoi(&fn[k]);

    copyVal(this, tn, "Тепловые потоки, Гкал/ч", n);
    copyVal(this, tn, "В том числе отопление", n);
    copyVal(this, tn, "В том числе вентиляция", n);
    copyVal(this, tn, "В том числе ГВС макс", n);
    copyVal(this, tn, "Прирост нагрузки", n);
    copyVal(this, tn, "В том числе прирост отопление", n);
    copyVal(this, tn, "В том числе прирост вентиляция", n);
    copyVal(this, tn, "В том числе прирост ГВС макс", n);

    tn.Format("Дополнительные технические мероприятия %d", n);
    setEdit(this, tn);
    tn.Format("Изменения продления %d", n);
    setEdit(this, tn);
    tn.Format("Дата изменения продления %d", n+1);
    setEdit(this, tn);
  }

  for (int n = 7; n >= 1; n--) {
    if (copyVal3(this, fn, "Тепловые потоки, Гкал/ч",        "Тепловые потоки, Гкал/ч",        n)) break;
    if (copyVal3(this, fn, "В том числе отопление",          "В том числе отопление",          n)) break;
    if (copyVal3(this, fn, "В том числе вентиляция",         "В том числе вентиляция",         n)) break;
    if (copyVal3(this, fn, "В том числе ГВС макс",           "В том числе ГВС макс",           n)) break;
    if (copyVal3(this, fn, "Прирост нагрузки",               "Прирост нагрузки",               n)) break;
    if (copyVal3(this, fn, "В том числе прирост отопление",  "В том числе прирост отопление",  n)) break;
    if (copyVal3(this, fn, "В том числе прирост вентиляция", "В том числе прирост вентиляция", n)) break;
    if (copyVal3(this, fn, "В том числе прирост ГВС макс",   "В том числе прирост ГВС макс",   n)) break;
  }

  if (!strcmp(fn, "Дата выдачи акта допуска")) {
  char *pic = "Дата изменения продления";

  for (int n = 7; n >= 1; n--) {
    CString fn;
    fn.Format("%s %d", pic, n);

      CControl *cntrl = getControl(fn);
      if (cntrl) {
        CString val = getValue(cntrl);
        if (val != "") {

          CString tn;  
/*
          tn.Format("Тепловые потоки, Гкал/ч %d", n);
          copyVal2(this, tn, "Тепловая нагрузка по акту допуска (проекту) Гкал/ч");
          tn.Format("В том числе отопление %d", n);
          copyVal2(this, tn, "В том числе отопление по акту");
          tn.Format("В том числе вентиляция %d", n);
          copyVal2(this, tn, "В том числе вентиляция по акту");
          tn.Format("В том числе ГВС макс %d", n);
          copyVal2(this, tn, "В том числе ГВС макс по акту");
*/

          tn.Format("Прирост нагрузки %d", n);
          copyVal2(this, tn, "Тепловая нагрузка по акту допуска (проекту) Гкал/ч");
          tn.Format("В том числе прирост отопление %d", n);
          copyVal2(this, tn, "В том числе отопление по акту");
          tn.Format("В том числе прирост вентиляция %d", n);
          copyVal2(this, tn, "В том числе вентиляция по акту");
          tn.Format("В том числе прирост ГВС макс %d", n);
          copyVal2(this, tn, "В том числе ГВС макс по акту");
          
          
          goto QUIT;
        }
      }
    }

/*
    copyVal2(this, "Тепловые потоки, Гкал/ч", "Тепловая нагрузка по акту допуска (проекту) Гкал/ч");
    copyVal2(this, "В том числе отопление", "В том числе отопление по акту");
    copyVal2(this, "В том числе вентиляция", "В том числе вентиляция по акту");
    copyVal2(this, "В том числе ГВС макс", "В том числе ГВС макс по акту");
*/

    copyVal2(this, "Прирост нагрузки", "Тепловая нагрузка по акту допуска (проекту) Гкал/ч");
    copyVal2(this, "В том числе прирост отопление", "В том числе отопление по акту");
    copyVal2(this, "В том числе прирост вентиляция", "В том числе вентиляция по акту");
    copyVal2(this, "В том числе прирост ГВС макс", "В том числе ГВС макс по акту");


QUIT:
    ;

  }


  return TRUE;
}

void CDialogNew2::setElem(int n6, const char *avar_el)
{
  CControl *cc = NULL;
  CControl *c1 = getControlR(avar_el, "Исправлено на подаче", 0);

  int top = 0;

  int tp[256];

  if (c1) {
    top = c1->m_top;

    for (int r = 0; r < n6; r++) {
      tp[r] = top;
    }
  }

  for (int r = 0; r < n6; r++) {
    CControl *c01 = getControlR(avar_el, "Повреждено на подаче", r);

    if (c01) {
      CControl *c02 = c01->m_form->Next(c01);
      if (c02) {
        double b1 = atof(c01->m_new_value);
        double b2 = atof(c02->m_new_value);

        bool nz1 = (b1 != 0 || b2 != 0);

        CControl *c1 = getControlR(avar_el, "Исправлено на подаче", r);
        if (c1) {
          CControl *c2 = getControlR(avar_el, "Исправлено на обратке", r);
          CControl *c0 = c1->m_form->Prev(c1);

          if (c0 && c1 && c2) {
            double d1 = atof(c1->m_new_value);
            double d2 = atof(c2->m_new_value);

            if (d1 != 0 || d2 != 0) nz1 = true;

            if (c0->is_visible != nz1) {
              cc = c0;
            }

            c0->is_visible = nz1;
            c1->is_visible = nz1;
            c2->is_visible = nz1;
            
            c0->m_top = top;
            c1->m_top = top;
            c2->m_top = top;

            if (nz1) {
              top += 24;
            }
          }
        }
      }
    }
  }

  if (cc) {
//    cc->m_form->setXY(NULL, NULL);
    CPoint pt = cc->m_form->GetScrollPosition();
    cc->m_form->ScrollToPosition(CPoint(0,0));
    cc->m_form->Init2();
    cc->m_form->ScrollToPosition(pt);
  }
}


void CDialogNew2::setElem2(int n6, const char *avar_el)
{
  CControl *cc = NULL;
  CControl *c1 = getControlR(avar_el, "Повреждено на подаче", 0);

  int top = 0;

  int tp[256];

  if (c1) {
    top = c1->m_top;

    for (int r = 0; r < n6; r++) {
      tp[r] = top;
    }
  }

  for (int r = 0; r < n6; r++) {
    CControl *c01 = getControlR(avar_el, "Повреждено на подаче", r);

    if (c01) {
      CControl *c02 = getControlR(avar_el, "Повреждено на обратке", r);
      if (c02) {
        double bo1 = 0, bo2 = 0;
        
        double b1 = atof(c01->m_new_value);
        double b2 = atof(c02->m_new_value);

        CControl *cPo = getControlR(avar_el, "Осмотрено на подаче", r);
        CControl *cOo = getControlR(avar_el, "Осмотрено на обратке", r);

        if (cPo && cOo) {
          bo1 = atof(cPo->m_new_value);
          bo2 = atof(cOo->m_new_value);
        }


        bool nzP = (b1 != 0 && bo1 != 0 );
        bool nzO = (b2 != 0 && bo2 != 0);

        CControl *cP = getControlR(avar_el, "Исправлено на подаче", r);
        if (cP) {
          CControl *cO = getControlR(avar_el, "Исправлено на обратке", r);

          if (cP && cO) {

            CControl *cP0 = getControlR(avar_el, "Запланировано на подаче", r);
            CControl *cP1 = getControlR(avar_el, "Мероприятие на подаче", r);
            
            CControl *cO0 = getControlR(avar_el, "Запланировано на обратке", r);
            CControl *cO1 = getControlR(avar_el, "Мероприятие на обратке", r);
            
            double dP = atof(cP->m_new_value);
            double dO = atof(cO->m_new_value);

            if (dP != 0) nzP = true;
            if (dO != 0) nzO = true;

            if (cP->is_visible != nzP || cO->is_visible != nzO) {
              cc = cP;
            }

            bool nz = nzP || nzO;


            cP->is_visible = nz;
            cP0->is_visible = nz;
            cP1->is_visible = nz;

            cO->is_visible = nz;
            cO0->is_visible = nz;
            cO1->is_visible = nz;


            cP->m_top = cP0->m_top = cP1->m_top = cO->m_top = cO0->m_top = cO1->m_top = top;

            CControl *cp = cP->m_form->Prev(cP1);
            if (cp) {
              cp->is_visible = nzP || nzO;
              cp->m_top = top;
            }


            if (nzP || nzO) {
              top += 24;
            }
          }
        }
      }
    }
  }

  if (cc) {
//    cc->m_form->setXY(NULL, NULL);
    CPoint pt = cc->m_form->GetScrollPosition();
    cc->m_form->ScrollToPosition(CPoint(0,0));
    cc->m_form->Init2();
    cc->m_form->ScrollToPosition(pt);
  }
}


BOOL CDialogNew2::killFocus(CControl *control)
{

  setElem(N_REMONT_TR, m_avar_tr);
  setElem2(N_REMONT_EL, m_avar_el);

  CPoint pt1 = control->m_form->GetScrollPosition();

  if (control->m_tn == m_avar_tr || control->m_tn == m_avar_el) {

    int qq = control->m_fieldName.Find("Повреждено");
    
    if (qq != -1) {
      int rjad = control->m_rjad;

      CString ss = control->m_fieldName;

      if (control->m_tn == m_avar_tr) {
        ss.Replace("Повреждено", "Исправлено");
      }
      else {
        ss.Replace("Повреждено", "Запланировано");
      }

      CControl *c01 = getControlR(control->m_tn, ss, rjad);

      if (c01) {
        if (c01->m_new_value == "") {
          setValue(c01, control->m_value);
        }
      }
    }
  }

  CPoint pt2 = control->m_form->GetScrollPosition();

  return TRUE;
}

BOOL CDialogNew2::chValue2(CControl *control, const char *val, const char *old_val)
{
  if (!strcmp(val, old_val)) return TRUE;

  int qq =  m_name.Find(" - Узел");

  if (qq == -1) {
    qq = m_name.Find(" - Потребитель обобщенный");
  }

  if (!strcmp(control->m_fieldName, "name_typ") && qq != -1) {
    if (AfxMessageBox("Изменить?", MB_YESNO) == IDYES) {
      if (saveDialog())
        EndDialog(2016);
    }
    else {
      setValue(control, control->m_value);
      return FALSE;
    }
  }
  return TRUE;
}

void read_txt2_2(FILE *f, list<OBJ2>& l_ob);


void read_txt2(FILE *f, vector <CString> &tabs, vector <vector <CString> > &fields)
{
  CString S;
  char str[256];

  tabs.clear();
  fields.clear();

  if (f) {
    while (!feof(f)) {
      fgetstr(str, 1023, f);
      if (feof(f)) break;
      if (str[0] == '#') break;

      S = str;
      S.TrimLeft();
      S.TrimRight();

      int n = fields.size();

      if (str[0] == ' ' && n > 0) {
        fields[n-1].push_back(S);
      }
      else {
        vector <CString> v1;
        tabs.push_back(S);
        fields.push_back(v1);
      }
    }
    fclose(f);
  }
}


bool read_lst2(const char *txt, map<CString, vector <CString> > & map_lst2)
{
  CString s = txt;

//  s.Replace("  ", "\t");

  CString tn, fn;

  vector <CString> tok1;
  vector <CString> tok;

  reads_field2(s, tn, fn, tok1);


  vector <CString>::const_iterator it = tok1.begin();

  if (it != tok1.end()) it ++;

  for (; it != tok1.end(); it++) {
    tok.push_back(*it);
  }
  
  map_lst2[fn] = tok;
  return true;
}



BOOL CDialogNew2::OnInitDialog() 
{
  CDialog::OnInitDialog();

  CString avar = "Дефект";

  if (m_n_q > 0) {
    avar = m_tn[0];
  }

  m_avar_tr = avar + "_Трубы";     
  m_avar_el = avar + "_Элементы";

  char s[256];

  SetWindowText(m_cap);

  db_row.init();

  for (int i = 0; i < m_n_q; i++) {
    InitTab(m_dbf->m_strFileName, m_tn[i], false);
  
    init_help(m_tn[i]);
//    db_row.readRows(m_dbf, m_name, m_q[i], m_nom[i]);
    db_row.readRows(m_dbf, m_tn[i], m_q[i], m_nom[i]);
  }

  if (tnOut != "") init_help(tnOut);



//  sprintf(s, "tabs3/%s.txt", m_name);
//  FILE *f = fopenexe(s, "r");

  FILE *f = fopen_tabs(m_dbf->m_strFileName, m_name, "txt", "r");

  if (f) {
    int nDlg = 0;

    vector <CString> tabs;
    vector <vector <CString> > fields;
    read_txt2(f, tabs, fields);
    fgetstr(s, 256, f);
    fclose(f);    

    for (int i = 0; i < tabs.size(); i++) 
    {
      int n = db_row.size(m_name);
      viewTableFile(m_name, tabs[i], fields[i], n);
    }
    if (tnOut != "") viewTableOut(m_dbf, tnOut, ngP, ngO, "Результат расчета");
  }


  if (!f) {
   
//    sprintf(s, "tabs3/%s.l", m_name);
//    f = fopenexe(s, "r");

    f = fopen_tabs(m_dbf->m_strFileName, m_name, "l", "r");

    if (f) {
      while (!feof(f)) {

        map<CString, CString> map_ob;
        char title[1024], fn[1024], ss[1024], mdbn[1024];

        if (fgetstr(title,1023,f) == NULL) break;
        if (fgetstr(mdbn,1023,f) == NULL) break;
        if (fgetstr(ss,1023,f) == NULL) break;

        fn[0] = 0;

        bool dfm = false;
        bool xls = false;
        bool txt2 = false;

        if (!strncmp(ss, "dfm", 3)) {
          strcpy(fn, &ss[4]);
          dfm = true;
        }
        else if (!strncmp(ss, "xls", 3)) {
          strcpy(fn, &ss[4]);
          xls = true;
        }
        else if (!strncmp(ss, "txt2", 4)) {
          strcpy(fn, &ss[5]);
          txt2 = true;
        }

//        sprintf(s, "tabs3/%s.lst", fn);
//        FILE *g = fopenexe(s, "r");
        FILE *g = fopen_tabs(m_dbf->m_strFileName, fn, "lst", "r");

        if (g) {
          char s[1024], ss[1024];

          while (!feof(g)) {
            if (fgetstr(s,1023,g) == NULL) break;

            for (int i = 0; s[i] && i < 1024; i++) {
              if (s[i] == ' ') {
                strncpy(ss, s, i); ss[i] = 0;
                map_ob[ss] = &s[i+1];
                break;
              }
            }
          }
          fclose(g);
        }


        g = fopen_tabs(m_dbf->m_strFileName, fn, "lst2", "r");

        if (g) {
          char s[1024];

          while (!feof(g)) {
            if (fgetstr(s,1023,g) == NULL) break;
            read_lst2(s, m_map_lst2);
          }
          fclose(g);
        }
        


        if (dfm) {
//          sprintf(s, "tabs3/%s.dfm", fn);
//          g = fopenexe(s, "r");

          g = fopen_tabs(m_dbf->m_strFileName, fn, "dfm", "r");

          if (g) {
            list<OBJ> l_ob;
            read_dfm(g, l_ob);
            fclose(g);
            viewTableDFM(mdbn, title, l_ob, map_ob, m_map_lst2);
          }
        }
        else if (xls) {
//          sprintf(s, "tabs3/%s.html", fn);
//          g = fopenexe(s, "r");

          g = fopen_tabs(m_dbf->m_strFileName, fn, "html", "r");

          if (g) {
            list<OBJ> l_ob;
            read_html(g, l_ob);
            fclose(g);

            viewTableDFM(mdbn, title, l_ob, map_ob, m_map_lst2);
          }
        }
        else if (txt2) {
//          sprintf(s, "tabs3/%s.txt", fn);
//          g = fopenexe(s, "r");

          g = fopen_tabs(m_dbf->m_strFileName, fn, "txt2", "r");
          
          if (g) {
            int nDlg = 0;

            list<OBJ2> l_ob;

            vector <CString> fields;
            read_txt2_2(g, l_ob);
            fgetstr(s, 256, g);
            fclose(g);    

            CFormDialog2 *view = viewTable2(mdbn, title, l_ob);

            if (view) {
              view->m_mdb = m_dbf->m_strFileName;
              view->m_fn = fn;
            }
          }
        }
      }
      fclose(f);
    }
  }

//  AfxMessageBox("1");

  if (!f) {
    viewTable(m_name, m_name, !m_readOnly);
  }

//  AfxMessageBox("2");


  CRect rect;
  GetClientRect(rect);
  setDialog(rect.Width(), rect.Height()); 

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  pSysMenu->AppendMenu(MF_SEPARATOR);

  pSysMenu->AppendMenu(MF_STRING, ID_FONT, _T("Шрифт"));
  pSysMenu->AppendMenu(MF_STRING, ID_COPY_GEO, _T("Копировать данные из Геобазы"));

  int bx = AfxGetApp()->GetProfileInt(szSection1, "nd_bx", 0);
  int by = AfxGetApp()->GetProfileInt(szSection1, "nd_by", 0);
  int cx = AfxGetApp()->GetProfileInt(szSection1, "nd_cx", 0);
  int cy = AfxGetApp()->GetProfileInt(szSection1, "nd_cy", 0);

  if (cx > 0 && cy > 0) {
    SetWindowPos(&wndTopMost,bx,by,cx,cy,SWP_NOZORDER);
  }

  if (m_tu > 0) {
    CWnd *w = GetDlgItem(IDC_TU);
    if (w) {
      w->ShowWindow(SW_SHOW);
      w->EnableWindow(TRUE);
    }
  }
  if (m_nagr > 0) {
    CWnd *w = GetDlgItem(IDC_NAGR);
    if (w) {
      w->ShowWindow(SW_SHOW);
      w->EnableWindow(TRUE);
    }
  }

  char *pic = "Дата изменения продления";

  BOOL ro = TRUE;

  for (int n = 8; n >= 1; n--) {
    CString fn;
    fn.Format("%s %d", pic, n);

    if (ro) {
      CControl *cntrl = getControl(fn);
      if (cntrl) {
        CString val = getValue(cntrl);
        if (val != "") {
          ro = FALSE;
        }
      }
    }
    if (!ro) {
      CString tn;
      tn.Format("Дополнительные технические мероприятия %d", n);
      setEdit(this, tn);
      tn.Format("Изменения продления %d", n);
      setEdit(this, tn);
      tn.Format("Тепловые потоки, Гкал/ч %d", n);
      setEdit(this, tn);
      tn.Format("В том числе отопление %d", n);
      setEdit(this, tn);
      tn.Format("В том числе вентиляция %d", n);
      setEdit(this, tn);
      tn.Format("В том числе ГВС макс %d", n);
      setEdit(this, tn);
      tn.Format("Прирост нагрузки %d", n);
      setEdit(this, tn);
      tn.Format("В том числе прирост отопление %d", n);
      setEdit(this, tn);
      tn.Format("В том числе прирост вентиляция %d", n);
      setEdit(this, tn);
      tn.Format("В том числе прирост ГВС макс %d", n);
      setEdit(this, tn);

      tn.Format("Дата изменения продления %d", n+1);
      setEdit(this, tn);
    }
  }

  CWnd *w1 = GetDlgItem(IDC_ANNUL);
  CWnd *w2 = GetDlgItem(IDC_ZAV);
  CWnd *w3 = GetDlgItem(IDC_DEISTV);

  CControl *cntrl = getControl("Состояние договора");
  if (cntrl) {
    CString id = getValue(cntrl);
    if (w1) w1->EnableWindow(id == "Действующий");
    if (w2) w2->EnableWindow(id == "Действующий");
    if (w3) w3->EnableWindow(id != "Действующий");
  }

  setElem(N_REMONT_TR, m_avar_tr);
  setElem2(N_REMONT_EL, m_avar_el);

  map<CString, CString>::const_iterator it = m_map_value0.begin();

  for (; it != m_map_value0.end(); it++) {
    setValue(it->first, it->second);
  }
  return TRUE;  // return TRUE unless you set the focus to b control
                // EXCEPTION: OCX Property Pbges should return FALSE
}                   

void CDialogNew2::OnSize(UINT nType, int cx, int cy) 
{
  CDialog::OnSize(nType, cx, cy);

  setDialog(cx, cy); 
}

BOOL CDialogNew2::PreTranslateMessage(MSG* pMsg) 
{
  if (m_hAccel != NULL && TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
    if (pMsg->wParam != VK_RETURN) return TRUE;
  }
  
  if ( WM_KEYDOWN == pMsg->message )
  {
    if ( ((int)pMsg->wParam == VK_RETURN) || ((int)pMsg->wParam == VK_ESCAPE) || ((int)pMsg->wParam == VK_TAB)) return CDialog::PreTranslateMessage(pMsg);
  }
  return m_pFrame->PreTranslateMessage( pMsg );
}

int getZdan(CWnd *wnd, char *tn1);
int editAlmaGeo(CWnd *parent, const char *fn, const char *tn, long kk, int ro);

void editGeo(CWnd *pbrent, const char *tn, int n)
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(tn);
  if (kls) {
    editAlmaGeo(pbrent, kls->m_db->m_strFileName, tn, n, TRUE);
//    editQ_1(pbrent, *kls->m_db, tn, n, 0, tn, IDD_DIbLOG_NEW2);
  }
}

int getZdan(CWnd *wnd, char *tn1);
void viewUzel(CWnd *wnd, bool readonly);


BOOL CDialogNew2::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  UINT nID = LOWORD(wParam);
  UINT msg = HIWORD(wParam);

  if (msg == BN_CLICKED) {
//    if (nID != IDOK && nID != IDCANCEL && nID != IDC_TAB) {
//      CString str;
//      str.Format("%d", nID);
//      AfxMessageBox(str);
//    }

    if (nID != IDOK && nID != IDCANCEL && nID != IDC_TAB && nID != IDC_HELP_F) {
      switch (nID) {
      case IDC_NAGR : 
        editGeo(this, "Здания потребителей", m_nagr);
        break;
      case IDC_TU : 
        editGeo(this, "Здания ТУ", m_tu);
        break;
      case IDC_REP : 
        if (saveDialog())
          EndDialog(2006);
        break;
      case IDC_TABLE : 
        if (saveDialog())
          EndDialog(2005);
        break;
      case IDC_TABLE2 : 
        if (saveDialog())
          EndDialog(2007);
        break;
      case IDC_TG : 
         {
            int ret;
            long n = m_nom[0];
            ret = ShowObjectFromName(m_name, argpath(), m_dbf->m_strFileName, n, TRUE, m_parent);
         }

//       AfxMessageBox("!");
//        if (saveDialog())
//          EndDialog(2007);
        break;
      case IDC_ZDAN : 
        {
          char tn[256];
          CGidrView *view = getView();
          if (view == m_parent)  {
            int n = getZdan(m_parent, tn);
            if (n != 0) {
              editGeo(this, tn, n);
//              Klassif *kls = m_geofile->m_kl_list->findKlN(tn);
//              editQ_1(this, *kls->m_db, tn, n, 0, tn, IDD_DIbLOG_NEW2);
            }
          }
        }
        break;
      case IDC_UZEL : 
        {
          CGidrView *view = getView();
          if (view == m_parent) {
            viewUzel(m_parent, m_readOnly);
          }
        }
        break;
      case IDC_PO : 
        {
          long n = m_nom_po;
            int ret;
            ret = ShowObjectFromName("Потребитель обобщенный", argpath(), m_dbf->m_strFileName, n, TRUE, m_parent);
        }
        break;
      case IDC_PR : 
        {
          long n = m_nom_po;
            int ret;
            ret = ShowObjectFromName("Потребитель реальный", argpath(), m_dbf->m_strFileName, n, TRUE, m_parent);
        }
        break;
      }
    }
  }
  
  return CDialog::OnCommand(wParam, lParam);
}


void CDialogNew2::OnOK() 
{
  if (check_data) {
    if (!check_data(this)) return;
  }
  if (saveDialog())
    CDialog::OnOK();
}

void addTU(CDialogNew2 *dlg);


void CDialogNew2::OnAdd() 
{
//  if (saveDialog())
//    EndDialog(2008);

  addTU(this);
}

void CDialogNew2::OnTab() 
{
  if (saveDialog())
    EndDialog(2005);
}

void CDialogNew2::OnDel() 
{
  if (saveDialog())
    EndDialog(2008);
}

void CDialogNew2::OnSave() 
{
  if (saveDialog())
    EndDialog(2015);
}

void CDialogNew2::ActivateFrame(CFrameWnd *frame) 
{
//  DrbwTab(m_tab.GetCurSel());

  int n = m_tab.GetCurSel();

  if (n >= 0) {
    for (int i = 0; i < m_n_tab; i++) {
      if (m_pFrames[i] == frame) {
        m_pFrames[i]->ShowWindow(SW_SHOW);
        m_pFrames[i]->EnableWindow(TRUE);
        m_pFrame = m_pFrames[i];
        m_tab.SetCurSel(i);
      }
      else {
        m_pFrames[i]->ShowWindow(SW_HIDE);
        m_pFrames[i]->EnableWindow(FALSE);
      }
    }
  }


  CRect rect;
  GetClientRect(rect);
  setDialog(rect.Width(), rect.Height()); 
}

void CDialogNew2::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
//  DrbwTab(m_tab.GetCurSel());

  int n = m_tab.GetCurSel();

  if (n >= 0) {
    m_pFrames[n]->ShowWindow(SW_SHOW);
    m_pFrames[n]->EnableWindow(TRUE);
    m_pFrame = m_pFrames[n];
    
    for (int i = 0; i < m_n_tab; i++) {
      if (i != n) {
        m_pFrames[i]->ShowWindow(SW_HIDE);
        m_pFrames[i]->EnableWindow(FALSE);
      }
    }
  }

  CRect rect;
  GetClientRect(rect);
  setDialog(rect.Width(), rect.Height()); 
  
  *pResult = 0;
}

void CDialogNew2::OnSelchangingTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
//  SaveTab(m_tab.GetCurSel());
  
  *pResult = 0;
}

void ShiftF1Out(const TCHAR *s1, const TCHAR *s2);
void ShiftF1Mag(const TCHAR *s1, const TCHAR *s2);

void CDialogNew2::Help(CControl *c)
{
  if (!c) return;

  ShiftF1Mag(c->m_tn, c->m_fn);
}


void CDialogNew2::Help(int base, const char *tn, const char *fn)
{
  if (base == BASE_OUT) {
    ShiftF1Out(tn, fn);
  }
  else {
    ShiftF1Mag(tn, fn);
  }
}

void CDialogNew2::OnShiftF1() 
{
  AfxMessageBox("!");
/*
  CString s1, str;
  char s2[256];

  s1 = m_cod;
  long col = m_grid.GetRow();

  map<int, int>::const_iterator it = map_fld.find(col);
  
  if (it != map_fld.end()) m_db->FieldName(it->second+1, s2);
  
//  ShiftF1Mag(s1.Left(2), s2);
  ShiftF1Mag(s1, s2);
*/
}



BOOL CDialogNew2::setValue(const char *tn, const char *fn, const char *val)
{
  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      if (view->setValue(tn, fn, val)) return TRUE;
    }
  }
  return FALSE;
}

int CDialogNew2::getID(const char *tn)
{
  int ID = 0;
  FIELD fld;

  if (db_row.getField(tn, "ID", fld, 0)) {
    ID = atoi(fld.val);
  }
  return ID;
}

CString CDialogNew2::getDbValue(const char *tn, const char *fn)
{
  int ID = 0;
  FIELD fld;

  if (db_row.getField(tn, fn, fld, 0)) {
    return fld.val;
  }
  return "";
}



CString CDialogNew2::getValue(CControl* control)
{
  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      return view->getValue(control);
    }
  }
  return "";
}


BOOL CDialogNew2::setValue(CControl* control, const char *val)
{
  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      if (view->setValue(control, val)) return TRUE;
    }
  }
  return FALSE;
}

BOOL CDialogNew2::setValue(CControl* control, const char *val, const char *real)
{
  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      if (view->setValue(control, val, real)) return TRUE;
    }
  }
  return FALSE;
}

CString CDialogNew2::getRealValue(const char *fn)
{
  CControl *cnt = getControl(fn);
  if (cnt) {
    return cnt->m_real_value;
  }
  return "";
}


CString CDialogNew2::getValue(const char *fn)
{
  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      CString val;
      BOOL ret = view->getValue(fn, val);
      if (ret) {
        return val;
      }
    }
  }
  return "";
}

void CDialogNew2::moveValue(const char *fn)
{
  CControl *c = getControl(fn);
  if (c) {
    ActivateFrame(c->m_frame);
    c->m_wnd->SetFocus();
  }
}

BOOL CDialogNew2::setValue0(const char *fn, const char *val)
{
  m_map_value0[fn] = val;
  
  return FALSE;
}


BOOL CDialogNew2::setValue(const char *fn, const char *val)
{
  CControl *c = getControl(fn);
  if (c) {
    return setValue(c, val);
  }
  return FALSE;
}


CControl *CDialogNew2::getControl(const char *fn)
{
  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      CString val;
      CControl *cntrl = view->getControl(fn);
      if (cntrl) {
        return cntrl;
      }
    }
  }
  return NULL;
}


CControl *CDialogNew2::getControlR(const char *tn, const char *fn, int rjad)
{
  for (int i = 0; i < m_n_tab; i++) {
    CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
    if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
    {
      CString val;
      CControl *cntrl = view->getControlR(tn, fn, rjad);
      if (cntrl) {
        return cntrl;
      }
    }
  }
  return NULL;
}



//CString get_progr_val(const char *progr, const char *val);
CString get_progr_val(CDialogNew2 *wnd,  CControl *cntrl, const char *progr, const char *val, CAdoFile *m_dbf, CString m_tn, CString m_q, int m_nom, CString &real);
CString get_progr_val3(CDialogNew2 *wnd, const char *progr, const char *val, CAdoFile *m_dbf, CString m_tn, CString m_q, int m_nom, CString &real1, CString &real2, CString &real3);
CString get_progr_val6(CDialogNew2 *wnd, const char *progr, const char *val, CAdoFile *m_dbf, CString m_tn, CString m_q, int m_nom, int nn, CString &real1, CString &real2, CString &real3, CString &real4, CString &real5, CString &real6);

#include "excel.h"

void createRemontExcel(const char * name, CExcel &ex, CDialogNew2 *dlg);




void print_index1(FILE *f, const char *title);
void print_index2(FILE *f);
void print_index(FILE *f, const char *title, const char *fn);
void print_doc_end(FILE *f);
FILE *print_doc(FILE *fi, const char *fn, const char *title, void (*filtr) (CString &s1) = NULL);


void createReport()
{
  CString tmpName;
  tmpName.Format("%s\\index2.html", getenv("TMP"));

  CString tn = STR_REMONT_DEFECT;

  Klassif *kls = m_geofile->m_kl_list->findKlN(tn);
  if (!kls || !kls->ins) {
    return;
  }



  FILE *f = fopen(tmpName, "w");
  if (!f) return;

  setlocale(LC_NUMERIC, "");

  print_index1(f, "");

  FILE *g = print_doc(f, "remont\\Ремонты 9.html", "Ремонты");

  if (g) {

/*
    CString getSelect(int sort, int desc);

    CString q = getSelect(10, 1);


    CAdoFile dbf;

    if (dbf.openMDB(kls->m_db->m_strFileName)) {
      if (dbf.openTable(q)) {


        if (!dbf.rs->IsEOF()) {

          dbf.rs->MoveFirst();

          while ( !dbf.rs->IsEOF() ) {
//            if (s[0]) m_ctl_street.AddString(s);
            fprintf(g, "<tr>");
            CString s;
            for (int i = 2; i <= 20; i++ ) {
              s = dbf.read(i); 
              fprintf(g, "<td class=xl67>%s\xA0</td>", s);
            }
            fprintf(g, "</tr>");
            dbf.rs->MoveNext();
          }
        }
        dbf.closeTable();
      }
      dbf.closeMDB();
    }
*/

    print_doc_end(g);
  }

  print_index2(f);
  fclose(f);
  setlocale(LC_NUMERIC, "eng");

  tmpName.Format("%s\\index2.html", getenv("TMP"));

  HINSTANCE hRun = ShellExecute (AfxGetMainWnd()->GetSafeHwnd (), "open", "excel", "\""+tmpName+"\"", NULL, SW_SHOW);
}

/*

void CDialogNew2::CreateWord(const char *m_doc_path, const char *tn, const char *fn)
{
  CGidrView *view = getView();

  if (m_doc_path != "")  {
    if (IsFile(m_doc_path)) {
      if (AfxMessageBox("Документ уже создан. Пересоздать?", MB_YESNO) != IDYES) return;
    }

    if (view->Word2(this, m_doc_path, m_tn[0], tn, fn)) {
      m_doc_name = GetName(m_doc_path1);
      m_nTimerWord = SetTimer(1, 1000, 0);
    }
  }
}

void CDialogNew2::ViewWord(const char *m_doc_path)
{
  CGidrView *view = getView();
  
  if (!IsFile(m_doc_path)) {
    AfxMessageBox("Документ не создан. Создайте");
    return;
  }

  if (view->Word1(m_doc_path)) {
    m_nTimerWord = SetTimer(1, 1000, 0);
  }
}
*/

void AddMeropr()
{
  CMMenu2 menu(AfxGetMainWnd(), IDD_MENU_DIAM, "");
  if (menu.DoModal() == IDOK) {
  }
}

CString CDialogNew2::getQ(const char *tn)
{
  for (int i = 0; i < m_n_q; i++) {
    if (m_tn[i].CompareNoCase(tn) == 0) {
      return m_q[i];
    }
  }
  return "";
}

int getIDR(const char *s1, const char *id1)
{
  CString s = s1;
  CString id = id1;

  int n = 0;
  int i = s.Find(" "+id+"=");

  if (i < 0) return 0;

  CString ss = s.Mid(i+2+id.GetLength());

  return atoi(ss);

}

void CDialogNew2::reread(CFormDialog2 *frm, const char *tn)
{
  if (!frm) return;

  CString q = getQ(tn);

  m_map_val.clear();
  saveFormDialog2(frm);
  saveDB2(tn, q, -1);

//  CFormDialog2 *frm = c->m_form;

  FILE *g = fopen_tabs(frm->m_mdb, frm->m_fn, "txt2", "r");
  
  if (g) {
    int nDlg = 0;

    char s[256];

    list<OBJ2> l_ob;

    db_row.delRowTable(tn);
    db_row.readRows(m_dbf, tn, q, -1);

    vector <CString> fields;
    read_txt2_2(g, l_ob);
    fgetstr(s, 256, g);
    fclose(g);    

    frm->reset();

    viewTable2(frm, tn, "1", l_ob);

    frm->Init();

    CPoint pt = frm->GetScrollPosition();
    frm->ScrollToPosition(CPoint(0,0));
    frm->Init2();
    frm->Init3();
    frm->ScrollToPosition(pt);
  }
}


#if 0


void CDialogNew2::delRow(CControl *control, int id)
{
  //CControl *c = control->m_form->Prev(control);

  int r = control->m_rjad;

  CControl *c = control->m_form->Prev(control);
  CString q = "";
  CString tn = "";
  CAdoFile *dbf = getDB();
 
  CFormDialog2 *frm = c->m_form;

  if (dbf) {
    for (int i = 0; i < m_n_q; i++) {
      if (m_tn[i] == c->m_tn) {
        q = m_q[i];
        tn = m_tn[i];
        if (dbf->openTable(q)) {
          dbf->rs->MoveFirst();

          int n = 0;
          while ( !dbf->rs->IsEOF() ) {
            if (n == r) {
              dbf->Edit();
              dbf->write("ID_ремонт", 0);
              dbf->Update();
              break;
            }
            dbf->rs->MoveNext();
            n++;
          }
          dbf->closeTable();


          reread(frm, tn);
          break;
  /*


          FILE *g = fopen_tabs(frm->m_mdb, frm->m_fn, "txt2", "r");
          
          if (g) {
            int nDlg = 0;

            char s[256];

            list<OBJ2> l_ob;

            vector <CString> fields;
            read_txt2_2(g, l_ob);
            fgetstr(s, 256, g);
            fclose(g);    

            frm->reset();

//             db_row.init();
//             db_row.readRows(m_dbf, m_tn[i], m_q[i], m_nom[i]);

            db_row.delRow(m_tn[i], r);

            viewTable2(frm, c->m_tn, "1", l_ob);

            frm->Init();

            CPoint pt = frm->GetScrollPosition();
            frm->ScrollToPosition(CPoint(0,0));
            frm->Init2();
            frm->Init3();
            frm->ScrollToPosition(pt);
          }
        }
        break;
*/
      }
    }
  }
  m_is_change = true;
}


void CDialogNew2::addRow(CControl *control, int id, const char *tn1)
{
  //CControl *c = control->m_form->Prev(control);

  CString tn = tn1;

  CString q = getQ(tn1);
  CAdoFile *dbf = getDB();

  if (q == "") return ;

  CFormDialog2 *frm = control->m_form;

  int idr = getIDR(q, "[ID_ремонт]");

  if (idr > 0 && dbf) {
    if (dbf->openTable(q)) {
      dbf->AddNew();
      dbf->write("ID_ремонт", idr);
      dbf->Update();
    }
    dbf->closeTable();

    FILE *g = fopen_tabs(frm->m_mdb, frm->m_fn, "txt2", "r");
  
    if (g) {
      int nDlg = 0;

      char s[256];

      list<OBJ2> l_ob;

      vector <CString> fields;
      read_txt2_2(g, l_ob);
      fgetstr(s, 256, g);
      fclose(g);    

      frm->reset();

      db_row.addRow(tn1);

      viewTable2(frm, tn1, "1", l_ob);

      frm->Init();

      CPoint pt = frm->GetScrollPosition();
      frm->ScrollToPosition(CPoint(0,0));
      frm->Init2();
      frm->Init3();
      frm->ScrollToPosition(pt);
    }
  }
  m_is_change = true;
}
#endif


void CDialogNew2::delRow(CControl *control, int id)
{
  //CControl *c = control->m_form->Prev(control);

  int r = control->m_rjad;

  CControl *c = control->m_form->Prev(control);
  CString q = "";
  CString tn = "";
  CAdoFile *dbf = getDB();
 
  CFormDialog2 *frm = c->m_form;

  if (dbf) {
    for (int i = 0; i < m_n_q; i++) {
      if (m_tn[i] == c->m_tn) {
        q = m_q[i];
          tn = m_tn[i];
        if (dbf->openTable(q)) {
          dbf->rs->MoveFirst();

          int n = 0;
          while ( !dbf->rs->IsEOF() ) {
            if (n == r) {
//              dbf->Edit();
//              dbf->write("ID_ремонт", 0);
//              dbf->Update();
              dbf->rs->Delete();
              break;
            }
            dbf->rs->MoveNext();
            n++;
          }
          dbf->closeTable();

          reread(frm, tn);
          break;
        }
      }
    }
  }
  m_is_change = true;
}


void CDialogNew2::addRow(CControl *control, int id, const char *tn1)
{
  //CControl *c = control->m_form->Prev(control);

  CString tn = tn1;

  CString q = getQ(tn1);
  CAdoFile *dbf = getDB();

  if (q == "") return ;

  CFormDialog2 *frm = control->m_form;

  CString fn = "ID_ремонт";

  int idr = getIDR(q, "[ID_ремонт]");

  if (idr == 0) {
    idr = getIDR(q, "[ID_опрессовка]");
    fn = "ID_опрессовка";
  }

  if (idr > 0 && dbf) {
    if (dbf->openTable(q)) {
      dbf->AddNew();
      dbf->write(fn, idr);
      dbf->Update();
    }
    dbf->closeTable();
    reread(frm, tn);
  }
  m_is_change = true;
}

void CDialogNew2::addRowOprMer(CControl *control, int id, const char *tn1)
{
  //CControl *c = control->m_form->Prev(control);

  CString tn = tn1;

  CString q = getQ(tn1);
  CAdoFile *dbf = getDB();

  if (q == "") return ;

  CFormDialog2 *frm = control->m_form;

  CString fn = "ID_ремонт";

  int idr = getIDR(q, "[ID_ремонт]");

  if (idr == 0) {
    idr = getIDR(q, "[ID_опрессовка]");
    fn = "ID_опрессовка";
  }

  if (idr > 0 && dbf) {
    if (dbf->openTable(q)) {
      dbf->AddNew();
      dbf->write(fn, idr);
      dbf->Update();
    }
    dbf->closeTable();
    reread(frm, tn);
  }
  m_is_change = true;
}




void CDialogNew2::Call(CControl *control, const char *qq1, const char *tn1)
{
  CString qq = qq1;

  CString tn = tn1;

/*
  if (qq == "akt_osm_def_create") {
    CreateWord(m_doc_path2, tn, "Акт осмотра");
  }
  else if (qq == "akt_osm_def_view") {
    ViewWord(m_doc_path2);
  }
  else if (qq == "create_report") {
    CreateWord(m_doc_path3, tn, "Отчет_ремонт_реконструкция");
    //createReport();
  }
  else if (qq == "view_report") {
    ViewWord(m_doc_path3);
  }
  else if (qq == "create") {
    CreateWord(m_doc_path1, tn, "Акт шурфа");
  }
  else if (qq == "view") {
    ViewWord(m_doc_path1);
  }
  else if (tn == "Шурф_влажность_грунта") {
    if (qq == "Button5") {
      CreateWord(m_doc_path4, tn, "Анализ проб на влажность");
    }
    else if (qq == "Button2") {
      ViewWord(m_doc_path4);
    }
  }
  else if (tn == "Шурф_коррозийная_активность") {
    if (qq == "Button1") {
      CreateWord(m_doc_path5, tn, "Анализ на коррозийность");
    }
    else if (qq == "Button2") {
      ViewWord(m_doc_path5);
    }
  }
  else if (tn == "Шурф_потенциалы_труба_земля") {
    if (qq == "Button1") {
      CreateWord(m_doc_path6, tn, "Протокол измерений");
    }
    else if (qq == "Button2") {
      ViewWord(m_doc_path6);
    }
  }
  else if (tn == "Шурф_водная_вытяжка") {
    if (qq == "Button1") {
      CreateWord(m_doc_path7, tn, "Отчет");
    }
    else if (qq == "Button2") {
      ViewWord(m_doc_path7);
    }
  }
  else 
*/  
  
  if (qq == "AddMeropr") {
    CControl *cntrl = getControl("Table1");
    if (cntrl) {
      CEditGrid2 *grid = (CEditGrid2 *) cntrl->m_wnd;
      long c = grid->GetCol();
      long r = grid->GetRow();
      CString s;
      s.Format("r=%d c=%d", r, c);
      AfxMessageBox(s);

    }
//    AddMeropr();
  }
  else if (qq == "DelMeropr") {
    AfxMessageBox("Del");
  }

  else if (qq == "AddMerNad") {
    addRow(control, 1, "Капремонт_колодцы_опоры_надземный");
  }
  else if (qq == "AddMerPod") {
    addRow(control, 2, "Капремонт_колодцы_опоры_подземный");
  }
  else if (qq == "AddMerTr") {
    addRow(control, 3, "Капремонт_трубопр_изол");
  }
  else if (qq == "DelMerNad") {
    delRow(control, 1);
  }
  else if (qq == "DelMerPod") {
    delRow(control, 2);
  }
  else if (qq == "DelMerTr") {
    delRow(control, 3);
  }
  else if (qq == "AddOpressovkaPovr") {
    addRow(control, 3, "Опрессовка_повреждения");
  }
  else if (qq == "DelOpressovkaPovr") {
    delRow(control, 3);
  }
  else if (qq == "AddMeroprOpr") {
    addRowOprMer(control, 3, "Опрессовка_повреждения");
  }
  else if (qq == "DelMeroprOpr") {
    delRow(control, 3);
  }

  


  else if (qq == "Сохранить_Индикатор_коррозии") {
    int id = getID(STR_REMONT_KORROZ);

    CString s;
    s.Format("od=%d", id);

    CString tn = "Индикатор_коррозии_по_годам";

    CString d1 = getRealValue("Дата установки");
    CString d2 = getRealValue("Дата извлечения");

    if (d1 == "" || d2 == "") {
      AfxMessageBox("Не заданы даты установки и извлечения. Сохранение не выполняется.");
      return;
    }

    COleDateTime dt1, dt2;

    dt1.ParseDateTime(d1, VAR_DATEVALUEONLY );
    dt2.ParseDateTime(d2, VAR_DATEVALUEONLY );


    CString st1 = dt1.Format("%#m/%#d/%#Y");   
    CString st2 = dt2.Format("%#m/%#d/%#Y");   

    int y1 = dt1.GetYear();

    CString q;
    q.Format("SELECT * FROM [%s] WHERE YEAR([Дата установки])=%d AND ID_I=%d", tn, y1, id);

    int nr = 0;

    if (m_dbf->openTable(q)) {
      nr = m_dbf->NRecs();
      m_dbf->closeTable();
    }

    if (nr > 0) {
      if (AfxMessageBox("Сохранение обработки индикатора коррозии уже выполнено. Пересохранить? Предыдущие данные обработки будут удалены.", MB_YESNO) != IDYES) return;

      q.Format("DELETE FROM [%s] WHERE YEAR([Дата установки])=%d AND ID_I=%d", tn, y1, id);
      m_dbf->Execute(q);      
    }


    if (m_dbf->openTable(tn)) {

      m_dbf->AddNew();
      m_dbf->write("ID_I", id);

      m_dbf->write("Количество пластин в сборке", getRealValue("Количество пластин в сборке"));
      m_dbf->write("Средний вес пластины при установке, г", getRealValue("Средний вес пластины при установке, г"));
      m_dbf->write("Радиус круглой пластины, мм", getRealValue("Радиус круглой пластины, мм"));
      m_dbf->write("Радиус втулки, мм", getRealValue("Радиус втулки, мм"));
      m_dbf->write("Толщина пластины, мм", getRealValue("Толщина пластины, мм"));
      m_dbf->write("Дата установки", getRealValue("Дата установки"));
      m_dbf->write("Дата извлечения", getRealValue("Дата извлечения"));

    COleDateTime d1, d2;


    d1.ParseDateTime(getRealValue("Дата установки"), VAR_DATEVALUEONLY );
    d2.ParseDateTime(getRealValue("Дата извлечения"), VAR_DATEVALUEONLY );

    COleDateTimeSpan ds1(d1), ds2(d2);
    int dt = ds2-ds1; // Время испытаний, сут

      m_dbf->write("Количество дней испытаний", dt);
      m_dbf->write("Средний вес пластины после испытаний, г", getRealValue("Средний вес пластины после испытаний, г"));
      m_dbf->write("Потеря массы средняя при кислотной обрабоке, г", getRealValue("Потеря массы средняя при кислотной обрабоке, г"));
      m_dbf->write("Средняя скорость коррозии, мм/год", getRealValue("Средняя скорость коррозии, мм/год"));
      m_dbf->write("Оценка коррозионного процесса", getRealValue("Оценка коррозионного процесса"));
      m_dbf->write("Агрессивность сетевой воды", getRealValue("Агрессивность сетевой воды"));
      m_dbf->write("Внешний вид пластин", getRealValue("Внешний вид пластин"));

      m_dbf->Update();
      m_dbf->closeTable();
  
      CFormDialog2 *view = (CFormDialog2 *) m_pFrames[1]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);

      reread(view, tn);
      m_is_change = true;

      AfxMessageBox("Выполнено");
    }

//    AfxMessageBox(s);
  }

}

CString getProfileString(const char *section, const char *reg)
{
  CGidrApp* pApp = (CGidrApp*) AfxGetApp();
  return pApp->GetProfileString(section, reg, "");
}


CString getPath(const char *reg)
{
  if (!strcmp(reg, "ANY")) return "";

  CString path = "";
  do {
    CGidrApp* pApp = (CGidrApp*) AfxGetApp();

    path = pApp->GetProfileString(szSection, reg, "");
    if (path == "") {
      if (!pApp->PathAny(reg)) return "";
    }
  } while (path == "");
  if (path != "" && path[path.GetLength()-1] != '\\') path += "\\";

  return path;
}


CString getMdb(CString mdb)
{
  if (mdb == "PS") {
    mdb = getPS();
  }
  if (mdb == "PS_STRUCT") {
    mdb = getPSAlma();
  }
  else if (mdb == "MapSprav") {
    mdb = getMapSprav();
  }
  else if (mdb == "PSSprav") {
    mdb = getPsSprav();
  }
  else if (mdb == "СтандОбор.mdb") {
    mdb = getStandObor();
  }
  else if (mdb == "Справочная.mdb") {
    mdb = getSpravochnaya();
  }

  return mdb;
}


CString TrimPath(CString val, CString path)
{
  if (path != "") {
    CString p = path; p.MakeLower();
    CString v = val; v.MakeLower();

    if (v.Find(p) == 0) {
      val = val.Mid(p.GetLength());
    }
  }
  return val;
}


void CDialogNew2::Do(CControl *control)
{
  CString val = "";

  if (control->m_main == NULL) {
    map <int, vector <CString> >::const_iterator it = m_map_dial.find(control->m_id);

    if (it != m_map_dial.end())  {
      vector <CString> tok = it->second;

      if (tok.size() >= 3) {
        CString qq = tok[2];
        Call(control, qq, control->m_tn);
      }

//      for (vector <CString>::const_iterator it2 = tok.begin(); it2 != tok.end(); it2++) {
//        AfxMessageBox(*it2);
//      }
    }
    else {
//      AfxMessageBox(control->m_value);
      Call(control, control->m_cod, control->m_tn);
    }
    return;
  }

  map <int, vector <CString> >::const_iterator it = m_map_dial.find(control->m_id);

  if (it == m_map_dial.end())  {
    it = m_map_dial.find(control->m_main->m_id);
  }


  if (it != m_map_dial.end())  {

    vector <CString> tok = it->second;

    if (tok.size() >= 2) {
      val = tok[1];

      if ((val == "File" || val == "PicBuf" )  && tok.size() >= 4) {
        CString path = "";
        if (tok.size() >= 5) {
          path = getPath(tok[4]);
        }

        CString ext = tok[2];
        CString filter = tok[3];
        int sz = tok.size();

        val = getValue(control->m_main);
        if (val == "") {

          if (IsBufDib() && AfxMessageBox("Взять картинку из буфера?", MB_YESNO) == IDYES) {
            
            CString initF;
            int id = getID(control->m_main->m_tn);
            initF.Format("%s-%d.PNG", control->m_main->m_fn, id); 

            CFileDialog fd(FALSE, _T("PNG"), initF, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST, _T("PNG-файл (*.png)|*.png|"), AfxGetMainWnd());
            if (path != "") fd.m_ofn.lpstrInitialDir = path;

            if (fd.DoModal() == IDOK) {
              if (CopyBuf(fd.GetPathName())) {
                val = fd.GetPathName();
                val = TrimPath(val, path);

                setValue(control->m_main, val);
              }
            }
          }
          else {
            CFileDialog fd(TRUE, ext, NULL, OFN_HIDEREADONLY, filter, this);

            if (path != "") fd.m_ofn.lpstrInitialDir = path;

            if (fd.DoModal() == IDOK) {
              val = fd.GetPathName();
              val = TrimPath(val, path);
              setValue(control->m_main, val);
            }
          }
        }
        
        if (!val.IsEmpty()) {

          if (val.Find(":") == -1) val = path+val;

          CString ext = GetExt(val);

          if (!IsFile(val) && (ext == "docx" || ext == "doc")) {
            ::CopyFile(argpath()+"new.docx", val, false);
          }

          HINSTANCE hi = ShellExecute(m_hWnd,"open",val,NULL,NULL,SW_SHOWNORMAL);

          if ((long)hi <= 32) {
            CString str;
            str.Format("Не могу открыть\n%s\nОшибка %d", val, hi);
            AfxMessageBox(str);
          }
        }
      }
      else if (val == "Dir") {
        val = getValue(control->m_main);
        if (val == "") {
          if (GetDirectory(val)) {
            setValue(control->m_main, val);
          }
        }

        if (!val.IsEmpty() && (int)ShellExecute(m_hWnd,"open",val,NULL,NULL,SW_SHOWNORMAL) <= 32) {
          CString str;
          str.Format("Не могу открыть\n%s", val);
          AfxMessageBox(str);
        }
      }
      
      else if (val == "Table2") {
        int nn = 2;
        CString progr = "";

        if (val == "Dialog_Call_Table") {
          progr = tok[nn++];
        }

        CString mdb = tok[nn++];
        CString table = tok[nn++];
        CString id = tok[nn++];
        CString fn = tok[nn++];

        val = getValue(control->m_main);

        mdb = getMdb(mdb);

//        CString val = getValue(control->m_main);
        CString val = control->m_main->m_real_value;

        CTableDialog dlg(this, mdb, table, id, val, table);

        if (dlg.DoModal() == IDOK) {

          control->m_main->m_real_value = dlg.m_val;
          CString s = getViewValue(dlg.m_val, control->m_main->mdb, control->m_main->table, control->m_main->id, control->m_main->fn_id);
          setValue(control->m_main, dlg.m_val);

          int n = m_tab.GetCurSel();

          CFormDialog2 *view = (CFormDialog2 *) m_pFrames[n]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);

          if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
          {
            view->setPS(control->m_main->m_fn, dlg.m_val, control->m_main->mdb, control->m_main->table, control->m_main->id, control->m_main->fn_id);
            view->CbnChange(control->m_main, dlg.m_val);
          }


/*
          setValue(control->m_main, s, dlg.m_val);

          int n = m_tab.GetCurSel();

          CFormDialog2 *view = (CFormDialog2 *) m_pFrames[n]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);

          if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
          {
            view->setPS(control->m_main->m_fn, dlg.m_val, control->m_main->mdb, control->m_main->table, control->m_main->id, control->m_main->fn_id);
          }
*/

        }
      }
      else if (val == "Dialog_Call_Table") {
        int nn = 2;
        CString real;
        CString progr = tok[nn++];

        CString mdb = tok[nn++];
        CString table = tok[nn++];
        CString id = tok[nn++];
        CString fn = tok[nn++];

        val = getValue(control->m_main);

        if (mdb == "PS") {
          mdb = getPS();
        }
        if (mdb == "PS_STRUCT") {
          mdb = getPSAlma();
        }

        CString val = control->m_main->m_real_value;

        CString s = get_progr_val(this, control->m_main, progr, val, m_dbf, control->m_main->m_tn, control->m_main->m_q, control->m_main->m_nom, real);
        if (s != val) {
          if (real == "") {
            setValue(control->m_main, s);
          }
          else {
            setValue(control->m_main, real, s);
          }
        }
      }
      else if (val == "Dialog_Call" || val == "EditCALL") {
        if (tok.size() > 6) {
          CString val = control->m_main->m_real_value;
          CString progr = tok[2];

          CString real[6];
          int nn = tok.size()-3;

          for (int i = 0; i < nn; i++ ) {
            real[i] = getValue(tok[3+i]);
          }

          CString s = get_progr_val6(this, progr, val, m_dbf, control->m_tn, control->m_q, control->m_nom, nn, real[0], real[1], real[2], real[3], real[4], real[5]);
          if (s != val) {
            for (int i = 0; i < nn; i++ ) {
              setValue(tok[3+i], real[i]);
            }
          }
        }
        else if (tok.size() >= 3) {
          CString val = control->m_main->m_real_value;
          CString progr = tok[2];
          CString real;

          if (progr == "dlina") {
            CString s;
            s.Format("%g", m_len);
            setValue(control->m_main, s);
          }
          else {
            CString s = get_progr_val(this, control->m_main, progr, val, m_dbf, control->m_main->m_tn, control->m_main->m_q, control->m_main->m_nom, real);
            if (s != val) {
              if (real == "") {
                setValue(control->m_main, s);
              }
              else {
                setValue(control->m_main, real, s);
              }
            }
          }
        }
      }
/*
      else if (val == "EditCALL") {
        if (tok.size() >= 3) {
          CString val = control->m_main->m_real_value;
          CString progr = tok[2];
          CString real;

          int num = -1;
          num = m_nom[0];

          CString s = get_progr_val(this, progr, val, m_dbf, control->m_main->m_tn, control->m_main->m_q, num, real);
          if (s != val) {
            if (real == "") {
              setValue(control->m_main, s);
            }
            else {
              setValue(control->m_main, real, s);
            }
          }
        }
      }
*/
      else if (val == "2") {
        CMMenu menu(this, IDD_MENU_MULTI, "");
        CControl * cm = control->m_main;

        int j, nf = (cm->tok.size()-2)/2;

        for ( j = 0; j < nf; j++) {
          CString v1 = cm->tok[2+j*2];
          CString v2 = cm->tok[2+j*2+1];

          v1.TrimRight();
          v2.TrimRight();

          menu.Add(v2, 0);
        }
        if (menu.DoModal() == IDOK) {
          setValue(cm, menu.m_strText);
        }
      }
    }
  }
  else {
    setValue(control->m_main, control->m_fieldName);
  }
}

void CDialogNew2::OnFont()
{
}

CString getPSTable(int n);
CString getPSTableGeo(int n);


void CDialogNew2::OnSysCommand(UINT nID, LPARAM lParam)
{
  switch (nID) {

  case ID_FONT :
    {
  //    m_dbtbb->SendMessbge(WM_COMMbND, ID_FONT);

      LOGFONT lf, *pLF = NULL;
      bool LoadFont2(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);
      unsigned long color;

      if (LoadFont2(m_font_save_name, lf, color)) {
        pLF = &lf;
      }

      CFontDialog fd(pLF,  CF_EFFECTS | CF_SCREENFONTS);
  //    fd.m_cf.rgbColors = m_kl->color;

      if (fd.DoModal() == IDOK) {

        fd.GetCurrentFont(&lf);

        for (int i = 0; i < m_n_tab; i++ ) {
          CFormDialog2 *view = (CFormDialog2 *) m_pFrames[i]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
          if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
          {
            view->setFont(lf);
          }
        }
        CRect rect;
        GetClientRect(rect);
        setDialog(rect.Width(), rect.Height()); 
        SaveFont(m_font_save_name, lf, 0);
      }
    }
    break;

  default:
    CDialog::OnSysCommand(nID, lParam);
    break;
  }
}

void CDialogNew2::closeDialog()
{
  WINDOWPLACEMENT wpi;
  GetWindowPlacement(&wpi); 

  AfxGetApp()->WriteProfileInt(szSection1, "nd_bx", wpi.rcNormalPosition.left);
  AfxGetApp()->WriteProfileInt(szSection1, "nd_by", wpi.rcNormalPosition.top);
  AfxGetApp()->WriteProfileInt(szSection1, "nd_cx", wpi.rcNormalPosition.right-wpi.rcNormalPosition.left);
  AfxGetApp()->WriteProfileInt(szSection1, "nd_cy", wpi.rcNormalPosition.bottom-wpi.rcNormalPosition.top);
}

/*

BOOL CDialogNew2::saveWord( CAdoFile *dbf, const char *fn, const char *m_doc_path)
{
  if (m_doc_path != "" && IsFile(m_doc_path)) {
    dbf->Edit();

    dbf->writeFile("Акт шурфа", m_doc_path1);
    dbf->writeFile("Акт осмотра", m_doc_path2);
    dbf->writeFile("Отчет по ремонту", m_doc_path3);
    dbf->writeFile("Анализ проб на влажность", m_doc_path4);
    dbf->writeFile("Анализ на коррозийность", m_doc_path5);

    dbf->Update();

    DeleteFile(m_doc_path1);
    m_doc_path1 = "";
    DeleteFile(m_doc_path2);
    m_doc_path2 = "";
    DeleteFile(m_doc_path3);
    m_doc_path3 = "";
    DeleteFile(m_doc_path4);
    m_doc_path4 = "";
    DeleteFile(m_doc_path5);
    m_doc_path5 = "";
  }
  return TRUE;
}

BOOL CDialogNew2::saveWord( CAdoFile *dbf, const char *fn, CString &m_doc_path)
{
  if (m_doc_path != "" && IsFile(m_doc_path)) {
    dbf->Edit();
    dbf->writeFile(fn, m_doc_path);
    dbf->Update();

    DeleteFile(m_doc_path);
    m_doc_path = "";
  }
  return TRUE;
}

*/


BOOL CDialogNew2::saveDB(const char *tn, CAdoFile *dbf, int row)
{
  CString str = "";

//  saveWord(dbf, "Акт шурфа", m_doc_path1);
//  saveWord(dbf, "Акт осмотра", m_doc_path2);
//  saveWord(dbf, "Отчет по ремонту", m_doc_path3);
//  saveWord(dbf, "Анализ проб на влажность", m_doc_path4);
//  saveWord(dbf, "Анализ на коррозийность", m_doc_path5);
//  saveWord(dbf, "Протокол измерений", m_doc_path6);
//  saveWord(dbf, "Отчет", m_doc_path7);

  map<CString, map < int, map <CString, CString> > > ::const_iterator it0 = m_map_val.find(tn);

  if (it0 == m_map_val.end()) {

    CString tn1 = tn; tn1.MakeUpper();

    it0 = m_map_val.find(tn1);
  }


  if (it0 == m_map_val.end()) return FALSE;

  map < int, map <CString, CString> >::const_iterator it1 = it0->second.find(row);

  if (it1 == it0->second.end()) return FALSE;

  if (m_nom[row] == 0) {
    dbf->AddNew();
  }
  else {
    dbf->Edit();
  }

  map<CString, CString>::const_iterator it;

  for (it = it1->second.begin(); it != it1->second.end(); it++) {
    CString s;
    s.Format("%s %s\n", it->first, it->second);
    str += s;
    CString fn = it->first; fn.Replace("T1.", "");
    dbf->write(fn, it->second);
  }


  dbf->Update();

  return TRUE;
}

BOOL CDialogNew2::saveDB()
{
  return TRUE;
}

BOOL CDialogNew2::saveDB2(const char *tn, const char *q, int nom)
{
  if (!q || !q[0]) return FALSE;
    
  CString que = q;

  int n = 0;

  if (m_dbf->openTable(que)) {
    if (nom == -1) {
      if (!m_dbf->rs->IsEOF()) {
        while ( !m_dbf->rs->IsEOF() ) {
          saveDB(tn, m_dbf, n++);

          m_dbf->rs->MoveNext();
        }
      }
    }
    else {
      if (GotoKey(m_dbf, nom)) {
        saveDB(tn, m_dbf, n++);
      }
    }
    m_dbf->closeTable();
  }
  return TRUE;
}

BOOL CDialogNew2::saveDB2()
{
  for (int i = 0; i < m_n_q; i++) {
    saveDB2(m_tn[i], m_q[i], m_nom[i]);
  }

  return TRUE;
}


void CDialogNew2::OnClose() 
{
  closeDialog();
  CDialog::OnClose();
}

void CDialogNew2::OnCancel() 
{
//  if (check_data) {
//    if (check_data(this)) return;
//  }

  if (isChange()) {
    if (AfxMessageBox("Отказаться от изменений?",  MB_YESNO) != IDYES ) return;
  }

  closeDialog();
  CDialog::OnCancel();
}

int editQ_2(CWnd *wnd, CAdoFile &dbf, const char *tn, const char *tn1, const char *qq1, int nom1, const char *tn2, const char *qq2, int nom2, int readOnly, const char *cap, int idd)
{
  int ret = IDCANCEL;

  CDialogNew2 dlg(wnd, &dbf, tn, readOnly, cap, idd);
  dlg.addQ(tn1, qq1, nom1);
  dlg.addQ(tn2, qq2, nom2);

  ret = dlg.DoModal();
  if (ret != IDCANCEL) {
    dlg.saveDB2();
  }
  return ret;
}

int editQ_2(CWnd *wnd, const char *mdb, const char *tn, const char *tn1, const char *q1, int nom1, const char *tn2, const char *q2, int nom2, int readOnly, const char *cap, int idd)
{
  int ret = IDCANCEL;

  CAdoFile dbf;
  if (dbf.openMDB(mdb)) {
    ret = editQ_2(wnd, dbf, tn, tn1, q1, nom1, tn2, q2, nom2, readOnly, cap, idd);
    dbf.closeMDB();
  }
  return ret;
}

int editQ_1(CWnd *wnd, CAdoFile &dbf, const char *tn, const char *qq1, int nom1, int readOnly, const char *cap, int idd)
{
  int ret = IDCANCEL;

  CDialogNew2 dlg(wnd, &dbf, tn, readOnly, cap, idd);
  dlg.addQ(tn, qq1, nom1);

  ret = dlg.DoModal();
  if (ret != IDCANCEL) {
    dlg.saveDB2();
  }
  return ret;
}

int editQ_1(CWnd *wnd, const char *mdb, const char *tn, const char *q1, int nom1, int readOnly, const char *cap, int idd)
{
  int ret = IDCANCEL;

  CAdoFile dbf;
  if (dbf.openMDB(mdb)) {
    ret = editQ_1(wnd, dbf, tn, q1, nom1, readOnly, cap, idd);
    dbf.closeMDB();
  }
  return ret;
}

int editQ_1(CWnd *wnd, CAdoFile &dbf, const char *tn, int nom1, int readOnly, const char *cap, int idd)
{
  int ret = IDCANCEL;
  CDialogNew2 dlg(wnd, &dbf, tn, readOnly, cap, idd);

//  CString qq1;
///  qq1.Format("SELECT * FROM [%s] WHERE [%s]=%d", tn, qqqq, nom1);

  dlg.addQ(tn, tn, nom1);

  ret = dlg.DoModal();
  if (ret != IDCANCEL) {
    dlg.saveDB2();
  }
  return ret;
}

int editQ_1(CWnd *wnd, const char *mdb, const char *tn, int nom1, int readOnly, const char *cap, int idd)
{
  int ret = IDCANCEL;

  CAdoFile dbf;
  if (dbf.openMDB(mdb)) {
    ret = editQ_1(wnd, dbf, tn, nom1, readOnly, cap, idd);
    dbf.closeMDB();
  }
  return ret;
}


void CDialogNew2::sosto(const char *op)
{
  CString d1 = "";

  CControl *c1 = getControl("Дата аннулирования");
  if (c1) {
    d1 = getValue(c1);
    ActivateFrame(c1->m_frame);
    c1->m_wnd->SetFocus();
    AfxMessageBox("Введите дату операции");
  }

  CControl *cntrl = getControl("Состояние договора");
  if (cntrl) {
    setValue(cntrl, op);
  }
//  if (saveDialog())
//    CDialog::OnOK();
}

void CDialogNew2::OnAnnul()
{
  sosto("А");
}

void CDialogNew2::OnMon()
{
  sosto("М");
}

void CDialogNew2::OnZav()
{
//  sosto("З");
  CString d1 = "";

  CControl *c1 = getControl("Дата выдачи акта допуска");
  if (c1) {
    d1 = getValue(c1);
    ActivateFrame(c1->m_frame);
    c1->m_wnd->SetFocus();
    AfxMessageBox("Введите дату операции");
  }

  CControl *cntrl = getControl("Состояние договора");
  if (cntrl) {
    setValue(cntrl, "З");
  }
}

void CDialogNew2::OnDeistv()
{
  sosto("Д");
}

void reads_tok(CString sss, CString field_name, const char *field, vector <CString> &tok)
{
  char str[1024];
  char seps[]   = "\t";
  char *token;
  int n = 0;


  for (int i = 0, k = 0; field[k]; i++, k++) {
    str[i] = field[k];
    if (field[k] == ' ' && field[k+1] == ' ') {
      str[i] = '\t';
      for (; field[k] == ' '; k++);
      k--;
    }
  }
  str[i] = 0;

  tok.push_back(field_name);
  tok.push_back(sss);

  token = strtok( str, seps );

  while( token != NULL )
  {
    if (!strcmp(token, "@")) {
      tok.push_back(CString(" "));
    }
    else {
      tok.push_back(CString(token));
    }
    token = strtok( NULL, seps );
    n++;
  }
}

void setEdit(CFormDialog2 *view, ROW2 &db_row, CControl *cnt, const char *tn , const char *field_name, int ID,  map<int, vector<CString> > &m_map_dial, int rjad, CString call, CString FileExt, CString FileFilter, CString FilePath)
{
  MFIELDS map1;

  cnt->m_tn = tn;

  int is_e = FALSE;

  FIELD fld;
  if (!db_row.getField(tn, field_name, fld, rjad)) return;

  if (ID == ID2_ReadOnly) {
    cnt->m_is_edit = false;
  }

  if (fld.typ == dbBoolean) {
    cnt->m_ctrl = TIP_CHECKBOX;
  }

  cnt->m_value = fld.val;
  cnt->m_new_value = cnt->m_value;

  cnt->m_typ = fld.typ;
  cnt->m_len = fld.size;
  cnt->m_tn = tn;
  cnt->m_fn = field_name;
  cnt->m_fieldName = field_name;

  cnt->m_rjad = rjad;

//  cnt->m_podp = obj.Caption;
//  cnt->m_num = n2;

  int ret = getMap(tn, field_name, map1);

  bool isButton = false;

  if (ret) {
//    bool isComboEdit = false;

    if (ID == ID2_ComboEdit2 ) {
      isButton = true;
    }
    else {
      int typ = 0;
      if (ID == ID2_ComboEdit) {
        typ = TIP_COMBOEDIT;
      }
      else {
        typ = TIP_COMBO;
      }
      cnt->m_ctrl = typ;
    }

    vector <CString> tok2;
    list<pair<CString, CString> >::const_iterator it2;

    tok2.push_back("1");
    tok2.push_back("2");

    for (it2 = map1.list2.begin(); it2 != map1.list2.end(); it2++) {
      CString s1 = it2->first;
      CString s2 = it2->second;
      tok2.push_back(s1);
      tok2.push_back(s2);
    }
    cnt->tok = tok2;
    cnt->m_value = fld.val;
    cnt->m_new_value = cnt->m_value;

    m_map_dial[cnt->m_id] = tok2;
  }

  cnt->m_file_ext = FileExt;
  cnt->m_file_Filter = FileFilter;
  cnt->m_file_path = FilePath;

  if (fld.typ == dbDate && ID != ID2_ReadOnly) isButton = true;

  if (ID == ID2_File || ID == ID2_Dir || ID == ID2_Picture) {
    isButton = true;
    cnt->m_is_file = true;

    vector <CString> tok2;

    tok2.push_back(cnt->m_fn);
    tok2.push_back("File");

    tok2.push_back(FileExt);
    tok2.push_back(FileFilter);
    tok2.push_back(FilePath);

    m_map_dial[cnt->m_id] = tok2;
  }


  if (call != "") {
//    m_fn_dial.push_back(field_name);
    vector <CString> tok;

    reads_tok("Dialog_Call", field_name,  call, tok);

    m_map_dial[cnt->m_id] = tok;

    cnt->m_is_file = true;


    isButton = true;
  }


  if (isButton) {
    CControl *cnt2 = view->addControlObj2(TIP_BUTTON);
    if (cnt2) {
      cnt2->m_podp = "...";
      cnt2->m_bkColor = cnt->m_bkColor;
      cnt2->m_ctrl = TIP_BUTTON;
      cnt2->m_main = cnt;
      cnt2->m_is_edit = cnt->m_is_edit;
      cnt2->m_tn = cnt->m_tn;
      cnt2->m_fn = cnt->m_fn;
      cnt2->m_fieldName = cnt->m_fieldName;
      cnt2->m_rjad = cnt->m_rjad;
    }
  }
}

BOOL CDialogNew2::viewTable2(CFormDialog2 *view, const char *tn, const char *title, list<OBJ2> & l_ob)
{
  int n2 = 1;
  bool isEdit = !m_readOnly;

  CControl *cnt;

  int x = 0;
  int y = 0;
  int width = 200;
  int height = 20;

  int x0 = 0;
  int y0 = 0;

  int row0 = 0;
  int col0 = 0;

  int row = 0;
  int col = 0;

  int cols = 1;
  int rows = 1;

  int font_h = 18;

  bool bold = false;
  bool italic = false;

  int col_w[N256], row_h[N256];
  CString table = tn;
  CString query = "";

  int i;

  for (i = 0; i < N256; i++) col_w[i] = 0;
  for (i = 0; i < N256; i++) row_h[i] = 0;

  int rjad = 0;
  int loop = 0;
  bool is_loop = false;

  CString call = "";

  CString FileExt = "JPG";
  CString FileFilter = "Схемы|*.jpg;*gif;*png;*.doc;*.docx|";
  CString FilePath = "PATH_CXEMA";


  COLORREF bkColor = 0xFFFFFF;

  list<OBJ2>::const_iterator loop_it;
  list<OBJ2>::const_iterator it = l_ob.begin();

  for (; it != l_ob.end(); it++) {
    OBJ2 obj = *it;

    if (loop == 0 && is_loop && obj.ID != ID2_EndLoop) {
      continue;
    }

    if (obj.ID == ID2_Set) {
  //        x = x0 = obj.col;
  //        y = y0 = obj.row;

      col = col0 = obj.col;
      row = row0 = obj.row;
    }

    else if (obj.ID == ID2_Loop) {
      is_loop = true;
      loop = obj.row;
      loop_it = it;
    }
    else if (obj.ID == ID2_LoopTable) {
      is_loop = true;
      loop = db_row.getRecs(table);
      loop_it = it;
    }
    else if (obj.ID == ID2_EndLoop) {
      loop --;
      if (loop > 0) {
        it = loop_it;
      }
      else {
        is_loop = false;
      }
    }

    else if (obj.ID == ID2_EditCall) {
      call = obj.text;
    }
/*
    else if (obj.ID == ID2_Call) {
      call = obj.text;

      if (call == "view") {
        readWord(db_row, tn, "Акт шурфа", m_doc_path1);
      }
      else if (call == "akt_osm_def_view") {
        readWord(db_row, tn, "Акт осмотра", m_doc_path2);
      }
      else if (call == "view_report") {
        readWord(db_row, tn, "Отчет по ремонту", m_doc_path3);
      }
    }
*/

    else if (obj.ID == ID2_FileExt) {
      FileExt = obj.text;
    }
    else if (obj.ID == ID2_FileFilter) {
      FileFilter = obj.text;
    }
    else if (obj.ID == ID2_FilePath) {
      FilePath = obj.text;
    }
    else if (obj.ID == ID2_Table) {
      table = obj.text;
      rjad = 0;
    }
    else if (obj.ID == ID2_NextRow) {
      rjad ++;
    }
    else if (obj.ID == ID2_FirstRow) {
      rjad = 0;
    }
    
    else if (obj.ID == ID2_Font) {
      font_h = obj.col;
    }
    else if (obj.ID == ID2_Bold) {
      bold = true;
    }
    else if (obj.ID == ID2_Italic) {
      italic = true;
    }
    else if (obj.ID == ID2_Normal) {
      bold = false;
      italic = false;
    }
    
    else if (obj.ID == ID2_Cols) {
      cols = obj.col;
    }
    else if (obj.ID == ID2_BkColor) {
      bkColor = obj.col;
    }
    else if (obj.ID == ID2_Rows) {
      rows = obj.row;
    }
    else if (obj.ID == ID2_Width) {
      if (obj.col >= 0 && obj.col < N256) {
        col_w[obj.col] = obj.width;
      }
    }
    else if (obj.ID == ID2_Height) {
      if (obj.row >= 0 && obj.row < N256) {
        row_h[obj.row] = obj.height;
      }
    }
    else if (obj.ID == ID2_Enter) {
      col = col0;
      row += rows;
    }
    else if (obj.ID == ID2_Label) {
      cnt = view->addControlObj2(TIP_PODPIS);
      if (cnt) {
        cnt->m_bold = bold;
        cnt->m_italic = italic;
        cnt->m_bkColor = bkColor;
        cnt->m_fieldName = obj.text;
        cnt->m_fn = obj.text;
        cnt->m_row = row;
        cnt->m_col = col;
        cnt->m_cols = cols;
        cnt->m_rows = rows;
        col += cols;
      } 
    }

    else if (obj.ID == ID2_DbQuery) {
      query = obj.text;
    }
    else if (obj.ID == ID2_DbTable) {
      cnt = view->addControlObj2(TIP_TABLE);
  //        cnt = view->addControl(TIP_TABLE, fn, m_row , m_cols, m_cols);
      if (cnt) {
  //          cnt->tok = tok2;
  //          cnt->m_value = val[j];
  //          cnt->m_new_value = cnt->m_value;
  //          cnt->m_tn = tnU;

        if (obj.text != "") {
          CString val1 = getValue("ID");
          query.Replace("$1", val1);
        }

        cnt->m_tn = tn;
        cnt->m_fieldName = obj.text;
        cnt->m_q = query;
        cnt->m_row = row;
        cnt->m_col = col;
        cnt->m_cols = cols;
        cnt->m_rows = rows;
        cnt->m_is_edit = isEdit;
        cnt->m_num = n2;
        col += cols;
      }
    }
    else if (obj.ID == ID2_Edit || obj.ID == ID2_ReadOnly || obj.ID == ID2_ComboEdit2 || obj.ID == ID2_ComboEdit || obj.ID == ID2_File || obj.ID == ID2_Dir || obj.ID == ID2_Picture) {
      cnt = view->addControlObj2(TIP_EDIT);
      if (cnt) {
        cnt->m_bkColor = bkColor;
        cnt->m_fieldName = obj.text;
        cnt->m_fn = obj.text;
        cnt->m_row = row;
        cnt->m_col = col;
        cnt->m_cols = cols;
        cnt->m_rows = rows;
        cnt->m_is_edit = isEdit;
        cnt->m_num = n2;
        col += cols;
        cnt->m_rjad = rjad;
        setEdit(view, db_row, cnt, table, obj.text, obj.ID, m_map_dial, rjad, call, FileExt, FileFilter, FilePath);
        call = "";
      }
    }
    else if (obj.ID == ID2_Button) {
      cnt = view->addControlObj2(TIP_BUTTON2);
      if (cnt) {
        cnt->m_bkColor = bkColor;
        cnt->m_main = NULL;
        cnt->m_podp = obj.text;
        cnt->m_fieldName = obj.text;
        cnt->m_fn = obj.text;
        cnt->m_row = row;
        cnt->m_col = col;
        cnt->m_cols = cols;
        cnt->m_rows = rows;
        cnt->m_rjad = rjad;
        col += cols;

        if (call != "") {
          vector <CString> tok;
          reads_tok("Dialog_Call", cnt->m_fieldName,  call, tok);
          m_map_dial[cnt->m_id] = tok;
          call = "";
        }
      }
    }
  }

  for (i = 0; i < N256; i++) {
    if (row_h[i] == 0) {
      row_h[i] = font_h+4;
    }
  }
  view->setFontH(font_h);
  view->setXY(row_h, col_w);

  return TRUE;
}


CFormDialog2 * CDialogNew2::viewTable2(const char *tn, const char *title, list<OBJ2> & l_ob)
{
  CFormDialog2 *view = (CFormDialog2 *) m_pFrames[m_n_tab]->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
  if (view != NULL && view->IsKindOf(RUNTIME_CLASS(CFormDialog2)))
  {
    int n2 = 1;

    m_tab.InsertItem(m_n_tab, title);
    m_n_tab++;

    viewTable2(view, tn, title, l_ob);

    m_n_tab--;
    m_pFrames[m_n_tab]->InitialUpdateFrame( NULL, TRUE );
    m_n_tab++;
  }
  return view;
}

int get_id_txt2(const char *fn);

void read_txt2_2(FILE *f, list<OBJ2>& l_ob)
{
  CString S;
  char str[10240];

  l_ob.clear();

  if (f) {
    while (!feof(f)) {
      fgetstr(str, 10230, f);
      if (feof(f)) break;
      if (str[0] == '#') break;

      S = str;
      S.TrimLeft();
      S.TrimRight();

      char cod[256];
      const char * s = (const char*)S;

      for (int i = 0; s[i] && s[i] != ' '; i++) {
      }

      strncpy(cod, s, i); cod[i] = 0;

      int ID = get_id_txt2(cod);
      int x, y;

//      char cod[256];
//      strncpy(cod, s, i); cod[i] = 0;

      OBJ2 obj;

      obj.ID = ID;

      switch(ID) {
      case ID2_Picture:
      case ID2_File:
      case ID2_Dir:
        obj.text = &s[i+1];
        break;

      case ID2_FileExt:
      case ID2_FileFilter:
      case ID2_FilePath:

      case ID2_EditCall :
      case ID2_Call :
      case ID2_Label :
      case ID2_Button :
      case ID2_ReadOnly :
      case ID2_Edit :
      case ID2_ComboEdit :
      case ID2_ComboEdit2 :
      case ID2_Table :
      case ID2_DbQuery :
      case ID2_DbTable :
        obj.text = &s[i+1];
        break;
      case ID2_Enter :
        break;
      case ID2_Set:
        if (sscanf(&s[i+1], "%d %d", &x, &y) == 2) {
          obj.row = y;
          obj.col = x;
        }
        break;
      case ID2_Font:
        if (sscanf(&s[i+1], "%d", &x) == 1) {
          obj.col = x;
        }
        break;
      case ID2_BkColor:
        if (sscanf(&s[i+1], "%x", &x) == 1) {
          obj.col = x;
        }
        break;

      case ID2_Cols:
        if (sscanf(&s[i+1], "%d", &x) == 1) {
          obj.col = x;
        }
        break;
      case ID2_Rows:
        if (sscanf(&s[i+1], "%d", &x) == 1) {
          obj.row = x;
        }
        break;
      case ID2_Loop:
        if (sscanf(&s[i+1], "%d", &x) == 1) {
          obj.row = x;
        }
        break;
      case ID2_EndLoop:
        break;
      case ID2_Width:
        if (sscanf(&s[i+1], "%d %d", &x, &y) == 2) {
          obj.col = x;
          obj.width = y;
        }
        break;
      case ID2_Height:
        if (sscanf(&s[i+1], "%d %d", &x, &y) == 2) {
          obj.row = x;
          obj.height = y;
        }
        break;
      }
      if (ID > 0) {
        l_ob.push_back(obj);
      }
    }
//    fclose(f);
  }
}

void CDialogNew2::OnTimer(UINT nIDEvent) 
{
  if (nIDEvent == m_nTimerWord)
  {
    HWND wnd = ::FindWindowEx(0, 0, "OpusApp", NULL);
    bool fnd = false;

    if (wnd) {
      CString cap;
      CWnd::FromHandle(wnd)->GetWindowText(cap);

      if (cap.Find(m_doc_name) != -1) fnd = true;
    }
    
    if (!wnd || !fnd) {
      KillTimer(m_nTimerWord);
      m_nTimerWord = 0;
//      if (AfxMessageBox("Сохранить документ в Базу данных?", MB_YESNO) != IDYES) {
//         DeleteFile(m_doc_path);
//      }
    }
  }
  
  CDialog::OnTimer(nIDEvent);
}

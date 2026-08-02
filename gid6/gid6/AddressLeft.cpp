// Address3.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "AddressLeft.h"
#include "gidrview.h"

#include "geofile.h"

#include "ado.h"

#include "wm_user.h"

#define GOROD _TR("Город")

//static list<CString> m_list_r;
//static list<pair<CString, CString> > m_list_u;


/////////////////////////////////////////////////////////////////////////////
// CAddressLeft property page

//IMPLEMENT_DYNCREATE(CAddressLeft, CPaneDialog)

CAddressLeft::CAddressLeft(int is_nagr) : CPaneDialog()
{
  //{{AFX_DATA_INIT(CAddressLeft)
//  m_street_find = "";
  //}}AFX_DATA_INIT
  m_ado = NULL;
  m_kl_list = NULL;
  m_bInit = FALSE;
  m_is_nagr = is_nagr;

  m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_DBTYPE));
}

CAddressLeft::~CAddressLeft()
{
}

void CAddressLeft::DoDataExchange(CDataExchange* pDX)
{
  CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAddressLeft)
  DDX_Control(pDX, IDC_STREET_FIND, m_ctrl_street_find);
  DDX_Control(pDX, IDC_STREET, m_ctrl_street);
  DDX_Control(pDX, IDC_RAJ, m_ctrl_raj);
  DDX_Control(pDX, IDC_DOM, m_ctrl_dom);
//  DDX_Text(pDX, IDC_STREET_FIND, m_street_find);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddressLeft, CPaneDialog)
  //{{AFX_MSG_MAP(CAddressLeft)
  ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
  ON_WM_SIZE()
  ON_LBN_SELCHANGE(IDC_RAJ, OnSelchangeRaj)
  ON_LBN_SELCHANGE(IDC_STREET, OnSelchangeStreet)
  ON_LBN_SELCHANGE(IDC_DOM, OnSelchangeDom)
  ON_EN_CHANGE(IDC_STREET_FIND, OnChangeStreetFind)
  ON_WM_CONTEXTMENU()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressLeft message handlers

void CAddressLeft::OnSize(UINT nType, int cx, int cy) 
{
  CPaneDialog::OnSize(nType, cx, cy);

//  if(!m_bInit)
//    return;

  if (!m_ctrl_raj.m_hWnd) return;

  CRect sz, sz1, sz2, sz3, sz4;

  GetClientRect(sz);

  int h = sz.bottom;
  int w = sz.right;

  sz1 = sz; sz1.bottom = h/3-28;
//  sz2 = sz; sz2.top = h/3;  sz2.bottom = h*2/3;
//  sz3 = sz; sz3.top = h*2/3;

  sz4 = sz; sz4.top = h/3-28; sz4.bottom = h/3;

  sz2 = sz; sz2.top = h/3; sz2.right = w*2/3;
  sz3 = sz; sz3.top = h/3; sz3.left = w*2/3;

//  sz1.DeflateRect(3,3);
//  sz2.DeflateRect(3,3);
//  sz3.DeflateRect(3,3);
//  sz4.DeflateRect(3,3);


  m_ctrl_raj.MoveWindow(sz1);
  m_ctrl_street_find.MoveWindow(sz4);
  m_ctrl_street.MoveWindow(sz2);
  m_ctrl_dom.MoveWindow(sz3);

 

  // TODO: Add your message handler code here
  
}

LRESULT CAddressLeft::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);

  Init();
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddressLeft::SetKls(KlList *kl_list)
{
  m_kl_list = kl_list;

//  if (!m_bInit) {
  Init();
//  }
}

bool CAddressLeft::Init(const char *cxema, const char *tn, const char *raj, const char *str, const char *dom)
{
  m_bInit = FALSE;

  if (!m_kl_list) return false;

  Klassif *kls = m_kl_list->findKlN(tn);

  if (!kls) return false;

  m_ctrl_raj.ResetContent();
  m_ctrl_street.ResetContent();
  m_ctrl_dom.ResetContent();

  
  m_tn = tn;
  m_raj = raj;
  m_str = str;
  m_nomer = dom;
  
  
  m_ado = kls->m_ado;

  m_list_r.clear();
  m_list_u.clear();

  CString q;

  q.Format("select distinct %s, %s from %s ORDER BY %s,%s", m_raj, m_str, m_tn, m_raj, m_str);

  if (m_raj == "") {
    q.Format("select distinct 'город', %s from %s ORDER BY %s", m_str, m_tn, m_str);
  }

  bool yes = false;


  if (m_ado->openTable0(q)) {
    while ( !m_ado->isEOF() ) {
      CString s1 = m_ado->readStr(0);
      CString s = m_ado->readStr(1);

//      COleVariant var = m_ado->read(1);
      
      if (s != "") {
        CString ss;
        ss.Format("%s %s", s1, s);
        m_ctrl_street.AddString(s);

/*

        LVITEMW lvi;

        m_ctrl_street.SendMessage(LVM_GETITEMW, (WPARAM)0, (LPARAM)&lvi);
*/

/*
        m_ctrl_street.SendMessage(LVM_SETITEMW, (WPARAM)0, (LPARAM)&lvi);

        

        lvi.cchTextMax = SysStringLen(var.bstrVal);
        lvi.pszText = var.bstrVal;



        m_ctrl_street.SendMessage(LVM_SETITEMW, (WPARAM)0, (LPARAM)&lvi);
*/

        




        m_list_u.push_back(pair<CString, CString> (s1, s));
      }
      m_ado->MoveNext();
      yes = true;
    }
    m_ado->closeTable();
  }

/*
  str.Format("select distinct %s from %s", m_str, m_tn);
  if (m_ado->openTable(str, TRUE)) {
    if (!m_ado->isEOF()) {
      while ( !m_ado->isEOF() ) {
        m_ado->read(1, s);
        if (s[0]) {
          m_ctrl_street.AddString(s);
        }
        m_ado->MoveNext();
      }
    }
    m_ado->closeTable();
  }
*/



  m_ctrl_raj.ResetContent();
  m_ctrl_raj.AddString(GOROD);

  q.Format("select distinct %s from %s where %s <> ''", m_raj, m_tn, m_str);
  q.Format("select distinct %s from %s ORDER BY %s", m_raj, m_tn, m_raj);
  if (m_ado->openTable0(q)) {
    while ( !m_ado->isEOF() ) {
      CString s = m_ado->readStr(0);
      if (s != "") {
        m_ctrl_raj.AddString(s);
        m_list_r.push_back(s);
      }
      m_ado->MoveNext();
    }
    m_ado->closeTable();
  }
  m_bInit = TRUE;

  return yes;
}

const CString szSection1 = "AddressInit";

//bool CAddressLeft::Init(const char *cxema, const char *tn, const char *raj, const char *str, const char *dom)

void CAddressLeft::Init()
{
/*

  if (!Init("", transl("Здания"), "NAME_MASSI", "NAME_STREE", "NUMBER_1"))
  if (!Init("", transl("Здания потребителей"), transl("Микрорайон"), transl("Улица"), transl("Дом")))
  if (!Init("", transl("Здания Карта"), "ID_ADR_MAS", "STREET_NAM", "NUMBER_1"))
  if (!Init("", transl("Строения"), "", transl("Улица"), transl("Дом")))
  if (!Init("", "building_polygon", "", "A_STRT", "A_HSNMBR")) {

*/

//     CString cxema = AfxGetApp()->GetProfileString(szSection1, "cxema", "");

     CString tn = AfxGetApp()->GetProfileString(szSection1,  "tn", "");

//     CString sz;
//     sz.Format("%s\\%s", szSection1, tn);

     CString raj = "", str = "", dom = "", cxema = "";

     void read_address_ini(CString tn, CString &raj, CString & str, CString & dom);

     read_address_ini(tn, raj, str, dom);


//     raj = AfxGetApp()->GetProfileString(sz, "raj", "");
//     str = AfxGetApp()->GetProfileString(sz, "str", "");
//     dom = AfxGetApp()->GetProfileString(sz, "dom", "");
     Init(cxema, tn, raj, str, dom);



//  }
}



void CAddressLeft::writeStreet(const char *raj, const char *pic) 
{
  m_ctrl_street.ResetContent();

//  list<pair<CString, CString> >::const_iterator it;

//    for (it = m_list_u.begin(); it != m_list_u.end(); it++) {

    set<CString> set_str;

    for (const auto & it :  m_list_u) {

    //    if (!strcmp(it->first, raj) || (!strcmp(raj, GOROD) && it->first == "")) {
    if (!strcmp(it.first, raj) || !strcmp(raj, GOROD)) {
      CString s = it.second, p = pic;
      s.MakeUpper();
      p.MakeUpper();
      if (p == "" || s.Find(p) >= 0) {
        if (s != "") {
//            m_ctrl_street.AddString(it.second);
            set_str.insert(it.second);
        }
      }
    }
  }

  for (const auto &it :  set_str) {
    m_ctrl_street.AddString(it);
  }



  return;

  CString str;

  str.Format("select distinct %s from %s", m_str, m_tn);

  if (strcmp(raj, GOROD)) {
    CString q;
   
    q.Format(" where %s = '%s'", m_raj, raj);
    str += q;
  }

  if (pic) {
    if (strcmp(raj, GOROD)) str += " AND ";
    else str += "WHERE ";

    CString q;
   
    q.Format(" %s LIKE '*%s*'", m_str, pic);
    str += q;
  }

  if (m_ado->openTable0(str)) {
    while ( !m_ado->isEOF() ) {
      CString s = m_ado->readStr(0);
      if (s != "") m_ctrl_street.AddString(s);

      m_ado->MoveNext();
    }
    m_ado->closeTable();
  }
}



/*
void CAddressLeft::writeStreet(const char *raj, const char *pic) 
{
  m_ctrl_street.ResetContent();

  CString str;
  char s[256];

  str.Format("select distinct [%s] from [%s]", m_str, m_tn);

  if (strcmp(raj, GOROD)) {
    CString q;
   
    q.Format(" where [%s] = '%s'", m_raj, raj);
    str += q;
  }

  if (pic) {
    if (strcmp(raj, GOROD)) str += " AND ";
    else str += "WHERE ";

    CString q;
   
    q.Format(" [%s] LIKE '*%s*'", m_str, pic);
    str += q;
  }

  if (m_ado->openTable(str, TRUE)) {
    if (!m_ado->isEOF()) {

//      m_ado->MoveFirst();

      while ( !m_ado->isEOF() ) {
        m_ado->read(1, s);
        if (s[0]) m_ctrl_street.AddString(s);

        m_ado->MoveNext();
      }
    }
    m_ado->closeTable();
  }
}
*/


int decode_dom(const char *s)
{
  return atoi(s);
}


struct less_dom
{
    using result_type = bool;
using first_argument_type = const CString&;
using second_argument_type = const CString&;
    result_type operator() ( const CString arg1, const CString & arg2) const
    {
        int n1 = decode_dom(arg1);
        int n2 = decode_dom(arg2);

        if (n1 < n2) return true;
        if (n1 > n2) return false;

        return arg1 < arg2;
    }
};



void CAddressLeft::OnSelchangeRaj() 
{
  if (!m_bInit) return;

  int i;
  CString str, street, raj;

  if (!m_ado) return;

  UpdateData(TRUE);

  int n = m_ctrl_street.GetCount();

  for (i=0;i < n;i++) {
    m_ctrl_street.DeleteString( 0 );
  }


  m_ctrl_raj.GetText( m_ctrl_raj.GetCurSel() , raj );

  writeStreet(raj);

  if (raj != GOROD) {
    double x1 = 1e10, x2 = -1e10, y1 = 1e10, y2 = -1e10;

    KlList *m_kl_list = m_geofile->m_kl_list;
    Klassif *kls = m_kl_list->findKlN(m_tn);

    if (kls) {
      int n = kls->geo4.size();
      for ( i = 0; i < n; i++ ) {
        kls->geo4[i]->mark = false;
      }
    }


    str.Format("select %s, ID from %s where %s = '%s'", m_nomer, m_tn, m_raj, raj);

    if (m_ado->openTable0(str)) {
      if (!m_ado->isEOF()) {

  //      m_ado->MoveFirst();

        while ( !m_ado->isEOF() ) {
          CString s = m_ado->readStr(0);
          long id = m_ado->read_long(1);

          if (kls) {
            int id2 = kls->GetNom2(id);
            if (id2 > 0) {
              kls->geo4[id2-1]->mark = true;
              CFRect rect = kls->geo4[id2-1]->rect;

              if (rect.left != 0 || rect.right != 0 || rect.top != 0 || rect.bottom != 0) {
                x1 = min(x1, rect.left);
                y1 = min(y1, -rect.bottom);
                x2 = max(x2, rect.right);
                y2 = max(y2, -rect.top);
              }
            }
          }
          m_ado->MoveNext();
        }
      }
      m_ado->closeTable();
    }


    set<CString, less_dom> set_str;

    str.Format("select distinct %s from %s where %s = '%s' and %s is null", m_nomer, m_tn, m_raj, raj, m_str);

    m_ctrl_dom.ResetContent();

    if (m_ado->openTable0(str)) {
      if (!m_ado->isEOF()) {

        while ( !m_ado->isEOF() ) {
          CString s = m_ado->readStr(0);
          if (s != "") set_str.insert(s);
          m_ado->MoveNext();
        }
      }
      m_ado->closeTable();
    }

    set<CString, less_dom>::const_iterator it = set_str.begin();

    for (; it != set_str.end(); it++) {
       m_ctrl_dom.AddString(*it);
    }


    CGidrView *pView = getView();

    if (pView && x1 != 1e10) {
  //    pView->moveXY(CFPoint((x1+x2)/2, -(y1+y2)/2));
      pView->MoveRect(CFRect(x1, -y2, x2, -y1));
      pView->moveXY(CFPoint((x1+x2)/2, -(y1+y2)/2));
      return;
    }
  }

}

void CAddressLeft::OnSelchangeStreet() 
{
  if (!m_bInit) return;
  
  int i;
  CString str, street;

  if (!m_ado) return;
  
  UpdateData(TRUE);

  int n = m_ctrl_dom.GetCount();

  for (i=0; i < n;i++) {
    m_ctrl_dom.DeleteString( 0 );
  }

  bool is_mark = false;

  m_ctrl_street.GetText( m_ctrl_street.GetCurSel() , street );


  KlList *m_kl_list = m_geofile->m_kl_list;
  Klassif *kls = m_kl_list->findKlN(m_tn);

  CGidrView *pView = getView();

  if (pView) {
    pView->m_isMarkOnly = 1;

    pView->Invalidate(FALSE);
    pView->UpdateWindow();
  }

//  m_geofile->m_kls000 = kls;

  if (kls) {
    n = kls->geo4.size();
    for ( i = 0; i < n; i++ ) {
      kls->geo4[i]->mark = false;
    }
  }

/*

//  str.Format("select distinct [%s], ID from [%s] where [%s] = '%s'", m_nomer, m_tn, m_str, street);
  str.Format("select distinct [%s] from [%s] where [%s] = '%s'", m_nomer, m_tn, m_str, street);

  m_ctrl_dom.ResetContent();

  if (m_ado->openTable(str, TRUE)) {
    if (!m_ado->isEOF()) {

//      m_ado->MoveFirst();

      while ( !m_ado->isEOF() ) {
        m_ado->read(1, s);
        if (s[0]) m_ctrl_dom.AddString(s);
        m_ado->MoveNext();
      }
    }
    m_ado->closeTable();
  }
*/

  CString raj;

  int nr = m_ctrl_raj.GetCurSel();

  if (nr < 0) nr = 0;

  m_ctrl_raj.GetText( nr , raj );

  set<CString, less_dom> set_str;

  str.Format("select distinct %s from %s where %s = '%s'", m_nomer, m_tn, m_str, street);

  if (raj != GOROD) {
    str.Format("select %s, ID from %s where %s = '%s' and %s = '%s'", m_nomer, m_tn, m_str, street, m_raj, raj);
    str.Format("select distinct %s from %s where %s = '%s' and %s = '%s'", m_nomer, m_tn, m_str, street, m_raj, raj);
  }
  else {
    str.Format("select distinct %s from %s where %s = '%s'", m_nomer, m_tn, m_str, street);
  }


  m_ctrl_dom.ResetContent();

  if (m_ado->openTable0(str)) {
    while ( !m_ado->isEOF() ) {
      CString s = m_ado->readStr(0);
      if (s != "") set_str.insert(s);
      m_ado->MoveNext();
    }
    m_ado->closeTable();
  }

  set<CString, less_dom>::const_iterator it = set_str.begin();

  for (; it != set_str.end(); it++) {
     m_ctrl_dom.AddString(*it);
  }
  


  
  double x1 = 1e10, x2 = -1e10, y1 = 1e10, y2 = -1e10;

  str.Format("select %s, ID from %s where %s = '%s'", m_nomer, m_tn, m_str, street);

  if (raj != GOROD) {
    str.Format("select %s, ID from %s where %s = '%s' and %s = '%s'", m_nomer, m_tn, m_str, street, m_raj, raj);
  }
  else {
    str.Format("select %s, ID from %s where %s = '%s'", m_nomer, m_tn, m_str, street );
  }


  if (m_ado->openTable0(str)) {
    while ( !m_ado->isEOF() ) {
      CString s = m_ado->readStr(0);
      long id = m_ado->read_long(1);

      if (kls && s[0]) {
        int id2 = kls->GetNom2(id);
        if (id2 > 0) {
          kls->geo4[id2-1]->mark = true;
          is_mark = true;
          CFRect rect = kls->geo4[id2-1]->rect;

          if (rect.left != 0 || rect.right != 0 || rect.top != 0 || rect.bottom != 0) {
            x1 = min(x1, rect.left);
            y1 = min(y1, -rect.bottom);
            x2 = max(x2, rect.right);
            y2 = max(y2, -rect.top);
          }
        }
      }
      m_ado->MoveNext();
    }
    m_ado->closeTable();
  }

  if (pView) {
    pView->m_isMarkOnly = 2;
    if (is_mark) {
      pView->Invalidate(FALSE);
      pView->UpdateWindow();
    }
    pView->m_isMarkOnly = 0;
  }

  if (pView && x1 != 1e10) {
//    pView->moveXY(CFPoint((x1+x2)/2, -(y1+y2)/2));
    pView->MoveRect(CFRect(x1, -y2, x2, -y1));
    pView->moveXY(CFPoint((x1+x2)/2, -(y1+y2)/2));
    return;
  }
}


void CAddressLeft::OnSelchangeDom() 
{
    if (!m_bInit) return;

    CString str;
    UpdateData(TRUE);

    CString rr = "";
    CString ss = "";
    CString dd = "";

    CGidrView *pView = getView();

    if (pView) {
        pView->m_isMarkOnly = 1;
        pView->Invalidate(FALSE);
        pView->UpdateWindow();
        pView->m_isMarkOnly = 0;
    }

    Klassif *kls = m_kl_list->findKlN(m_tn);
    if (kls) {
        int n = kls->geo4.size();
        for (int i = 0; i < n; i++) {
            kls->geo4[i]->mark = false;
        }
    }

    int n;

    n = m_ctrl_raj.GetCurSel();
    if (n >= 0) m_ctrl_raj.GetText(n, rr);

    n = m_ctrl_street.GetCurSel();
    if (n >= 0) m_ctrl_street.GetText(n, ss);

    n = m_ctrl_dom.GetCurSel();
    if (n >= 0) m_ctrl_dom.GetText(n, dd);


    if (ss == "" && rr != "") {
        str.Format("select ID from %s where %s = '%s' AND %s = '%s' AND %s is NULL", m_tn, m_raj, rr, m_nomer, dd, m_str);
    }
    else {
        str.Format("select ID from %s where %s = '%s' AND %s = '%s'", m_tn, m_str, ss, m_nomer, dd);
    }

    if (m_ado->openTable0(str)) {

        if (!m_ado->isEOF()) {
            int id = m_ado->read_long("ID");
            CGidrView *pView = getView();
            if (pView) {
                pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
            }
        }
    }
}

void CAddressLeft::OnChangeStreetFind()
{
    if (!m_bInit) return;

    UpdateData(TRUE);

    char pic[256];

    m_ctrl_street_find.GetLine(0, pic, 255);
    pic[m_ctrl_street_find.LineLength(0)] = 0;

    m_street_find = pic;


    CString raj = GOROD;

    int n = m_ctrl_raj.GetCurSel();

    if (n >= 0) {
        m_ctrl_raj.GetText(n, raj);
    }

    writeStreet(raj, m_street_find);
}


void CAddressLeft::setTyp(int is_nagr)
{
    m_is_nagr = is_nagr;
}



BOOL CAddressLeft::PreTranslateMessage(MSG* pMsg) 
{
//  if (m_hAccel != NULL && TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
//    if (pMsg->wParam != VK_RETURN) return TRUE;
//      return TRUE;
//  }
  return CPaneDialog::PreTranslateMessage(pMsg);
}



bool CAddressLeft::Addr(CString &tn,  CString &str,  CString &raj,  CString &nomer)
{
  tn     = m_tn;
  str    = m_str;
  raj    = m_raj;
  nomer  = m_nomer;

  return tn != "";
}
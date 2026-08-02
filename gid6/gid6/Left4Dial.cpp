// Address3.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Left4Dial.h"
#include "gidrview.h"

#include "ado.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CGeoFile *m_geofile;

/////////////////////////////////////////////////////////////////////////////
// CLeft4Dialog property page

//IMPLEMENT_DYNCREATE(CLeft4Dialog, CPaneDialog)

CLeft4Dialog::CLeft4Dialog() : CPaneDialog()
{
  //{{AFX_DATA_INIT(CLeft4Dialog)
  m_Pic = "";
  //}}AFX_DATA_INIT
  m_ado = NULL;
  m_kl_list = NULL;
  m_bInit = FALSE;
}

CLeft4Dialog::~CLeft4Dialog()
{
}

void CLeft4Dialog::DoDataExchange(CDataExchange* pDX)
{
  CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLeft4Dialog)
  DDX_Control(pDX, IDC_EDIT, m_Edit);
  DDX_Text(pDX, IDC_EDIT, m_Pic);

  
  DDX_Control(pDX, IDC_LIST, m_ctrlList);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLeft4Dialog, CPaneDialog)
  //{{AFX_MSG_MAP(CLeft4Dialog)
  ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
  ON_WM_SIZE()
  ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
  ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
  ON_WM_CONTEXTMENU()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeft4Dialog message handlers

void CLeft4Dialog::OnSize(UINT nType, int cx, int cy) 
{
  CPaneDialog::OnSize(nType, cx, cy);

//  if(!m_bInit)
//    return;

  if (!m_ctrlList.m_hWnd) return;

  CRect sz, szE, szL;

  GetClientRect(sz);

  szE = sz; 
  szL = sz; 

  int h = 20;

  szE.bottom = h;
  szL.top = h;

  szL.DeflateRect(3,3);


  m_Edit.MoveWindow(szE);
  m_ctrlList.MoveWindow(szL);

  // TODO: Add your message handler code here
  
}

LRESULT CLeft4Dialog::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);
  
  Init("");
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CLeft4Dialog::SetKls(KlList *kl_list)
{
  m_kl_list = kl_list;

  Init(m_Pic);
}

void CLeft4Dialog::Init(const char *cpic)
{
  m_tn = transl("Жилые_комплексы");
  m_str = transl("Название_ЖК");

  CString pic = cpic;
  pic.MakeUpper();

  m_bInit = FALSE;

  m_ctrlList.ResetContent();

  if (!m_kl_list) return;

  Klassif *kls = m_kl_list->findKlN(transl(m_tn));

  if (!kls) return;

  m_ado = kls->m_ado;
  CString q;

  q.Format("select distinct %s from %s ORDER BY %s", transl(m_str), transl(m_tn), transl(m_str));
  if (m_ado->openTable0(q)) {
    while ( !m_ado->isEOF() ) {
      CString str = m_ado->readStr(0);
      CString s = str;
      str.MakeUpper();

      if (pic == "" || strstr(str, pic)) {
         m_ctrlList.AddString(s);
      }
      m_ado->MoveNext();
    }
    m_ado->closeTable();
  }
  m_bInit = TRUE;
}


void CLeft4Dialog::OnChangeEdit() 
{
  UpdateData(TRUE);
  
  Init(m_Pic);
}

bool readWKB(COleVariant &var, CCoordList &cl);


void CLeft4Dialog::OnSelchangeList() 
{

  if (!m_bInit) return;
  
  CString str, ss;
  UpdateData(TRUE);

  CGidrView *pView = getView();

  int n;
  
  int m_numb = -1;

  n = m_ctrlList.GetCurSel();
  if (n >= 0) { 
    m_ctrlList.GetText( n , ss );


    str.Format("select ID, shape.STAsBinary() AS wkb from [%s] where [%s] = '%s'", m_tn, m_str, ss);
    if (m_ado->openTable0(str)) {
//      m_ado->setGeometry("SHAPE");
                          
      if (!m_ado->isEOF()) {
        m_numb = m_ado->read_long(0);
        CCoordList cl;
//        m_ado->readCoord(cl);

        COleVariant var = m_ado->read("wkb");

        readWKB(var, cl);

        CFPoint m_pt = cl.first();
  //      CFRect rect = cl.GetFRect();

        CGidrView *pView = getView();
        if (pView) {
  //        pView->MoveRect(rect);
          pView->moveXYmas(m_pt);
        }
      }
    }
  }
}

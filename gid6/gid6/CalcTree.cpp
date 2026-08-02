// OpcTree.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CalcTree.h"
#include "ado.h"
#include "cxema.h"
#include "graph2.h"

/////////////////////////////////////////////////////////////////////////////
// CCalcTree dialog

bool ado_execute(CAdoFile *ado, const char *q, long * affected);
long ado_executeInsert(CAdoFile *ado, const char *q, long * affected);


CCalcTree::CCalcTree(CWnd* pParent, CGraph2 *graph, CAdoFile *ado)
  : CDialog(CCalcTree::IDD, pParent)
{
  //{{AFX_DATA_INIT(CCalcTree)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_graph = graph;
  m_ado = ado;
}


void CCalcTree::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCalcTree)
  DDX_Control(pDX, IDC_TREE, m_tree);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCalcTree, CDialog)
  //{{AFX_MSG_MAP(CCalcTree)
//  ON_COMMAND(ID_ADD, OnPopupAdd)
      ON_COMMAND(ID_DEL, OnPopupDel)
//  ON_COMMAND(ID_EDIT, OnPopupEdit)

//  ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalcTree message handlers


struct Fragm
{
  CString name;
  HTREEITEM h;
};

CString getSelectFragments();


BOOL CCalcTree::OnInitDialog() 
{
   CDialog::OnInitDialog();

    HTREEITEM h;

    CAdoFile *ado = getAdo(getGidAdoName());

    map<int, Fragm> map_f;

    CString q = "SELECT id, name FROM fragments WHERE removed=0";

///    CString q = getSelectFragments();

    if (ado->openTable0(q)) {

        while (!ado->isEOF()) {
            Fragm f;
            int id = ado->read_long("id");
            f.name = ado->readStr("name");
            f.h = m_tree.InsertItem(f.name, TVI_ROOT);

            map_f[id] = f;

            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (ado->openTable0("SELECT * FROM CALCULATION")) {

        while (!ado->isEOF()) {
            int id = ado->read_long("id");
            int fileID = ado->read_long("fileID");
            COleDateTime date1 = ado->read_date("date1");

            CString str = date1.Format(_T("%d.%m.%Y %H.%M"));

            map<int, Fragm>::const_iterator it = map_f.find(fileID);
            if (it != map_f.end()) {
              h = m_tree.InsertItem(str, it->second.h);
              m_tree.SetItemData(h, id);
            }


            ado->MoveNext();
        }
        ado->closeTable();
    }



//    m_h1 = m_tree.InsertItem(_TR("Система теплоснабжения"), TVI_ROOT);


  // TODO: Add extra initialization here
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CCalcTree::OnOK() 
{
  // TODO: Add extra validation here

  CString s;

  HTREEITEM h = m_tree.GetSelectedItem();

/*
  str = "";

  while (h != NULL) {
    s = m_tree.GetItemText(h);
    if (str == "") str = s;
    else str = s + "." + str;
    h = m_tree.GetParentItem(h);
  }
*/ 
  CDialog::OnOK();
}



BOOL CCalcTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
  LPNMHDR lpnmh = (LPNMHDR) lParam;
  TVHITTESTINFO ht = {0};
  HTREEITEM hItem1 = 0;
  HTREEITEM hItem2 = 0;
  HTREEITEM hItem3 = 0;
  HTREEITEM hItem4 = 0;

  if ((lpnmh->code == NM_CLICK) || (lpnmh->code == NM_RCLICK)) {
      DWORD dwpos = GetMessagePos();

      // include <windowsx.h> and <windows.h> header files
      ht.pt.x = GET_X_LPARAM(dwpos);
      ht.pt.y = GET_Y_LPARAM(dwpos);
      ::MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);
      TreeView_HitTest(lpnmh->hwndFrom, &ht);

      hItem1 = m_tree.GetParentItem(ht.hItem);
      hItem2 = m_tree.GetParentItem(hItem1);
      hItem3 = m_tree.GetParentItem(hItem2);
      hItem4 = m_tree.GetParentItem(hItem3);
  }

  if ((lpnmh->code == NM_CLICK)) {
    CString tn = m_tree.GetItemText(ht.hItem);

    CString s;
    s.Format("%d", ht.hItem);
//    AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
  }
  else if((lpnmh->code  == NM_RCLICK)) {

      CMenu menu;
      VERIFY(menu.CreatePopupMenu());

      POINT point = ht.pt;

      ClientToScreen(&point);

      m_id = (int)m_tree.GetItemData(ht.hItem);

      m_h = 0;

      if (hItem2 == 0 && hItem1 != 0) {
          m_h = ht.hItem;
          menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить расчет"));
      }

      menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
  }

  return CDialog::OnNotify(wParam, lParam, pResult);
}

void CCalcTree::add(const char *tn, const char *fn, HTREEITEM h) 
{
    if (!m_graph->m_cxema->m_cur_file) return;

    int idF = m_graph->m_cxema->m_cur_file->id;

    CString q;
    q.Format("INSERT INTO %s (fileID) VALUES (%d)", tn, idF);
        long affected;
    long id = ado_executeInsert(m_ado, q, &affected);
    if (id) {
        HTREEITEM hh = m_tree.InsertItem("?????", h);
        m_tree.SetItemData(hh, id);
    }
}



LRESULT CCalcTree::OnAdoUpdated(WPARAM wParam, LPARAM)
{
    return 0;
}

void deleteChildren(CTreeCtrl *tree, HTREEITEM h);

void CCalcTree::OnPopupDel()
{
    long affected;
    CString q, s;

    s.Format(_TR("Удалить расчет"));

    if (AfxMessageBox(s, MB_YESNO) == IDYES) {
        q.Format("DELETE FROM CALCULATION WHERE ID=%d",  m_id);
        bool ret = ado_execute(m_ado, q, &affected);

        if (ret) {
            m_tree.DeleteItem(m_h);
        }
    }
}

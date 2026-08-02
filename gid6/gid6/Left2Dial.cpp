// CLeft2Dialog.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Left2Dial.h"
#include <WINDOWSX.H>
#include "KlEdit.h"
#include "gid6Doc.h"
#include "gidrView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MSG_CHECKSTATECHANGE (WM_USER + 100)

BEGIN_MESSAGE_MAP(CLeft2Dialog, CPaneDialog)
  //{{AFX_MSG_MAP(CLeft2Dialog)
  ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
  ON_COMMAND(ID_GEO_ADD, OnAdd)
  ON_COMMAND(ID_GEO_VIEW, OnView)
//  ON_COMMAND(ID_DEL, OnDel)
  ON_WM_SIZE()
  ON_WM_CONTEXTMENU()
  ON_NOTIFY(TVN_SELCHANGED, IDC_EDIT, OnSelchangedEdit)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

HTREEITEM InsertItem(CTreeCtrl &ctlTree, LPCTSTR txt, int nImage, int nSelectedImage, HTREEITEM i);


static HTREEITEM m_hItem;
static map<HTREEITEM, int> map_ch;

int get_ch(HTREEITEM i)
{
  map<HTREEITEM, int>::const_iterator it = map_ch.find(i);

  if (it != map_ch.end()) {
    return it->second;
  }

  return 0;
}



BOOL CLeft2Dialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: Add your specialized code here and/or call the base class

    LPNMHDR lpnmh = (LPNMHDR) lParam;
    TVHITTESTINFO ht = {0};

//    if((lpnmh->code  == NM_CLICK) && (lpnmh->idFrom == IDC_TREE_ACCESS))
    if((lpnmh->code  == NM_CLICK))
    {
        DWORD dwpos = GetMessagePos();

        // include <windowsx.h> and <windows.h> header files
        ht.pt.x = GET_X_LPARAM(dwpos);
        ht.pt.y = GET_Y_LPARAM(dwpos);
        ::MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);

        TreeView_HitTest(lpnmh->hwndFrom, &ht);

            CString str;
            str.Format("%d", ht.flags);
//            AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

        if (2 & ht.flags || 4 & ht.flags)
//        if(TVHT_ONITEMSTATEICON & ht.flags)
        {
            TRACE("OnNotify Check\n");
            SetCheckTree(ht.hItem, ht.flags, -1);
//            SetCheckTree(ht.hItem, -1);

//            PostMessage(MSG_CHECKSTATECHANGE,0,(LPARAM)ht.hItem);
        }
    }
    else if((lpnmh->code  == NM_RCLICK))
    {
        DWORD dwpos = GetMessagePos();

        // include <windowsx.h> and <windows.h> header files
        ht.pt.x = GET_X_LPARAM(dwpos);
        ht.pt.y = GET_Y_LPARAM(dwpos);
        ::MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);

        TreeView_HitTest(lpnmh->hwndFrom, &ht);

        if (ht.hItem) {
          CDocument *pDocument = (CDocument *) m_ctlTree.GetItemData(ht.hItem);
          m_hItem = ht.hItem;
        }

        POINT point = ht.pt;
        CMenu menu;
        VERIFY(menu.CreatePopupMenu());

        ClientToScreen(&point);

        CString str1 = m_ctlTree.GetItemText(m_hItem);

        if (ht.hItem && str1 != "") {
          menu.AppendMenu(MF_STRING, ID_GEO_ADD, _TR("Активная"));
          menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
        }
    }

    return CPaneDialog::OnNotify(wParam, lParam, pResult);
}



void InvalidView();
//void OnOff(CTreeCtrl &m_ctlTree, HTREEITEM hItem, int ch);
void OnOff3(CTreeCtrl &m_ctlTree, HTREEITEM hItem, int ch)
{
  switch (ch) {
    case 0 : m_ctlTree.SetItemImage( hItem, 5, 5); break;
    case 1 : m_ctlTree.SetItemImage( hItem, 4, 4); break;
    case 2 : m_ctlTree.SetItemImage( hItem, 6, 6); break;
  }
  map_ch[hItem] = ch;
}



bool IsCheckable(CTreeCtrl &m_ctlTree, HTREEITEM hItem);
bool IsCheckable3(CTreeCtrl &m_ctlTree, HTREEITEM hItem);

void CLeft2Dialog::SetCheckTree(HTREEITEM hItem, int flags, int ch)
{
  if (hItem==0) return ;

  CGidrView *view = getView();

  if (flags == 2 && IsCheckable3(m_ctlTree, hItem))  {

    if (ch == -1) {
//      ch = m_ctlTree.GetItemData(hItem);
      ch = get_ch(hItem);
      ch = (ch+1)%2;
    }

    m_ctlTree.SetItemData(hItem, ch);

    OnOff3(m_ctlTree, hItem, ch);

    if (m_ctlTree.ItemHasChildren(hItem))
    {
        HTREEITEM hti = m_ctlTree.GetNextItem(hItem, TVGN_CHILD);
        while (hti)
        {
//          SetCheckTree(hti, flags, ch);
          OnOff3(m_ctlTree, hti, ch);
          if (!m_ctlTree.ItemHasChildren(hti))
          {
            if (view) view->setRS(m_ctlTree.GetItemText(hti), ch);
            InvalidView();
          }


          HTREEITEM hti2 = m_ctlTree.GetNextItem(hti, TVGN_CHILD);
          while (hti2)
          {
            OnOff3(m_ctlTree, hti2, ch);
            hti2 = m_ctlTree.GetNextItem(hti2, TVGN_NEXT);
            if (!m_ctlTree.ItemHasChildren(hti2))
            {
              if (view) view->setRS(m_ctlTree.GetItemText(hti2), ch);
              InvalidView();
            }
          }


          hti = m_ctlTree.GetNextItem(hti, TVGN_NEXT);
        }
    }
    else {
      m_ctlTree.SelectItem(hItem);
      if (view) {
        view->setRS(m_ctlTree.GetItemText(hItem), ch);
      }
      InvalidView();
    }
  }

  if (flags == 4) {
    if (view) {
      view->findIS(m_ctlTree.GetItemText(hItem));
    }
  }
}




/////////////////////////////////////////////////////////////////////////////
// CLeft2Dialog dialog

CLeft2Dialog::CLeft2Dialog()
  : CPaneDialog()
{
  m_bInit = FALSE;
  //{{AFX_DATA_INIT(CLeft2Dialog)
  //}}AFX_DATA_INIT
}



void CLeft2Dialog::DoDataExchange(CDataExchange* pDX)
{
  CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLeft2Dialog)
  DDX_Control(pDX, IDC_EDIT, m_ctlTree);
  //}}AFX_DATA_MAP
}



/////////////////////////////////////////////////////////////////////////////
// CLeft2Dialog message handlers

void CLeft2Dialog::OnSize(UINT nType, int cx, int cy)
{
  CPaneDialog::OnSize(nType, cx, cy);

  if(!m_bInit)
    return;

  CRect sz;
  GetClientRect(sz);
//  sz.DeflateRect(5,5);

  m_ctlTree.MoveWindow(sz);
}

LRESULT CLeft2Dialog::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);

  CRect sz;
  GetClientRect(sz);
  sz.DeflateRect(5,5);

  m_bInit = TRUE;

  m_imgList.Create( IDB_BITMAP_LIST, 16, 1, RGB( 0, 128, 128 ) );

  void setTreeFont(CTreeCtrl * m_ctrlTree, CFont * m_Font);
  setTreeFont(&m_ctlTree, &m_Font);

  m_ctlTree.SetImageList( &m_imgList, TVSIL_NORMAL );



//  i1 = m_ctlTree.InsertItem(_TR("Слои"), 0, 0);
  i1 = 0;

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CLeft2Dialog::OnOK()
{
  // TODO: Add extra validation here

//  CPaneDialog::OnOK();
}

void CLeft2Dialog::OnCancel()
{
  // TODO: Add extra validation here

//  CPaneDialog::OnCancel();
}

void CLeft2Dialog::Init()
{
  if (::IsWindow(m_ctlTree)) {
    m_ctlTree.DeleteAllItems( );
//    i1 = m_ctlTree.InsertItem(_TR("Слои"), 0, 0);
  }
}

/*
HTREEITEM CLeft2Dialog::InsertItem(LPCTSTR txt, int nImage, int nSelectedImage, HTREEITEM i) {
  if (!i) i = i1;

  HTREEITEM i2 = m_ctlTree.InsertItem(txt, nImage, nSelectedImage, i);
  m_ctlTree.SelectItem(i2);
  return i2;
}
*/


void CLeft2Dialog::InsertDocument(LPCTSTR txt, const CDocument *pDocument) {

  HTREEITEM i2 = m_ctlTree.InsertItem(txt, 1, 2, i1);
  m_ctlTree.SetItemData(i2, (DWORD) pDocument);
  m_ctlTree.SelectItem(i2);
}

HTREEITEM CLeft2Dialog::FindDocument(const CDocument *pDocument) {
  HTREEITEM hItem;
  DWORD data;

  hItem = m_ctlTree.GetFirstVisibleItem();

  while ( hItem != NULL ) {
    data = m_ctlTree.GetItemData( hItem );
    if (data == (DWORD) pDocument) {
      return hItem;
    }
    hItem = m_ctlTree.GetNextItem( hItem,  TVGN_NEXTVISIBLE);
  }
  return NULL;
}


void CLeft2Dialog::InsertView(LPCTSTR txt, const CView *pView) {

  HTREEITEM i1 = FindDocument(pView->GetDocument());

  HTREEITEM i2 = m_ctlTree.InsertItem(txt, 3, 3, i1);
  m_ctlTree.SetItemData(i2, (DWORD) (pView->m_hWnd));
  m_ctlTree.SelectItem(i2);
}

void CLeft2Dialog::RemoveView(const CView *pView) {
  HTREEITEM hItem;
  DWORD data;

  hItem = m_ctlTree.GetFirstVisibleItem();

  while ( hItem != NULL ) {
    data = m_ctlTree.GetItemData( hItem );
    if (data == (DWORD) (pView->m_hWnd)) {
      m_ctlTree.DeleteItem( hItem );
      break;
    }
    hItem = m_ctlTree.GetNextItem( hItem,  TVGN_NEXTVISIBLE);
  }
}

void CLeft2Dialog::FindView(const CView *pView) {
  HTREEITEM hItem;
  DWORD data;

  hItem = m_ctlTree.GetFirstVisibleItem();

  while ( hItem != NULL ) {
    data = m_ctlTree.GetItemData( hItem );
    if (data == (DWORD) (pView->m_hWnd)) {
      m_ctlTree.SelectItem( hItem );
      break;
    }
    hItem = m_ctlTree.GetNextItem( hItem,  TVGN_NEXTVISIBLE);
  }
}


void CLeft2Dialog::OnSelchangedEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  // TODO: Add your control notification handler code here

  HTREEITEM hItem = m_ctlTree.GetSelectedItem();

  DWORD data = m_ctlTree.GetItemData( hItem );

  if (data) {
    HWND hWnd = (HWND) data;

    if (::IsWindow(hWnd)) {
      hWnd = ::GetParent(hWnd);
      //      AfxGetMainWnd()->SendMessage(WM_MDIACTIVATE, (WPARAM) hWnd, 0);
      ::SendMessage(static_cast<CMDIFrameWnd*>(AfxGetMainWnd())->m_hWndMDIClient,
        WM_MDIACTIVATE, (WPARAM)hWnd, 0);
    }
  }

  *pResult = 0;
}


void CLeft2Dialog::OnAdd()
{
  CGidrView *pView = getView();
  if (pView) {
    int id = (int) m_ctlTree.GetItemData(m_hItem);
    pView->setActiv(id);
  }
}

void CLeft2Dialog::OnView()
{
}

void CLeft2Dialog::OnDel()
{
  CGidrView *pView = getView();
  Cgid6Doc *pDoc = (Cgid6Doc *) pView->GetDocument();

  pView->GetDocument()->SetModifiedFlag(true);
}



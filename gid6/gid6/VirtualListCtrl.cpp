// VirtualListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "VirtualListCtrl.h"

#include "wm_user.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirtualListCtrl

CVirtualListCtrl::CVirtualListCtrl()
{
  m_virtListData = NULL;
  m_row = -1;
  m_col = -1;
  m_iSortColumn =  -1;
  m_bSortAscending = TRUE;


}

CVirtualListCtrl::~CVirtualListCtrl()
{
  delete m_virtListData;
}


BEGIN_MESSAGE_MAP(CVirtualListCtrl, CListCtrl)
  //{{AFX_MSG_MAP(CVirtualListCtrl)
  ON_WM_DESTROY()
  ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
  ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnOdcachehint)
  ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)

  ON_WM_RBUTTONDOWN()
  ON_WM_LBUTTONDBLCLK()

  ON_COMMAND(ID_EDIT, OnPopupEdit)
  ON_COMMAND(ID_MOVE_TO, OnPopupMoveTo)
  ON_COMMAND(ID_DEL, OnPopupDel)



  ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
  ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirtualListCtrl message handlers

void CVirtualListCtrl::init(int id)
{
  InitListCtrl(id);

  PostMessage( WM_COMMAND, ID_LIST_SETCOUNT);
}

// Вернуть строку из n нулей

CString getO(int n)
{
  CString  s = "";
  int i;

  for (i = 0; i < n; i++) {
    s += "Щ";
  }
  return s;
}

void CVirtualListCtrl::InitListCtrl(int id)
{
//  m_virtListData = m_virtListDataInit;
  
  // Создаем колонки
  CRect rect;
  GetClientRect(&rect);
  int n1 = rect.Width()/3;
  int n2 = rect.Width()%3;

  CString tn = "CVirtualListCtrl";
  GetOwner()->GetWindowText(tn);
  m_section.Format("%s\\%s\\%s", szSection, "CVirtualListCtrl", tn);

  CVirtListData* pData = m_virtListData;
  if (!pData) return;

//  this->DeleteAllItems();

  int i;

  int nColumnCount = GetHeaderCtrl()->GetItemCount();

  for (i=0; i < nColumnCount; i++) {
    DeleteColumn(0);
  }

  int nf = pData->GetNFlds();

  for (i = 0; i < nf; i++) {
    CSize size = GetDC()->GetTextExtent(getO(pData->GetFieldWidth(i)));

    CString p2;
    p2.Format("%d", i);

    int width = AfxGetApp()->GetProfileInt(m_section, p2, size.cx);


    int format = pData->GetFormat(i);
    InsertColumn( i, pData->GetFieldName(i),  format, width);
  }

  // Создаем картинки
//  VERIFY(m_ImageSmall.Create(IDB_LIST_SMALL, 16, 4, RGB(255, 0, 255)));
//  GetListCtrl().SetImageList(&m_ImageSmall, LVSIL_SMALL);
//  VERIFY(m_ImageState.Create(IDB_LIST_STATE, 16, 4, RGB(255, 0, 255)));
//  GetListCtrl().SetImageList(&m_ImageState, LVSIL_STATE);

  // Разрешаем использовать иконки состояния
  SendMessage( LVM_SETCALLBACKMASK , LVIS_STATEIMAGEMASK , 0);

  // Выделение во всю строку, рисование сетки

  //unsigned long LVS_EX_DOUBLEBUFFER = 0x00010000;

  SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
//  SetExtendedStyle(LVS_EX_GRIDLINES);

  // Число элементов 100000 !!!!
  UpdateCount();

//  SetHotItem(0);

  int id2 = pData->GetId2(id);

  if (id2 >= 0) {
      SetFocus();
      SetItemState(id2, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
      EnsureVisible(id2, FALSE);
  }
//  SetSelectionMark(new_selected_item);
}


void CVirtualListCtrl::UpdateCount()
{ 
  CVirtListData* pData = m_virtListData;
  if (!pData) return;

  SetItemCount( pData->GetCount() );
}


void CVirtualListCtrl::setVirtListData(CVirtListData *data) 
{
  m_virtListData = data;  
};

BOOL CVirtualListCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
  // TODO: Add your specialized code here and/or call the base class
  
  return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


/////////////////////////////////////////////////////////////////////////////
// CVirtListView message handlers

void CVirtualListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
  LV_ITEM* pItem= &(pDispInfo)->item;
  
  CVirtListData* pData = m_virtListData;

  if (!pData) return;

  DWORD n = pItem->iItem; 

  int id_first = GetTopIndex();
  int id_last = id_first + GetCountPerPage()+10;

  if (pItem->mask & LVIF_TEXT) {
    // требуется текст элемента?

    CString txt = pData->GetItemText( pItem->iSubItem, n, id_first, id_last);
    if (txt.GetLength() > pItem->cchTextMax-1) {
      txt = txt.Left(pItem->cchTextMax-1);
    }

    strcpy( pItem->pszText, txt);
  }

  if (pItem->mask & LVIF_IMAGE) // требуются картинки?
  {
    pItem->iImage = pData->GetImage(n);
    pItem->state = pData->GetStateImage(n);
  }

  *pResult = 0;
}

void CVirtualListCtrl::OnOdcachehint(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLVCACHEHINT* pCacheHint = (NMLVCACHEHINT*)pNMHDR;
  TRACE("From: %d, To: %d\n", pCacheHint->iFrom, pCacheHint->iTo);
  *pResult = 0;
}


int CVirtualListCtrl::getCur() 
{
  UINT i, uSelectedCount = GetSelectedCount();
  int  nItem = -1;

  // Update all of the selected items.
  if (uSelectedCount > 0)
  {
    for (i=0; i < uSelectedCount; i++)
    {
      nItem = GetNextItem(nItem, LVNI_SELECTED);
      ASSERT(nItem != -1);
      Update(nItem); 

      return nItem;
    }
  }
  return 0;
}

int*  CVirtualListCtrl::getCurElements() {
    UINT i, uSelectedCount = GetSelectedCount();
    int  nItem = -1;
    int *arr = new int[uSelectedCount];
    // Update all of the selected items.
    if (uSelectedCount > 0)
    {
        for (i = 0; i < uSelectedCount; i++)
        {
            nItem = GetNextItem(nItem, LVNI_SELECTED);
            ASSERT(nItem != -1);
            Update(nItem);
            arr[i] = nItem;

            
        }
        
    }
    return arr;
}


void CVirtualListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
//  int  nItem = getCur();
//  GetOwner()->SendMessage(WM_LDBL, nItem);

  GetOwner()->SendMessage(WM_LDBL, m_row);
  
  CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CVirtualListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

  if ((pNMListView->uChanged & LVIF_STATE) 
        && (pNMListView->uNewState & LVIS_SELECTED))
  {
    m_row = pNMListView->iItem;
    m_col = pNMListView->iSubItem;
  }


  *pResult = 0;
}


void CVirtualListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
  // TODO: Add your control notification handler code here
  CPoint point;
  GetCursorPos(&point);  
  ScreenToClient(&point);

  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  LVHITTESTINFO hti;  
  hti.pt = point;
  hti.flags = LVM_SUBITEMHITTEST;

  // CMT: Select the item the user clicked on.
  UINT uFlags;
  int nItem = HitTest(point, &uFlags);

  // CMT: If the click has been made on some valid item
  if (-1 != nItem)
  {
    DWORD dwData = GetItemData(nItem);

    if (uFlags & LVHT_ONITEMLABEL)
    {
      SubItemHitTest(&hti);
      int nSubItem =  hti.iSubItem;
      
      // There is your cell
    }
  }
}

void CVirtualListCtrl::PreSubclassWindow()
{
  // the list control must have the report style.
  ASSERT( GetStyle() & LVS_REPORT );

  CListCtrl::PreSubclassWindow();
  VERIFY( m_ctlHeader.SubclassWindow( GetHeaderCtrl()->GetSafeHwnd() ) );
}


void CVirtualListCtrl::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  const int iColumn = pNMListView->iSubItem;

  // if it's a second click on the same column then reverse the sort order,
  // otherwise sort the new column in ascending order.
//  Sort( iColumn, iColumn == m_iSortColumn ? !m_bSortAscending : TRUE );

//  iColumn, 
  BOOL bAscending = iColumn == m_iSortColumn ? !m_bSortAscending : TRUE;

  m_iSortColumn = iColumn;
  m_bSortAscending = bAscending;

  m_ctlHeader.SetSortArrow( m_iSortColumn, m_bSortAscending );

  *pResult = 0;
}


void CVirtualListCtrl::setFindText(const char *text)
{
  m_virtListData->setFindText(text);
}



void CVirtualListCtrl::OnDestroy()
{

  CVirtListData* pData = m_virtListData;
  if (pData) {
      int nf = pData->GetNFlds();

      for (int i = 0; i < nf; i++) {
        CString p2;
        p2.Format("%d", i);

        HDITEM hdi;
        CHeaderCtrl *hd = GetHeaderCtrl();
        
        hdi.mask = HDI_WIDTH;

        if (hd && hd->GetItem(i, &hdi)) {
            AfxGetApp()->WriteProfileInt(m_section, p2, hdi.cxy);
        }
      }
  }


    CListCtrl::OnDestroy();
}

void CVirtualListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
    CMenu menu;
    VERIFY(menu.CreatePopupMenu());

    ClientToScreen(&point);

//     menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать объект"));
     menu.AppendMenu(MF_STRING, ID_MOVE_TO, _TR("Перейти к объекту"));
     menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить объект"));

    menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);


    CListCtrl::OnRButtonDown(nFlags, point);
}


void CVirtualListCtrl::OnPopupEdit()
{
//  GetOwner()->SendMessage(WM_LDBL, m_row);
}

void CVirtualListCtrl::OnPopupMoveTo()
{
  GetOwner()->SendMessage(WM_LDBL, m_row);
}

void CVirtualListCtrl::OnPopupDel()
{
  GetOwner()->SendMessage(WM_LDBL, m_row);
}


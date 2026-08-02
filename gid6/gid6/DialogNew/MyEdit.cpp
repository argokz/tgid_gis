// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
//{{AFX_MSG_MAP(CMyEdit)
  ON_WM_KEYDOWN()
  ON_WM_CTLCOLOR_REFLECT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers

void CMyEdit::GetText(CString& rString )
{
  GetWindowText(rString);
};
void CMyEdit::SetText(LPCTSTR lpszString)
{
  SetWindowText(lpszString );
};
void CMyEdit::Move(LPCRECT lpRect)
{
  CRect r = lpRect;
  r.InflateRect(-1, -1);
  MoveWindow(r);
  SetSel(0, -1);
  SetFocus();
}

void CMyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  int nStartChar, nEndChar;
  CString str;
  GetSel( nStartChar, nEndChar );
  GetWindowText(str);

  if (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB
    || nChar == VK_UP || nChar == VK_DOWN
    || nChar == VK_NEXT || nChar == VK_PRIOR
    || ((nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_HOME || nChar == VK_END)
    && nStartChar == 0 && nEndChar == str.GetLength())
    ) 
  {
    GetParent()->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
    return;
  }
  if (nChar == VK_DELETE || nChar == VK_INSERT) GetParent()->PostMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));

  CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


BEGIN_MESSAGE_MAP(CMyComboBox, CComboBox)
//{{AFX_MSG_MAP(CMyComboBox)
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_CTLCOLOR_REFLECT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyComboBox message handlers

CMyComboBox::CMyComboBox() : str("")
{
    m_crTextColor = 0;
    m_cr = 0xFFFFFF;
};

void CMyComboBox::GetText(CString& rString )
{
  int n = GetCurSel();
  if (n >= 0) rString = (const char *)GetItemData(n);
}
void CMyComboBox::SetText(LPCTSTR lpszString)
{
   SelectString(-1, lpszString);
}
void CMyComboBox::Move(LPCRECT lpRect)
{
  ShowWindow(SW_HIDE);
  CRect r = lpRect;
  SetItemHeight(-1, r.Height()-5);
  r.bottom += r.Height()*10;
  MoveWindow(r);
  ShowWindow(SW_SHOW);
  SetFocus();
}


void CMyComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   ASSERT(lpDrawItemStruct->CtlType == ODT_COMBOBOX);
   LPCTSTR lpszText = (LPCTSTR) lpDrawItemStruct->itemData;
   ASSERT(lpszText != NULL);
   CDC dc;

   CRect rect = lpDrawItemStruct->rcItem;

   rect.DeflateRect(1,1,1,1);

   dc.Attach(lpDrawItemStruct->hDC);

   // Save these value to restore them when done drawing.
   COLORREF crOldTextColor = dc.GetTextColor();
   COLORREF crOldBkColor = dc.GetBkColor();

   // If this item is selected, set the background color 
   // and the text color to appropriate values. Erase
   // the rect by filling it with the background color.
   if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
      (lpDrawItemStruct->itemState  & ODS_SELECTED))
   {
      dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
   }
   else
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
 
  rect.left++;

   // Draw the text.
   dc.DrawText(
      lpszText,
      lstrlen(lpszText),
      &rect,
      DT_LEFT|DT_SINGLELINE|DT_VCENTER);

   // Reset the background color and the text color back to their
   // original values.
   dc.SetTextColor(crOldTextColor);
   dc.SetBkColor(crOldBkColor);

   dc.Detach();
}


void CMyComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (!GetDroppedState() &&
    (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB
    || nChar == VK_UP || nChar == VK_DOWN
    || nChar == VK_LEFT || nChar == VK_RIGHT
    || nChar == VK_NEXT || nChar == VK_PRIOR
    ))
  {
    GetParent()->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
    return;
  }

  if (GetDroppedState() && nChar == VK_ESCAPE) {
    ShowDropDown(FALSE);
    SetCurSel(-1);
    GetParent()->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
    return;
  }

  if (nChar == VK_DELETE) {
    SetCurSel(-1);
    return;
  }

  
  CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  CString s;

  if (nChar == VK_BACK) {
    str.Delete(str.GetLength()-1);
  }
  else {
    str.Insert(1000, nChar);
  }
  str.MakeUpper();

  for (int i = 0; i < GetCount(); i++) {
    s = (const char *)GetItemData(i);

    s.MakeUpper();
    if (s.Find(str) != -1) {
      SetCurSel(i);
      return;
    }
  }
  str.Delete(str.GetLength()-1);

  CComboBox::OnChar(nChar, nRepCnt, nFlags);
}




void CMyComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
   ASSERT(lpMeasureItemStruct->CtlType == ODT_COMBOBOX);

   if (lpMeasureItemStruct->itemID != (UINT) -1)
   {
      LPCTSTR lpszText = (LPCTSTR) lpMeasureItemStruct->itemData;
      ASSERT(lpszText != NULL);
      CSize   sz;
      CDC* pDC = GetDC();

      sz = pDC->GetTextExtent(lpszText);

      ReleaseDC(pDC);

      lpMeasureItemStruct->itemHeight = sz.cy;
   }
}

int CMyComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
   ASSERT(lpCompareItemStruct->CtlType == ODT_COMBOBOX);
   LPCTSTR lpszText1 = (LPCTSTR) lpCompareItemStruct->itemData1;
   ASSERT(lpszText1 != NULL);
   LPCTSTR lpszText2 = (LPCTSTR) lpCompareItemStruct->itemData2;
   ASSERT(lpszText2 != NULL);

   return strcmp(lpszText2, lpszText1);
}



BEGIN_MESSAGE_MAP(CMyComboBox2, CComboBox)
//{{AFX_MSG_MAP(CMyComboBox2)
  ON_WM_KEYDOWN()
  ON_WM_CTLCOLOR_REFLECT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyComboBox2 message handlers

CMyComboBox2::CMyComboBox2()
{
};

void CMyComboBox2::GetText(CString& rString )
{
  GetWindowText(rString);
}

void CMyComboBox2::SetText(LPCTSTR lpszString)
{
  SetWindowText(lpszString);
}

void CMyComboBox2::Move(LPCRECT lpRect)
{
  ShowWindow(SW_HIDE);
  CRect r = lpRect;
  r.bottom += r.Height()*10;
  MoveWindow(r);
  SetItemHeight(-1, ((CRect)lpRect).Height()-5);
  SetEditSel(0, -1);
  ShowWindow(SW_SHOW);
  SetFocus();
}

/*
void CMyComboBox2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  int dwSel;
  CString str;
  dwSel = GetEditSel();
  
  GetWindowText(str);

  if (!GetDroppedState() &&
    (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB
    || nChar == VK_UP || nChar == VK_DOWN
    || nChar == VK_NEXT || nChar == VK_PRIOR
    || ((nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_HOME || nChar == VK_END)
    && LOWORD(dwSel) == 0 && HIWORD(dwSel) == str.GetLength())
    ))
  {
    GetParent()->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
    return;
  }

  if (GetDroppedState() && nChar == VK_ESCAPE) {
    ShowDropDown(FALSE);
    SetCurSel(-1);
    GetParent()->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
    return;
  }
  
  CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/

BOOL CMyComboBox2::PreTranslateMessage(MSG* pMsg) 
{
  if( pMsg->message == WM_KEYDOWN )
  {
    int ch = pMsg->wParam;
    int dwSel;
    CString str;
    dwSel = GetEditSel();
    GetWindowText(str);

    if (!GetDroppedState() &&
      (ch == VK_RETURN || ch == VK_ESCAPE || ch == VK_TAB
      || ch == VK_UP || ch == VK_DOWN
      || ch == VK_NEXT || ch == VK_PRIOR 
      || ((ch == VK_LEFT || ch == VK_RIGHT || ch == VK_HOME || ch == VK_END)
      && LOWORD(dwSel) == 0 && HIWORD(dwSel) == str.GetLength())
      ))
    {
      GetParent()->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
      return TRUE;
    }

    if (ch == VK_DELETE || ch == VK_INSERT) GetParent()->PostMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
  
    if (GetDroppedState() && ch == VK_ESCAPE) {
      ShowDropDown(FALSE);
      SetCurSel(-1);
      GetParent()->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
      return TRUE;
    }
  }
  return CComboBox::PreTranslateMessage(pMsg);
}



BEGIN_MESSAGE_MAP(CMyButton, CButton)
//{{AFX_MSG_MAP(CMyButton)
//  ON_WM_KEYDOWN()
//  ON_WM_CTLCOLOR_REFLECT()
//  ON_WM_CTLCOLOR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyButton message handlers

void CMyButton::GetText(CString& rString )
{
  GetWindowText(rString);
};
void CMyButton::SetText(LPCTSTR lpszString)
{
  SetWindowText(lpszString );
};
void CMyButton::Move(LPCRECT lpRect)
{
}

HBRUSH CMyButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
 HBRUSH hbr = CButton::OnCtlColor(pDC, pWnd, nCtlColor);

  pDC->SetBkMode(TRANSPARENT);

   hbr = m_br;

  return hbr;
}







BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
//{{AFX_MSG_MAP(CMyStatic)
  ON_WM_KEYDOWN()
  ON_WM_CTLCOLOR_REFLECT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyStatic message handlers

void CMyStatic::GetText(CString& rString )
{
  GetWindowText(rString);
};
void CMyStatic::SetText(LPCTSTR lpszString)
{
  SetWindowText(lpszString );
};
void CMyStatic::Move(LPCRECT lpRect)
{
  CRect r = lpRect;
  r.InflateRect(-1, -1);
  MoveWindow(r);
  SetFocus();
}

void CMyStatic::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  int nStartChar, nEndChar;
  CString str;
//  GetSel( nStartChar, nEndChar );
  GetWindowText(str);

  if (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB
    || nChar == VK_UP || nChar == VK_DOWN
    || nChar == VK_NEXT || nChar == VK_PRIOR
    || ((nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_HOME || nChar == VK_END)
    && nStartChar == 0 && nEndChar == str.GetLength())
    ) 
  {
    GetParent()->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
    return;
  }
  if (nChar == VK_DELETE || nChar == VK_INSERT) GetParent()->PostMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));

  CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}

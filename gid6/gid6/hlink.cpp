///////////////////////////////////////////////////////////////////////////
//  File:    hlink.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by MFC Codeguru "gurus"
//  E-mail:     prantl@ff.cuni.cz, http://www.codeguru.com
//
//  Hyperlink control
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef HLINK_NOOLE
#define INITGUID
#endif
#include "resource.h"
#ifndef HLINK_NOOLE
#include <initguid.h>
#endif
#include <winnetwk.h>
#include <winnls.h>
#include <shlobj.h>
#ifndef HLINK_NOOLE
#include <intshcut.h>
#endif
#define FP_SRC_HYPERLINK
#include "hlink.h"

/////////////////////////////////  Macros & Statics ///////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////// Implementation //////////////////////////////

#define TOOLTIP_ID 1


BEGIN_MESSAGE_MAP (CHyperlink, CStatic)
ON_WM_CONTEXTMENU ()
//{{AFX_MSG_MAP(CHyperlink)
ON_WM_SETCURSOR ()
ON_WM_LBUTTONDOWN ()
ON_WM_MOUSEMOVE ()
ON_COMMAND (ID_POPUP_OPEN, OnOpen)
ON_WM_CTLCOLOR_REFLECT ()
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()


CHyperlink::CHyperlink ()
{
  m_Color = RGB (0, 0, 255);
  m_VisitedColor = RGB (128, 0, 128);
  m_FailedColor = RGB (128, 128, 128);
  m_HighlightColor = RGB (255, 0, 0);

  m_bShrinkToFit = TRUE;
  m_bUseHighlight = TRUE;
  m_State = ST_NOT_VISITED;
  m_OldState = ST_NOT_VISITED;
  m_bShowingContext = FALSE;
  m_bAllowContextMenu = TRUE;

  //Load up the cursors
  m_hLinkCursor = AfxGetApp ()->LoadCursor (IDC_HLINK);
  m_hBadCursor = AfxGetApp ()->LoadCursor (IDC_HLINK_BAD);
  m_hArrowCursor = AfxGetApp ()->LoadStandardCursor (IDC_ARROW);
}


CHyperlink:: ~ CHyperlink ()
{
}


void CHyperlink::
SetHyperLink (const CString & sActualLink)
{
  SetActualHyperLink (sActualLink);
  m_ToolTip.UpdateTipText (m_sToolTip, this, TOOLTIP_ID);
}


void CHyperlink::
SetHyperLinkDescription (const CString & sDescription)
{
  SetWindowText (sDescription);
}


CString CHyperlink::
GetHyperLinkDescription ()
const
{
  CString sDescription;
  GetWindowText (sDescription);
  return sDescription;
}


void CHyperlink::SetActualHyperLink (const CString & sActualLink)
{
  m_sActualLink = sActualLink;
  CheckActualHyperLink ();
}

void CHyperlink::
CheckActualHyperLink ()
{
  int pos = m_sActualLink.Find (_T ('\n'));
  if (pos != -1)
    {
      SetHyperLinkDescription (m_sActualLink.Left (pos));
      int pos1 = m_sActualLink.Mid (pos + 1).Find (_T ('\n'));
      if (pos1 != -1)
        {
          m_sToolTip = m_sActualLink.Mid (pos + pos1 + 2);
          m_sActualLink = m_sActualLink.Mid (pos + 1, pos1);
        }
      else
        {
          m_sActualLink = m_sActualLink.Mid (pos + 1);
          m_sToolTip = m_sActualLink;
        }
    }
}

BOOL CHyperlink::
OnSetCursor (CWnd * /*pWnd */ , UINT /*nHitTest */ , UINT /*message */ )
{
  if (m_bShowingContext)
    ::SetCursor (m_hArrowCursor);
  else if (m_State == ST_VISIT_FAILED)
    ::SetCursor (m_hBadCursor);
  else
    ::SetCursor (m_hLinkCursor);
  return TRUE;
}

void CHyperlink::
OnLButtonDown (UINT /*nFlags */ , CPoint /*point */ )
{
  PostMessage (WM_COMMAND, ID_POPUP_OPEN);
}


void CHyperlink::
OnOpen ()
{
  if (Open ())
    m_State = ST_VISITED;
  else
    m_State = ST_VISIT_FAILED;
}


void CHyperlink::
SetLinkColor (const COLORREF & color)
{
  m_Color = color;
  UpdateWindow ();
}


void CHyperlink::
SetVisitedLinkColor (const COLORREF & color)
{
  m_VisitedColor = color;
  UpdateWindow ();
}


void CHyperlink::
SetHighlightLinkColor (const COLORREF & color)
{
  m_HighlightColor = color;
  UpdateWindow ();
}


void CHyperlink::
OnMouseMove (UINT nFlags, CPoint point)
{
  if (!m_bUseHighlight)
    return;

  CRect rc;
  GetClientRect (rc);
  if (rc.PtInRect (point))
    {
      if (m_State != ST_HIGHLIGHTED)
        {
          SetCapture ();
          HighLight (TRUE);
        }
    }
  else
    {
      if (m_State == ST_HIGHLIGHTED)
        {
          HighLight (FALSE);
          ReleaseCapture ();
        }
    }

  CStatic::OnMouseMove (nFlags, point);
}


void CHyperlink::
HighLight (BOOL state)
{
  if (state)
    {
      if (m_State != ST_HIGHLIGHTED)
        {
          m_OldState = m_State;
          m_State = ST_HIGHLIGHTED;
          Invalidate ();
        }
    }
  else
    {
      if (m_State == ST_HIGHLIGHTED)
        {
          m_State = m_OldState;
          Invalidate ();
        }
    }
}




void CHyperlink::
OnCopyShortcut ()
{
  if (OpenClipboard ())
    {
      int nBytes = sizeof (TCHAR) * (m_sActualLink.GetLength () + 1);
      HANDLE hMem = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, nBytes);
      TCHAR *pData = (TCHAR *) GlobalLock (hMem);
      _tcscpy (pData, (LPCTSTR) m_sActualLink);
      GlobalUnlock (hMem);
      SetClipboardData (CF_TEXT, hMem);
      CloseClipboard ();
    }
}



BOOL CHyperlink::Open (int how /*= OPEN*/ )
const
{
  CWaitCursor cursor;

  HINSTANCE hRun = ShellExecute (GetParent ()->GetSafeHwnd (), "open", m_sActualLink, NULL, NULL, SW_SHOW);

  if ((int) hRun <= HINSTANCE_ERROR)
    {
      TRACE ("Failed to invoke URL using ShellExecute\n");
      return FALSE;
    }
  return TRUE;
}


LONG
GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
  HKEY hkey;
  LONG retval = RegOpenKeyEx (key, subkey, 0, KEY_QUERY_VALUE, &hkey);

  if (retval == ERROR_SUCCESS)
    {
      long datasize = MAX_PATH;
      TCHAR data[MAX_PATH];
      RegQueryValue (hkey, NULL, data, &datasize);
      _tcscpy (retdata, data);
      RegCloseKey (hkey);
    }

  return retval;
}


#ifdef _DEBUG
HBRUSH CHyperlink::
CtlColor (CDC * pDC, UINT nCtlColor)
#else
HBRUSH CHyperlink::
CtlColor (CDC * pDC, UINT /*nCtlColor */ )
#endif
{
  ASSERT (nCtlColor == CTLCOLOR_STATIC);

  DWORD dwStyle = GetStyle ();
  if (!(dwStyle & SS_NOTIFY))
    {
      // Turn on notify flag to get mouse messages and STN_CLICKED.
      // Otherwise, I'll never get any mouse clicks!
      ::SetWindowLong (m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
    }


  HBRUSH hbr = NULL;
  if ((dwStyle & 0xFF) <= SS_RIGHT)
    {
      // Modify the font to be underline
      if (!((HFONT) m_font))
        {
          LOGFONT lf;
          GetFont ()->GetObject (sizeof (lf), &lf);

          lf.lfUnderline = TRUE;
          m_font.CreateFontIndirect (&lf);
        }
      pDC->SelectObject (&m_font);


      //set the text colors
      switch (m_State)
        {
        case ST_HIGHLIGHTED:
          pDC->SetTextColor (m_HighlightColor);
          break;
        case ST_VISITED:
          pDC->SetTextColor (m_VisitedColor);
          break;
        case ST_VISIT_FAILED:
          pDC->SetTextColor (m_FailedColor);
          break;
        case ST_NOT_VISITED:
          pDC->SetTextColor (m_Color);
          break;
        default:
          ASSERT (FALSE);
        }
      pDC->SetBkMode (TRANSPARENT);

      // return hollow brush to preserve parent background color
      hbr = (HBRUSH)::GetStockObject (HOLLOW_BRUSH);
    }

  return hbr;
}

BOOL CHyperlink::
PreTranslateMessage (MSG * pMsg)
{
  m_ToolTip.RelayEvent (pMsg);
  return CStatic::PreTranslateMessage (pMsg);
}

void CHyperlink::
PreSubclassWindow ()
{
  //  if(m_sActualLink.IsEmpty() && m_sActualLink.LoadString(GetDlgCtrlID())) {
  if (m_sActualLink.IsEmpty ())
    GetWindowText (m_sActualLink);
  CheckActualHyperLink ();
  CRect rect;
  GetClientRect (rect);
  m_ToolTip.Create (this);
  m_ToolTip.AddTool (this, m_sToolTip, rect, TOOLTIP_ID);
  CStatic::PreSubclassWindow ();
}

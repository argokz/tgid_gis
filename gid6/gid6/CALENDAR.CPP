// calendar.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "calendar.h"
#include <stdlib.h> 

//#include "Ch_math.h"
/////////////////////////////////////////////////////////////////////////////
// CCalendar


CString DateToString(const COleDateTime &Date)
{
  CString S;


  if (Date.m_dt == 0) return "";
//  if (Date.GetStatus() == COleDateTime::null) return "";

  S.Format("%d.%02d.%02d", (int)Date.GetDay(), (int)Date.GetMonth(), (int)Date.GetYear());
  return S;
}


BOOL StringToDate(const CString Date,COleDateTime &Dest)
{
  CString nYear,nMonth,nDay;
  CString S=Date;
//  AllCommaMadePoint(S);
  if (S=="") 
  {
    CTime theTime = CTime::GetCurrentTime();
    S=theTime.Format("%d.%m.%Y");   
  }
  int Pos=0,Prev=0,Day,Month,Year;
  Pos=S.Find(".");
  if (Pos<0)return FALSE;
  Prev=Pos;
  nDay=S.Left(Pos);
  Day=atoi(nDay);
  
  Pos=S.Find(".",Prev+1);
  if (Pos<0)return FALSE;
  nMonth=S.Mid(Prev+1,Pos-Prev-1);
  Month=atoi(nMonth);
  nYear=S.Right(S.GetLength()-Pos-1);


  Year=atoi(nYear);

  if (Year < 100) {
    if (Year < 50) Year += 2000;
    else Year += 1900;
  }


  
  Dest.SetDate(Year,Month,Day);
  return TRUE;
}


CCalendar::CCalendar()
{
  CanSaveChange=TRUE;
//  EditGrid=NULL;
}
//----------------------------------------------------------------------------
CCalendar::~CCalendar()
{
}
//----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCalendar, CMonthCalCtrl)
  //{{AFX_MSG_MAP(CCalendar)
  ON_WM_KILLFOCUS()
  ON_WM_SETFOCUS()
  ON_WM_KEYDOWN()
  ON_WM_LBUTTONDBLCLK()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendar message handlers

void CCalendar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  CanSaveChange=TRUE;
  ShowWindow(SW_HIDE);
  GetParent()->SetFocus();
  
//  CMonthCalCtrl::OnLButtonDblClk(nFlags, point);
}


BOOL CCalendar::PreTranslateMessage(MSG* pMsg) 
{
  if (pMsg->message==WM_KEYDOWN)
  {
    if (pMsg->wParam==VK_LEFT || pMsg->wParam==VK_RIGHT || pMsg->wParam==VK_UP || pMsg->wParam==VK_DOWN) {
      SendMessage(WM_KEYDOWN,pMsg->wParam,0);
      return TRUE;
    }

    if (pMsg->wParam==VK_RETURN)
    {
      CanSaveChange=TRUE;
      ShowWindow(SW_HIDE);
      GetParent()->SetFocus();
      return TRUE;
    }
    if (pMsg->wParam==VK_ESCAPE)
    {
      CanSaveChange=FALSE;
      ShowWindow(SW_HIDE);
      GetParent()->SetFocus();
      return TRUE;
    }
  }
  return CMonthCalCtrl::PreTranslateMessage(pMsg);
}
//----------------------------------------------------------------------------
void CCalendar::OnKillFocus(CWnd* pNewWnd) 
{
  CMonthCalCtrl::OnKillFocus(pNewWnd);
  if (CanSaveChange==TRUE)
  {
    CanSaveChange=FALSE;
    CString S;

    SYSTEMTIME stime;
    GetCurSel(&stime);
    stime.wHour=0;
    stime.wMinute=0;
    stime.wSecond=0;
    stime.wMilliseconds=0;
    COleDateTime tim(stime);
    S = DateToString(tim);

//      if (m_typ == KalendarM) {
//        S=DateToStringM(tim);
//      }

    ShowWindow(SW_HIDE);
    GetParent()->SendMessage(WM_CALENDAR, 0, (LPARAM) tim.m_dt);
  }
}
//----------------------------------------------------------------------------
void CCalendar::OnSetFocus(CWnd* pOldWnd) 
{
  CMonthCalCtrl::OnSetFocus(pOldWnd);
//  if (EditGrid==NULL) 
//  {
//    EditGrid=(CEditGrid *)GetParent();
    //SetParent(GetDesktopWindow());
//  }
  CanSaveChange=TRUE;
}
//----------------------------------------------------------------------------
void CCalendar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
/*
  switch (nChar)
  {
  case VK_LEFT :{SendMessage(WM_KEYDOWN,VK_LEFT,0);break;}
  case VK_RIGHT :{SendMessage(WM_KEYDOWN,VK_RIGHT,0);break;}
  case VK_UP :{SendMessage(WM_KEYDOWN,VK_UP,0);break;}
  case VK_DOWN :{SendMessage(WM_KEYDOWN,VK_DOWN,0);break;}
  case VK_HOME :{SendMessage(WM_KEYDOWN,VK_HOME,0);break;}
  case VK_END :{SendMessage(WM_KEYDOWN,VK_END,0);break;}
  case VK_PRIOR :{SendMessage(WM_KEYDOWN,VK_PRIOR,0);break;}
  case VK_NEXT :{SendMessage(WM_KEYDOWN,VK_NEXT,0);break;}
  }
*/
  CMonthCalCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
//----------------------------------------------------------------------------
void CCalendar::InitCalendar(const CString &Date, int typ)
{
  m_typ = typ;
  COleDateTime tim;


//  SetYearView(TRUE);

  StringToDate(Date,tim);
  SetCurSel(tim);
} 

void CCalendar::PositionCalendar(RECT PosR)
{
  RECT MinR,RealR,WinRect;
//  RealR=PosR;

  GetMinReqRect(&MinR);
  GetParent()->GetClientRect(&WinRect); //По идее парент

  int w = MinR.right-MinR.left;
  int h = MinR.bottom-MinR.top;

/*  
  RealR.left=PosR.left+(PosR.right-w)/2.0;
  RealR.right=RealR.left+w;
  RealR.top=PosR.top+(PosR.bottom-h)/2.0;
  RealR.bottom=RealR.top+h;   
*/

  RealR.left=PosR.left;
  RealR.right=RealR.left+w;
  RealR.top=PosR.top;
  RealR.bottom=RealR.top+h;   

  if (RealR.left < WinRect.left)
  {
    long Tmp=(WinRect.left-RealR.left);
    RealR.left+=Tmp;
    RealR.right+=Tmp;
  }
  else 
  {
    if (RealR.right > WinRect.right)
    {
      long Tmp=(WinRect.right-RealR.right);
      RealR.left+=Tmp;
      RealR.right+=Tmp;
    }
  }

  if (RealR.top < WinRect.top)
  {
    long Tmp=(WinRect.top-RealR.top);
    RealR.top+=Tmp;
    RealR.bottom+=Tmp;
  }
  else 
  {
    if (RealR.bottom > WinRect.bottom)
    {
      long Tmp=(WinRect.bottom-RealR.bottom);
      RealR.top+=Tmp;
      RealR.bottom+=Tmp;
    }
  }
  ShowWindow(SW_HIDE);
  SetWindowPos(&wndTopMost,RealR.left,RealR.top,(RealR.right-RealR.left),(RealR.bottom-RealR.top),SWP_NOZORDER);
  ShowWindow(SW_SHOW);
  BringWindowToTop();

  SetFocus(); 
}

BOOL CCalendar::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.style &= ~CS_DBLCLKS; 

  if( !CMonthCalCtrl::PreCreateWindow(cs) )
    return FALSE;

  return TRUE;
}

// daowork.cpp : implementation file
//
#include "stdafx.h"
#include "temlpwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplWnd
CTemplWnd::CTemplWnd()
{
}
//----------------------------------------------------------------------------
CTemplWnd::~CTemplWnd()
{ 
}
//----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CTemplWnd message handlers
void CTemplWnd::_ResetColors(SCREEN * scr)
{
  scr->ForeColor.R=255;
  scr->ForeColor.G=255;
  scr->ForeColor.B=255;

  scr->BackColor.R=0;
  scr->BackColor.G=0;
  scr->BackColor.B=128;

  scr->BackColor2.R=0;
  scr->BackColor2.G=0;
  scr->BackColor2.B=0;

  scr->AnimColor.R=255;
  scr->AnimColor.G=255;
  scr->AnimColor.B=0;
  
  scr->ColdColor.R=0;
  scr->ColdColor.G=102;
  scr->ColdColor.B=102;
    
  scr->HotColor.R=153;
  scr->HotColor.G=102;
  scr->HotColor.B=204;  
  
  NodeColor.R=255;
  NodeColor.G=0;
  NodeColor.B=0;
}
//----------------------------------------------------------------------------
void CTemplWnd::ResetColors(SCREEN * scr)
{
  if (scr!=NULL)
  {
    _ResetColors(scr);
  }
  else
  {
    _ResetColors(&sc);
  }
}
//----------------------------------------------------------------------------
void CTemplWnd::ResetWorkspace()
{
  worksp.DrawVect=false;
  worksp.ShowFont=true;
  worksp.ShowBalloons=true;   
}
//----------------------------------------------------------------------------


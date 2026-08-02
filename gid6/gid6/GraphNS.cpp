// Graph.cpp : implementation file
//
#include "stdafx.h"
#include "gid6.h"

#include "GraphNs.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CGraphNs dialog
CGraphNs::CGraphNs(GRAPH_PARAM * graph_par, CWnd* pParent /*=NULL*/)   // standard constructor
  : CGraphTemplate(graph_par,CGraphNs::IDD, pParent)
{

  //{{AFX_DATA_INIT(CGraphNs)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}
//----------------------------------------------------------------------------
void CGraphNs::DoDataExchange(CDataExchange* pDX)
{
  CGraphTemplate::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CGraphNs)
  DDX_Control(pDX, IDOK, m_OK);
  DDX_Control(pDX, IDC_COPY_TO, m_copy_to);
  //}}AFX_DATA_MAP
}
//----------------------------------------------------------------------------
CString CGraphNs::GetCaptionStrt()
{
  CString Str;
  Str.LoadString(IDS_STRING_ST_NAS_CHAR);
  return Str; 
}
//----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGraphNs, CGraphTemplate)
  //{{AFX_MSG_MAP(CGraphNs)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphNs message handlers
void CGraphNs::Draw(CDC &dc)
{
  CGraphTemplate::Draw(dc);
  CPen p1(PS_SOLID,1,_basic_color);
  CPen * def;
  def=dc.SelectObject( &p1 );
  DrawChart(dc,H_chart,&apH,FALSE);
  DrawChart(dc,N_chart,&apN,TRUE);
  DrawChart(dc,KPD_chart,&apKPD,TRUE);
  DrawEdge(dc);  
  dc.SelectObject(def);
  
}


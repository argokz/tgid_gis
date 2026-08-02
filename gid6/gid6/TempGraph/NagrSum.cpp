// NagrSum.cpp : implementation file
//

#include "stdafx.h"

//#include "../TeploCalc.h"
#include "../resource.h"

#include "NagrSum.h"
#include "ado.h"

CString get_PathOsnovn();

//namespace NEW {
//#include "../../potr4/prog/1/read_xx.h"
//}



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNagrSum dialog


static int isOpen;


CNagrSum::CNagrSum(CWnd* pParent, int id)
  : CDialog(CNagrSum::IDD, pParent)
{
  //{{AFX_DATA_INIT(CNagrSum)
  m_check1 = TRUE;
  m_check2 = TRUE;
  m_check3 = TRUE;
  m_check4 = TRUE;
  m_check5 = TRUE;
  m_edit1 = 0.0;
  m_edit2 = 0.0;
  m_edit3 = 0.0;
  m_edit4 = 0.0;
  m_edit5 = 0.0;
  m_edit_sum = 0.0;
  //}}AFX_DATA_INIT

  isOpen = FALSE;

  CAdoFile* ado = getAdo(getGidAdoName());

  CString q;

  q.Format(
      "SELECT \n"
//      "fileID,\n"
      "hsid,\n"
      "hs.name,\n"
      "sum(otop) AS otop,\n"
      "sum(vent) AS vent,\n"
      "sum(gvz) AS gvz,\n"
      "sum(gvoo) AS gvoo,\n"
      "sum(gvop) AS gvop,\n"
      "sum(otop+vent+gvz+gvoo+gvop) AS n_all\n"

      "FROM \n"
      "(\n"

      "SELECT \n"
      "n.fileID,\n"
      "ec.heatSourceID AS hsid,\n"
      "calcHLdep+calcHLindep AS otop,\n"
      "calcHLventil AS vent,\n"
      "avgHLGVScloseParall+avgHLGVScloseMix+avgHLGVScloseConseq+avgHLGVSclosePreON AS gvz,\n"
      "contAvgHLGVSopenFlow AS gvop,\n"
      "contAvgHLGVSopenRet AS gvoo\n"

      "FROM realConsumers rs\n"
      "JOIN nodes n ON n.id=rs.nodeID\n"
      "JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
      "WHERE rs.consumerStateID=1\n"

      "UNION\n"

      "SELECT\n"
      "n.fileID,\n"
      "ec.heatSourceID AS hsid,\n"
      "calcHLdep+calcHLindep+calcHLconseq+calcHLmix+calcHLparall+calcHLpreON AS otop,\n"
      "calcHLventil AS vent,\n"
      "calcHLGVSconseq+calcHLGVSmix+calcHLGVSparall+calcHLGVSpreON AS gvz,\n"
      "avgHLGVSopenSysFlow AS gvop,\n"
      "avgHLGVSopenSysRet AS gvoo\n"

      "FROM generalizedConsumers rs\n"
      "JOIN nodes n ON n.id=rs.nodeID\n"
      "JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
      "WHERE rs.consumerStateID=1\n"

      ") _Q\n"
      "LEFT JOIN heatSources hs ON hs.id=_Q.hsid\n"
      "LEFT JOIN nodes n2 ON n2.id=hs.nodeID\n"
      "WHERE hsid = %d AND n2.fileID= _Q.fileID\n"
      "group by hsid, hs.name\n", id);


  if (ado->openTable0(q)) {
      if (!ado->isEOF()) {

          m_edit1 = ado->read_double("otop");
          m_edit2 = ado->read_double("vent");
          m_edit3 = ado->read_double("gvz"); 
          m_edit4 = ado->read_double("gvop");
          m_edit5 = ado->read_double("gvoo");

          ado->MoveNext();
      }
      ado->closeTable();
  }
  m_edit_sum = m_edit1+m_edit2+m_edit3+m_edit4+m_edit5;

}


void CNagrSum::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CNagrSum)
  DDX_Check(pDX, IDC_CHECK1, m_check1);
  DDX_Check(pDX, IDC_CHECK2, m_check2);
  DDX_Check(pDX, IDC_CHECK3, m_check3);
  DDX_Check(pDX, IDC_CHECK4, m_check4);
  DDX_Check(pDX, IDC_CHECK5, m_check5);
  DDX_Text(pDX, IDC_EDIT1, m_edit1);
  DDX_Text(pDX, IDC_EDIT2, m_edit2);
  DDX_Text(pDX, IDC_EDIT3, m_edit3);
  DDX_Text(pDX, IDC_EDIT4, m_edit4);
  DDX_Text(pDX, IDC_EDIT5, m_edit5);
  DDX_Text(pDX, IDC_EDIT6, m_edit_sum);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNagrSum, CDialog)
  //{{AFX_MSG_MAP(CNagrSum)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNagrSum message handlers



BOOL CNagrSum::OnCommand(WPARAM wParam, LPARAM lParam) 
{

  if ( isOpen && HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) != IDC_EDIT6
    || HIWORD(wParam) == BN_CLICKED ) {
    UpdateData(TRUE);
    m_edit_sum = m_check1*m_edit1+m_check2*m_edit2+m_check3*m_edit3+m_check4*m_edit4+m_check5*m_edit5;
    UpdateData(FALSE);
  }
  
  return CDialog::OnCommand(wParam, lParam);
}


BOOL CNagrSum::OnInitDialog() 
{
  CDialog::OnInitDialog();
 
  
  isOpen = TRUE;
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

// PotrCx.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "Sheme\SHEME.H"

#include "PotrCx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double getScreenMas();


/////////////////////////////////////////////////////////////////////////////
// CPotrCx dialog


CPotrCx::CPotrCx(CWnd* pParent, const char *name)
  : CDialog(CPotrCx::IDD, pParent)
{
  //{{AFX_DATA_INIT(CPotrCx)
  m_gws = FALSE;
  m_otopl = FALSE;
  m_zavis = -1;
  m_elevat = -1;
  m_rez = FALSE;
  m_ventil = FALSE;
  m_nomer = name;
  m_gvs_nom = -1;
  //}}AFX_DATA_INIT

  m_gws = TRUE;
  m_otopl = TRUE;
  m_zavis = 0;
  m_elevat = 0;
  m_rez = FALSE;
  m_ventil = FALSE;

  m_Shem = NULL;

  m_profile = "CPotrCx";

}


void CPotrCx::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPotrCx)

  DDX_Control(pDX, ID_CXEMA_PR, m_cx);
  
  DDX_Check(pDX, IDC_GWS, m_gws);
  DDX_Check(pDX, IDC_OTOPL, m_otopl);
  DDX_Radio(pDX, IDC_ZAVIS, m_zavis);
  DDX_Radio(pDX, IDC_ELEVAT, m_elevat);
  DDX_Check(pDX, IDC_REZ, m_rez);
  DDX_Check(pDX, IDC_VENTIL, m_ventil);
  DDX_Text(pDX, IDC_NOMER, m_nomer);
  DDX_Radio(pDX, IDC_PODACHA, m_gvs_nom);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPotrCx, CDialog)
  //{{AFX_MSG_MAP(CPotrCx)
  ON_WM_SIZE()
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPotrCx message handlers


void CPotrCx::SetED() 
{
  GetDlgItem(IDC_ZAVIS)->EnableWindow(m_otopl);
  GetDlgItem(IDC_NEZAVIS)->EnableWindow(m_otopl);
  GetDlgItem(IDC_ELEVAT)->EnableWindow(m_otopl && m_zavis == 0);
  GetDlgItem(IDC_BEZELEVAT)->EnableWindow(m_otopl && m_zavis == 0);

  GetDlgItem(IDC_REZ)->EnableWindow(m_gws);

  GetDlgItem(IDC_PS)->EnableWindow(m_gws);
  GetDlgItem(IDC_PR)->EnableWindow(m_gws);
  GetDlgItem(IDC_SM)->EnableWindow(m_gws);
  GetDlgItem(IDC_PW)->EnableWindow(m_gws);

  GetDlgItem(IDC_PODACHA)->EnableWindow(m_gws);
  GetDlgItem(IDC_OBRATKA)->EnableWindow(m_gws);
}

CString get_nom_cxema(int gvpr, int gvsm, int gvps, int gvpw, int rez, int gvop, int gvoo, int otopln, int otoplz, int ventil, int elevat);

bool get_cxema(CString nomer, int &gvpr, int &gvsm, int &gvps, int &gvpw, int &rez, int &gvop, int &gvoo, int &otopln, int &otoplz, int &ventil, int &elevat)
{
    for (ventil = 0; ventil < 2; ventil++) {
        for (otopln = 0; otopln < 2; otopln++) {
            for (otoplz = 0; otoplz < 2; otoplz++) {
                for (gvop = 0; gvop < 2; gvop++) {
                    for (gvoo = 0; gvoo < 2; gvoo++) {
                        for (gvpr = 0; gvpr < 2; gvpr++) {
                            for (gvsm = 0; gvsm < 2; gvsm++) {
                                for (gvps = 0; gvps < 2; gvps++) {
                                    for (gvpw = 0; gvpw < 2; gvpw++) {
                                        for (rez = 0; rez < 2; rez++) {
                                            for (elevat = 0; elevat < 2; elevat++) {
                                                CString nomer2 = get_nom_cxema(gvpr, gvsm, gvps, gvpw, rez, gvop, gvoo, otopln, otoplz, ventil, elevat);
                                                if (nomer2 == nomer) {
                                                    CString nomer3 = get_nom_cxema(gvpr, gvsm, gvps, gvpw, rez, gvop, gvoo, otopln, otoplz, ventil, elevat);
                                                    return true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

CString argpath();


void CPotrCx::chCxema(const char *nomer)
{
    if (m_Shem) {
      delete m_Shem;
    }

    RECT Clre;
    CString s;

    s.Format("%s/Ñõåìû/%s.con", argpath(), nomer);

    m_Shem = new CSheme(s, Clre);
    m_cx.GetClientRect(&Clre); 

    if ( !m_Shem->Create(NULL,_T("Hi"), WS_CHILD | WS_VISIBLE, Clre, &m_cx, 8888) ) {
      m_Shem = NULL;
    }
    if (m_Shem != NULL)
    {
//      UpdateSheme();      
    }
}


BOOL CPotrCx::OnInitDialog()
{
    CDialog::OnInitDialog();

    int gvpr, gvsm, gvps, gvpw, rez, gvop, gvoo, otopln, otoplz, ventil, elevat;
    bool ret = get_cxema(m_nomer, gvpr, gvsm, gvps, gvpw, rez, gvop, gvoo, otopln, otoplz, ventil, elevat);

    if (ret) {
        m_gws = gvop || gvoo || gvpr || gvsm || gvps || gvpw;
        m_otopl = otopln || otoplz;
        m_zavis = otoplz ? 0 : 1;
        m_elevat = elevat;
        m_rez = rez;
        m_ventil = ventil;
        m_gvs_nom = gvop ? 0 : gvoo ? 1 : gvpr ? 2 : gvsm ? 3 : gvps ? 4 : gvpw ? 5 : -1;
    }

    SetED();

    chCxema(m_nomer);

    UpdateData(FALSE);

    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
      SetWindowPos(&wndTopMost,bx,by,cx,cy,SWP_NOZORDER);
    }

    return TRUE;
}


void CPotrCx::set_nomer()
{
  int ventil = m_ventil ? 1 : 0;
  int otopln = m_otopl && m_zavis == 1 ? 1 : 0;
  int otoplz = m_otopl && m_zavis == 0 ? 1 : 0;

  int gvop = m_gws && m_gvs_nom == 0 ? 1 : 0;
  int gvoo = m_gws && m_gvs_nom == 1 ? 1 : 0;
  int gvpr = m_gws && m_gvs_nom == 2 ? 1 : 0;
  int gvsm = m_gws && m_gvs_nom == 3 ? 1 : 0;
  int gvps = m_gws && m_gvs_nom == 4 ? 1 : 0;
  int gvpw = m_gws && m_gvs_nom == 5 ? 1 : 0;
  
  int rez = m_rez ? 1 : 0;

  m_nomer = get_nom_cxema(gvpr, gvsm, gvps, gvpw, rez, gvop, gvoo, otopln, otoplz, ventil, m_elevat);

  chCxema(m_nomer);
}


BOOL CPotrCx::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UpdateData(TRUE);

    SetED();
    set_nomer();
    UpdateData(FALSE);
    return CDialog::OnCommand(wParam, lParam);
}


void CPotrCx::OnSize(UINT nType, int cx, int cy) 
{
  CDialog::OnSize(nType, cx, cy);

  if (!m_cx.m_hWnd) return;

  CRect sz;
  GetClientRect(sz);

  sz.left = 360*getScreenMas();

// GetSystemMetrics(int nIndex
//);

  sz.DeflateRect(5, 5);

  m_cx.MoveWindow(sz);

  if (m_Shem) {

	  sz.right -= sz.left;
	  sz.left = 0;

	  sz.bottom -= sz.top;
	  sz.top = 0;


	  m_Shem->MoveWindow(sz);
//	  m_Shem->FitToScreen(); //Add/////////////

//	  RECT Clre;
//	  m_cx.GetClientRect(&Clre);
//  	m_Shem->setSize(Clre);
//    m_Shem->
  }
}

void CPotrCx::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi); 

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right-wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom-wpi.rcNormalPosition.top);

    CDialog::OnDestroy();
}


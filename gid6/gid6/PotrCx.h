#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPotrCx dialog

class CSheme;

class CPotrCx : public CDialog
{
// Construction
public:
  CPotrCx(CWnd* pParent, const char *name);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CPotrCx)
  enum { IDD = IDD_POTRCXEMA };
  BOOL  m_gws;
  BOOL  m_otopl;
  int   m_zavis;
  int   m_elevat;
  BOOL  m_rez;
  BOOL  m_ventil;
  CString m_nomer;
  CStatic m_cx;
  int   m_gvs_nom;
  //}}AFX_DATA

  CSheme *m_Shem;
  CString m_profile;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPotrCx)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

// Implementation
protected:
  void SetED();
  void set_nomer();

  void chCxema(const char *nomer);

  // Generated message map functions
  //{{AFX_MSG(CPotrCx)
  virtual void OnSize(UINT nType, int cx, int cy);
  virtual BOOL OnInitDialog();
  virtual void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


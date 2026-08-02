#if !defined(AFX_OTOBRINF_H__ED81385E_E4F8_490D_B318_FB7B3F54172A__INCLUDED_)
#define AFX_OTOBRINF_H__ED81385E_E4F8_490D_B318_FB7B3F54172A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtobrInf.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COtobrInf dialog

class COtobrInf : public CDialog
{
// Construction
public:
  COtobrInf(CWnd* pParent, BOOL *bIsInfo);   // standard constructor

// Dialog Data
  //{{AFX_DATA(COtobrInf)
  enum { IDD = IDD_INF };
  BOOL  m_diametr;
  BOOL  m_dlina;
  BOOL  m_napor;
  BOOL  m_objem;
  BOOL  m_rashod;
  BOOL  m_temp;
  BOOL  m_ras_pot;
  BOOL  m_raspolag;
  //}}AFX_DATA

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COtobrInf)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  BOOL *m_bIsInfo;

  // Generated message map functions
  //{{AFX_MSG(COtobrInf)
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTOBRINF_H__ED81385E_E4F8_490D_B318_FB7B3F54172A__INCLUDED_)

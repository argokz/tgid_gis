#pragma once

/////////////////////////////////////////////////////////////////////////////
// CNagrSum dialog

class CNagrSum : public CDialog
{
// Construction
public:
  CNagrSum(CWnd* pParent, int id);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CNagrSum)
  enum { IDD = IDD_NAGR_SUM };
  BOOL  m_check1;
  BOOL  m_check2;
  BOOL  m_check3;
  BOOL  m_check4;
  BOOL  m_check5;
  double  m_edit1;
  double  m_edit2;
  double  m_edit3;
  double  m_edit4;
  double  m_edit5;
  double  m_edit_sum;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CNagrSum)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CNagrSum)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


#pragma once

#include "virtlistView.h"
#include "VirtualListCtrl.h"

#include "Ultimate Grid\AdoCUG.h" 
#include "Ultimate Grid\ADOImpDatasource.h"

#include "TempGraph\TgWnd.h"


/////////////////////////////////////////////////////////////////////////////
// CTgDlg dialog

class CTgDlg : public CDialog
{
// Construction
public:
  CTgDlg(CWnd* pParent, int type_of_net, CString conn, CString db, int ist, CString cap);
  ~CTgDlg();

// Dialog Data
  //{{AFX_DATA(CTgDlg)
  enum { IDD = IDD_TG_DLG };
//  CEdit m_edit;
  AdoCug m_grid;
  CTgWnd m_tg;
  CString m_out;

  BOOL  m_check1;
  BOOL  m_check2;
  BOOL  m_check3;
  BOOL  m_check4;
  BOOL  m_check5;

  //}}AFX_DATA

  int m_type_of_net;

  long m_id;
  CString m_text;
  CString m_q;
  CString m_tn;
  CString m_db;
  CString m_connect;
  CString m_cap;
  CString m_profile;

  int m_ist;
  bool m_isTG;

  AdoHead m_head;

  _tagDSList *m_pDSList;




// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTgDlg)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  void moveID(int id);
  void Size();
  void resize();

  // Generated message map functions
  //{{AFX_MSG(CTgDlg)
  virtual LRESULT OnDblClick(WPARAM, LPARAM);
  virtual BOOL OnInitDialog();
//  virtual void OnDestroy();
  virtual void OnSize(UINT nType, int cx, int cy);
//  virtual void OnChangeEdit1();
  virtual void OnDestroy();
  virtual void OnCalc();
  virtual void OnData();
  virtual void OnPrint();
  virtual void OnPrintTable();
  virtual LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);
  virtual LRESULT OnTableChange(WPARAM wParam, LPARAM);
  virtual LRESULT OnSetOut(WPARAM wParam, LPARAM);
  virtual void OnCheck();
  virtual void OnStationary();
  virtual LRESULT OnTgStatUpdated(WPARAM wParam, LPARAM);
  

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  bool m_modal;
  virtual void OnOK();
public:
    afx_msg void OnStnClickedOut();
};

#if !defined(AFX_FILELIST_H__D1297E5B_6A3A_4B08_8136_AE608120717A__INCLUDED_)
#define AFX_FILELIST_H__D1297E5B_6A3A_4B08_8136_AE608120717A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileList dialog

class CFileList : public CDialog
{
// Construction
public:
  CFileList(CWnd* pParent, set <CString> *set_db, int is_geo);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CFileList)
  enum { IDD = IDD_FILE_LIST };
  CListBox  m_list;
//  CCheckListBox  m_list;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFileList)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  set <CString> *m_set_db;
  int m_is_geo;

  // Generated message map functions
  //{{AFX_MSG(CFileList)
  afx_msg void OnAdd();
  afx_msg void OnDelete();
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILELIST_H__D1297E5B_6A3A_4B08_8136_AE608120717A__INCLUDED_)

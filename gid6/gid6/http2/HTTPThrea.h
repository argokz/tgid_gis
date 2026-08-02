#if !defined(AFX_OUTTHREA_H__F4E1E580_73BB_11D3_B157_DEBA3DD0DA45__INCLUDED_)
#define AFX_OUTTHREA_H__F4E1E580_73BB_11D3_B157_DEBA3DD0DA45__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OutThrea.h : header file
//


class CWebServerDlg2;

/////////////////////////////////////////////////////////////////////////////
// CHttpThread thread

class CHttpThread : public CWinThread
{
  DECLARE_DYNCREATE(CHttpThread)
protected:
  CHttpThread();           // protected constructor used by dynamic creation

// Attributes
public:
  CWebServerDlg2 *m_dlg;
  void *session;

  
  CRITICAL_SECTION cs_png;
  CRITICAL_SECTION cs_log;
  CRITICAL_SECTION cs_db;
  CRITICAL_SECTION cs_find;
  CRITICAL_SECTION cs_file;

// Operations
public:
   CHttpThread(CWebServerDlg2 *m_dlg, int progr, int port, int pto, BOOL is_ssl);
   virtual ~CHttpThread();
   void stop();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CHttpThread)
  public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  //}}AFX_VIRTUAL

// Implementation
protected:
  HANDLE m_hProcess;
  bool isExit;
  int m_port;
  int m_pto;
  int m_progr;
  BOOL m_is_ssl;

  // Generated message map functions
  //{{AFX_MSG(CHttpThread)
    // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTTHREA_H__F4E1E580_73BB_11D3_B157_DEBA3DD0DA45__INCLUDED_)

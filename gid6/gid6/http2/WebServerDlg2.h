// WebServerDlg.h : header file
//

#if !defined(AFX_WEBSERVERDLG2_H__8F4AD293_7AED_4ED5_A263_0ED2ED7032C8__INCLUDED_)
#define AFX_WEBSERVERDLG2_H__8F4AD293_7AED_4ED5_A263_0ED2ED7032C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWebServerDlg2 dialog

//#include "HTTPServer2.h"
//#include "mongoose.h"


#define TIMER_ID_1  1
#define TIMER_TO_1  500

class CHttpThread;

class CWebServerDlg2 : public CDialog
{
public:
  UINT  nTimerID;

private:
    bool m_updated = false;

// Construction

public:
  void RestoreSettings();
  void SaveSettings();
  CWebServerDlg2(CWnd* pParent = NULL); // standard constructor

// Dialog Data
  //{{AFX_DATA(CWebServerDlg2)
  enum { IDD = IDD_WEBSERVER_DIALOG2 };
  CStatic m_nVisitors;
  CStatic m_nBytesRecv;
  CStatic m_nBytesSent;
  CStatic m_nRequests;
  CStatic m_nActiveConn;
  CString m_szHomeDir;
  CString m_szTilesDir;
  CString m_szWebDir;
  CString m_szIP;
  CString m_szDefIndex;
  int   m_Port;
  int   m_PortHttps;
  int   m_PTO;
  BOOL  m_HTTPS;
  CString m_szPrivate;
  CString m_szPublic;
  CString m_szStatus;
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CWebServerDlg2)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  HICON m_hIcon;
//  CHTTPServer WebServer;
  CHttpThread *m_pThread;
  CHttpThread *m_pThread80;

  // Generated message map functions
  //{{AFX_MSG(CWebServerDlg2)
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnStart();
  afx_msg void OnStop();
  afx_msg void OnClose();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnReset();
  afx_msg void OnHomedirbrowse();
  afx_msg void OnTiledirbrowse();
  afx_msg void OnWebdirbrowse();
  afx_msg void OnPublicbrowse();
  afx_msg void OnPivatebrowse();
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBSERVERDLG2_H__8F4AD293_7AED_4ED5_A263_0ED2ED7032C8__INCLUDED_)

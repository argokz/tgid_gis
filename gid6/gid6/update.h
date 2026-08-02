#pragma once

class CUpdateThread : public CWinThread
{
  DECLARE_DYNCREATE(CUpdateThread)
protected:
  CUpdateThread();           // protected constructor used by dynamic creation

// Attributes
public:
  CUpdateThread(void *t); 

// Operations
public:
  virtual ~CUpdateThread();


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CUpdateThread)
  public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CUpdateThread)
    // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG

//  CGidrView *m_view;

  DECLARE_MESSAGE_MAP()
};

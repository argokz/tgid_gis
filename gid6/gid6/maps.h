#pragma once

class CMapsThread : public CWinThread
{
  DECLARE_DYNCREATE(CMapsThread)
protected:
  CMapsThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
  CMapsThread(void *t); 
  virtual ~CMapsThread();

  void Tic();
  void Exit();
//  void Exit(CEvent *event2);
  BOOL isOff();

  void stop()
  {
    threadno = 0;
    m_isExit = 1;
  }

  int m_isExit;

  CEvent *m_event;
//  CEvent *m_event2;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMapsThread)
  public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CMapsThread)
    // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG

//  CGidrView *m_view;
  int m_tic;
  int m_isOff;

  void *threadno;

  DECLARE_MESSAGE_MAP()
};

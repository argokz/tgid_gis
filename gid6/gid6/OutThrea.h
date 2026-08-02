#if !defined(AFX_OUTTHREA_H__F4E1E580_73BB_11D3_B157_DEBA3DD0DA45__INCLUDED_)
#define AFX_OUTTHREA_H__F4E1E580_73BB_11D3_B157_DEBA3DD0DA45__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OutThrea.h : header file
//

//#include "Output.h"

class CProt;

/////////////////////////////////////////////////////////////////////////////
// COutThread thread

class COutThread : public CWinThread
{
  DECLARE_DYNCREATE(COutThread)
protected:
  COutThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
//   COutThread(COutput *output, FILE *f, HANDLE hProcess);
   COutThread(CProt *prot, FILE *f, HANDLE hProcess);
   virtual ~COutThread();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COutThread)
  public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  //}}AFX_VIRTUAL

// Implementation
protected:
  FILE * m_pF;
//  COutput *m_Output;
  CProt *m_Prot;
  HANDLE m_hProcess;

  // Generated message map functions
  //{{AFX_MSG(COutThread)
    // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTTHREA_H__F4E1E580_73BB_11D3_B157_DEBA3DD0DA45__INCLUDED_)

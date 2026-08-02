#pragma once

#include "opc.h"


/////////////////////////////////////////////////////////////////////////////
// COPCThread2 thread

class COPCThread2 : public CWinThread
{
  DECLARE_DYNCREATE(COPCThread2)
protected:

// Attributes
public:

// Operations
public:
  COPCThread2();           // protected constructor used by dynamic creation
  virtual ~COPCThread2();

  void setLst(const CArray<CString,CString> & lst);

  void Tic();
  void Exit();
  BOOL isOff();

   CString get_value(const char *name);
//  bool get_value(const char *name, char *val);

  void openOPC();
  CString getOpcVar(const char *name, double a);
//  bool getOpcVar(const char *name, char *val);
  void read_opc();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COPCThread2)
  public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(COPCThread2)
    // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG

//  CGidrView *m_view;
  int m_tic;
  int m_isExit;
  int m_isOff;
  set<string> m_lst;
  map <string, string> m_val1;
  map <string, string> m_val;

  OPCServer *opc_server;
  OPCGroup *opc_gr;
  CEvent *event;

  DECLARE_MESSAGE_MAP()
};


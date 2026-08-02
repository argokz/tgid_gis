#pragma once

#include "ugctrl.h"
#include "UGCTsarw.h"

#include "Virtdata.h"

class CVirtListData;

//#define WM_LDBL WM_USER+1567

class MyCug:public CUGCtrl
{
public:
  MyCug();
  ~MyCug();

  void setup();
  void setName(const char *tn);

protected:
  CFont m_defFont;
  CFont m_fontHeading;

  CUGSortArrowType m_sortArrow;
  int m_curSortCol;
  int m_sortDir;


  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMainFrame)
  //}}AFX_VIRTUAL

public:
  CString m_section;
  CString m_tn;
  CVirtData m_data;
  int m_index;

  bool init(CVirtListData *data);
  void HideColumn();
  void Find();
  void Sort(int col);
  
  //{{AFX_MSG(MyCug)
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()


  virtual void OnSheetSetup( int nIndex );
  virtual void OnGetCell(int col,long row,CUGCell *cell);

  virtual int OnCanSizeRow(long row);
  virtual void OnRowSized(long row,int *height);
  virtual void OnColSized(int col,int *width);
  virtual int OnTopHdgSized(int *height);


  virtual void OnSetup();
  virtual void OnHitBottom(long numrows,long rowspast,long rowsfound);
  virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
//  virtual void OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);

  virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);

};

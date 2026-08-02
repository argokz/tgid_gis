#pragma once

// AdoCug.h : interface of the AdoCug class
//
/////////////////////////////////////////////////////////////////////////////
#include "ugctrl.h"
// include the sort arrow celltype
#include "UGCTsarw.h"

#include "virt_data.h"

#include "AdoHead.h"

#include "..\PropertyGrid\PropertyGridMonthCalCtrl.h"

class AdoCug:public CUGCtrl
{
public:
  AdoCug();
  ~AdoCug();

  void setName(const char *tn, VIRT_DATA typ);

  void setEdit(bool b);
  void setEdit2(bool b);
  void setAdd (bool b);
  void setDel (bool b);
  
  void HideColumn();

  int EditRow(long row);

  void RefreshData();
  void Refresh2();
  void UpdateNow();

  VIRT_DATA m_typ;

  void setHead(AdoHead *h);
  void showAll(bool show_all);

  long GetNumRowsComplete();

  CFont *GetFontNormal();
  int Execute(const char *q);


protected:
  CFont m_defFont;
  // member variables required for the sort feedback
  CUGSortArrowType m_sortArrow;
  int m_curSortCol;
  int m_sortDir;

//  _tagDSList *m_pDSList;


private:

  CString m_section;
  CString m_tn;

  int m_row;

  bool m_is_edit;
  bool m_is_edit2;
  bool m_is_add;
  bool m_is_del;

  int m_row_cur;
  int m_col_cur;

  AdoHead *m_head;
  CPropertyGridMonthCalCtrl *m_calCtrl;

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMainFrame)
  //}}AFX_VIRTUAL

  
  //{{AFX_MSG(AdoCug)
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG

public:
  //***** Over-ridable Notify Functions *****
  virtual void OnSetup();
  virtual void OnSheetSetup( int nIndex );
  //movement and sizing
  virtual void OnHitBottom(long numrows,long rowspast,long rowsfound);
  virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
  virtual void OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);
  virtual void OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed=0);
  virtual void OnCharDown(UINT *vcKey,BOOL processed);
  //GetCellIndirect notification
  virtual void OnGetCell(int col,long row,CUGCell *cell);
  //menu notifications
  virtual void OnMenuCommand(int col,long row,int section,int item);
  virtual int  OnMenuStart(int col,long row,int section);

  virtual void OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);

  virtual void OnTabSelected(int ID);

  virtual void OnColSized(int col,int *width);
  virtual void OnRowSized(long row,int *height);
  virtual int OnTopHdgSized(int *height);
  virtual void OnDataSourceNotify(int ID,long msg,long param);
  virtual LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);
  virtual void OnCellChange(int oldcol,int newcol,long oldrow,long newrow);
  virtual LRESULT OnDateChanged(WPARAM wParam, LPARAM lParam);
  virtual void OnDestroy();
  virtual void OnExcel();
  virtual void OnMoveTo();
  virtual void OnHideColumn();
  virtual void OnEdit();

  DECLARE_MESSAGE_MAP()
};

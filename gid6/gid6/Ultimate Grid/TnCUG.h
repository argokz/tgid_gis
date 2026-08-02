#pragma once


#include "ugctrl.h"

class CAdoFile;

class TpCug:public CUGCtrl
{

public:
  CPen m_pen;

  TpCug();
  ~TpCug();

//  map<int, CString> m_view_map_ist;

  bool init(map<int, CString> &map_ist, int m4, int m10, const char *db);
  bool save();
  bool save_row(int r, int m, int sezon);
  bool save_row_ist(int r, int m, int sezon);
  bool save_row0(int r, int m, int sezon);

  bool QuickGetDouble(int col, int row, double &d);


protected:
  CAdoFile *m_ado;
  CFont m_defFont;

  int n_ist;
  int m_m4;
  int m_m10;

  int m_c0;
  int m_dc;

  map<int, CString> m_map_ist;

  void sred(int col, long row, double d);


  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMainFrame)
  //}}AFX_VIRTUAL

public:

  void setTG(int m4, int d4, int m10, int d10);
  void set_temp(const double *t, int m4, int d4, int m10, int d10);
  void set_length(int m4, int d4, int m10, int d10);

  
  //{{AFX_MSG(TpCug)
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()


  //***** Over-ridable Notify Functions *****
  virtual void OnSetup();

  //mouse and key strokes
  virtual void OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,int processed);
  virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
  virtual void OnCharDown(UINT *vcKey,BOOL processed);
  
  //GetCellIndirect notification
  virtual void OnGetCell(int col,long row,CUGCell *cell);
  //SetCell notification
//  virtual void OnSetCell(int col,long row,CUGCell *cell);
  
  //cell type notifications
  virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

  //editing
  virtual int OnEditStart(int col, long row,CWnd **edit);
  virtual int OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow);

  //focus rect setup
  virtual void OnDrawFocusRect(CDC *dc,RECT *rect);

  virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
  
  COLORREF OnGetDefBackColor(int section);
};

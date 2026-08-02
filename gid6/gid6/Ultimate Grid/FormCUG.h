#pragma once


#include "ugctrl.h"

class CAdoFile;

struct FormRow
{
  CString name;
  CString label;
  CString razmer;
  CString m_val[256];
  int typ;
};

class FormCug:public CUGCtrl
{

public:
  CPen m_pen;

  FormCug();
  ~FormCug();

//  map<int, CString> m_view_map_ist;

  bool init();
  bool save();
  bool save_row(int r, int m, int sezon);
  bool save_row_ist(int r, int m, int sezon);
  bool save_row0(int r, int m, int sezon);

  bool QuickGetDouble(int col, int row, double &d);

  vector<FormRow> m_row;

protected:
  CAdoFile *m_ado;
  CFont m_defFont;

  CString m_section;

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

  map<int, map<int, CString> > m_map_change;
  
  //{{AFX_MSG(FormCug)
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  virtual void OnGetCell(int col,long row,CUGCell *cell);

  //***** Over-ridable Notify Functions *****
  virtual void OnSetup();

  //mouse and key strokes
  virtual void OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,int processed);
  virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
  virtual void OnCharDown(UINT *vcKey,BOOL processed);
  
  //GetCellIndirect notification
  //SetCell notification
//  virtual void OnSetCell(int col,long row,CUGCell *cell);
  
  //cell type notifications
  virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

  //editing
  virtual int OnEditStart(int col, long row,CWnd **edit);
  virtual int OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow);
  virtual int OnTopHdgSized(int *height);
  virtual void OnColSized(int col,int *width);
  virtual void OnRowSized(long row,int *height);

  //focus rect setup
  virtual void OnDrawFocusRect(CDC *dc,RECT *rect);

  virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
  
  COLORREF OnGetDefBackColor(int section);
};

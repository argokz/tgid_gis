#if !defined(AFX_FORMDIALOG_H__8EFF51A9_21B6_4B60_9987_D808D93FE733__INCLUDED_)
#define AFX_FORMDIALOG_H__8EFF51A9_21B6_4B60_9987_D808D93FE733__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormDialog2.h : header file
//

//#include "EditGrid2.h"

#define N256 256

/////////////////////////////////////////////////////////////////////////////
// CFormDialog2 form view

#include "dfm.h"

#include "calendar.h"

class CMyEdit;
class CMyComboBox;
class CMyComboBox2;

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

enum {
  TIP_NO = 0,
  TIP_STATIC,
  TIP_PODPIS,
  TIP_PODPIS2,
  TIP_EDIT,
  TIP_DATE,
  TIP_READONLY,
  TIP_COMBO,
  TIP_COMBOEDIT,
  TIP_CHECKBOX,
  TIP_CHECKBOX2,
  TIP_GROUPBOX,
  TIP_BUTTON,
  TIP_BUTTON2,
  TIP_TABLE,
  TIP_PZGRAPH,
  TIP_MEROPR,
  TIP_NEXT
};

enum {
  BASE_NO = 0,
  BASE_MAIN,
  BASE_OUT
};


struct CControl
{
  CControl() {
    m_cod = "";
    m_ctrl = -1;
    m_id = -1;
    m_tn = "";
    m_podp = "";
    m_fieldName = "";
    m_fn = "";
    m_new_value = "";
    m_value = "";
    m_real_value = "";
    m_old_value = "";
    
    m_file_ext = "";  ///"DOCX");
    m_file_Filter = "";  // tok2.push_back("Документы|*.docx;*.doc|");
    m_file_path = ""; // 
    
    
    m_wnd = NULL;
    m_typ = -1;
    m_len = -1;
    m_is_edit = false;
    m_next = true;
    m_width = 1.;
    m_base = BASE_MAIN;
    m_main = NULL;
    m_bkColor = 0xFFFFFF;

    m_rjad = 0;

    m_bold = false;
    m_italic = false;

    is_visible = true;
    m_buf_copy = false;
    m_new_file = false;

    m_col = 1;
    m_row = 0;
    m_cols = 1;
    m_rows = 1;
    mdb = table = id = fn_id = "";
    m_q = "";
    m_nom = -1;
    m_isText = false;
    m_form = NULL;
    m_frame = NULL;
    m_left = 0; m_top = 0; m_width = 1; m_height = 1;
    m_is_file = false;

    tok.push_back("");
    int j = tok.size();
    int k = j;
  };
  
  vector <CString> tok;
  int m_ctrl;
  int m_base;
  int m_id;
  int m_col;
  bool m_isText;
  CString m_cod;
  CString m_q;
  int m_nom;
  CString m_podp;
  CString m_fn;
  CString m_real_fn;
  CString m_tn;
  CString m_fieldName;
  CString m_new_value;
  CString m_value;
  CString m_real_value;
  CString m_old_value;
  CString m_path;
  CString m_file_ext;  ///"DOCX");
  CString m_file_Filter;  // tok2.push_back("Документы|*.docx;*.doc|");
  CString m_file_path; // 


  CWnd *m_wnd;
  int m_num;
  int m_typ;
  int m_len;
  int m_row;
  int m_cols;
  int m_rows;
  int m_rjad;
  bool m_is_edit;
  bool m_next;
  bool m_is_file;
  COLORREF m_bkColor;
  bool m_bold;
  bool m_italic;
  bool is_visible;
  bool m_buf_copy;
  bool m_new_file;

//  double m_width;
  CControl *m_main;
  CFormDialog2 *m_form;
  CFrameWnd  *m_frame;

  int m_left, m_top, m_width, m_height;


  CString mdb, table, id, fn_id;

  bool operator<(const CControl& mc) const
  {
    return m_wnd < mc.m_wnd;
  }

};


class CFormDialog2 : public CFormView
{
protected:
  CFormDialog2();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CFormDialog2)


// Form Data
public:
  //{{AFX_DATA(CFormDialog2)
//  enum { IDD = IDD_DIALOG_IN };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

// Attributes
public:
  
  map <CControl, CString> map_field;
  map<int, CControl*> map_control;

  CString m_mdb;
  CString m_fn;


// Operations
public:
  CControl *addControl(int ctrl, const char *fn, int row, int col, int cols);
  CControl *addControlObj2(int ctrl);
  CControl *addControlObj(int ctrl, OBJ obj);
  void nextRow();
  BOOL setValue(const char *tn, const char *fn, const char *val);
  void chValue(CControl *control, const char *val);
  CControl *getControl(const char *fn);
  CControl *getControlR(const char *tn, const char *fn, int rjad);
  BOOL getValue(const char *fn, CString &val);
  BOOL setValue(CControl *control, const char *val);
  BOOL setValue(CControl *control, const char *val, const char *real);
  CString getValue(CControl *control);

  CControl *getControlC(int id);
  void setPS(const char *fn, const char *val, const char *mdb1, const char *table, const char *id, const char *fn_id);
  int get_c2(int i);
  void setXY(const int *row_h, const int *col_h);

  void reset();

  void Init();
  void Init2();
  void Init3();
  void setFont(LOGFONT lf);
  void initSize();
  int getWidth(CControl *cnt);
  void setFontH(int h);

//  CControl *Next(CControl *control);
//  CControl *Prev(CControl *control);
  CControl *Next(CControl *control, int n = 1);
  CControl *Prev(CControl *control, int n = 1);

  void CbnChange(CControl *control, const char *real_value);


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMyView)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  virtual void OnInitialUpdate();
  protected:
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL



  void ScrollTo(CControl *control);
  void setSize(int cx, int cy);
  void setSizeD(int cx, int cy);
  CSize getSize(const char *text);


// Implementation
protected:
  virtual ~CFormDialog2();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
//  HACCEL m_hAccel;                     // dialog accelerators

  BOOL m_bSizedBefore;

  CFont *myFont;
  CFont *myFontB;
  CFont *myFontI;
  CControl m_control[10240];
  CCalendar m_calendar;
  CControl *m_currentC;
  int m_n_control;
  int m_width;
  int m_height;
  LOGFONT m_lf;
  int is_font;
  int m_isOpen;
  int m_font_h;
  int m_id;


  int m_col_w[N256], m_row_h[N256];


  void OnEnUpdate(CControl *control);

  void OnKillFocus(CControl *control);
  void OnSetFocus(CControl *control);
  void OnEnChange(CControl *control);
  void OnBnClicked(CControl *control);
  void OnCbnChange(CControl *control);


  // Generated message map functions
  //{{AFX_MSG(CFormDialog2)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnShiftF1();
  afx_msg LRESULT OnCalendar(WPARAM wParam, LPARAM lParam);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMDIALOG_H__8EFF51A9_21B6_4B60_9987_D808D93FE733__INCLUDED_)

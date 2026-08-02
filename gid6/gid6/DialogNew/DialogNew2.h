#if !defined(AFX_DIALOGNEW2_H__48CF1F5D_7476_4409_93EE_490BE08304D2__INCLUDED_)
#define AFX_DIALOGNEW2_H__48CF1F5D_7476_4409_93EE_490BE08304D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogNew.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogNew2 dialog

class CFormDialog2;
struct CControl;


#include "ado.h"
#include "coordlis.h"

#include "DialogNew2Row.h"
#include "dfm.h"

#define N_REMONT_EL 38
// Кол-во элементов ремонт
#define N_REMONT_TR 6
// Кол-во труб ремонт



class CDialogNew2 : public CDialog
{
// Construction
public:
  CDialogNew2(CWnd* pParent, CAdoFile *dbf, const char *tn, int readOnly, const char *cap = NULL, int idd = CDialogNew2::IDD);   // standard constructor

  void Help(CControl *control);
  void Help(int base, const char *tn, const char *fn);
  void Do(int base, const char *tn, const char *fn);
  void Do(CControl *control);
  BOOL saveDB();
  BOOL saveDB(const char *tn, CAdoFile *dbf, int row);
  BOOL saveDB2();
  BOOL saveDB2(const char *tn, const char *q, int nom);
  BOOL chValue(CControl *control, const char *val, const char *old_val);
  BOOL chValue2(CControl *control, const char *val, const char *old_val);
  BOOL killFocus(CControl *control);
  BOOL setValue(const char *tn, const char *fn, const char *val);
  void addQ(const char *tn, const char *q, int nom);
  void addOut(const char *tn, int nP, int nO);
  void addProgr();
  void addCheck(bool (*_check_data) (CDialogNew2 *dlg)) {check_data = _check_data;};

  void moveValue(const char *fn);
  int getID(const char *tn);
  CString getDbValue(const char *tn, const char *fn);
  CString getValue(const char *fn);
  CString getRealValue(const char *fn);
  BOOL setValue(const char *fn, const char *val);
  BOOL setValue0(const char *fn, const char *val);

  
  CControl *getControl(const char *fn);
  CControl *getControlR(const char *tn, const char *fn, int rjad);
  CString getValue(CControl* control);

  void ActivateFrame(CFrameWnd *frame);

  void setLen(double len) {m_len = len; }

  void Call(CControl *control, const char *qq, const char *tn);
  
  void reread(CFormDialog2 *frm, const char *tn);

  CAdoFile *getDB() {
    return m_dbf;
  }

  void setPO(int n) {
    m_nom_po = n;
  }

  void setNagr(int nagr) {
    m_nagr = nagr;
  }
  void setTU(int tu) {
    m_tu = tu;
  }



// Dialog Data
  //{{AFX_DATA(CDialogNew)
  enum { IDD = IDD_DIALOG_NEW };
/*
  CButton m_ok;
  CButton m_cancel;
  CButton m_table;
  CButton m_save;
  CButton m_copy_geo;
*/
  CFrameWnd  *m_pFrame;
  CFrameWnd  *m_pFrames[256];
//  int m_cur;
  int m_n_tab;
  CTabCtrl  m_tab;
  CString szSection1;
  //}}AFX_DATA

  CString m_q[256];
  int m_nom[256];
  CString m_tn[256];
  int m_n_q;
  int ngP;
  int ngO;
  CString tnOut;
  CString m_name;
  CString m_font_save_name;

private:
  CAdoFile *m_dbf;

  double m_len;

  bool (*check_data) (CDialogNew2 *dlg);

  CString m_cap;
  int m_geo_nom;
  int m_readOnly;
  int m_nom_po;

  int m_nagr;
  int m_tu;

  bool m_is_change;

  ROW2 db_row;

  map<CString, map<int, map<CString, CString> > > m_map_val;
  map<int, vector<CString> > m_map_dial;
  map<CString, CString> m_map_f_t;
  map<CString, vector <CString> > m_map_lst2;
  map<CString, CString> m_map_value0;


  CWnd *m_parent;
  int m_cx0, m_cy0;

  HACCEL m_hAccel;                     // dialog accelerators

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDialogNew)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

  void setDialog(int cx, int cy);

  BOOL viewTable(const char *tn, const char *title, bool isEdit);
  CFormDialog2 *viewTable2(const char *tn, const char *title, list<OBJ2> & l_ob);
  BOOL viewTable2(CFormDialog2 *view, const char *tn, const char *title, list<OBJ2> & l_ob);

  bool setTok(const char *t2, const char *t3, const char *t4, const char *t5, vector <CString> &tok2);
  BOOL viewTableFile(const char *tn, const char *title, const vector <CString> & fields, int n);
  BOOL viewTableFile2(const char *tn, const char *title, const vector <CString> & fields, int n);
  BOOL viewTableDFM(const char *tn, const char *title, list<OBJ> l_ob, map<CString, CString> &map_ob, map<CString, vector <CString> > & map_lst2);
  BOOL isChange();


  BOOL viewTableOut(CAdoFile *dbf, const char *tn, int n1, int n2, const char *title);
  BOOL viewLine(CAdoFile *dbf, CFormDialog2 *view, const char *tn);
  BOOL viewLine1(CAdoFile *dbf, CFormDialog2 *view, const char *tn, const char *podp, const char *kod, const char *uzel, const char *pr);
  BOOL viewField(CAdoFile *dbf, CFormDialog2 *view, const char *tn, const char *fn, double width);
  BOOL getTab();
  bool saveDialog();
  bool saveFormDialog2(CFormDialog2 *view);
  void copyGeo2(const char *geoF, int m_geo_nom, const char *tn, const char *q) ;

  void closeDialog();
  void copyGeo();
  
  void setElem(int n, const char *avar_el);
  void setElem2(int n, const char *avar_el);


  BOOL setValue(CControl* control, const char *val);
  BOOL setValue(CControl* control, const char *val, const char *real);

  void sosto(const char *op);
  void CreateWord(const char *m_doc_path1, const char *tn, const char *fn);
  void ViewWord(const char *m_doc_path);
  BOOL saveWord( CAdoFile *dbf, const char *fn, CString &m_doc_path);

  void delRow(CControl *control, int id);
  void addRow(CControl *control, int id, const char *tn);
  void addRowOprMer(CControl *control, int id, const char *tn);

  CString getQ(const char *tn);


// Implementation
protected:

  UINT m_nTimerWord;
  CString m_doc_name;
  CString m_doc_path1;
  CString m_doc_path2;
  CString m_doc_path3;
  CString m_doc_path4;
  CString m_doc_path5;
  CString m_doc_path6;
  CString m_doc_path7;
  CString m_avar_el;
  CString m_avar_tr;

  CCoordList m_NP;


  // Generated message map functions
  //{{AFX_MSG(CDialogNew)
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSelchangingTab1(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnAdd();
  afx_msg void OnFont();
  afx_msg void OnTab();
  afx_msg void OnDel();
  afx_msg void OnSave();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnClose();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  afx_msg void OnAnnul();
  afx_msg void OnZav();
  afx_msg void OnMon();
  afx_msg void OnDeistv();
  afx_msg void OnShiftF1();
  afx_msg void OnTimer(UINT nIDEvent);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


//int editQ_2(CWnd *wnd, CAdoFile &dbf, const char *tn, const char *qq, int nom, int readOnly, const char *cap = NULL, int idd = IDD_DIALOG_NEW);
//int editQ_2(CWnd *wnd, const char *mdb, const char *tn, const char *q, int nom, int readOnly, const char *cap = NULL, int idd = IDD_DIALOG_NEW);


int editQ_2(CWnd *wnd, CAdoFile &dbf, const char *tn, const char *tn1, const char *qq1, int nom1, const char *tn2, const char *qq2, int nom2, int readOnly, const char *cap, int idd);
int editQ_2(CWnd *wnd, const char *mdb, const char *tn, const char *tn1, const char *q1, int nom1, const char *tn2, const char *q2, int nom2, int readOnly, const char *cap, int idd);


int editQ_1(CWnd *wnd, CAdoFile &dbf, const char *tn, const char *qq, int nom, int readOnly, const char *cap = NULL, int idd = IDD_DIALOG_NEW);
int editQ_1(CWnd *wnd, const char *mdb, const char *tn, const char *q1, int nom1, int readOnly, const char *cap = NULL, int idd = IDD_DIALOG_NEW);

int editQ_1(CWnd *wnd, const char *mdb, const char *tn, int nom1, int readOnly, const char *cap = NULL, int idd = IDD_DIALOG_NEW);
int editQ_1(CWnd *wnd, CAdoFile &dbf, const char *tn, int nom1, int readOnly, const char *cap, int idd = IDD_DIALOG_NEW);



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGNEW2_H__48CF1F5D_7476_4409_93EE_490BE08304D2__INCLUDED_)

#pragma once

class CGidrView;

class CNode2;
class CCoordList;

/////////////////////////////////////////////////////////////////////////////
// CPtsOpressovka dialog

class KlList;

class CPtsOpressovka : public CPaneDialog
{
//  DECLARE_DYNCREATE(CPtsOpressovka)

// Construction
public:
//  CPtsOpressovka();
  CPtsOpressovka(const char *cap, int is_mag);
  ~CPtsOpressovka();

  void SetKls(KlList *kl_list);
  void Init0();

// Dialog Data
  //{{AFX_DATA(CPtsOpressovka)
//  enum { IDD = IDD_PTS_ALMA1 };
  CTreeCtrl m_ctrlTree;
  CListBox  m_list_uzel1;
  CListBox  m_list_uzel2;
  CString m_uzel1;
  CString m_uzel2;
  //}}AFX_DATA

  void setEdit() {m_is_edit = TRUE;};
  bool findMsRs(int id);
  bool findMag(int mag);
  void reset();
//  void setNode(int id);

// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CPtsOpressovka)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  CImageList m_imgList;
  CFont m_Font;
  BOOL m_bInit;
  KlList *m_kl_list;
  int m_id;
  int m_id2;
  int m_is_edit;
  
  CString m_cap;

  HTREEITEM m_raion;
  HTREEITEM m_nachal;
  HTREEITEM m_nach;
  HTREEITEM m_magistr;

  HTREEITEM m_hi;

  map<CString, const CNode2 *> map_node;



  int m_mag;

  void addRaion(HTREEITEM ir);
  void addRaion1(HTREEITEM ir);
  void addMaster(HTREEITEM ir);
  void addMaster(HTREEITEM ir, int id, int typ);
  void addRS(HTREEITEM ir, int id, int typ, int master);

  void setU(int id);

  bool Init2(int message);
  void addMag(HTREEITEM ir);
  void addKvartal(HTREEITEM ir, int id);
  void addKvartal2(HTREEITEM ir, int id, const char *kv);
  void addUt(HTREEITEM ir);
  void addUt(HTREEITEM ir, int id);
  void addUt(HTREEITEM ir, int id, CString kod);
  void addUtKv(HTREEITEM ir, int id, int nom);
  void addUt(HTREEITEM ir, const CCoordList &cl);
  void addUt(HTREEITEM ir, CString kod);
  void addUtID(HTREEITEM ir, const CCoordList &cl, int id);
  void addKv(HTREEITEM ir, const char *tn, const char *fn);
  void addKvID(HTREEITEM ir, const char *tn, const char *fn, int id);
  void runTable(CGidrView *pView, CNode2 *n, int id);
  void runTableKv(CGidrView *pView, int id);
  void runTableGeoKv(CGidrView *pView, int id, int typ);
  void runTableGeoMaster(CGidrView *pView, int id, int typ);
  void runTableMaster(CGidrView *pView, int id);
  void runTableGeo(CGidrView *pView, CNode2 *n, int id);
  void runTableGeo2(CGidrView *pView, CString kod, int id);

  void runTableVyd(CGidrView *pView, int id);

  void vydelUT(CGidrView *pView, CNode2 *nn, int id);
  void vydelUTKv(CGidrView *pView, int id);
  void vydelUTMaster(CGidrView *pView, int id);
  void printKv(int id);
  void printKv(CCoordList &cl);
  void printKv(set<CCoordList> & set_cl);

  void vydMSRS(int id);
  void vydMag(int id);


  // Generated message map functions
  //{{AFX_MSG(CPtsOpressovka)
//  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnSelchangeListUzel1();
  afx_msg void OnSelchangeListUzel2();
  afx_msg void OnPopupEdit();
  afx_msg void OnPopupEditMag();
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
//  virtual BOOL OnInitDialog();
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  afx_msg void OnPopupSaveMag();
  afx_msg void OnMoveMag();
  afx_msg void OnMove2();
  afx_msg void OnEditGraph();
  afx_msg void OnEditGraphMag();
  afx_msg void OnPassport();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
  virtual LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

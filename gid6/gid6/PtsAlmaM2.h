#pragma once

class CGidrView;

class CNode2;
class CCoordList;


/////////////////////////////////////////////////////////////////////////////
// CPtsAlmaM2 dialog

class KlList;

class CPtsAlmaM2 : public CPaneDialog
{
//  DECLARE_DYNCREATE(CPtsAlmaM2)

// Construction
public:
//  CPtsAlmaM2();
  CPtsAlmaM2(const char *cap, int is_mag);
  ~CPtsAlmaM2();

  void SetKls(KlList *kl_list);
  void Init0();

// Dialog Data
  //{{AFX_DATA(CPtsAlmaM2)
//  enum { IDD = IDD_PTS_ALMA1 };
  CTreeCtrl m_ctrlTree;
  CListBox  m_list_uzel1;
  CListBox  m_list_uzel2;
  CString m_uzel1;
  CString m_uzel2;
  int m_Index1;
  int m_Index2;  
//}}AFX_DATA

  void setEdit() {m_is_edit = TRUE;};
  bool findMsRs(int id);
  bool findMag(int mag);
  void reset();
//  void setNode(int id);

// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CPtsAlmaM2)
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
  map<int, int> map_line;


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

  void vydMSRS(int id, bool is_only_big_pipe);
  void vydMag(int id);

  bool selectKod(CWnd *wnd, CTreeCtrl &m_ctrlTree, HTREEITEM m_nachal, int ue, int id, int m_mag);

  void Click(HTREEITEM hItem);

  // Generated message map functions
  //{{AFX_MSG(CPtsAlmaM2)
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
  afx_msg void OnMoveNach();
  afx_msg void OnMove2();
  afx_msg void OnEditGraph();
  afx_msg void OnNodePTS();
  afx_msg void OnNodePTSUt();
  afx_msg void OnEditGraphMag();
  afx_msg void OnPassport();
  afx_msg void OnPassport2();
  afx_msg void OnOsmotrNew();     
  afx_msg void OnOsmotrLast();    
  afx_msg void OnOsmotrHist();    
  afx_msg void OnOsmotrHistNach();
  
  
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
  virtual LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

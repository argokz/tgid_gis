#pragma once

class CGraph2;
class CAdoFile;

enum { 
  TIP_calcTemperatures = 1,
  TIP_externalCodes,
  TIP_fragments,
  TIP_gvsLoadGraphs,
  TIP_heatSources,
  TIP_heatSystem,
  TIP_specExpends,
  TIP_varCoefficients
};

/////////////////////////////////////////////////////////////////////////////
// CUnvisualTree dialog

class CUnvisualTree : public CDialog
{
// Construction
public:
  CUnvisualTree(CWnd* pParent, CGraph2 *graph, CAdoFile *ado, int fileID);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CUnvisualTree)
  enum { IDD = IDD_UNVISUAL };
  CTreeCtrl m_tree;
  //}}AFX_DATA

  CString str;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CUnvisualTree)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

    CGraph2 *m_graph;
    HTREEITEM m_hCT, m_hFR, m_hRS, m_hZN, m_h4, m_hUR, m_hTR, m_hKV, m_hGV, m_hIST, m_hIST2, m_hIST3;
    HTREEITEM m_h;
    CAdoFile *m_ado;
    int m_id;
    int m_typ;
    CString m_tn;
    CString m_tn_name;
    int m_fileID;
    bool m_isTG;

  CFont m_Font;


  void init();
  void add(const char *tn, const char *fn, HTREEITEM h);

  // Generated message map functions
  //{{AFX_MSG(CUnvisualTree)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

  virtual void OnPopupAdd();
  virtual void OnPopupDel();
  virtual void OnPopupEdit();
  virtual void OnTG();
  virtual void OnTP();
  virtual void OnMoveTo();

  virtual LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

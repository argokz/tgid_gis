#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMagNode dialog

class CGidrView;

CString num_MSRS(const char *s);

struct MAG_NODE
{
  int id;
  CString name;
  CString opis;
};

extern const CNode2 *m_n1_opr_1;
extern const CNode2 *m_n1_opr_2;


struct lessMSRS1
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    result_type operator() ( const CString &arg1, const CString & arg2) const
    {
        CString n1 = num_MSRS(arg1);
        CString n2 = num_MSRS(arg2);
        return n1 < n2;
    }
};

class CMagNode : public CDialog
{
// Construction
public:
  CMagNode(CGidrView* pParent, int mag);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CMagNode)
  enum { IDD = IDD_MAG_NODE };
  CListBox  m_list_node2;
  CListBox  m_list_node;
  CListBox  m_list_mag;
  int   m_mag;
  int   m_node;
  int   m_node2;
  //}}AFX_DATA

  int m_mag_init;

  const CNode2 *m_n1;
  const CNode2 *m_n2;

  map<CString, CNode2 *> map_node2;
  list <CNode2 *> list_node0;
  list <const CNode2 *> list_all_node;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMagNode)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  map<CString, MAG_NODE, lessMSRS1> mag_mag;
  map<CString, const CNode2 *> map_node;

  CGidrView *gidView;

  void CMagNode::fillList(list<CNode2*> &list_node);

  // Generated message map functions
  //{{AFX_MSG(CMagNode)
  virtual BOOL OnInitDialog();
  afx_msg void OnSelchangeListMag();
  afx_msg void OnSelchangeListNode();
  afx_msg void OnSelchangeListNode2();
  afx_msg void OnDblclkListNode();
  virtual void OnOK();
  afx_msg void OnDblclkListNode2();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


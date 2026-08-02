// PropGridDlg.h : header file
//

#pragma once

#include "..\PropertyGrid.h"
#include "GradientItem.h"
#include "SerialItem.h"
#include "RectItem.h"
#include "TreeItem.h"


enum TYP_OF_P
{
    TP_ZERO = 0,
    TP_NODE = 1,
    TP_LINE = 2,
    TP_GEO = 3,
    TP_OPRES = 4,
    TP_REMONT = 5,
    TP_OSVIDET = 6,
    TP_OSMOTR = 7,
    TP_FAKTORY = 8,

    TP_OSMOTR_AUTODELETE = 9,
    TP_OPRES_AUTODELETE = 10,
    TP_REMONT_AUTODELETE = 11,

    
    TP_MS = 127,
    TP_RS = 128,
    TP_FAKTORY_OSMOTR = 129,
    TP_FAKTORY_AUTODELETE = 130,

    TP_DMITRI = 131,
    
    TP_AUTODELETE = 7777,
    TP_READONLY = 7777
};


struct CPropertyParam;

struct CGridData
{
  CGridData() 
  {
    name = label = help = "";
    isList = false;
    isFirst = true;
  };
  CString getIdent()
  {
    CString s;
    s.Format("%d##%s", table_num, name);
    return s;
    
//    return table + "##" + name;
  };

  CGridData(CString name, CString val);

  
  CString table;
  int id;
  CString name;
  CString label;
  CString help;
  COleVariant value;
  short typ;
  short DefinedSize;
  short NumericScale;
  short Precision;
  int table_num;
  bool isList;
  bool isFirst;
  CString q;
};

typedef bool (*fun_prop)(list<CItemChanged> & lst, void *param, long id);

// CPropGridDlg dialog
class CPropGridDlg : public CDialog
{
  // Construction
public:
  CPropGridDlg(CWnd* pParent, fun_prop fun_proc, void *param, const char *table, const char *capt, TYP_OF_P typ_of_p, void *param2, bool shift = true); // standard constructor

//  BOOL Populate(const char *tn, list<CGridData> &lst);
  HSECTION addSection(const char *tn, bool collapsed, int number);
  HITEM addDataGrid(HSECTION hs, CGridData &dg);
  HITEM addButton(HSECTION hs, const char *label, const char *fun, int id);

  void setSend(long id) { m_send_value = id;};

  void setID(int id) { m_id = id;};
  int getID() { return m_id;};


  // Dialog Data
  enum { IDD = IDD_PROPGRID_DIALOG_MODAL };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

protected:
  CGradientItem m_item_gradient;
  CSerialItem m_item_serial;
  CRectItem m_item_rect;
  CTreeItem m_item_tree;
  int m_id;

  // Implementation
protected:
  HICON m_hIcon;
  HITEM m_hItemLines;
  HITEM m_hItemShade;
  HITEM m_hItemGutter;
  HITEM m_hItemFocusDis;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()

public:
  CPropertyGrid m_ctrlGrid;
  CEdit m_status;
  CString m_str_status;

  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMove(int x, int y);
  afx_msg LRESULT OnItemChanged(WPARAM, LPARAM);
  afx_msg LRESULT OnFocusChanged(WPARAM, LPARAM);
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCollapseAll();
  afx_msg void OnBnClickedExpandAll();
  afx_msg void OnBnClickedCustomColors();
  afx_msg void OnBnClickedCategorized();
  afx_msg void OnBnClickedAlphabetical();
  afx_msg void OnBnClickedNosort();
  afx_msg void OnBnClickedCopy();

  afx_msg void OnBnClickedA();
  afx_msg void OnBnClickedZ();
  afx_msg void OnBnClickedD();
  
  afx_msg void OnDestroy();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nHitTest, CPoint point);
  afx_msg void OnOK();
  afx_msg void OnCancel();
  afx_msg BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);

  void *m_param;
  CPropertyParam *m_property_param;

private:
  int m_b;
  bool m_resizing_gutter2;
  CPoint m_ptLast;
  CRect m_sz;
  fun_prop m_fun;

  bool m_shift;

  TYP_OF_P m_typ_of_p;
  void *m_param2;

  long m_send_value;
  CFont m_font;

  CString m_strCaption;
  CString m_szSection;
  CString m_table;

  CWnd *m_par;

};

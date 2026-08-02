//{{AFX_INCLUDES()
#include "grid/msflexgrid.h"
//}}AFX_INCLUDES

#pragma once

// TeplopotrBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTeplopotrBox dialog

struct ITOG{
  ITOG()
  {
    n_otz = n_otn = n_vn = n_kn = n_txz = n_txop = n_txoo = n_gvop = n_gvoo = n_rez = n_gvz = 0;
    q_otz = q_otn = q_vn = q_kn = q_txz = q_txop = q_txoo = q_gvop = q_gvoo = q_rez = q_gvz = 0;
  }
  char    kodrs[3+1];
                          /*  нагрузки итоговые по расч.схеме*/
  double   n_otz;          /*  на отопление                   */
  double   n_otn;          /*  на отопление                   */
  double   n_vn;           /*  на вентиляцию                  */
  double   n_kn;           /*  на кондиционирование           */
  double   n_txz;          /*  на технологию закрытую         */
  double   n_txop;         /*  технология откр. из подачи     */
  double   n_txoo;         /*  технология откр. из обратки    */
  double   n_gvop;         /*  г.в. открытое из подачи        */
  double   n_gvoo;         /*  г.в. открытое из обратки       */
  double   n_rez;          /*  рециркуляцион. нагрузка откр.ГВ*/
  double   n_gvz;          /*  г.в. закрытое суммарное        */
                          /*  расходы итоговые по расч.схеме */
  double   q_otz;          /*  на отопление                   */
  double   q_otn;          /*  на отопление                   */
  double   q_vn;           /*  на вентиляцию                  */
  double   q_kn;           /*  на кондиционирование           */
  double   q_txz;          /*  на технологию закрытую         */
  double   q_txop;         /*  технология откр. из подачи     */
  double   q_txoo;         /*  технология откр. из обратки    */
  double   q_gvop;         /*  г.в. открытое из подачи        */
  double   q_gvoo;         /*  г.в. открытое из обратки       */
  double   q_rez;          /*  рециркуляционный расход откр.ГВ*/
  double   q_gvz;          /*  г.в. закрытое суммарное        */
};


class CTeplopotrBox : public CDialog
{
// Construction
public:
  CTeplopotrBox(CWnd* pParent, ITOG &itog, ITOG &itog1, const char *cap);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CTeplopotrBox)
  enum { IDD = IDD_TEPLOPOTR };
  CMSFlexGrid m_grid;
  //}}AFX_DATA

  CString m_strCaption;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTeplopotrBox)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  ITOG m_itog;
  ITOG m_itog1;

  // Generated message map functions
  //{{AFX_MSG(CTeplopotrBox)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


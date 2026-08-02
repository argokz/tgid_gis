// TeplopotrBox.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "TeplopotrBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTeplopotrBox dialog


CTeplopotrBox::CTeplopotrBox(CWnd* pParent, ITOG &itog, ITOG &itog1, const char *capt)
	: CDialog(CTeplopotrBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTeplopotrBox)
	//}}AFX_DATA_INIT

  m_itog = itog;
  m_itog1 = itog1;

  m_strCaption = "";
  if ( capt ) {
    m_strCaption = capt;
  }
}


void CTeplopotrBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTeplopotrBox)
	DDX_Control(pDX, IDC_MSFLEXGRID1, m_grid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTeplopotrBox, CDialog)
	//{{AFX_MSG_MAP(CTeplopotrBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTeplopotrBox message handlers


void SetDoubleMatrix(CMSFlexGrid &m_grid, int row, int col, double q)
{
  CString str;
  str.Format("%g", q);
  m_grid.SetTextMatrix(row, col, str);
}

void SetITOG(CMSFlexGrid &m_grid, int col, ITOG &itog)
{
  int i = 1;
//  i++;
  SetDoubleMatrix(m_grid, i++, col, itog.n_otz);          /*  на отопление                   */
  SetDoubleMatrix(m_grid, i++, col, itog.n_otn);          /*  на отопление                   */
  SetDoubleMatrix(m_grid, i++, col, itog.n_vn);           /*  на вентиляцию                  */
//  SetDoubleMatrix(m_grid, i++, col, itog.n_kn);           /*  на кондиционирование           */
//  SetDoubleMatrix(m_grid, i++, col, itog.n_txz);          /*  на технологию закрытую         */
//  SetDoubleMatrix(m_grid, i++, col, itog.n_txop);         /*  технология откр. из подачи     */
//  SetDoubleMatrix(m_grid, i++, col, itog.n_txoo);         /*  технология откр. из обратки    */
  SetDoubleMatrix(m_grid, i++, col, itog.n_gvop);         /*  г.в. открытое из подачи        */
  SetDoubleMatrix(m_grid, i++, col, itog.n_gvoo);         /*  г.в. открытое из обратки       */
  SetDoubleMatrix(m_grid, i++, col, itog.n_rez);          /*  рециркуляцион. нагрузка откр.ГВ*/
  SetDoubleMatrix(m_grid, i++, col, itog.n_gvz);          /*  г.в. закрытое суммарное        */
//  i++;
                       /*  расходы итоговые по расч.схеме */
  SetDoubleMatrix(m_grid, i++, col, itog.q_otz);          /*  на отопление                   */
  SetDoubleMatrix(m_grid, i++, col, itog.q_otn);          /*  на отопление                   */
  SetDoubleMatrix(m_grid, i++, col, itog.q_vn);           /*  на вентиляцию                  */
//  SetDoubleMatrix(m_grid, i++, col, itog.q_kn);           /*  на кондиционирование           */
//  SetDoubleMatrix(m_grid, i++, col, itog.q_txz);          /*  на технологию закрытую         */
//  SetDoubleMatrix(m_grid, i++, col, itog.q_txop);         /*  технология откр. из подачи     */
//  SetDoubleMatrix(m_grid, i++, col, itog.q_txoo);         /*  технология откр. из обратки    */
  SetDoubleMatrix(m_grid, i++, col, itog.q_gvop);         /*  г.в. открытое из подачи        */
  SetDoubleMatrix(m_grid, i++, col, itog.q_gvoo);         /*  г.в. открытое из обратки       */
  SetDoubleMatrix(m_grid, i++, col, itog.q_rez);          /*  рециркуляционный расход откр.ГВ*/
  SetDoubleMatrix(m_grid, i++, col, itog.q_gvz);          /*  г.в. закрытое суммарное        */
}



BOOL CTeplopotrBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  SetWindowText(m_strCaption);

  m_grid.SetRows(25);
  m_grid.SetCols(4);
	
  m_grid.SetColWidth(0, 500);
  m_grid.SetColWidth(1, 5800);
  m_grid.SetColWidth(2, 1200);
  m_grid.SetColWidth(3, 1350);

  int i = 1;

//  m_grid.SetTextMatrix(i++, 1, _TR("нагрузки итоговые по расч.схеме"));
  m_grid.SetTextMatrix(i++, 1, _TR("Нагрузка на отопление, зависимое присоед., Гкал/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Нагрузка на отопление, независимое присоед., Гкал/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Нагрузка на вентиляцию, Гкал/ч"));
//  m_grid.SetTextMatrix(i++, 1, _TR("на кондиционирование"));
//  m_grid.SetTextMatrix(i++, 1, _TR("на технологию закрытую"));
//  m_grid.SetTextMatrix(i++, 1, _TR("технология откр. из подачи"));
//  m_grid.SetTextMatrix(i++, 1, _TR("технология откр. из обратки"));
  m_grid.SetTextMatrix(i++, 1, _TR("Нагрузка на ГВ в открытых системах из подающего, Гкал/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Нагрузка на ГВ в открытых системах из обратного, Гкал/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Нагрузка на рециркуляцию в открытых ГВ, Гкал/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Нагрузка на ГВ в закрытых системах, Гкал/ч"));
//  m_grid.SetTextMatrix(i++, 1, _TR("расходы итоговые по расч.схеме"));
  m_grid.SetTextMatrix(i++, 1, _TR("Расход на отопление, зависимое присоед., т/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Расход на отопление, независимое присоед., т/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Расход на вентиляцию, т/ч"));
//  m_grid.SetTextMatrix(i++, 1, _TR("на кондиционирование"));
//  m_grid.SetTextMatrix(i++, 1, _TR("на технологию закрытую"));
//  m_grid.SetTextMatrix(i++, 1, _TR("технология откр. из подачи"));
//  m_grid.SetTextMatrix(i++, 1, _TR("технология откр. из обратки"));
  m_grid.SetTextMatrix(i++, 1, _TR("Расход на ГВ в открытых системах из подающего, т/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Расход на ГВ в открытых системах из обратного, т/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Расход на рециркуляцию в открытых ГВ, т/ч"));
  m_grid.SetTextMatrix(i++, 1, _TR("Расход в закрытых системах, т/ч"));


  SetITOG(m_grid, 2, m_itog);
  SetITOG(m_grid, 3, m_itog1);


  m_grid.SetTextMatrix(0, 1, _TR("Параметры"));
  m_grid.SetTextMatrix(0, 2, _TR("Общие"));
  m_grid.SetTextMatrix(0, 3, _TR("Выделенные"));


  for (int j = 1; j <= i; j++) {
    CString str;
    str.Format("%d", j);
    m_grid.SetTextMatrix(j, 0, str);  
  } 


  m_grid.SetRows(i);


  m_grid.SetBackColor(0x00CCFFFF);
  m_grid.SetBackColorBkg(0x009FB8C8);
  m_grid.SetBackColorFixed(0x006FB7FF);
  m_grid.SetBackColorSel(0x0000000D);
  m_grid.SetForeColor(0x80000008);
  m_grid.SetForeColorFixed(0x80000012);
  m_grid.SetForeColorSel(0x8000000E);
  m_grid.SetGridColor(0x00C0C0C0);
  m_grid.SetGridColorFixed(0x00000000);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

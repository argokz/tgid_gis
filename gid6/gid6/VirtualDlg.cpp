// VirtualDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "gid6Doc.h"
#include "VirtualDlg.h"
#include "wm_user.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "gidrView.h"
//

double getScreenMas();
bool LoadFont2(const TCHAR* szSection, LOGFONT& lf, COLORREF& color);


/////////////////////////////////////////////////////////////////////////////
// CVirtualDlg dialog


//const CString m_profile = "VirtualDlg";

CVirtualDlg::CVirtualDlg(CWnd* pParent, CVirtListData* data, int id, const char* cap, bool modal, UINT nIDTemplate, bool fnd, bool add, bool edit, bool del, bool chk)
	: CDialog(nIDTemplate == 0 ? CVirtualDlg::IDD : nIDTemplate, pParent)
{
	m_chk_all_res = chk;
	//{{AFX_DATA_INIT(CVirtualDlg)
	m_fnd = "";
	//}}AFX_DATA_INIT

	m_data = data;
	m_cap = cap;
	m_modal = modal;

	if (m_cap == "") m_cap = "";

	m_profile.Format("VirtualDlg\\%s", m_cap);

	m_id = id;
	m_text = "";

	m_fnd = fnd;

	LOGFONT lf;
	COLORREF color;

	if (LoadFont2(_T("PropGridFont"), lf, color)) {
		m_font.CreateFontIndirect(&lf);
	}
	else {
		m_font.CreatePointFont(120, _T("Tahoma"));
	}

}

CVirtualDlg::~CVirtualDlg()
{
	//  delete m_data;
}




BOOL CVirtualDlg::PreTranslateMessage(MSG* pMsg)
{

	if (WM_KEYDOWN == pMsg->message)
	{
		if ((int)pMsg->wParam == VK_DELETE) {
			EndDialog(1004);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CVirtualDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVirtualDlg)
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_LIST1, m_grid);
	DDX_Text(pDX, IDC_EDIT1, m_find);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVirtualDlg, CDialog)
	//{{AFX_MSG_MAP(CVirtualDlg)
	ON_MESSAGE(WM_LDBL, OnDblClick)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirtualDlg message handlers


BOOL CVirtualDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_cap);

    m_grid.SetFont(&m_font, TRUE);
    m_edit.SetFont(&m_font, TRUE);
    
	Size();
	m_grid.setVirtListData(m_data);
	m_grid.init(m_id);

	int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
	int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
	int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
	int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

	if (cx > 0 && cy > 0) {
		SetWindowPos(&wndTopMost, bx, by, cx, cy, SWP_NOZORDER);
	}

	if (m_fnd) {
		m_edit.ShowWindow(SW_SHOW);
		m_edit.EnableWindow(TRUE);
	}

	Size();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CVirtualDlg::Size()
{
	CRect sz, sz1, sz2;

	GetClientRect(sz);

	sz.DeflateRect(2, 2);

	sz1 = sz2 = sz;

    LOGFONT lf;

    m_font.GetLogFont(&lf);

//	sz1.bottom = sz2.top = abs(lf.lfHeight*2)*getScreenMas();
    sz1.bottom = sz2.top = abs(lf.lfHeight)*2;

	CWnd* edt = GetDlgItem(IDC_EDIT1);

	if (edt && !edt->IsWindowEnabled()) {
		sz1.bottom = sz2.top = 0;
	}

	sz1.DeflateRect(2, 2);
	sz2.DeflateRect(2, 2);

	if (::IsWindow(m_edit.m_hWnd))
		m_edit.MoveWindow(sz1);

	if (::IsWindow(m_grid.m_hWnd))
		m_grid.MoveWindow(sz2);
}


void CVirtualDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	Size();
}

void CVirtualDlg::moveID(int row)
{
	VIRT_DATA typ = m_grid.getVirtListData()->getType();
	int id = (int)m_grid.getVirtListData()->getData(row);

	CGidrView* pView = getView();
	if (pView) {
		if (m_modal) {
		}
		else {
			switch (typ) {
			case virt_data_node:
				pView->SendMessage(WM_USER_FIND_NODE, (WPARAM)id);
				break;
			case virt_data_line:
				pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
				break;
			}
		}
	}
}


void CVirtualDlg::OnDestroy()
{
	WINDOWPLACEMENT wpi;
	GetWindowPlacement(&wpi);

	AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
	AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
	AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right - wpi.rcNormalPosition.left);
	AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom - wpi.rcNormalPosition.top);


	CDialog::OnDestroy();
	if (!m_modal)
		delete this; // Shown as non-modal, we'll clean up ourselves
}

void CVirtualDlg::OnChangeEdit1()
{
	UpdateData(TRUE);

	m_grid.getVirtListData()->setFindText(m_find);
	m_grid.UpdateCount();
	m_grid.Invalidate();

}



LRESULT CVirtualDlg::OnDblClick(WPARAM wParam, LPARAM lParam)
{
	long id = (long)wParam;

	if (!m_modal) {
		moveID(id);
	}
	else {
		if (m_chk_all_res)
		{
			int* arr = m_grid.getCurElements();
			int last = m_grid.GetSelectedCount();
			for (int i = 0; i < last; i++)
			{
				m_text += m_grid.getVirtListData()->GetItemText(-1, arr[i], arr[i], arr[i] + 1);
				if (i != last - 1)
					m_text += ",";
			}
		}
		else
			m_text = m_grid.getVirtListData()->GetItemText(0, id, id, id + 1);
		m_id = (int)m_grid.getVirtListData()->getData(id);
		EndDialog(IDOK);
	}

	//  ON_MESSAGE(WM_USER_FIND_LINE, OnFindLine)

	return 0;
}

void CVirtualDlg::OnOK()
{
	CString str;
	UpdateData(TRUE);

	long id = m_grid.getCur();
	if (m_modal) {
		if (m_chk_all_res)
		{
			int* arr = m_grid.getCurElements();
			int last = m_grid.GetSelectedCount();
			for(int i=0; i<last; i++)
			{
				m_text += m_grid.getVirtListData()->GetItemText(-1, arr[i], arr[i], arr[i] + 1);
				if (i != last - 1)
					m_text += ",";
			}
		}
		else
			m_text = m_grid.getVirtListData()->GetItemText(0, id, id, id + 1);
			
		m_id = (int)m_grid.getVirtListData()->getData(id);

		int nc = m_grid.getVirtListData()->GetCount();

		for (int i = 0; i < nc && i < 256; i++) {
			m_text_list[i] = m_grid.getVirtListData()->GetItemText(i, id, id, id + 1);
		}


		CDialog::OnOK();
	}
	else {
		moveID(m_id);
	}
}




#include "adolist.h"
#include "ado.h"
#include "gidrView.h"
#include "win.h"
bool viewVirtualDlg(CWnd* wnd, const char* _bd, const char* q, long& id, const char* cap, CString& text, bool fnd = false, bool add = false, bool edit = false, bool del = false, CString tn = "", bool chk = false);

bool viewVirtualDlg(CWnd* wnd, const char* _bd, const char* q, long& id, const char* cap, CString& text, bool fnd, bool add, bool edit, bool del, /*пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ*/ CString tn, bool chk)
{
	CGidrView* view = getView();
	CGidrView* view2 = (CGidrView*)wnd;
	CString bd = _bd;
	bd.Replace("$ps$", getPsAdoName());
	bd.Replace("$main$", getGidAdoName());

	CAdoListData* nd = new CAdoListData();

	nd->setIdVisible(false);
	if (chk)
		nd->setAfterIdVisible(false);
	int n = nd->initQpat(getAdo(bd), q);

	if (n > 0) {
		nd->setIdVisible(false);
		if (chk)
			nd->setAfterIdVisible(false);

		if (n > 20) fnd = true;
		CVirtualDlg dlg(wnd, nd, id, cap, true, IDD_VIRTUAL_LOOKUP, fnd, add, edit, del, chk);
		int ret = dlg.DoModal();

    	switch (ret)
		{
		case IDOK:
			text = dlg.m_text;
			id = dlg.m_id;
			if (!tn.IsEmpty())
			{
				if (tn.Find("realConsumers") == 0) {
					//поиск узла по id
					CNode2* node = view->m_cxema->m_graph->findNode2(TIP_PR, id);
					if (IsRezhim(node)) return false;
					//подмена меню на режим А
                    TypMenu tmpMenu = m_idMenu;
					m_idMenu = TYP_MENU_PTS;
					//вызов информации о объекте
					view->info(view->m_cxema, node, NULL);
					//возврат меню
					m_idMenu = tmpMenu;
					//переход к объекту
					view->moveNode(node);
				}
				else {
					view->moveGeo(tn, id);
//					view2->GeoInfo(tn, id, false);
                    view->GeoInfo(tn, id, false);
                }
				return true;
			}
			return true;
			break;
		case 1004:
			text = "";
			id = -1;
			return true;
			break;
		case IDCANCEL:
			break;
		default:
			break;
		}
	}
	else {
		AfxMessageBox("Нет данных", MB_OK|MB_ICONINFORMATION);
	}
	return false;
}

int regex_match(const char* p, const char* buf, vector<CString>& match);


CString getVirtualDlgText(const char* _bd, const char* q, long id)
{
	CString bd = _bd;
	bd.Replace("$ps$", getPsAdoName());
	bd.Replace("$main$", getGidAdoName());

	CString qq;

	//  qq.Format("SELECT * FROM (%s\n) __TT WHERE ID=%d", q, id);

	CString q0 = q;

	vector<CString> match;
	int l = regex_match("(.+)\\s+ORDER\\s+BY\\s+[A-Za-z0-9_]+", q, match);

	if (l > 0) {
		q0 = match[1];
	}

	q0.Format("SELECT * FROM (%s\n) ____T____123 WHERE ID=%d", q0, id);

	CAdoFile* ado = getAdo(bd);

	CString txt = "";
	if (!ado) return "";

	if (ado->openTable0(q0)) {
		txt = ado->readStr(1);
		ado->closeTable();
	}
	return txt;
}





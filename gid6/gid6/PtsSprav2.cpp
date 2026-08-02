// Address3.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "PtsSprav2.h"
#include "gidrview.h"
#include "win.h"

#include <WINDOWSX.H>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CGeoFile* m_geofile;

#define GOROD _TR("Город")

CString getPsAdoName();
CString getGidAdoName();


/////////////////////////////////////////////////////////////////////////////
// CPtsSprav21 property page

//IMPLEMENT_DYNCREATE(CPtsSprav2, CPtsTab)

CPtsSprav2::CPtsSprav2(const char* cap, const char* fn) : CPaneDialog() {
	//{{AFX_DATA_INIT(CPtsSprav2)
	//}}AFX_DATA_INIT
  //  m_dbf = NULL;
	m_kl_list = NULL;
	m_bInit = FALSE;

	m_fn = fn;

	//  m_cap = cap;
}

CPtsSprav2::~CPtsSprav2()
{
}

void CPtsSprav2::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPtsSprav2)
	DDX_Control(pDX, IDC_TREE1, m_ctrlTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPtsSprav2, CPaneDialog)
	//{{AFX_MSG_MAP(CPtsSprav2)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPtsSprav2 message handlers



void CPtsSprav2::OnSize(UINT nType, int cx, int cy)
{
	CPaneDialog::OnSize(nType, cx, cy);

	//  if(!m_bInit)
	//    return;

	if (!m_ctrlTree.m_hWnd) return;

	CRect sz;

	GetClientRect(sz);
	sz.DeflateRect(3, 3);

	m_ctrlTree.MoveWindow(sz);

	// TODO: Add your message handler code here

}

/*

struct QUE3 {
  CString q;
  CString tab;
  CString mdb;
};

static map <CString, QUE3> map_q;
*/

LRESULT CPtsSprav2::HandleInitDialog(WPARAM, LPARAM)
{
	UpdateData(FALSE);

	m_imgList.Create(IDB_TREE, 16, 1, RGB(0, 128, 128));
	m_ctrlTree.SetImageList(&m_imgList, TVSIL_NORMAL);

	void setTreeFont(CTreeCtrl * m_ctrlTree, CFont * m_Font);
	setTreeFont(&m_ctrlTree, &m_Font);

	m_ctrlTree.SetItemData(TVI_ROOT, (DWORD)0);

	HTREEITEM i0 = 0;
	CString par = "";

    CString tn;
	char s[10240];

    int nn = 0;

    FILE* f = fopenexe(m_fn, "r");
	if (f) {
    	map_q.clear();
        QUE3 que;

    	while (!feof(f)) {
			fgetstr(s, 10230, f);

    		if (s[0] == 0) continue;
			if (s[0] == '-') continue;

    		if (s[0] == '=') {
    			i0 = m_ctrlTree.InsertItem(&s[1], 0);
    			par = &s[1];
                continue;
            }
    		if (s[0] != ' ') {
        		tn = que.tab = s;
                que.q = "";
                que.mdb = "";
                nn = 0;
        		HTREEITEM i = m_ctrlTree.InsertItem(tn, i0);
            }
    		else {
                CString ss = s;
                ss.TrimLeft();
                ss.TrimRight();
                if (ss == "") continue;

                if (nn == 0) {
                    que.mdb = ss;
                    if (ss.Find("MAP") == 0) {
                        vector<CString> match;
                        int l = regex_match("MAP\\s+(.+)$", ss, match);
                        if (l > 0) {
                            que.mdb = "MAP";
                			que.tab = match[1];
                        }
                    }
                }
                else if (nn == 1) {
            		que.q = ss;
                }
                else if (nn == 2) {
                    que.add = ss;
                }
                nn++;
            }
            map_q[par + "#" + tn] = que;
        }
        fclose(f);
    }


#if 0

	FILE* f = fopenexe(m_fn, "r");
	if (f) {
		map_q.clear();

		CString tn;
		char s[1024];
		while (!feof(f)) {
			QUE3 que;
			fgetstr(s, 1023, f);

			if (s[0] == '-') continue;

			if (!strcmp(s, "STRING")) {
				fgetstr(s, 1023, f);
				par = s;
				i0 = m_ctrlTree.InsertItem(s, 0);
				continue;
			}

			tn = que.tab = s;
			fgetstr(s, 1023, f);
			que.mdb = s;

			if (que.mdb == "MAP") {
				fgetstr(s, 1023, f);
				que.tab = s;
			}

			fgetstr(s, 1023, f);
			que.q = s;

			map_q[par + "#" + tn] = que;

			//      m_ctrl_list.AddString(tn);
			HTREEITEM i = m_ctrlTree.InsertItem(tn, i0);
		}

		fclose(f);
	}
#endif

	SetWindowText(m_cap);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPtsSprav2::SetKls(KlList* kl_list)
{
	m_kl_list = kl_list;
}

#include "ado.h"
#include "join2.h"


BOOL CPtsSprav2::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	LPNMHDR lpnmh = (LPNMHDR)lParam;
	TVHITTESTINFO ht = { 0 };

	if ((lpnmh->code == NM_CLICK)) {

		DWORD dwpos = GetMessagePos();

		// include <windowsx.h> and <windows.h> header files
		ht.pt.x = GET_X_LPARAM(dwpos);
		ht.pt.y = GET_Y_LPARAM(dwpos);
		::MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);

		TreeView_HitTest(lpnmh->hwndFrom, &ht);

		//    m_ctrl_list.GetText(n, tn);

		CString tn = m_ctrlTree.GetItemText(ht.hItem);
		HTREEITEM hItem1 = m_ctrlTree.GetParentItem(ht.hItem);
		CString par = m_ctrlTree.GetItemText(hItem1);



		if (tn == "") return 0;

		map <CString, QUE3>::const_iterator it;

		it = map_q.find(par + "#" + tn);

		if (it == map_q.end()) return 0;

		CGidrView* pView = getView();  if (!pView) return 0;
		CCxema* m_cxema = pView->m_cxema;

		//    tn = transl(tn);

		QUE3 que = it->second;

		CString q = que.q, mdb = que.mdb;

		mdb.Replace("$ps$", getPsAdoName());
		mdb.Replace("$main$", getGidAdoName());

		bool isJ = (q == "");

		CString tn1 = transl(tn);

		if (q.Find("SELECT") != 0) {
			if (q != "") {
				tn1 = transl(q);
				q.Format("SELECT * FROM [%s]", transl(q));
			}
			isJ = true;
		}
		else {
			CString getTableNameFromSQL(CString q);

			tn1 = getTableNameFromSQL(q);
		}

		if (isJ) {
			//      tn = transl(tn1);
			if (mdb == "MAP") {
				Klassif* kls = m_geofile->m_kl_list->findKlN(transl(que.tab));

				if (kls) {
					mdb = kls->m_database;
				}
			}

			CAdoFile* ado = getAdo(mdb);

			if (ado) {
				Join2 j;
				int n0 = j.initJoin(ado, tn1, true);
				j.allJoin(ado, tn1, n0);
				q = j.getQ_as();
			}
		}

        TypMenu idMenu = m_idMenu;
		m_idMenu = TYP_MENU_REZHIM;

		if (mdb == "MAIN") {
			pView->RunTable(tn1, q, -1, virt_data_edit_add_del, que.add, tn);
		}
		else {
			pView->RunTableDB(mdb, tn1, q, -1, virt_data_edit_add_del, que.add, tn);
		}

		m_idMenu = idMenu;
	}
	return CPaneDialog::OnNotify(wParam, lParam, pResult);
}

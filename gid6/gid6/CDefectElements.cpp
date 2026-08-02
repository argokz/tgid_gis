// CDefectElements.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CDefectElements.h"
#include "afxdialogex.h"


// CDefectElements dialog

IMPLEMENT_DYNAMIC(CDefectElements, CDialogEx)

CDefectElements::CDefectElements(int id, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEFECT_ELEMENTS, pParent)
{
    m_id = id;

}

CDefectElements::~CDefectElements()
{
}

void CDefectElements::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);                   
}


BEGIN_MESSAGE_MAP(CDefectElements, CDialogEx)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
END_MESSAGE_MAP()


// CDefectElements message handlers

static map<int, int> map_id;


BOOL CDefectElements::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return TRUE;
    
    m_list1.SetExtendedStyle(m_list1.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
    m_list2.SetExtendedStyle(m_list2.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

    m_list1.InsertColumn(0, "Наименование", LVCFMT_LEFT, 400);
    m_list2.InsertColumn(0, "Наименование", LVCFMT_LEFT, 400);


    CString q1 = "SELECT id, name FROM remontTruboprovodaSpisok ORDER BY ord";

    q1.Format("SELECT\n"

"act.id, act.name, d.id AS did FROM remontTruboprovodaSpisok act\n"
"LEFT JOIN defectTube d ON d.activityID=act.id AND d.objID=%d\n"
"ORDER BY act.ord\n", m_id);



    if (ado->openTable0(q1)) {
        int i = 0;
        int id_old = -1;
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            CString s = ado->readStr(1);
            int did = ado->read_long(2);

            if (id != id_old) {
                m_list1.InsertItem(i, s);
                m_list1.SetCheck(i, did);

                map_id[i] = id;

                i++;
            }
            id_old = i;

            ado->MoveNext();
        }
        ado->closeTable();
    }


    CString q2 = "SELECT id, name FROM spisokElementov ORDER BY ord";

    if (ado->openTable0(q2)) {
        int i = 0;
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            CString s = ado->readStr(1);
            m_list2.InsertItem(i, s);
            m_list2.SetCheck(i, 0);
            i++;

            ado->MoveNext();
        }
        ado->closeTable();
    }
	return TRUE; 
}


void CDefectElements::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if ((pNMListView->uChanged & LVIF_STATE) 
        && (pNMListView->uNewState & LVIS_SELECTED))
    {
        int i = pNMListView->iItem;

        auto it = map_id.find(i);
        if (it != map_id.end()) {
            int id = it->second;
            if (id == 7 || id == 8 || id == 9 || id == 10) {
                m_list2.EnableWindow(TRUE);
            }
            else {
                m_list2.EnableWindow(FALSE);
            }
        }
    }
}
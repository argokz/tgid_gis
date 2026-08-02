// CDefectElem.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CDefectElem.h"
#include "afxdialogex.h"
#include "ado.h"


void ButtonsMoveH(CWnd* wnd, int cx, int cy, int height, int off);


// CDefectElem dialog

IMPLEMENT_DYNAMIC(CDefectElem, CDialogEx)

CDefectElem::CDefectElem(int id, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEFECT_ELEM, pParent)
{
  m_profile = "CDefectElem";

  m_id = id;
}

CDefectElem::~CDefectElem()
{
}

void CDefectElem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_tree);
}


BEGIN_MESSAGE_MAP(CDefectElem, CDialogEx)
  ON_WM_SIZE()
  ON_WM_DESTROY()
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_TREE, OnCustomDraw)
END_MESSAGE_MAP()


// CDefectElem message handlers

void OnOff(CTreeCtrl &m_ctlTree, HTREEITEM hItem, int ch);


BOOL CDefectElem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return TRUE;

    m_imgList.Create( IDB_BITMAP_LIST, 16, 1, RGB( 0, 128, 128 ) );
    m_tree.SetImageList( &m_imgList, TVSIL_NORMAL );

    void setTreeFont(CTreeCtrl * m_ctrlTree, CFont * m_Font);
    setTreeFont(&m_tree, &m_Font);

    int id = 223;
    CString q;
    q.Format(

        "SELECT\n"
        "act.id,\n"
        "d.activityID,\n"
        "act.name,\n"
        "el1.id,\n"
        "el1.name,\n"
        "el.id,\n"
        "d.id\n"
        "FROM remontTruboprovodaSpisok act\n"
        "LEFT JOIN defectTube d ON d.activityID=act.id AND d.objID=%d\n"
        "LEFT JOIN spisokElementov el1 ON act.id IN (7,8,9,10)\n"
        "LEFT JOIN spisokElementov el ON el.id=el1.id AND el.id=d.elementID\n"
        "ORDER BY act.ord,el1.id,el.id DESC\n", m_id);

    if (ado->openTable0(q)) {
        int i = 0;
        int id_old = -1;
        int eid_old = -1;
        HTREEITEM h = TVI_ROOT, h2;
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            int id2 = ado->read_long(1);
            CString name = ado->readStr(2);
            int eid = ado->read_long(3);
            CString s2 = ado->readStr(4);
            int eid2 = ado->read_long(5);
            int did = ado->read_long(6);

            if (id != id_old) {
                h = m_tree.InsertItem(name, TVI_ROOT);
                m_tree.SetItemData(h, id);

                m_tree.SetCheck(h, id2);
                OnOff(m_tree, h, id2);
            }

            if (eid && (eid != eid_old || id != id_old)) {
                m_tree.Expand(h, TVE_EXPAND);
                h2 = m_tree.InsertItem(s2, h);
                m_tree.SetCheck(h2, eid2);
                OnOff(m_tree, h2, eid2);
                m_tree.SetItemData(h2, eid);
                eid_old = eid;
            }

            id_old = id;

            ado->MoveNext();
        }
        ado->closeTable();
    }


    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
      SetWindowPos(&wndTopMost,bx,by,cx,cy,SWP_NOZORDER);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void CDefectElem::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi); 

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right-wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom-wpi.rcNormalPosition.top);

    CDialogEx::OnDestroy();
}


void CDefectElem::OnSize(UINT nType, int cx, int cy) 
{
  CDialogEx::OnSize(nType, cx, cy);

    CRect sz;
    GetClientRect(sz);

double getScreenMas();

    double mas = getScreenMas();

    sz.DeflateRect(5*mas, 5*mas);

    int height = 20;
  
  ButtonsMoveH(this, cx, cy, height*mas, 5*mas);

  sz.bottom -= (height+4)*mas;

  if (::IsWindow(m_tree.m_hWnd) )
    m_tree.MoveWindow(sz); 
}

BOOL CDefectElem::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: Add your specialized code here and/or call the base class

    LPNMHDR lpnmh = (LPNMHDR) lParam;
    TVHITTESTINFO ht = {0};

//    if((lpnmh->code  == NM_CLICK) && (lpnmh->idFrom == IDC_TREE_ACCESS))
    if((lpnmh->code  == NM_CLICK))
    {
        DWORD dwpos = GetMessagePos();

        // include <windowsx.h> and <windows.h> header files
        ht.pt.x = GET_X_LPARAM(dwpos);
        ht.pt.y = GET_Y_LPARAM(dwpos);
        ::MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);

        TreeView_HitTest(lpnmh->hwndFrom, &ht);

            CString str;
            str.Format("%d", ht.flags);
//            AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

        if (2 & ht.flags || 4 & ht.flags)
//        if(TVHT_ONITEMSTATEICON & ht.flags)
        {
            TRACE("OnNotify Check\n");
//            SetCheckTree(ht.hItem, ht.flags, -1);

              CString str = m_tree.GetItemText(ht.hItem);

              BOOL ch = m_tree.GetCheck(ht.hItem);
              ch = !ch;
              m_tree.SetCheck(ht.hItem, ch);
              OnOff(m_tree, ht.hItem, ch);

              if (m_tree.ItemHasChildren(ht.hItem) && ! ch)
              {
                  HTREEITEM hti = m_tree.GetNextItem(ht.hItem, TVGN_CHILD);
                  while (hti)
                  {
                      CString str = m_tree.GetItemText(hti);

                      m_tree.SetCheck(hti, ch);
                      OnOff(m_tree, hti, ch);
                      hti = m_tree.GetNextItem(hti, TVGN_NEXT);
                  }
              }

              if (ch) {
                  HTREEITEM hti = m_tree.GetNextItem(ht.hItem, TVGN_PARENT);
                  m_tree.SetCheck(hti, 1);
                  OnOff(m_tree, hti, 1);
              }

            




        }
    }
    else if((lpnmh->code  == NM_RCLICK))
    {
    }

    return CDialogEx::OnNotify(wParam, lParam, pResult);
}


void CDefectElem::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NMTVCUSTOMDRAW *pcd = (NMTVCUSTOMDRAW   *)pNMHDR;
    switch ( pcd->nmcd.dwDrawStage )
    {
    case CDDS_PREPAINT: 
        *pResult = CDRF_NOTIFYITEMDRAW;     
        break;

    case CDDS_ITEMPREPAINT : 
        {
            HTREEITEM   hItem = (HTREEITEM)pcd->nmcd.dwItemSpec;

            BOOL ch = m_tree.GetCheck(hItem);

//            if ( this->IsSelected(hItem ))
            if (ch)
            {
                pcd->clrText = GetSysColor(COLOR_WINDOWTEXT);
                pcd->clrTextBk = GetSysColor(COLOR_WINDOW);
            }
            else {
                pcd->clrText = GetSysColor(COLOR_GRAYTEXT);
                pcd->clrTextBk = GetSysColor(COLOR_WINDOW);
            }

            *pResult = CDRF_DODEFAULT;// do not set *pResult = CDRF_SKIPDEFAULT
            break;
        }
    }
}

void CDefectElem::OnOK()
{
    UpdateData(TRUE);

    HTREEITEM it = m_tree.GetRootItem();

    int id = 0, eid = 0;

    CString q = "INSERT INTO defectTube (objID, activityID, elementID) VALUES ";
    CString q0 = "";

    while (it) {
        CString str = m_tree.GetItemText(it);

        CString s;

        BOOL ch = m_tree.GetCheck(it);

        if (ch) {
            id = m_tree.GetItemData(it);

            HTREEITEM child = m_tree.GetNextItem(it, TVGN_CHILD);

            bool is_elem = false;

            while (child) {
                CString str = m_tree.GetItemText(child);
                eid = m_tree.GetItemData(child);
                BOOL ch = m_tree.GetCheck(child);

                if (ch) {
                    CString qq;
                    qq.Format("(%d,%d,%d)", m_id, id, eid);
                    if (q0 != "") q0 += ",";
                    q0 += qq;
                    is_elem = true;
                }
                child = m_tree.GetNextItem(child, TVGN_NEXT);
            }
            if (!is_elem) {
                CString qq;
                qq.Format("(%d,%d,NULL)", m_id, id);
                if (q0 != "") q0 += ",";
                q0 += qq;
            }
        }
        it = m_tree.GetNextItem(it, TVGN_NEXT);
    }

    if (q0 != "") {
        q += q0;

        CAdoFile* ado = getAdo(getGidAdoName());
        if (!ado) {
            AfxMessageBox("Îøèáêà!!!");
        }
        else {
            CString q1;
            q1.Format("DELETE FROM defectTube WHERE objID=%d", m_id);
            ado->Execute(q1);
            ado->Execute(q);
        }

    }

    CDialogEx::OnOK();
}

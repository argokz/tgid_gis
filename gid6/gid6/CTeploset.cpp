// CTeploset.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CTeploset.h"
#include "afxdialogex.h"

#include "ps_alma.h"

#include <memory>

// CTeploset dialog

 void OnOff(CTreeCtrl & m_ctlTree, HTREEITEM hItem, int ch);


IMPLEMENT_DYNAMIC(CTeploset, CDialogEx)

CTeploset::CTeploset(CWnd* pParent, bool is_CTeploset, bool second)
	: CDialogEx(IDD_TEPLOSET, pParent)
{
  m_all = true;
  m_second = second;

  m_profile = "CTeploset";

  if (second) {
      m_profile = "CTeploset_second";
  }
  m_is_CTeploset = is_CTeploset;
}

CTeploset::~CTeploset()
{
}

void CTeploset::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_tree);
}


BEGIN_MESSAGE_MAP(CTeploset, CDialogEx)
  ON_WM_SIZE()
  ON_WM_DESTROY()
//  ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_TREE, OnCustomDraw)
END_MESSAGE_MAP()


// CTeploset message handlers

struct lessMSRS6
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    bool operator() (const pair<int, CString>& arg1, const pair<int, CString>& arg2) const;
};

CString num_MSRS(const char* s);


bool lessMSRS6::operator() (const pair<int, CString>& arg1, const pair<int, CString>& arg2) const
{
    if (arg1.first != arg2.first) return arg1.first < arg2.first;

    CString n1 = num_MSRS(arg1.second);
    CString n2 = num_MSRS(arg2.second);


    CString ss;

    ss.Format("%s\n%s,\n%s\n%s", n1, arg1.second, n2, arg2.second);

    if (n1 == n2 && arg1.second != arg2.second) {
        return arg1.second < arg2.second;
    }

    return n1 < n2;
}

BOOL CTeploset::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    m_imgList.Create( IDB_BITMAP_LIST, 16, 1, RGB( 0, 128, 128 ) );
    m_tree.SetImageList( &m_imgList, TVSIL_NORMAL );


    void setTreeFont(CTreeCtrl * m_ctrlTree, CFont * m_Font);
    setTreeFont(&m_tree, &m_Font);


    HTREEITEM i0 = 0;

    HTREEITEM m_nach = m_tree.InsertItem(_TR("Начальники"), i0);

    map<pair<int, CString>, UMSRS, lessMSRS6> mapMSRS;

    int id = 0;

    for ( auto &it :  ps_alma.map_UMS) {
        UMS rs = it.second;
        UMSRS ms;

        ms.id = rs.id;
        ms.id2 = id;
        ms.name = rs.name;
        ms.opis = rs.opis;
        //      ms.kod = rs.kod;
        //      ms.re = rs.re;
        ms.ue = rs.ue;
        ms.kod1 = rs.kod1;      ms.uzel1 = rs.uzel1;
        ms.kod2 = rs.kod2;      ms.uzel2 = rs.uzel2;

        //      ms.nach = rs.nach;
        ms.mag = RT_MS;

        mapMSRS[pair<int, CString> (1, ms.opis)] = ms;
        id++;
    }


    for (auto& it : ps_alma.map_URS) {
        URS rs = it.second;
        UMSRS ms;

        ms.id = rs.id;
        ms.id2 = id;
        //      ms.name = rs.name;
        ms.opis = rs.opis;
        //      ms.kod = rs.kod;
        //      ms.re = rs.re;
        ms.ue = rs.ue;

        ms.kod1 = rs.kod1;      ms.uzel1 = rs.uzel1;
        ms.kod2 = rs.kod2;      ms.uzel2 = rs.uzel2;

        //      ms.nach = rs.nach;
        ms.mag = RT_RS;

        mapMSRS[pair<int, CString>(2, ms.opis)] = ms;
        id++;
    }

    map<CString, NACH> map_nach;

    for (auto& it : ps_alma.map_NACH) {
        map_nach[it.second.name] = it.second;
    }

    for ( auto & it : map_nach) {
        CString nach = it.second.name;
        BOOL ch0 = TRUE;

        if (m_second && m_is_CTeploset) {
            ch0 = FALSE;

            for (auto &it1 : ps_alma.map_UE) {
                UE ue = it1.second;
                if (ue.nach == it.second.id) {
                    for (auto & it3 : mapMSRS) {
                        UMSRS rs = it3.second;
                        if (rs.ue == ue.id) {
                            CString s;
                            s.Format("CTeploset\\checked");
                            BOOL ch1 = AfxGetApp()->GetProfileInt(s, rs.opis, 1);
                            if (ch1) {
                                ch0 = TRUE;
                            }
                        }
                    }
                }
            }
        }

        if (ch0) {
            HTREEITEM i1 = m_tree.InsertItem(nach, m_nach);
            for (auto &it1 : ps_alma.map_UE) {
                UE ue = it1.second;
                if (ue.nach == it.second.id) {
                    for (auto & it3 : mapMSRS) {
                        UMSRS rs = it3.second;
                        if (rs.ue == ue.id) {
                            BOOL ch0 = TRUE; //  Отмечено в основном
                            if (m_second) {
                                CString s;
                                s.Format("CTeploset\\checked");
                                ch0 = AfxGetApp()->GetProfileInt(s, rs.opis, 1);
                            }
                            if (ch0) {
                                HTREEITEM i2 = m_tree.InsertItem(rs.opis, i1);
                                UMSRS* prs = new UMSRS(rs);
                                m_tree.SetItemData(i2, (DWORD_PTR)prs);
                            }
                        }
                    }
                }
            }
        }
    }

    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
      SetWindowPos(&wndTopMost,bx,by,cx,cy,SWP_NOZORDER);
    }

   
    HTREEITEM it = m_tree.GetRootItem();
    HTREEITEM it_root = it;

    while (it) {
        HTREEITEM it0 = it;
        CString str = m_tree.GetItemText(it);

        CString s;

        s.Format("%s\\checked", m_profile);
        BOOL ch = AfxGetApp()->GetProfileInt(s, str, 0);

        s.Format("%s\\expanded", m_profile);
        BOOL exp = AfxGetApp()->GetProfileInt(s, str, 0);

//        if (it != it_root) {
            m_tree.SetCheck(it, ch);
            OnOff(m_tree, it, ch);
//        }



        if (exp) {
            m_tree.Expand(it, TVE_EXPAND);
        }


        it = m_tree.GetNextItem(it0, TVGN_CHILD);
        if (!it) {
            it = m_tree.GetNextItem(it0, TVGN_NEXT);
        }
        if (!it) {
            do {
                it0 = m_tree.GetNextItem(it0, TVGN_PARENT);
                it = m_tree.GetNextItem(it0, TVGN_NEXT);
            } while (!it && it0);
        }
    }


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void ButtonsMoveH(CWnd* wnd, int cx, int cy, int height, int off);


void CTeploset::OnSize(UINT nType, int cx, int cy) 
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


void CTeploset::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi); 

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right-wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom-wpi.rcNormalPosition.top);


/*

    HTREEITEM it = m_tree.GetRootItem();

    while (it) {
        HTREEITEM it0 = it;
        CString str = m_tree.GetItemText(it);

        CString s;


        BOOL ch = m_tree.GetCheck(it);


        s.Format("%s\\checked", m_profile);
        AfxGetApp()->WriteProfileInt(s, str, ch);

        s.Format("%s\\expanded", m_profile);

        HTREEITEM child = m_tree.GetNextItem(it0, TVGN_CHILD);

        if (child) {
            BOOL exp = (m_tree.GetItemState(it, TVIS_EXPANDED) & TVIS_EXPANDED) != 0;
            AfxGetApp()->WriteProfileInt(s, str, exp);
        }


        it = m_tree.GetNextItem(it0, TVGN_CHILD);
        if (!it) {
            it = m_tree.GetNextItem(it0, TVGN_NEXT);
        }
        if (!it) {
            do {
                it0 = m_tree.GetNextItem(it0, TVGN_PARENT);
                it = m_tree.GetNextItem(it0, TVGN_NEXT);
            } while (!it && it0);
        }
    }
*/



    CDialogEx::OnDestroy();
}


BOOL CTeploset::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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

              if (m_tree.ItemHasChildren(ht.hItem))
              {
                  HTREEITEM hti = m_tree.GetNextItem(ht.hItem, TVGN_CHILD);
                  while (hti)
                  {
                      CString str = m_tree.GetItemText(hti);

                      m_tree.SetCheck(hti, ch);
                      OnOff(m_tree, hti, ch);

                      if (m_tree.ItemHasChildren(hti))
                      {
                          HTREEITEM hti2 = m_tree.GetNextItem(hti, TVGN_CHILD);
                          while (hti2)
                          {

                              CString str = m_tree.GetItemText(hti2);


                              m_tree.SetCheck(hti2, ch);
                              OnOff(m_tree, hti2, ch);

                              hti2 = m_tree.GetNextItem(hti2, TVGN_NEXT);
                          }
                      }
                      hti = m_tree.GetNextItem(hti, TVGN_NEXT);
                  }
              }



//            if (m_kl_list) m_kl_list->save2();

//            SetCheckTree(ht.hItem, -1);

//            PostMessage(MSG_CHECKSTATECHANGE,0,(LPARAM)ht.hItem);
        }
    }
    else if((lpnmh->code  == NM_RCLICK))
    {
    }

    return CDialogEx::OnNotify(wParam, lParam, pResult);
}


void CTeploset::OnOK() 
{
    m_all = true;

    UpdateData(TRUE);

    HTREEITEM it = m_tree.GetRootItem();

    while (it) {
        HTREEITEM it0 = it;
        CString str = m_tree.GetItemText(it);

        CString s;

        BOOL ch = m_tree.GetCheck(it);

        if (ch) {
            DWORD_PTR ptr = m_tree.GetItemData(it);

            if (ptr) {
                UMSRS* rs = (UMSRS*)ptr;

                if (rs->mag == RT_MS) {
                    m_set_ms.insert(rs->id);
                }
                else {
                    m_set_rs.insert(rs->id);
                }
            }
        }
        else {
            m_all = false;
        }


        s.Format("%s\\checked", m_profile);
        AfxGetApp()->WriteProfileInt(s, str, ch);

        s.Format("%s\\expanded", m_profile);

        HTREEITEM child = m_tree.GetNextItem(it0, TVGN_CHILD);

        if (child) {
            BOOL exp = (m_tree.GetItemState(it, TVIS_EXPANDED) & TVIS_EXPANDED) != 0;
            AfxGetApp()->WriteProfileInt(s, str, exp);
        }

        it = m_tree.GetNextItem(it0, TVGN_CHILD);
        if (!it) {
            it = m_tree.GetNextItem(it0, TVGN_NEXT);
        }
        if (!it) {
            do {
                it0 = m_tree.GetNextItem(it0, TVGN_PARENT);
                it = m_tree.GetNextItem(it0, TVGN_NEXT);
            } while (!it && it0);
        }
    }


    
    CDialogEx::OnOK();
}


void CTeploset::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
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
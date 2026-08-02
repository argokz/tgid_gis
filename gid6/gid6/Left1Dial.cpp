    // CLeft1Dialog.cpp : implementation file
//



#include "stdafx.h"


#include "gid6.h"
#include "Left1Dial.h"
#include <WINDOWSX.H>
#include "KlEdit.h"
#include "gid6Doc.h"
#include "gidrView.h"



#include "ado.h"
#include "win.h"



#define MSG_CHECKSTATECHANGE (WM_USER + 100)

extern CGeoFile *m_geofile;


BEGIN_MESSAGE_MAP(CLeft1Dialog, CPaneDialog)
  //{{AFX_MSG_MAP(CLeft1Dialog)
  ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
  ON_COMMAND(ID_GEO_ADD, OnAdd)
  ON_COMMAND(ID_GEO_ADD_TAB, OnAddTab)
  ON_COMMAND(ID_GEO_DEL_TAB, OnDelTab)
  ON_COMMAND(ID_GEO_TAB, OnTab)
  ON_COMMAND(ID_GEO_VIEW, OnView)
  ON_COMMAND(ID_GEO_KLS, OnKls)
  ON_COMMAND(ID_ADDRESS, OnAddress)
  ON_COMMAND(ID_TEXT, OnName)

//  ON_COMMAND(ID_DEL, OnDel)
  ON_WM_SIZE()
  ON_WM_CONTEXTMENU()
  ON_NOTIFY(TVN_SELCHANGED, IDC_EDIT, OnSelchangedEdit)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


static Klassif *g_kls = NULL;
static HTREEITEM m_hItem;

CView *getViewWnd()
{
  Cgid6App* theApp = (Cgid6App*)AfxGetApp();


  CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

  if (pFrame) {
    CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
    return pChild->GetActiveView();
  }
  return NULL;
}



HTREEITEM FindChild(CTreeCtrl *tree, HTREEITEM hItem, const char *txt)
{
  hItem = tree->GetChildItem(hItem);
  while (hItem)
  {
    CString str1 = tree->GetItemText(hItem);
    if (str1 == txt) {
      return hItem;
    }
    hItem = tree->GetNextItem(hItem, TVGN_NEXT);
  }

  return 0;
}


HTREEITEM find_ins(CTreeCtrl *tree, HTREEITEM i2, int kod, const char *fn)
{
  HTREEITEM i3; 

  i3 = FindChild(tree, i2, fn);
  if (!i3) {
    i3 = tree->InsertItem(fn, kod, kod, i2);
  }

  return i3;
}

bool is_in_file_list(const char* tn);



BOOL CLeft1Dialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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
            SetCheckTree(ht.hItem, ht.flags, -1);

            if (m_kl_list) m_kl_list->save2();

//            SetCheckTree(ht.hItem, -1);

//            PostMessage(MSG_CHECKSTATECHANGE,0,(LPARAM)ht.hItem);
        }
    }
    else if((lpnmh->code  == NM_RCLICK))
    {
        DWORD dwpos = GetMessagePos();

        // include <windowsx.h> and <windows.h> header files
        ht.pt.x = GET_X_LPARAM(dwpos);
        ht.pt.y = GET_Y_LPARAM(dwpos);
        ::MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);

        TreeView_HitTest(lpnmh->hwndFrom, &ht);

        g_kls = NULL;
        if (ht.hItem) {
          g_kls = (Klassif *) m_ctlTree.GetItemData(ht.hItem);
          m_hItem = ht.hItem;
        }

        POINT point = ht.pt;
        CMenu menu;
        VERIFY(menu.CreatePopupMenu());

//        CMenu menu2;
//        VERIFY(menu2.CreatePopupMenu());

        ClientToScreen(&point);

//        menu.AppendMenu(MF_STRING, ID_GEO_VIEW, _TR("Вид"));

        CGidrView *pView = getView();

        if (pView) {
          if (g_kls) {
            menu.AppendMenu(MF_STRING, ID_GEO_TAB, _TR("Таблица"));

            menu.AppendMenu(MF_STRING, ID_GEO_KLS, _TR("Классификатор"));

            menu.AppendMenu(MF_STRING, ID_ADDRESS, _TR("Использовать в адресном поиске"));
//            menu.AppendMenu(MF_STRING, ID_TEXT, _TR("Дать понятное имя"));
            
            if ( isEditGeo(g_kls->nazv) && !is_in_file_list(g_kls->nazv)) {
              menu.AppendMenu(MF_STRING, ID_GEO_ADD, _TR("Добавить объект"));
//              menu.AppendMenu(MF_STRING, ID_GEO_DEL_TAB, _TR("Удалить слой"));
            }

//            menu2.AppendMenu(MF_STRING, 1000, _TR("Создать файл полей"));
//            menu2.AppendMenu(MF_STRING, 1001, _TR("Конвертировать поля"));
//            menu2.AppendMenu(MF_STRING, 1002, _TR("Сохранить переименованные поля"));
//            menu.AppendMenu(MF_POPUP|MF_STRING, (UINT)menu2.m_hMenu,  _TR("Переименование полей") );
          
          
          
          }
          else {
            if (isEditGeo("")) {
//              menu.AppendMenu(MF_STRING, ID_GEO_ADD_TAB, _TR("Добавить слой"));
            }
//            menu.AppendMenu(MF_STRING, ID_GEO_DEL_TAB, _TR("Удалить слой"));
          }
        }
//        if (g_kls) {
//          menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить"));
//        }

        // route commands through this main frame

//        ClientToScreen(&point);

        menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
        return 0;

    }

    return CPaneDialog::OnNotify(wParam, lParam, pResult);
}





void InvalidView()
{
  CGidrView *pView = getView();
  if (pView) pView->Invalidate();
}

void OnOff(CTreeCtrl &m_ctlTree, HTREEITEM hItem, int ch)
{
  if (ch) {
    m_ctlTree.SetItemImage( hItem, 5, 5);
  }
  else {
    m_ctlTree.SetItemImage( hItem, 4, 4);
  }
}

bool IsCheckable(CTreeCtrl &m_ctlTree, HTREEITEM hItem)
{
  int nImage, nSelectedImage;
  m_ctlTree.GetItemImage( hItem, nImage, nSelectedImage );

  return nImage == 5 || nImage == 4;
}

bool IsCheckable3(CTreeCtrl &m_ctlTree, HTREEITEM hItem)
{
  int nImage, nSelectedImage;
  m_ctlTree.GetItemImage( hItem, nImage, nSelectedImage );

  return nImage == 5 || nImage == 4 || nImage == 6;
}


void addTableRusName(CString database, CString name_e, CString name_r);
void saveTableRusName(CString database);


void CLeft1Dialog::SetCheckTree(HTREEITEM hItem, int flags, int ch)
{
  if (hItem==0) return ;

  Klassif *kls = (Klassif *) m_ctlTree.GetItemData(hItem);

  if (flags == 2 && IsCheckable(m_ctlTree, hItem))  {

    if (ch == -1) {
      ch = m_ctlTree.GetCheck(hItem);
      ch = !ch;
    }

    m_ctlTree.SetCheck(hItem, ch);

    OnOff(m_ctlTree, hItem, ch);

    if (m_ctlTree.ItemHasChildren(hItem))
    {
        HTREEITEM hti = m_ctlTree.GetNextItem(hItem, TVGN_CHILD);
        while (hti)
        {
          SetCheckTree(hti, flags, ch);
          hti = m_ctlTree.GetNextItem(hti, TVGN_NEXT);
        }
    }
    else {
      m_ctlTree.SelectItem(hItem);

      if (kls) {
        kls->ins = ch;

//        if (m_kl_list) {
//          m_kl_list->save2();
//          if (m_geofile) m_geofile->reset_geo(kls);
//        }

        CGidrView *pView = getView();
        if (pView) {
          pView->GetDocument()->SetModifiedFlag(true);
        }
        InvalidView();
      }
      InvalidView();
    }
  }

  if (flags == 4) {
    if (kls) {
      CKlEdit ke(this, kls);

      CString oldrus = ke.m_rus;

      if (ke.DoModal() == IDOK) {
        if (m_kl_list) {
          m_kl_list->save2();

          if (oldrus != ke.m_rus) {
              addTableRusName(kls->m_ado->m_schema, kls->nazv, ke.m_rus);
              m_ctlTree.SetItemText(hItem, ke.m_rus);
          }
          saveTableRusName(kls->m_ado->m_schema);


//          if (m_geofile) m_geofile->reset_geo(kls);
        }
        CGidrView *pView = getView();
        if (pView) {
          pView->GetDocument()->SetModifiedFlag(true);
        }
        InvalidView();
      }
    }
  }
}


void TraverseTree(CTreeCtrl& treeCtrl, HTREEITEM hItem, KlList *m_kl_list)
{
    if (!m_kl_list) return;

    if (hItem == nullptr)
        return;

    // Обрабатываем текущий элемент
    CString itemText = treeCtrl.GetItemText(hItem);
//    AfxMessageBox(itemText); // Вы можете заменить это на нужную вам обработку
    

    Klassif* kls = (Klassif*)treeCtrl.GetItemData(hItem);

    if (kls) {
        OnOff(treeCtrl, hItem, kls->ins);
    }

    // Получаем первого дочернего элемента
    HTREEITEM hChildItem = treeCtrl.GetChildItem(hItem);
    while (hChildItem != nullptr)
    {
        // Рекурсивно обходим дочерние элементы
        TraverseTree(treeCtrl, hChildItem, m_kl_list);

        // Переходим к следующему дочернему элементу
        hChildItem = treeCtrl.GetNextSiblingItem(hChildItem);
    }
}

void TraverseAllTreeItems(CTreeCtrl& treeCtrl, KlList *m_kl_list)
{
    if (!m_kl_list) return;

    // Получаем корневой элемент
    HTREEITEM hRootItem = treeCtrl.GetRootItem();
    while (hRootItem != nullptr)
    {
        // Обходим дерево начиная с корневого элемента
        TraverseTree(treeCtrl, hRootItem, m_kl_list);

        // Переходим к следующему корневому элементу, если есть
        hRootItem = treeCtrl.GetNextSiblingItem(hRootItem);
    }
}

void CLeft1Dialog::setKlsOnOff()
{
    TraverseAllTreeItems(m_ctlTree, m_kl_list);
    Invalidate();
}




/////////////////////////////////////////////////////////////////////////////
// CLeft1Dialog dialog

CLeft1Dialog::CLeft1Dialog()
  : CPaneDialog()
{
  m_bInit = FALSE;

  m_kl_list = NULL;

  //{{AFX_DATA_INIT(CLeft1Dialog)
  //}}AFX_DATA_INIT
}



void CLeft1Dialog::DoDataExchange(CDataExchange* pDX)
{
  CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLeft1Dialog)
  DDX_Control(pDX, IDC_EDIT, m_ctlTree);
  //}}AFX_DATA_MAP
}



/////////////////////////////////////////////////////////////////////////////
// CLeft1Dialog message handlers

void CLeft1Dialog::OnSize(UINT nType, int cx, int cy)
{
  CPaneDialog::OnSize(nType, cx, cy);

  if(!m_bInit)
    return;

  CRect sz;
  GetClientRect(sz);
//  sz.DeflateRect(5,5);

  m_ctlTree.MoveWindow(sz);
}

LRESULT CLeft1Dialog::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);

  CRect sz;
  GetClientRect(sz);
  sz.DeflateRect(5,5);

  m_bInit = TRUE;

  m_imgList.Create( IDB_BITMAP_LIST, 16, 1, RGB( 0, 128, 128 ) );

  void setTreeFont(CTreeCtrl * m_ctrlTree, CFont * m_Font);
  setTreeFont(&m_ctlTree, &m_Font);

  m_ctlTree.SetImageList( &m_imgList, TVSIL_NORMAL );

  i1 = 0;

  return TRUE;
}

void CLeft1Dialog::OnOK()
{
  // TODO: Add extra validation here

//  CPaneDialog::OnOK();
}

void CLeft1Dialog::OnCancel()
{
  // TODO: Add extra validation here

//  CPaneDialog::OnCancel();
}

void Init(CTreeCtrl *ctlTree)
{
  if (::IsWindow(*ctlTree)) {
    ctlTree->DeleteAllItems( );
//    HTREEITEM i1 = ctlTree->InsertItem(_TR("Слои"), 0, 0);
  }
}

/*
void CLeft1Dialog::Init(const char *fn)
{
  HTREEITEM hItem = FindChild( &m_ctlTree, TVI_ROOT, fn);

  if (hItem) {
    m_ctlTree.DeleteItem( hItem );
  }
}
*/


HTREEITEM InsertItem(CTreeCtrl *ctlTree, LPCTSTR txt, int nImage, int nSelectedImage, HTREEITEM i) 
{
//  if (!i) i = i1;

  HTREEITEM i2 = ctlTree->InsertItem(txt, nImage, nSelectedImage, i);
  ctlTree->SelectItem(i2);
  return i2;
}


void CLeft1Dialog::InsertDocument(LPCTSTR txt, const CDocument *pDocument) {

  HTREEITEM i2 = m_ctlTree.InsertItem(txt, 1, 2, i1);
  m_ctlTree.SetItemData(i2, (DWORD) pDocument);
  m_ctlTree.SelectItem(i2);
}

HTREEITEM CLeft1Dialog::FindDocument(const CDocument *pDocument) {
  HTREEITEM hItem;
  DWORD data;

  hItem = m_ctlTree.GetFirstVisibleItem();

  while ( hItem != NULL ) {
    data = m_ctlTree.GetItemData( hItem );
    if (data == (DWORD) pDocument) {
      return hItem;
    }
    hItem = m_ctlTree.GetNextItem( hItem,  TVGN_NEXTVISIBLE);
  }
  return NULL;
}


void CLeft1Dialog::InsertView(LPCTSTR txt, const CView *pView) 
{
  HTREEITEM i1 = FindDocument(pView->GetDocument());

  HTREEITEM i2 = m_ctlTree.InsertItem(txt, 3, 3, i1);
  m_ctlTree.SetItemData(i2, (DWORD) (pView->m_hWnd));
  m_ctlTree.SelectItem(i2);
}

void CLeft1Dialog::RemoveView(const CView *pView) {
  HTREEITEM hItem;
  DWORD data;

  hItem = m_ctlTree.GetFirstVisibleItem();

  while ( hItem != NULL ) {
    data = m_ctlTree.GetItemData( hItem );
    if (data == (DWORD) (pView->m_hWnd)) {
      m_ctlTree.DeleteItem( hItem );
      break;
    }
    hItem = m_ctlTree.GetNextItem( hItem,  TVGN_NEXTVISIBLE);
  }
}

void CLeft1Dialog::FindView(const CView *pView) {
  HTREEITEM hItem;
  DWORD data;

  hItem = m_ctlTree.GetFirstVisibleItem();

  while ( hItem != NULL ) {
    data = m_ctlTree.GetItemData( hItem );
    if (data == (DWORD) (pView->m_hWnd)) {
      m_ctlTree.SelectItem( hItem );
      break;
    }
    hItem = m_ctlTree.GetNextItem( hItem,  TVGN_NEXTVISIBLE);
  }
}


void CLeft1Dialog::OnSelchangedEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  // TODO: Add your control notification handler code here

  HTREEITEM hItem = m_ctlTree.GetSelectedItem();

  DWORD data = m_ctlTree.GetItemData( hItem );

  if (data) {
    HWND hWnd = (HWND) data;

    if (::IsWindow(hWnd)) {
      hWnd = ::GetParent(hWnd);
      //      AfxGetMainWnd()->SendMessage(WM_MDIACTIVATE, (WPARAM) hWnd, 0);
      ::SendMessage(static_cast<CMDIFrameWnd*>(AfxGetMainWnd())->m_hWndMDIClient,
        WM_MDIACTIVATE, (WPARAM)hWnd, 0);
    }
  }

  *pResult = 0;
}


void CLeft1Dialog::OnAdd()
{
  CGidrView *pView = getView();
  if (pView && g_kls && g_kls->ins) {

    if (pView->addGeo(g_kls)) {
       CString str;
       str.Format(_TR("Начинайте создавать %s"), g_kls->RusName());
       AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
       pView->GetDocument()->SetModifiedFlag(true);
    }
  }
}

CString CLeft1Dialog::getMdbName()
{
  CString str = "";
  if (m_ctlTree.ItemHasChildren(m_hItem))
  {
    HTREEITEM hti = m_ctlTree.GetNextItem(m_hItem, TVGN_CHILD);
    if (hti)
    {
      str = m_ctlTree.GetItemText(hti);
      Klassif *kls = (Klassif *) m_ctlTree.GetItemData(hti);
      if (kls) {
        str = kls->FileName();
      }
      else {
        if (m_ctlTree.ItemHasChildren(hti))
        {
          hti = m_ctlTree.GetNextItem(hti, TVGN_CHILD);
          if (hti)
          {
            str = m_ctlTree.GetItemText(hti);
            kls = (Klassif *) m_ctlTree.GetItemData(hti);
            if (kls) {
              str = kls->FileName();
            }
          }
        }
      }
    }
  }
  return str;
}


void CLeft1Dialog::OnDelTab()
{
  CGidrView *pView = getView();
  if (pView) {
    pView->delGeoTab(g_kls);
  }
}

void CLeft1Dialog::OnAddTab()
{
  CGidrView *pView = getView();
  if (pView) {
    CString str = getMdbName();
    if (str != "") {
      pView->addGeoTab(str);
    }
  }
}


void CLeft1Dialog::OnTab()
{
  CGidrView *pView = getView();
  if (pView) {
    pView->RunGeoTable(g_kls);
  }
}

void CLeft1Dialog::OnKls()
{
  if (g_kls) {
    CKlEdit ke(this, g_kls);
    CString oldrus = ke.m_rus;
    
    if (ke.DoModal() == IDOK) {
      if (m_kl_list) {
        m_kl_list->save2();

        if (oldrus != ke.m_rus) {
            addTableRusName(g_kls->m_ado->m_schema, g_kls->nazv, ke.m_rus);
            m_ctlTree.SetItemText(m_hItem, ke.m_rus);
        }
        saveTableRusName(g_kls->m_ado->m_schema);
//        if (m_geofile) m_geofile->reset_geo(g_kls);
      }
      CGidrView *pView = getView();
      if (pView) {
        pView->GetDocument()->SetModifiedFlag(true);
      }
      InvalidView();
    }
  }
}

#include "MainFrm.h"
#include "CAddrInit.h"
#include "AddressLeft.h"

const CString szSection1 = "AddressInit";


void read_address_ini(CString tn, CString &raj, CString & str, CString & dom)
{
    CString sz;
    sz.Format("%s\\%s", szSection1, tn);

    raj = "";
    str = "";
    dom = "";

    if (tn == "building3") {  // OpenStreetMap
        raj = "";
        str = "addr_stree";
        dom = "addr_house";
    }
    if (tn == "zdaniya") {    // Астана
        raj = "NAME_MASSI";
        str = "NAME_STREE";
        dom = "NUMBER_1";
    }

    if (tn == "zdaniya_karta") {      // Алматы
        raj = "id_adr_mas";
        str = "street_nam";
        dom = "number_1";
    }

    if (getGidAdoName().Find("AlmatyGID") == 0) {
        if (tn == "zdaniya_tu") {      // Алматы ТУ
            raj = "mikrorayon";
            str = "ulitsa";
            dom = "dom";
        }
        if (tn == "zdaniya_potrebiteley") {      // Алматы ТУ нагрузки
            raj = "mikrorayon";
            str = "ulitsa";
            dom = "dom";
        }
    }
    else {
        if (tn == "zdaniya_tu") {      // Алматы ТУ
            return;
        }
        if (tn == "zdaniya_potrebiteley") {      // Алматы ТУ нагрузки
            return;
        }
    }

    if (tn == "zdaniya_2") {      // Алматы ТУ нагрузки
        raj = "id_adr_mas";
        str = "street_nam";
        dom = "number_1";
    }
    
/*
    if (tn == "") {      // Алматы ТУ нагрузки
        raj = "mikrorayon";
        str = "street_nam";
        dom = "number_1";
    }
*/

    raj = AfxGetApp()->GetProfileString(sz, "raj", raj);
    str = AfxGetApp()->GetProfileString(sz, "str", str);
    dom = AfxGetApp()->GetProfileString(sz, "dom", dom);
}



void CLeft1Dialog::OnAddress()
{
    if (g_kls) {
            CString tn = g_kls->nazv;
            CString sz, raj = "", str = "", dom = "";

            read_address_ini(tn, raj, str, dom);

            CAddrInit dlg(this, g_kls, raj, str, dom);

            if (dlg.DoModal() == IDOK) {
//                AfxMessageBox(dlg.m_s1+dlg.m_s2+dlg.m_s3, MB_OK|MB_ICONINFORMATION);
                CAddressLeft *a = (CAddressLeft *)getDockablePane(IDD_ADDRESS3);
                if (a) {
//                    AfxGetApp()->WriteProfileString(szSection1, "cxema", "");
                    AfxGetApp()->WriteProfileString(szSection1, "tn",  g_kls->nazv);
/*
                    AfxGetApp()->WriteProfileString(szSection1, "raj", dlg.m_s1);
                    AfxGetApp()->WriteProfileString(szSection1, "str", dlg.m_s2);
                    AfxGetApp()->WriteProfileString(szSection1, "dom", dlg.m_s3);
*/

//                    AfxGetApp()->WriteProfileString(szSection1, "cxema", "");
                    sz.Format("%s\\%s", szSection1, g_kls->nazv);

                    AfxGetApp()->WriteProfileString(sz, "raj", dlg.m_s1);
                    AfxGetApp()->WriteProfileString(sz, "str", dlg.m_s2);
                    AfxGetApp()->WriteProfileString(sz, "dom", dlg.m_s3);


                    a->Init("", g_kls->nazv, dlg.m_s1, dlg.m_s2, dlg.m_s3);
                }
            }
    }
}

CString findTableRusName(CString d, CString n);
void addTableRusName(CString database, CString name_e, CString name_r);
void saveTableRusName(CString database);

#include "InputDia.h"
#include "ado.h"

void CLeft1Dialog::OnName()
{
    if (g_kls) {
        CString s;
        CString rus = findTableRusName(g_kls->m_ado->m_schema, g_kls->nazv);

        s.Format("Введите понятное имя для %s", g_kls->nazv);

        CInputDialog inD(AfxGetMainWnd(), "", s, rus);
        if (inD.DoModal() == IDOK) {
            addTableRusName(g_kls->m_ado->m_schema, g_kls->nazv, inD.m_strInput);
            saveTableRusName(g_kls->m_ado->m_schema);
        }
    }
}



void CLeft1Dialog::OnView()
{
//  SendNotifyMessage(TVN_SELCHANGED, WPARAM wParam, LPARAM lParam);

/*
  CGidrView *pView = getView();
  if (pView) {
    pView->addGeo(g_kls);
    pView->GetDocument()->SetModifiedFlag(true);
  }
*/
}


void CLeft1Dialog::Del(Klassif *g_kls, HTREEITEM &m_hItem)
{
  KlList::iterator it;
  int nopen = 0;

  CGidrView *pView = getView();
  Cgid6Doc *pDoc = (Cgid6Doc *) pView->GetDocument();

}


void CLeft1Dialog::OnDel()
{
  CGidrView *pView = getView();
  Cgid6Doc *pDoc = (Cgid6Doc *) pView->GetDocument();

  pView->GetDocument()->SetModifiedFlag(true);
  if (g_kls) {
    Del(g_kls, m_hItem);
  }
  else {
    if (m_ctlTree.ItemHasChildren(m_hItem))
    {
      HTREEITEM hti = m_ctlTree.GetNextItem(m_hItem, TVGN_CHILD);
      while (hti)
      {
        if (IsCheckable(m_ctlTree, hti)) {
//          Klassif *kls = (Klassif *) m_ctlTree.GetItemData(hti);
//          pView->FileDel(kls);
          break;
        }
        hti = m_ctlTree.GetNextItem(hti, TVGN_NEXT);
      }
    }
  }
}


void CLeft1Dialog::SetKls(KlList *kl_list)
{
  m_kl_list = kl_list;
}

void CLeft1Dialog::OnContextMenu(CWnd* pWnd, CPoint point)
{
}


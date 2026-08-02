// virtlistView.cpp : implementation of the CVirtListView class
//

#include "stdafx.h"
#include "gid6.h"

//#include "virtlistDoc.h"
#include "virtlistView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirtListView

IMPLEMENT_DYNCREATE(CVirtListView, CListView)

BEGIN_MESSAGE_MAP(CVirtListView, CListView)
  //{{AFX_MSG_MAP(CVirtListView)
  ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
  ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnOdcachehint)
  //}}AFX_MSG_MAP
  // Standard printing commands
  ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirtListView construction/destruction

//CVirtListView::m_virtListDataInit = 0;
static CVirtListData *m_virtListDataInit = 0;


CVirtListView::CVirtListView()
{
  // TODO: add construction code here

  m_virtListData = NULL;
}

CVirtListView::~CVirtListView()
{
  delete m_virtListData;
  m_virtListDataInit = NULL;
}

BOOL CVirtListView::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.style |= (LVS_OWNERDATA | LVS_REPORT );
  return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVirtListView drawing

void CVirtListView::OnDraw(CDC* pDC)
{
//  CVirtListDoc* pDoc = GetDocument();
//  ASSERT_VALID(pDoc);
  // TODO: add draw code for native data here
}

void CVirtListView::InitListCtrl(int id)
{
  m_virtListData = m_virtListDataInit;
  
  // Создаем колонки
  CRect rect;
  GetListCtrl().GetClientRect(&rect);
  int n1 = rect.Width()/3;
  int n2 = rect.Width()%3;

  CVirtListData* pData = getVirtListData();
  if (!pData) return;
  
  int nf = pData->GetNFlds();


  for (int i = 0; i < nf; i++) {
    GetListCtrl().InsertColumn( i, pData->GetFieldName(i),   LVCFMT_LEFT, pData->GetFieldWidth(i));
  }

  // Создаем картинки
//  VERIFY(m_ImageSmall.Create(IDB_LIST_SMALL, 16, 4, RGB(255, 0, 255)));
//  GetListCtrl().SetImageList(&m_ImageSmall, LVSIL_SMALL);
//  VERIFY(m_ImageState.Create(IDB_LIST_STATE, 16, 4, RGB(255, 0, 255)));
//  GetListCtrl().SetImageList(&m_ImageState, LVSIL_STATE);

  // Разрешаем использовать иконки состояния
  SendMessage( LVM_SETCALLBACKMASK , LVIS_STATEIMAGEMASK , 0);

  // Выделение во всю строку, рисование сетки

  //unsigned long LVS_EX_DOUBLEBUFFER = 0x00010000;

  GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

  // Число элементов 100000 !!!!
  UpdateCount();
}

void CVirtListView::OnInitialUpdate()
{
  CListView::OnInitialUpdate();

  // Инициализация списка
  InitListCtrl(-1);

  PostMessage( WM_COMMAND, ID_LIST_SETCOUNT);
}

/////////////////////////////////////////////////////////////////////////////
// CVirtListView printing

BOOL CVirtListView::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CVirtListView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add extra initialization before printing
}

void CVirtListView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CVirtListView diagnostics

#ifdef _DEBUG
void CVirtListView::AssertValid() const
{
  CListView::AssertValid();
}

void CVirtListView::Dump(CDumpContext& dc) const
{
  CListView::Dump(dc);
}

//CVirtListDoc* CVirtListView::GetDocument() // non-debug version is inline
//{
//  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVirtListDoc)));
//  return (CVirtListDoc*)m_pDocument;
//}
#endif //_DEBUG



void CVirtListView::setVirtListData(CVirtListData *data) 
{
  m_virtListDataInit = data;  
};


CVirtListData* CVirtListView::getVirtListData() 
{
  return m_virtListData;
}


/////////////////////////////////////////////////////////////////////////////
// CVirtListView message handlers

void CVirtListView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
  LV_ITEM* pItem= &(pDispInfo)->item;
  
  CVirtListData* pData = getVirtListData();

  if (!pData) return;

  DWORD n = pItem->iItem; 

  int id_first = GetListCtrl().GetTopIndex();
  int id_last = id_first + GetListCtrl().GetCountPerPage()+10;

  if (pItem->mask & LVIF_TEXT) // требуется текст элемента?
    strcpy( pItem->pszText, pData->GetItemText( pItem->iSubItem, n, id_first, id_last));

  if (pItem->mask & LVIF_IMAGE) // требуются картинки?
  {
    pItem->iImage = pData->GetImage(n);
    pItem->state = pData->GetStateImage(n);
  }

  *pResult = 0;
}

void CVirtListView::OnOdcachehint(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLVCACHEHINT* pCacheHint = (NMLVCACHEHINT*)pNMHDR;
  TRACE("From: %d, To: %d\n", pCacheHint->iFrom, pCacheHint->iTo);
  *pResult = 0;
}

void CVirtListView::UpdateCount()
{ 
  CVirtListData* pData = getVirtListData();
  if (!pData) return;

  GetListCtrl().SetItemCount( pData->GetCount() );
}

void CVirtListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  UpdateCount();
}


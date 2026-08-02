// OpcTree.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "UnvisualTree.h"
#include "ado.h"
#include "cxema.h"
#include "graph2.h"
#include "gidrView.h"

#include "wm_user.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void viewTG(CWnd *wnd, int id_ist);
CString findTableRusNameGeo(CString n);


/////////////////////////////////////////////////////////////////////////////
// CUnvisualTree dialog

bool ado_execute(CAdoFile *ado, const char *q, long * affected);
long ado_executeInsert(CAdoFile *ado, const char *q, long * affected);

#include "ado2.h"


#define ID_TG 1004
#define ID_TP 1005

CUnvisualTree::CUnvisualTree(CWnd* pParent, CGraph2 *graph, CAdoFile *ado, int fileID)
  : CDialog(CUnvisualTree::IDD, pParent)
{
  //{{AFX_DATA_INIT(CUnvisualTree)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_fileID = fileID;
  m_graph = graph;
  m_ado = ado;
  m_isTG = false;
}


void CUnvisualTree::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CUnvisualTree)
  DDX_Control(pDX, IDC_TREE, m_tree);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUnvisualTree, CDialog)
  //{{AFX_MSG_MAP(CUnvisualTree)
  ON_COMMAND(ID_ADD, OnPopupAdd)
  ON_COMMAND(ID_DEL, OnPopupDel)
  ON_COMMAND(ID_EDIT, OnPopupEdit)
  ON_COMMAND(ID_TG, OnTG)
  ON_COMMAND(ID_TP, OnTP)
  ON_COMMAND(ID_MOVE_TO, OnMoveTo)

  ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnvisualTree message handlers

/*
void initRS(CTreeCtrl *tree, HTREEITEM h0, CGraph2 *graph, int fileID)
{
  map<long, CString>::const_iterator it = graph->map_kod.begin();

  for (; it != graph->map_kod.end(); it++) {
      HTREEITEM h = tree->InsertItem(it->second, h0);
      tree->SetItemData(h, it->first);
  }
}
*/



void initAdo(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, const char *tn, const char *fn, int fileID, const char *fn2 = "fileID")
{
  CString q;

  CString fn3;

  q.Format("SELECT id, %s FROM %s WHERE %s=%d ORDER BY %s", fn, tn, fn2, fileID, fn3);

  q.Format(

"  SELECT"
"      id,"
"      name,"
"      al,"
"      CONVERT(INT, IIF(PATINDEX('%%[^0-9]%%', num1) > 1, LEFT(num1, PATINDEX('%%[^0-9]%%', num1) - 1), num1)) AS num"

"      FROM("
"          SELECT"
"          id,"
"          %s AS name,"

"          IIF(PATINDEX('%%[0-9]%%', %s) > 1, SUBSTRING(%s, PATINDEX('%%[0-9]%%', %s), LEN(%s)), '0') AS num1,"
//"          IIF(PATINDEX('%%[0-9]%%', %s) > 1, LEFT(%s, PATINDEX('%%[0-9]%%', %s) - 1), '') AS al"
"          IIF(PATINDEX('%%[0-9]%%', %s) > 1, LEFT(%s, PATINDEX('%%[0-9]%%', %s) - 1), %s) AS al"

"          FROM %s"

"          WHERE %s = %d"
"      ) _AA"


"      ORDER BY"
"      UPPER(al),"
"      num,"
"      name", 

    fn,fn,fn,fn,fn,fn,fn,fn,fn,tn, fn2, fileID);




  if (ado->openTable0(q)) {
      while (!ado->isEOF()) {
          long id = ado->read_long(0);
          CString s = ado->readStr(1);
          HTREEITEM h = tree->InsertItem(s, h0);
          tree->SetItemData(h, id);
          ado->MoveNext();
      }
      ado->closeTable();
  }
}

#include "geodez.h"


void initCT(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, int fileID)
{
  CString q;

  q.Format("SELECT * FROM heatSystem");

  if (ado->openTable0(q)) {
      if (!ado->isEOF()) {
          long id = ado->read_long("id");
          CString s = ado->readStr("name");
          HTREEITEM h = tree->InsertItem(s, h0);
          tree->SetItemData(h, id);


          double Central_Meridian = ado->read_double("Central_Meridian");
          double Latitude_Of_Origin  = ado->read_double("Latitude_Of_Origin");
          double False_Easting       = ado->read_double("False_Easting");
          double False_Northing      = ado->read_double("False_Northing");
          double Scale_Factor        = ado->read_double("Scale_Factor");
          double Angle               = ado->read_double("Angle");

          double dx               = ado->read_double("dx");
          double dy               = ado->read_double("dy");


          if (False_Easting == 0) False_Easting = Central_Meridian * 10000;

          set_system_coord(False_Easting, False_Northing, Central_Meridian, Latitude_Of_Origin, Scale_Factor, Angle, dx, dy);

          ado->MoveNext();
      }
      ado->closeTable();
  }
}


void initFR(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, int fileID)
{
    initAdo(tree, h0, ado, "fragments", "name", fileID, "id");
}



void initRS(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, int fileID)
{
    initAdo(tree, h0, ado, "externalCodes", "name", fileID);
}


void initUR(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, int fileID)
{
  initAdo(tree, h0, ado, "specExpends", "specExpendID", fileID);
}

void initTR(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, int fileID)
{
    initAdo(tree, h0, ado, "calcTemperatures", "calcTemperatureID", fileID);
}

void initKV(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, int fileID)
{
    initAdo(tree, h0, ado, "varCoefficients", "kodkv", fileID);
}

void initGV(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, int fileID)
{
    initAdo(tree, h0, ado, "gvsLoadGraphs", "gvsLoadGraphID", fileID);
}


void initIST(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, int tec, CGraph2 *graph)
{
    CString q, where;

//    q.Format("SELECT heatSources.id, heatSources.sourceName FROM heatSources JOIN nodes ON heatSources.nodeID=nodes.id WHERE fileID=%d", fileID);

    if (tec == 1 || tec == 2 || tec == 3) {
      where.Format("hSourceTypeID=%d", tec);
    }
    else {
      where.Format("NOT hSourceTypeID IN (1,2,3)");
    }

    q.Format("SELECT heatSources.id, heatSources.sourceName, heatSources.name, nodes.externalNodeName FROM heatSources JOIN nodes ON heatSources.nodeID=nodes.id WHERE %s ORDER BY hSourceTypeID, heatSources.name, heatSources.sourceName, nodes.externalNodeName", where);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString s1 = ado->readStr("sourceName");
            CString s2 = ado->readStr("name");
            CString s3 = ado->readStr("externalNodeName");

            CString s = s2;
            if (s == "") s = s1;
            if (s == "") s = s3;

            if (graph && graph->findNode2(TIP_IS, id)) {
              HTREEITEM h = tree->InsertItem(s, h0);
              tree->SetItemData(h, id);
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }
}


BOOL CUnvisualTree::OnInitDialog() 
{
   CDialog::OnInitDialog();

//    HTREEITEM h;

    void setTreeFont(CTreeCtrl * m_ctrlTree, CFont * m_Font);
    setTreeFont(&m_tree, &m_Font);


    m_hCT = m_tree.InsertItem(_TR("Система теплоснабжения"), TVI_ROOT);

    initCT(&m_tree, m_hCT, m_ado, m_fileID);


    m_hFR = m_tree.InsertItem(_TR("Фрагмент тепловой сети"), TVI_ROOT);

    initFR(&m_tree, m_hFR, m_ado, m_fileID);
    
    
    m_hRS = m_tree.InsertItem(_TR("Расчетные схемы"), TVI_ROOT);
//    initRS(&m_tree, m_hRS, m_graph);
    initRS(&m_tree, m_hRS, m_ado, m_fileID);

//    m_hZN = m_tree.InsertItem(_TR("Узел с заданным напором"), TVI_ROOT);
//    m_h4 = m_tree.InsertItem(_TR("Узел подпитки"), TVI_ROOT);
    m_hUR = m_tree.InsertItem(_TR("Удельные расходы"), TVI_ROOT);
    initUR(&m_tree, m_hUR, m_ado, m_fileID);
    
    m_hTR = m_tree.InsertItem(_TR("Расчетные температуры"), TVI_ROOT);
    initTR(&m_tree, m_hTR, m_ado, m_fileID);

    m_hKV = m_tree.InsertItem(_TR("Коэффициенты вариации"), TVI_ROOT);
    initKV(&m_tree, m_hKV, m_ado, m_fileID);
    
//    m_hGV = m_tree.InsertItem(_TR("График нагрузки ГВС"), TVI_ROOT);
//    initGV(&m_tree, m_hGV, m_ado, m_fileID);

    m_hIST = m_tree.InsertItem(_TR("ТЭЦ"), TVI_ROOT);
    initIST(&m_tree, m_hIST, m_ado, 1, m_graph);

    m_hIST2 = m_tree.InsertItem(_TR("Районные котельные"), TVI_ROOT);
    initIST(&m_tree, m_hIST2, m_ado, 2, m_graph);

    m_hIST3 = m_tree.InsertItem(_TR("Местные котельные"), TVI_ROOT);
    initIST(&m_tree, m_hIST3, m_ado, 3, m_graph);

    return TRUE;
}

void CUnvisualTree::OnOK() 
{
  // TODO: Add extra validation here

  CString s;

  HTREEITEM h = m_tree.GetSelectedItem();

/*
  str = "";

  while (h != NULL) {
    s = m_tree.GetItemText(h);
    if (str == "") str = s;
    else str = s + "." + str;
    h = m_tree.GetParentItem(h);
  }
*/ 
  CDialog::OnOK();
}



BOOL CUnvisualTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
  LPNMHDR lpnmh = (LPNMHDR) lParam;
  TVHITTESTINFO ht = {0};
  HTREEITEM hItem1 = 0;
  HTREEITEM hItem2 = 0;
  HTREEITEM hItem3 = 0;
  HTREEITEM hItem4 = 0;

  if ((lpnmh->code == NM_CLICK) || (lpnmh->code == NM_RCLICK)) {
      DWORD dwpos = GetMessagePos();

      // include <windowsx.h> and <windows.h> header files
      ht.pt.x = GET_X_LPARAM(dwpos);
      ht.pt.y = GET_Y_LPARAM(dwpos);
      ::MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);
      TreeView_HitTest(lpnmh->hwndFrom, &ht);

      hItem1 = m_tree.GetParentItem(ht.hItem);
      hItem2 = m_tree.GetParentItem(hItem1);
      hItem3 = m_tree.GetParentItem(hItem2);
      hItem4 = m_tree.GetParentItem(hItem3);
  }

  if ((lpnmh->code == NM_CLICK)) {
    CString tn = m_tree.GetItemText(ht.hItem);

    POINT point = ht.pt;
    ClientToScreen(&point);
    m_id = (int)m_tree.GetItemData(ht.hItem);
    m_h = 0;

      if (hItem1 == m_hCT) {
          m_tn = "heatSystem";
          m_typ = TIP_heatSystem;
          m_tn_name = _TR("Система теплоснабжения");
          m_h = m_hCT;
//          OnPopupEdit();
          PostMessage(WM_COMMAND, ID_EDIT);
      }

      if (ht.hItem == m_hFR || hItem1 == m_hFR) {
          m_tn = "fragments";
          m_typ = TIP_fragments;
          m_tn_name = _TR("Фрагмент тепловой сети");
          m_h = m_hFR;

          if (hItem1 == m_hFR) {
//              OnPopupEdit();
              PostMessage(WM_COMMAND, ID_EDIT);
          }
      }


      if (ht.hItem == m_hRS || hItem1 == m_hRS) {
          m_tn = "externalCodes";
          m_typ = TIP_externalCodes;
          m_tn_name = _TR("Расчетная схема");
          m_h = m_hRS;

          if (hItem1 == m_hRS) {
//              OnPopupEdit();
              PostMessage(WM_COMMAND, ID_EDIT);
          }
      }

      if (ht.hItem == m_hUR || hItem1 == m_hUR) {
          m_tn = "specExpends";
          m_typ = TIP_specExpends;
          m_tn_name = _TR("Удельные расходы");
          m_h = m_hUR;
          if (hItem1 == m_hUR) {
//              OnPopupEdit();
              PostMessage(WM_COMMAND, ID_EDIT);
          }
      }


      if (ht.hItem == m_hTR || hItem1 == m_hTR) {
          m_tn = "calcTemperatures";
          m_typ = TIP_calcTemperatures;
          m_tn_name = _TR("Расчетные температуры");
          m_h = m_hTR;
          if (hItem1 == m_hTR) {
//              OnPopupEdit();
              PostMessage(WM_COMMAND, ID_EDIT);
          }
      }


      if (ht.hItem == m_hKV || hItem1 == m_hKV) {
          m_tn = "varCoefficients";
          m_typ = TIP_varCoefficients;
          m_tn_name = _TR("Коэффициенты вариации");
          m_h = m_hKV;
          if (hItem1 == m_hKV) {
//              OnPopupEdit();
              PostMessage(WM_COMMAND, ID_EDIT);
          }
      }

/*
      if (ht.hItem == m_hGV || hItem1 == m_hGV) {
          m_tn = "gvsLoadGraphs";
          m_typ = TIP_gvsLoadGraphs;
          m_tn_name = _TR("График нагрузки ГВС");
          m_h = m_hGV;
          if (hItem1 == m_hGV) {
//              OnPopupEdit();
              PostMessage(WM_COMMAND, ID_EDIT);
          }
      }
*/

      if (hItem1 == m_hIST || hItem1 == m_hIST2 || hItem1 == m_hIST3) {
          m_tn = "heatSources";
          m_typ = TIP_heatSources;
          m_tn_name = _TR("Источник тепла");
          m_h = hItem1;
//          OnPopupEdit();
          PostMessage(WM_COMMAND, ID_EDIT);
      }
  }
  else if((lpnmh->code  == NM_RCLICK)) {

      CMenu menu;
      VERIFY(menu.CreatePopupMenu());

      POINT point = ht.pt;

      ClientToScreen(&point);


      m_id = (int)m_tree.GetItemData(ht.hItem);

      m_h = 0;


      if (ht.hItem == m_hCT || hItem1 == m_hCT) {
          m_tn = "heatSystem";
          m_typ = TIP_heatSystem;
          m_tn_name = _TR("Система теплоснабжения");
          m_h = m_hRS;

          if (hItem1 == m_hCT) {
              menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать Систему теплоснабжения"));
          }
      }

      if (ht.hItem == m_hFR || hItem1 == m_hFR) {
          m_tn = "fragments";
          m_typ = TIP_fragments;
          m_tn_name = _TR("Фрагмент тепловой сети");
          m_h = m_hFR;

          if (hItem1 == m_hFR) {
              menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать Фрагмент тепловой сети"));
          }
      }


      if (ht.hItem == m_hRS || hItem1 == m_hRS) {
          menu.AppendMenu(MF_STRING, ID_ADD, _TR("Добавить Расчетную схему"));
          m_tn = "externalCodes";
          m_typ = TIP_externalCodes;
          m_tn_name = _TR("Расчетная схема");
          m_h = m_hRS;

          if (hItem1 == m_hRS) {
              menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать Расчетную схему"));
              menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить Расчетную схему"));
          }
      }

      if (ht.hItem == m_hUR || hItem1 == m_hUR) {
          m_tn = "specExpends";
          m_typ = TIP_specExpends;
          m_tn_name = _TR("Удельные расходы");
          m_h = m_hUR;
          menu.AppendMenu(MF_STRING, ID_ADD, _TR("Добавить Удельные расходы"));
          if (hItem1 == m_hUR) {
              menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать Удельные расходы"));
              menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить Удельные расходы"));
          }
      }


      if (ht.hItem == m_hTR || hItem1 == m_hTR) {
          m_tn = "calcTemperatures";
          m_typ = TIP_calcTemperatures;
          m_tn_name = _TR("Расчетные температуры");
          m_h = m_hTR;
          menu.AppendMenu(MF_STRING, ID_ADD, _TR("Добавить Расчетные температуры"));
          if (hItem1 == m_hTR) {
              menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать Расчетные температуры"));
              menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить Расчетные температуры"));
          }
      }


      if (ht.hItem == m_hKV || hItem1 == m_hKV) {
          m_tn = "varCoefficients";
          m_typ = TIP_varCoefficients;
          m_tn_name = _TR("Коэффициенты вариации");
          m_h = m_hKV;
          menu.AppendMenu(MF_STRING, ID_ADD, _TR("Добавить Коэффициенты вариации"));
          if (hItem1 == m_hKV) {
              menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать Коэффициенты вариации"));
              menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить Коэффициенты вариации"));
          }
      }

/*
      if (ht.hItem == m_hGV || hItem1 == m_hGV) {
          m_tn = "gvsLoadGraphs";
          m_typ = TIP_gvsLoadGraphs;
          m_tn_name = _TR("График нагрузки ГВС");
          m_h = m_hGV;
          menu.AppendMenu(MF_STRING, ID_ADD, _TR("Добавить График нагрузки ГВС"));
          if (hItem1 == m_hGV) {
              menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать График нагрузки ГВС"));
              menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить График нагрузки ГВС"));
          }
      }
*/


      if (hItem1 == m_hIST || hItem1 == m_hIST2 || hItem1 == m_hIST3) {
          m_tn = "heatSources";
          m_typ = TIP_heatSources;
          m_tn_name = _TR("Источник тепла");
          m_h = hItem1;
          menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Редактировать Источник тепла"));
          menu.AppendMenu(MF_STRING, ID_TG, _TR("Температурный график"));
          menu.AppendMenu(MF_STRING, ID_TP, _TR("Тепловые потери"));
          menu.AppendMenu(MF_STRING, ID_MOVE_TO, _TR("Перейти"));
      }



      menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
  }

  return CDialog::OnNotify(wParam, lParam, pResult);
}

void CUnvisualTree::add(const char *tn, const char *fn, HTREEITEM h) 
{
    if (!m_graph->m_cxema->m_cur_file) return;

    int idF = m_graph->m_cxema->m_cur_file->id;

    CString q;
    q.Format("INSERT INTO %s (fileID) VALUES (%d)", tn, idF);
        long affected;
    long id = ado_executeInsert(m_ado, q, &affected);
    if (id) {
        HTREEITEM hh = m_tree.InsertItem("?????", h);
        m_tree.SetItemData(hh, id);

          m_tn = tn;
          m_tn_name = findTableRusNameGeo(tn);
          m_h = h;
          m_id = id;
//          OnPopupEdit();
          PostMessage(WM_COMMAND, ID_EDIT);
    }
}


void CUnvisualTree::OnPopupAdd() 
{
    add(m_tn, "", m_h) ;
}


void deleteChildren(CTreeCtrl *tree, HTREEITEM h)
{
    // Delete all of the children of hmyItem.
    if (tree->ItemHasChildren(h))
    {
        HTREEITEM hNextItem;
        HTREEITEM hChildItem = tree->GetChildItem(h);

        while (hChildItem != NULL)
        {
            hNextItem = tree->GetNextItem(hChildItem, TVGN_NEXT);
            tree->DeleteItem(hChildItem);
            hChildItem = hNextItem;
        }
    }
}

void CUnvisualTree::init()
{
    if (m_h) {
        deleteChildren(&m_tree, m_h);

        if (m_h == m_hCT) {
            initCT(&m_tree, m_h, m_ado, m_fileID);
        }

        else if (m_h == m_hFR) {
            initFR(&m_tree, m_h, m_ado, m_fileID);
        }
        else if (m_h == m_hRS) {
            initRS(&m_tree, m_h, m_ado, m_fileID);
        }
        else if (m_h == m_hUR) {
            initUR(&m_tree, m_h, m_ado, m_fileID);
        }
        else if (m_h == m_hTR) {
            initTR(&m_tree, m_h, m_ado, m_fileID);
        }
        else if (m_h == m_hKV) {
            initKV(&m_tree, m_h, m_ado, m_fileID);
        }
/*
        else if (m_h == m_hGV) {
            initGV(&m_tree, m_h, m_ado, m_fileID);
        }
*/
        else if (m_h == m_hIST || m_h == m_hIST2 || m_h == m_hIST3) {
            deleteChildren(&m_tree, m_hIST);
            deleteChildren(&m_tree, m_hIST2);
            deleteChildren(&m_tree, m_hIST3);
            initIST(&m_tree, m_hIST, m_ado, 1, m_graph);
            initIST(&m_tree, m_hIST2, m_ado, 2, m_graph);
            initIST(&m_tree, m_hIST3, m_ado, 3, m_graph);
        }
    }
}

void CUnvisualTree::OnPopupEdit() 
{
    ado_info(this, m_ado, m_tn, m_id, m_tn_name, TP_ZERO, NULL, false);
}

#include "TempGraph/tempgraph.h"


void CUnvisualTree::OnTG() 
{
    viewTG(this, m_id);
//    m_isTG = true;
//    int ret = ado_info(this, m_ado, "heatSources", m_id, m_tn, "TG");
}


void CUnvisualTree::OnTP() 
{
    ado_info(this, m_ado, "heatSources", m_id, _TR("Температурный график"), TP_ZERO, "TP", false);
}

bool inTable(CAdoFile* ado, const char* tn, const char* fn, int id)
{
	CString q;
	q.Format("SELECT id FROM %s WHERE %s=%d", tn, fn, id);
	if (getCountTable2(ado, q) > 0) {
		return true;
	}
	return false;
}


bool inConsumer(CAdoFile *ado, const char *fn, int id)
{
	if (inTable(ado, "realConsumers", fn, id)) return true;
	if (inTable(ado, "generalizedConsumers", fn, id)) return true;

	return false;
}

bool inNode(CAdoFile* ado, const char* fn, int id)
{
    if (inTable(ado, "nodes", fn, id)) return true;

    return false;
}



void CUnvisualTree::OnPopupDel()
{
    long affected;
    CString q, s;

    if (m_typ == TIP_specExpends) {
		if (inConsumer(m_ado, "specExpendID", m_id)) {
			AfxMessageBox(_TR("Нельзя удалять"), MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
    else if (m_typ == TIP_externalCodes) {
        if (inNode(m_ado, "externalCodeID", m_id)) {
            AfxMessageBox(_TR("Нельзя удалять"), MB_OK|MB_ICONINFORMATION);
            return;
        }
    }
    else if (m_typ == TIP_varCoefficients) {
		if (inConsumer(m_ado, "varCoeffID", m_id) || inTable(m_ado, "heatPipeSections", "varCoeffIDflow", m_id) || inTable(m_ado, "heatPipeSections", "varCoeffIDret", m_id)) {
			AfxMessageBox(_TR("Нельзя удалять"), MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
    else if (m_typ == TIP_gvsLoadGraphs) {
		if (inConsumer(m_ado, "gvsLoadGraphID", m_id)) {
			AfxMessageBox(_TR("Нельзя удалять"), MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
    else if (m_typ == TIP_calcTemperatures) {
		if (inConsumer(m_ado, "calcTemperatureID", m_id)) {
			AfxMessageBox(_TR("Нельзя удалять"), MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
	else  {
        AfxMessageBox(_TR("Нельзя удалять"), MB_OK|MB_ICONINFORMATION);
        return;
    }



    s.Format(_TR("Удалить %s"), m_tn_name);

    if (AfxMessageBox(s, MB_YESNO) == IDYES) {
        q.Format("DELETE FROM %s WHERE ID=%d", m_tn, m_id);
        bool ret = ado_execute(m_ado, q, &affected);

        if (ret) {
            init();
        }
    }
}


LRESULT CUnvisualTree::OnAdoUpdated(WPARAM wParam, LPARAM)
{
    CAdoFile *ado = getAdo(getGidAdoName());

    if (ado) {
        if (m_h == m_hIST && m_isTG) {
/*

            CTempGraph tg(ado, m_id);

            CString q;

            CString TableName1 = "deployedTempGraphs";
            q.Format("SELECT id,tn,Q_otn,t1,t2,t3,tv,t_bn,tg FROM %s WHERE hSourceID=%d ORDER BY tn DESC", TableName1, m_id);
            CGidrView *pView = getView();
            pView->viewQ2(ado->m_schema, q, TableName1, virt_data_tg);
            EndDialog(IDOK);
*/
        }
        else if (m_h == m_hFR) {
        }
        else if (m_h == m_hRS) {
            CCxema *cxema = m_graph->m_cxema;
            cxema->openIst(ado);
        }
        else if (m_h == m_hCT) {
            CCxema *cxema = m_graph->m_cxema;
            cxema->open_heatSystem(ado);
        }


    }

    m_isTG = false;

    init();


    return 0;
}


void CUnvisualTree::OnMoveTo() 
{
    CGidrView *pView = getView();
    CNode2 *n = m_graph->findNode2(TIP_IS, m_id);

    if (n) {
        pView->SendMessage(WM_USER_FIND_NODE, n->id);
        EndDialog(IDOK);
    }
}

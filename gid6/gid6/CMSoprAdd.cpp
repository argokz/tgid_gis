// CMSoprAdd.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CMSoprAdd.h"
#include "afxdialogex.h"


#if 0

IDD_UTP_MSOPROT_ADD DIALOGEX 0, 0, 285, 151
STYLE DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION _TR("Установка местных сопротивлений трубопроводов")
FONT 8, "MS Sans Serif", 0, 0, 0x1
BEGIN
    EDITTEXT        IDC_EDIT4,153,42,55,12,ES_AUTOHSCROLL
    EDITTEXT        IDC_EDIT5,153,60,55,12,ES_AUTOHSCROLL
    CONTROL         "Spin1",IDC_SPIN1,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,215,58,10,14
    PUSHBUTTON      _TR("Выбрать"),IDOK,176,128,50,14
    PUSHBUTTON      _TR("Отмена"),IDCANCEL,230,128,50,14
    EDITTEXT        IDC_EDIT1,64,8,210,12,ES_AUTOHSCROLL | ES_READONLY
    EDITTEXT        IDC_EDIT2,82,22,55,12,ES_AUTOHSCROLL | ES_READONLY
    EDITTEXT        IDC_EDIT3,219,22,55,12,ES_AUTOHSCROLL | ES_READONLY
    EDITTEXT        IDC_EDIT6,153,109,68,12,ES_AUTOHSCROLL | ES_READONLY
    GROUPBOX        _TR("Параметры"),-1,2,0,280,85
    LTEXT           _TR("Наименование"),-1,10,9,52,8
    LTEXT           _TR("Сопротивление(min)"),-1,10,24,68,8
    LTEXT           _TR("Сопротивление"),-1,94,44,53,8
    LTEXT           _TR("Итоговое местное сопротивление"),-1,30,111,117,8
    GROUPBOX        _TR("Результаты"),-1,3,99,278,27
    LTEXT           _TR("Количество"),-1,107,62,40,8
    LTEXT           _TR("Сопротивление(max)"),-1,144,24,70,8
END


#endif


// CMSoprAdd dialog

//IMPLEMENT_DYNAMIC(CMSoprAdd, CDialogEx)

CMSoprAdd::CMSoprAdd(CWnd* pParent, int id, const char *name, double s_min, double s_max, double s, int n)
	: CDialogEx(IDD_UTP_MSOPROT_ADD, pParent)
{
  m_name = name;
  m_s_min = s_min;
  m_s_max = s_max;
  m_s = s;
  m_n = n;
  m_s_itog = 0;

  m_init = false;
}

CMSoprAdd::~CMSoprAdd()
{
}

void CMSoprAdd::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMSoprAdd)
    DDX_Text(pDX, IDC_EDIT1, m_name);
    DDX_Text(pDX, IDC_EDIT2, m_s_min);
    DDX_Text(pDX, IDC_EDIT3, m_s_max);
    DDX_Text(pDX, IDC_EDIT4, m_s);
    DDX_Text(pDX, IDC_EDIT5, m_n);
    DDX_Text(pDX, IDC_EDIT6, m_s_itog);

    DDX_Control(pDX, IDC_EDIT1, Edit1);
    DDX_Control(pDX, IDC_EDIT2, Edit2);
    DDX_Control(pDX, IDC_EDIT3, Edit3);
    DDX_Control(pDX, IDC_EDIT4, Edit4);
    DDX_Control(pDX, IDC_EDIT5, Edit5);
    DDX_Control(pDX, IDC_EDIT6, Edit6);

    
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMSoprAdd, CDialogEx)
  //{{AFX_MSG_MAP(CMSoprAdd)
  ON_EN_CHANGE(IDC_EDIT4, OnChangeS)
  ON_EN_CHANGE(IDC_EDIT5, OnChangeS)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



// CMSoprAdd message handlers


void CMSoprAdd::OnChangeS()
{

    if (m_init) {
        UpdateData(TRUE);
        m_s_itog = m_s * m_n;
        UpdateData(FALSE);
    }
}



BOOL CMSoprAdd::OnInitDialog() 
{
  CDialogEx::OnInitDialog();
  
  m_init = true;
  
  return TRUE;  // return TRUE unless you set the focus to a control
              // EXCEPTION: OCX Property Pages should return FALSE
}

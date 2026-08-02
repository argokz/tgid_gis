// utp_diam_rasch.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"

//#include "..\TeploCalc.h"

#include "utp_diam_rasch.h"
#include "math.h"
//#include "..\Ch_math.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#include "UTP_DiamHelp.h"
// 
/////////////////////////////////////////////////////////////////////////////
// CUTP_diam_rasch dialog


CUTP_diam_rasch::CUTP_diam_rasch(CWnd* pParent /*=NULL*/)
	: CDialog(CUTP_diam_rasch::IDD, pParent)
{
//	if (Wsp==NULL)return;
//	wsp=Wsp;
	//{{AFX_DATA_INIT(CUTP_diam_rasch)
	//}}AFX_DATA_INIT
	Edit1_str="";
	Edit2_str="";
	Edit3_str="";
	Edit4_str="";
	Edit5_str="";
	Combo3_i=0;
	Combo4_i=0;
	Combo5_i=0;
	Combo6_i=0;
}
//----------------------------------------------------------------------------
void CUTP_diam_rasch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUTP_diam_rasch)
//	DDX_Control(pDX, IDC_BUTTON1, m_HELP);
	DDX_Control(pDX, IDCANCEL, Button1);
	DDX_Control(pDX, IDOK, Button2);
	DDX_Control(pDX, IDC_EDIT2, Edit2);
	DDX_Control(pDX, IDC_EDIT1, Edit1);
	DDX_Control(pDX, IDC_EDIT3, Edit3);
	DDX_Control(pDX, IDC_EDIT4, Edit4);
	DDX_Control(pDX, IDC_EDIT5, Edit5);
	DDX_Control(pDX, IDC_COMBO6, Combo6);
	DDX_Control(pDX, IDC_COMBO5, Combo5);
	DDX_Control(pDX, IDC_COMBO4, Combo4);
	DDX_Control(pDX, IDC_COMBO3, Combo3);
	//}}AFX_DATA_MAP
}
//----------------------------------------------------------------------------
void CUTP_diam_rasch::Raschet()
{
	//float k1[]={0.28,0.00023,1};
	//float k2[]={9.81,9810,1,1000000,9.81};
	CString Str;
	char buffer[100];
	
	double G=0,R=0,ro=0,sigma=0,ke=0,e=0,Rn=0,lya=0,Re=0,w=0;
	CString str;
	double PI=3.1415926536;
	double k1;
	double k2[]={1,9803.9215686274509803921568627451};
	double ro_[]={983.2,977.8,971.8,965.3,958.4,943.4,962.4,907.5,887.0};
	double sigma_[]={0.478,0.416,0.367,0.328,0.296,0.246,0.212,0.192,0.174};
	
	if ( (Combo3.GetCurSel()<0)||(Combo4.GetCurSel()<0)||(Combo5.GetCurSel()<0)||(Combo6.GetCurSel()<0) )goto bad_calc;
	if (Combo5.GetCurSel()!=Combo6.GetCurSel())Combo6.SetCurSel(Combo5.GetCurSel());
	
	Edit1.GetWindowText(str);	
	if (Combo3.GetCurSel()==1) G=atof(str)*1000/3600.0;
	else G=atof(str);	
	Edit2.GetWindowText(str);	
	R=atof(str)*k2[Combo4.GetCurSel()];
	ro=ro_[Combo5.GetCurSel()];
	sigma=sigma_[Combo6.GetCurSel()]*0.000001;
	Edit5.GetWindowText(str);	
	ke=atof(str);
	
	d=0.001;	
	do 
	{
		d+=0.001;
		if ( (ro<=0.0000000000001)&&(ro>=-0.0000000000001) )goto bad_calc;
		w=(G*4)/PI/d/d/ro;
		if ( (sigma<=0.0000000000001)&&(sigma>=-0.0000000000001) )goto bad_calc;
		Re=w*d/sigma;
		if ( (d<=0.0000000000001)&&(d>=-0.0000000000001) )goto bad_calc;
		if ( (Re<=0.0000000000001)&&(Re>=-0.0000000000001) )goto bad_calc;
		lya=0.11*pow((ke/d+68/Re),0.25);
		Rn=lya*w*w*ro/(2*d);
		if ( (R<=0.0000000000001)&&(R>=-0.0000000000001) )goto bad_calc;
		e=fabs((R-Rn)/R);
		if(d>1.392)goto bad_calc;
		/*
		{
			e=0;c
			R=Rn;
			MessageBox("Недопустимый режим, уточните данные.","Ошибка");
			return;
		}
		*/
	}while (e>=0.1);
	sprintf(buffer, "%.3g",w);
	Str=buffer;
	Edit3.SetWindowText(Str);
	d=d*1000;
	sprintf(buffer, "%.3g",d);
	Str=buffer;
	d=atof(Str);
	Edit4.SetWindowText(Str);	
	Button2.EnableWindow(TRUE);
	return;

bad_calc:
	Str.LoadString(IDS_STRING_ERROR);	
	Edit3.SetWindowText(Str);
	Edit4.SetWindowText(Str);
	Button2.EnableWindow(FALSE);
	return;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CUTP_diam_rasch, CDialog)
	//{{AFX_MSG_MAP(CUTP_diam_rasch)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT1, Raschet)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT5, Raschet)
	ON_CBN_CLOSEUP(IDC_COMBO6, OnCloseupCombo6)
	ON_CBN_CLOSEUP(IDC_COMBO5, OnCloseupCombo5)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_EN_CHANGE(IDC_EDIT2, Raschet)
	ON_CBN_CLOSEUP(IDC_COMBO3, Raschet)
	ON_CBN_CLOSEUP(IDC_COMBO4, Raschet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUTP_diam_rasch message handlers

/**
HBRUSH CUTP_diam_rasch::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return theApp.OnCtlColorDark(pDC,pWnd,nCtlColor);
}
*/
//----------------------------------------------------------------------------


BOOL CUTP_diam_rasch::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	Combo3.SetReadOnly();
//	Combo4.SetReadOnly();
//	Combo5.SetReadOnly();
//	Combo6.SetReadOnly();
//	Edit1.InitEdit(EDIT_FLOAT,FALSE);
//	Edit2.InitEdit(EDIT_FLOAT,FALSE);
//	Edit5.InitEdit(EDIT_FLOAT,FALSE);

	Edit1.SetWindowText(Edit1_str);
	Edit2.SetWindowText(Edit2_str);
	Edit3.SetWindowText(Edit3_str);
	Edit4.SetWindowText(Edit4_str);
	Edit5.SetWindowText(Edit5_str);

	if ( (Combo3_i>=0)&&(Combo3_i<Combo3.GetCount()) )Combo3.SetCurSel(Combo3_i);
	if ( (Combo4_i>=0)&&(Combo4_i<Combo4.GetCount()) )Combo4.SetCurSel(Combo4_i);
	if ( (Combo5_i>=0)&&(Combo5_i<Combo5.GetCount()) )Combo5.SetCurSel(Combo5_i);
	if ( (Combo6_i>=0)&&(Combo6_i<Combo6.GetCount()) )Combo6.SetCurSel(Combo6_i);
	d=0;
	Raschet();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CUTP_diam_rasch::OnDestroy() 
{
	CDialog::OnDestroy();
	Edit1.GetWindowText(Edit1_str);
	Edit2.GetWindowText(Edit2_str);
	Edit3.GetWindowText(Edit3_str);
	Edit4.GetWindowText(Edit4_str);
	Edit5.GetWindowText(Edit5_str);
	Combo3_i=Combo3.GetCurSel();
	Combo4_i=Combo4.GetCurSel();
	Combo5_i=Combo5.GetCurSel();
	Combo6_i=Combo6.GetCurSel();
}

void CUTP_diam_rasch::OnCloseupCombo5() 
{
	if (Combo5.GetCurSel()!=Combo6.GetCurSel())Combo6.SetCurSel(Combo5.GetCurSel());	
	Raschet();
}


void CUTP_diam_rasch::OnCloseupCombo6() 
{
	if (Combo5.GetCurSel()!=Combo6.GetCurSel())Combo5.SetCurSel(Combo6.GetCurSel());	
	Raschet();
}

void CUTP_diam_rasch::OnButton1() 
{
//	CUTP_DiamHelp UTP_DiamHelp(wsp);		
//	UTP_DiamHelp.DoModal();
}


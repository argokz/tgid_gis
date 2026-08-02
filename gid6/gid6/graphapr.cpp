// graphapr.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "graphapr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphApr dialog

CGraphApr::CGraphApr(GRAPHR_PARAM * Param,CWnd* pParent /*=NULL*/)
  : CGraphTemplate(&Param->gr,CGraphApr::IDD, pParent)
{
  //{{AFX_DATA_INIT(CGraphApr)
  m_IsFnom = -1;
  m_IsDnom = -1;
  //}}AFX_DATA_INIT
  if (Param==NULL) return;
  
  GrApr=(* Param);  
}
//----------------------------------------------------------------------------
void CGraphApr::DoDataExchange(CDataExchange* pDX)
{
  CGraphTemplate::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CGraphApr)
  DDX_Control(pDX, IDC_REAL_CHECK, m_real_check);
  DDX_Control(pDX, IDC_NOM_CHECK, m_nom_check);
  DDX_Control(pDX, IDC_REP, m_c_rep_but);
  DDX_Control(pDX, IDOK, m_OK);
  DDX_Control(pDX, IDCANCEL, m_CANCEL);
  DDX_Control(pDX, IDC_COPY_TO, m_copy_to);
  DDX_Radio(pDX, IDC_RADIO7, m_IsFnom);
  DDX_Radio(pDX, IDC_RADIO9, m_IsDnom);
  //}}AFX_DATA_MAP
}
//----------------------------------------------------------------------------
CString CGraphApr::GetCaptionStrt()
{
  CString Str;
  Str.LoadString(IDS_STRING_ST_NAS_CHARG);
  return Str; 
}
//----------------------------------------------------------------------------
void CGraphApr::ChahgeDrawParams(TTypeChart type,APPROX_PARAM * apr_par,CPen * &_pen)
{
  double p1 = 1.0, p2 = 1.0;
  if (!GrApr.IsFnom) p2 = (double)GrApr.c_vr / (double)GrApr.c_vr1;
  if (!GrApr.IsDnom) p1 = (double)GrApr.diam / (double)GrApr.diam1;
        
  switch (type)
  {
  case H_chart :
    {
      if (Show_Hr_chart)
      { 
        if( p1 && p2 )
        {
          apr_par->r0 = apr_par->r0 * pow( p1, 2 ) * pow( p2, 2 );
          apr_par->r1 = apr_par->r1 * p1 * p2;
          if (GrApr.Spr)apr_par->r2 = apr_par->r2 - GrApr.S;
          else apr_par->r2 = apr_par->r2;
          if (_pen==NULL) 
          {
            _pen=new CPen(PS_SOLID,1,_HzadQ_color);//График HQ_zad              
          }
        }
        else CGraphTemplate::ChahgeDrawParams(type,apr_par,_pen);       
      }
      else 
      {
        CGraphTemplate::ChahgeDrawParams(type,apr_par,_pen);        
      }
      break;
    }
  case N_chart :
    {
      if (Show_Nr_chart)
      { 
        if( p1 && p2 )
        {
          apr_par->r0 = apr_par->r0 * pow( p2, 3 ) * pow( p1, 5 );
          apr_par->r1 = apr_par->r1 * pow( p2, 2 ) * pow( p1, 4 );
          apr_par->r2 = apr_par->r2  * p2 * pow( p1, 3 );
          if (_pen==NULL) 
          {
            _pen=new CPen(PS_SOLID,1,_NzadQ_color);//График NQ_zad              
          }
        }
        else CGraphTemplate::ChahgeDrawParams(type,apr_par,_pen);       
      }
      else 
      {
        CGraphTemplate::ChahgeDrawParams(type,apr_par,_pen);        
      }
      break;
    }
  case KPD_chart :
    {
      if (Show_KPDr_chart)
      { 
        if( p1 && p2 )
        {
          apr_par->r0 = apr_par->r0;
          apr_par->r1 = apr_par->r1  / ( p1 * p2 );
          apr_par->r2 = apr_par->r2  / ( pow( p1, 2 ) * pow( p2, 2 ) );
          if (_pen==NULL) 
          {
            _pen=new CPen(PS_SOLID,1,_KPDzadQ_color);//График NQ_zad              
          }
        }
        else CGraphTemplate::ChahgeDrawParams(type,apr_par,_pen);       
                
      }
      else 
      {
        CGraphTemplate::ChahgeDrawParams(type,apr_par,_pen);        
      }
      break;
    }
  }
}
/*case 2:
          rr0 = r0 * p1 * p2;
          rr1 = r1 / ( p1 * p2 );
          rr2 = r2 / ( pow( p1, 3 ) * pow( p2, 3 ) );
          break;
         case 3:
          rr0 = r0 * pow( p2, 3 ) * pow( p1, 5 );
          rr1 = r1 * pow( p2, 2 ) * pow( p1, 4 );
          rr2 = r2 * p2 * pow( p1, 3 );
          break;
         case 4:
          rr0 = r0;
          rr1 = r1 / ( p1 * p2 );
          rr2 = r2 / ( pow( p1, 2 ) * pow( p2, 2 ) );
          break;
          */
        
//----------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CGraphApr, CGraphTemplate)
  //{{AFX_MSG_MAP(CGraphApr)
  ON_BN_CLICKED(IDC_REP, OnRep)
  ON_BN_CLICKED(IDC_NOM_CHECK, OnNomCheck)
  ON_BN_CLICKED(IDC_REAL_CHECK, OnRealCheck)
  ON_BN_DOUBLECLICKED(IDC_REP, OnRep) 
  ON_BN_CLICKED(IDC_RADIO7, CmChangeVal)
  ON_BN_CLICKED(IDC_RADIO8, CmChangeVal)
  ON_BN_CLICKED(IDC_RADIO9, CmChangeVal)
  ON_BN_CLICKED(IDC_RADIO10, CmChangeVal)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphApr message handlers
void CGraphApr::CmChangeVal()
{
  UpdateData(TRUE);
  if (GrApr.IsFnom)m_IsFnom=0;
  else m_IsFnom=1;
  if (GrApr.IsDnom)m_IsDnom=0;
  else m_IsDnom=1;
  UpdateData(FALSE);
}
//----------------------------------------------------------------------------
void CGraphApr::Draw(CDC &dc)
{
  CGraphTemplate::Draw(dc);
  CString Str;
  Str.LoadString(IDS_STRING_ST_NAS_CHARG);
  dc.TextOut(llx+ 270,lly+ 5,Str);
  char buf[1000];
  if (!gr_rap)
  {
    CPen p1(PS_SOLID,1,_basic_color);
    CPen * def;
    def=dc.SelectObject( &p1 );
    BOOL prepared=FALSE,Temp_chart;
    if (Show_H_chart) 
    {
      Temp_chart=Show_Hr_chart;
      Show_Hr_chart=FALSE;
      DrawChart(dc,H_chart,&apH,prepared);
      Show_Hr_chart=Temp_chart;
      prepared=TRUE;
    }
    if (Show_Hr_chart) 
    {
      DrawChart(dc,H_chart,&apHr,prepared);
      prepared=TRUE;
    }
    if (Show_N_chart) 
    {
      Temp_chart=Show_Nr_chart;
      Show_Nr_chart=FALSE;      
      DrawChart(dc,N_chart,&apN,prepared);
      Show_Nr_chart=Temp_chart;
      prepared=TRUE;
    }
    if (Show_Nr_chart) 
    {
      DrawChart(dc,N_chart,&apNr,prepared);
      prepared=TRUE;
    }
    
    if (Show_KPD_chart) 
    {
      Temp_chart=Show_KPDr_chart;
      Show_KPDr_chart=FALSE;      
      DrawChart(dc,KPD_chart,&apKPD,prepared);
      Show_KPDr_chart=Temp_chart;
      prepared=TRUE;
    }
    if (Show_KPDr_chart) 
    {
      DrawChart(dc,KPD_chart,&apKPDr,prepared);
      prepared=TRUE;
    }
    /*
    DrawChart(dc,N_chart,&apNr,TRUE);
    Show_N_chart=!Show_N_chart;
    DrawChart(dc,N_chart,&apN,TRUE);
    Show_N_chart=!Show_N_chart;

    DrawChart(dc,KPD_chart,&apKPDr,TRUE);
    Show_KPD_chart=!Show_KPD_chart; 
    DrawChart(dc,KPD_chart,&apKPD,TRUE);
    Show_KPD_chart=!Show_KPD_chart; 
    */
    if (prepared) DrawEdge(dc);  
    dc.SelectObject(def);
  }
  else 
  {
    int posit=40;
    #define ___Shift 400
    dc.TextOut(llx+15,lly+(posit), _TR("Количество насосов : ") );

        sprintf( buf, "%4d", GrApr.gr.k_kol ); dc.TextOut(llx+ ___Shift,lly+ posit, buf );
    
    posit+=20;
    dc.TextOut(llx+ 15,lly+ (posit), _TR("Номинальный диаметр рабочего колеса, мм") );
    sprintf( buf, "%4.3f", GrApr.diam1 ); dc.TextOut(llx+ ___Shift,lly+ posit, buf );
    
    posit+=20;
    dc.TextOut(llx+ 15,lly+ (posit), _TR("Заданный диаметр рабочего колеса, мм") );
    sprintf( buf, "%4.3f", GrApr.diam ); dc.TextOut(llx+ ___Shift,lly+ posit, buf );
    
    posit+=20;  
    dc.TextOut(llx+ 15,lly+ (posit), _TR("Номинальная частота вращения, 1/мин") );
    sprintf( buf, "%4.3f", GrApr.c_vr1 ); dc.TextOut( llx+___Shift,lly+ posit, buf );
    
    posit+=20;    
    dc.TextOut( llx+15,lly+ (posit), _TR("Заданная частота вращения, 1/мин") );
    sprintf( buf, "%4.3f", GrApr.c_vr ); dc.TextOut(llx+ ___Shift,lly+ posit, buf );

    dc.TextOut(llx+ 200,lly+ 140, _TR("Рабочая зона :") );
    dc.TextOut(llx+ 15,lly+ 160,  _TR("Левая  граница: ") );
    dc.TextOut(llx+ 15,lly+ 180,  _TR("Правая граница: ") );
    dc.TextOut(llx+ 130,lly+ 160,  _TR("напор -              м,") );
    dc.TextOut(llx+ 130,lly+ 180,  _TR("напор -              м,") );
    dc.TextOut(llx+ 250,lly+ 160,  _TR("расход -                  т/ч") );
    dc.TextOut(llx+ 250,lly+ 180,  _TR("расход -                  т/ч") );
     double a = DiamGR.Q_min;
     double b = apH.r0+apH.r1*a+apH.r2*pow(a,2);
     for(int i = 0; i<2; i++ )
     {
      sprintf( buf, "%5.2f", a ); dc.TextOut(llx+ 300,lly+ 160+20*i, buf );
      sprintf( buf, "%5.2f", b ); dc.TextOut(llx+ 170,lly+ 160+20*i, buf );
        a = DiamGR.Q_max;
        b = apH.r0+apH.r1*a+apH.r2*pow(a,2);     
     }        
    posit+=80;
    dc.TextOut(llx+ 170,lly+ posit, _TR("Уравнения характеристик:") );
    posit+=20;
    dc.TextOut(llx+ 50,lly+ posit, _TR("Номинальный режим:") );
    dc.TextOut(llx+ 350,lly+ posit, _TR("Заданный режим:") );
    posit+=20;
    sprintf( buf, "H = %4.2f + %4.2f Q + %4.2f Q^2", apH.r0,apH.r1,apH.r2); dc.TextOut(llx+ 15,lly+ posit, buf );
    sprintf( buf, "H = %4.2f + %4.2f Q + %4.2f Q^2", apHr.r0,apHr.r1,apHr.r2); dc.TextOut(llx+ 300,lly+ posit, buf );
    posit+=20;
    sprintf( buf, "(Макс. ошибка аппроксимации - %4.6f)",apH.pog);dc.TextOut(llx+ 15,lly+ posit, buf );
    sprintf( buf, "(Макс. ошибка аппроксимации - %4.6f)",apHr.pog);dc.TextOut(llx+ 250,lly+ posit, buf );
    posit+=20;
    sprintf( buf, "N = %4.2f + %4.2f Q + %4.2f Q^2", apN.r0,apN.r1,apN.r2); dc.TextOut(llx+ 15,lly+ posit, buf );
    sprintf( buf, "N = %4.2f + %4.2f Q + %4.2f Q^2", apNr.r0,apNr.r1,apNr.r2); dc.TextOut(llx+ 300,lly+ posit, buf );
    posit+=20;
    sprintf( buf, "(Макс. ошибка аппроксимации - %4.6f)",apN.pog);dc.TextOut(llx+ 15,lly+ posit, buf );
    sprintf( buf, "(Макс. ошибка аппроксимации - %4.6f)",apNr.pog);dc.TextOut(llx+ 250,lly+ posit, buf );
    posit+=20;
    sprintf( buf, _TR("КПД = %4.2f + %4.2f Q + %4.2f Q^2"), apKPD.r0,apKPD.r1,apKPD.r2); dc.TextOut(llx+ 15,lly+ posit, buf );
    sprintf( buf, _TR("КПД = %4.2f + %4.2f Q + %4.2f Q^2"), apKPDr.r0,apKPDr.r1,apKPDr.r2); dc.TextOut(llx+ 300,lly+ posit, buf );
    posit+=20;
    sprintf( buf, "(Макс. ошибка аппроксимации - %4.6f)",apKPD.pog);dc.TextOut(llx+ 15,lly+ posit, buf );
    sprintf( buf, "(Макс. ошибка аппроксимации - %4.6f)",apKPDr.pog);dc.TextOut(llx+ 250,lly+ posit, buf );
    posit+=20;
    sprintf( buf, _TR("где    [H] = м.вод.ст;    [Q] = т/ч;    [КПД] = проценты"));dc.TextOut(llx+ 120,lly+ posit, buf );
    
        
  } 
}
//----------------------------------------------------------------------------
void CGraphApr::OnRep() 
{
  CGraphTemplate::OnRep();
  char *buttonName[2] = { "Отчет", "График" };
  m_c_rep_but.SetWindowText( buttonName[gr_rap] );  
}
//----------------------------------------------------------------------------
void CGraphApr::CopyRaport(char *cbuf)
{
  char buf[1000];
  CString Str;
  Str.LoadString(IDS_STRING_ST_NAS_CHARG);  strcpy(cbuf,Str);
  sprintf( buf, "\n" ); strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPHR_TYPE);      strcat(cbuf,Str);
  sprintf(buf, ":%s\n", DiamGR.Tip );     strcat(cbuf, buf);    
  
  Str.LoadString(IDS_GRAPR_KOL);        strcat(cbuf,Str);
  sprintf(buf, "\t%4d\n",GrApr.gr.k_kol );  strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPHR_DNOM);      strcat(cbuf,Str);
  sprintf(buf, "\t%4.3f\n",GrApr.diam1 );   strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_DZAD);       strcat(cbuf,Str);
  sprintf(buf, "\t%4.3f\n",GrApr.diam );    strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPHR_FNOM);      strcat(cbuf,Str);
  sprintf(buf, "\t%4.3f\n",GrApr.c_vr1 );   strcat(cbuf, buf);    
  Str.LoadString(IDS_GRAPR_FZAD);       strcat(cbuf,Str);
  sprintf(buf, "\t%4.3f\n",GrApr.c_vr );    strcat(cbuf, buf);    
  
  Str.LoadString(IDS_GRAPR_RZONE);      strcat(cbuf,Str);
  sprintf( buf, "\n" );           strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_LEFT_EDGE);    strcat(cbuf,Str);
  Str.LoadString(IDS_GRAPR_NAPOR);      strcat(cbuf,Str);
  double a = DiamGR.Q_min;
  double b = apH.r0+apH.r1*a+apH.r2*pow(a,2);
  sprintf( buf, "%5.2f", b );         strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPHR_METER);     strcat(cbuf,Str);
  
  Str.LoadString(IDS_GRAPR_RASHOD);     strcat(cbuf,Str);
  sprintf( buf, "%5.2f", a );         strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPHR_TCH);       strcat(cbuf,Str);
  sprintf( buf, "\n" );           strcat(cbuf, buf);

  Str.LoadString(IDS_GRAPR_RIGHT_EDGE);   strcat(cbuf,Str);
  Str.LoadString(IDS_GRAPR_NAPOR);      strcat(cbuf,Str);
  a = DiamGR.Q_max;
  b = apH.r0+apH.r1*a+apH.r2*pow(a,2);     
  sprintf( buf, "%5.2f", b );         strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPHR_METER);     strcat(cbuf,Str);
  
  Str.LoadString(IDS_GRAPR_RASHOD);     strcat(cbuf,Str);
  sprintf( buf, "%5.2f", a );         strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPHR_TCH);       strcat(cbuf,Str);
  sprintf( buf, "\n" );           strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_EQUATIONS);    strcat(cbuf,Str);
  sprintf( buf, "\n" );           strcat(cbuf, buf);
  
  Str.LoadString(IDS_GRAPR_EQUATIONS_NOM);    strcat(cbuf,Str);
  sprintf( buf, "\n" );           strcat(cbuf, buf);
  sprintf( buf, "H = %4.2f + %4.2f Q + %4.2f Q^2  ", apH.r0,apH.r1,apH.r2);strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_EQUATIONS_MAXERR); strcat(cbuf,Str);
  sprintf( buf, "%4.6f)\n",apH.pog);      strcat(cbuf, buf);
  sprintf( buf, "N = %4.2f + %4.2f Q + %4.2f Q^2  ", apN.r0,apN.r1,apN.r2);strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_EQUATIONS_MAXERR); strcat(cbuf,Str);
  sprintf( buf, "%4.6f)\n",apN.pog);      strcat(cbuf, buf);
  sprintf( buf, _TR("КПД = %4.2f + %4.2f Q + %4.2f Q^2  "), apKPD.r0,apKPD.r1,apKPD.r2);strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_EQUATIONS_MAXERR); strcat(cbuf,Str);
  sprintf( buf, "%4.6f)\n",apKPD.pog);    strcat(cbuf, buf);
  
  Str.LoadString(IDS_GRAPR_EQUATIONS_ZAD);    strcat(cbuf,Str);
  sprintf( buf, "\n" );           strcat(cbuf, buf);
  sprintf( buf, "H = %4.2f + %4.2f Q + %4.2f Q^2  ", apHr.r0,apHr.r1,apHr.r2);strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_EQUATIONS_MAXERR); strcat(cbuf,Str);
  sprintf( buf, "%4.6f)\n",apHr.pog);     strcat(cbuf, buf);
  sprintf( buf, "N = %4.2f + %4.2f Q + %4.2f Q^2  ", apNr.r0,apNr.r1,apNr.r2);strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_EQUATIONS_MAXERR); strcat(cbuf,Str);
  sprintf( buf, "%4.6f)\n",apNr.pog);     strcat(cbuf, buf);
  sprintf( buf, _TR("КПД = %4.2f + %4.2f Q + %4.2f Q^2  "), apKPDr.r0,apKPDr.r1,apKPDr.r2);strcat(cbuf, buf);
  Str.LoadString(IDS_GRAPR_EQUATIONS_MAXERR); strcat(cbuf,Str);
  sprintf( buf, "%4.6f)\n",apKPDr.pog);   strcat(cbuf, buf);


}
//----------------------------------------------------------------------------






void CGraphApr::OnNomCheck() 
{
  if (m_nom_check.GetCheck()!=0)
  {
    Show_H_chart=TRUE;
    Show_N_chart=TRUE;
    Show_KPD_chart=TRUE;
  } 
  else
  {
    Show_H_chart=FALSE;
    Show_N_chart=FALSE;
    Show_KPD_chart=FALSE;
  }
  
//  CClientDC Dc(this);
//  Draw(Dc);
  Invalidate();

}

void CGraphApr::OnRealCheck() 
{
  if (m_real_check.GetCheck()!=0)
  {
    Show_Hr_chart=TRUE;
    Show_Nr_chart=TRUE;
    Show_KPDr_chart=TRUE;
  } 
  else
  {
    Show_Hr_chart=FALSE;
    Show_Nr_chart=FALSE;
    Show_KPDr_chart=FALSE;
  }
//  CClientDC Dc(this);
//  Draw(Dc);
  Invalidate(); 
}

BOOL CGraphApr::OnInitDialog() 
{
  CGraphTemplate::OnInitDialog();
  
  CmChangeVal();
  m_nom_check.SetCheck(1);
  OnNomCheck(); 
  m_real_check.SetCheck(1);
  OnRealCheck();
  CClientDC Dc(this);
  Draw(Dc);
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

// graphtemplate.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "graphtemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#include "..\Ch_math.h" //необходимые вычислительные функции

double getScreenMas();

/////////////////////////////////////////////////////////////////////////////
// CGraphTemplate dialog
CGraphTemplate::CGraphTemplate(GRAPH_PARAM * GR,UINT IDD,CWnd* pParent /*=NULL*/)
  : CDialog(IDD, pParent)
{
  if (GR==NULL) return;
  DiamGR=(*GR);
  gr_rap=0;
  error_nasos = FALSE;

  double sm = getScreenMas();

  stepX  = 20*sm; // шаг между линиями сетки по X
  stepY  = 14*sm; // ... по Y

  nGridX = 20; // количество линий сетки по X
  nGridY = 24; // ... по Y
  llx=20;
  lly=10; 
  lX =llx*sm+45*sm,     // значение X левого верхнего угла
  lY =lly*sm+30*sm,     // значение Y ...
  rX = lX+stepX*nGridX, // значение X проавого нижнего угла
  rY = lY+stepY*nGridY; // значение Y ...

  font=NULL;
  inihq( DiamGR.Q, DiamGR.Q, DiamGR.k_kol ); //масштабирование к кол-ву насосов   
  DiamGR.Q_min=DiamGR.Q_min*DiamGR.k_kol;//масштабирование к кол-ву насосов   
  DiamGR.Q_max=DiamGR.Q_max*DiamGR.k_kol;//масштабирование к кол-ву насосов   

  //{{AFX_DATA_INIT(CGraphTemplate)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CGraphTemplate::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CGraphTemplate)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}
//----------------------------------------------------------------------------
void CGraphTemplate::SetMyFont(CDC &dc)
{
  if (font==NULL)
  {
    font = new CFont();
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
    lf.lfHeight = 14;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_NORMAL;
    lf.lfItalic = 0;
    lf.lfUnderline = 0;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfCharSet = RUSSIAN_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = PROOF_QUALITY;
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
    lstrcpy(lf.lfFaceName, "Times New Roman");
    VERIFY(font->CreateFontIndirect(&lf));  // create the font          
  }
  dc.SelectObject(font);
}
//----------------------------------------------------------------------------
void CGraphTemplate::OnRep() 
{
  gr_rap=!gr_rap;
  CClientDC * cdc;
  cdc=new CClientDC(this); // device context for painting
  Draw(*cdc);
  delete cdc;
}
//----------------------------------------------------------------------------
double CGraphTemplate::FindMax( double *arr )
{
  double _maxY_=arr[0];
  for( int i=1; i<10; i++ ) if( arr[i] > _maxY_) _maxY_= arr[i];
  return _maxY_;
}
//----------------------------------------------------------------------------
void CGraphTemplate::DrawGraph( double r0, double r1, double r2,
           CDC* dc, short col, short row )
{
  double p1 = (double)(maxX - minX) / (rX - lX);
  double Q = minX;
  double H = r0 + r1*Q + r2*pow( Q, 2 );
  double sX = col + Q / ( (double)masX / (stepX) );
  double sY = row - H / ( (double)masY / (stepY) );
  double a1 = (double)masX / stepX;
  double a2 = (double)masY / stepY;
  int iter = rX - lX;
  if( sY >= rY ) sY = rY;
  if( sY <= lY ) sY = lY;
  dc->MoveTo((int)sX,(int)sY);
  for( int i = 0; i < iter; i++ )
  {
    H = r0 + r1*Q + r2*pow( Q, 2 );
    sX = col + Q / a1;
    sY = row - H / a2;
    if( sY <= rY && sY >= lY ) dc->LineTo((int)sX,(int)sY);
    Q += p1;
  } 
}
//----------------------------------------------------------------------------
void CGraphTemplate::DrawGraphLine( double * H, double * Q,
           CDC* dc, short col, short row )
{
  double p1 = (double)(maxX - minX) / (rX - lX);
  //double Q = minX;
  //double H = r0 + r1*Q + r2*pow( Q, 2 );
  double sX = col + Q[0] / ( (double)masX / (stepX) );
  double sY = row - H[0] / ( (double)masY / (stepY) );
  double a1 = (double)masX / stepX;
  double a2 = (double)masY / stepY;
  int iter = rX - lX;
  if( sY >= rY ) sY = rY;
  if( sY <= lY ) sY = lY;
  dc->MoveTo((int)sX,(int)sY);
  for (int i=0;i<10;i++)
  {
    sX = col + Q[i] / a1;
    sY = row - H[i] / a2;
    if( sY <= rY && sY >= lY ) dc->LineTo((int)sX,(int)sY);
  }
}
void CGraphTemplate::MasSh(double  min, double max, short kol, double *mas )
{
  double p = (double)(max - min) / kol;
  int masd =(int) p + 1;
  if( masd > 10 )
  {
   int j = 1;
   while( j!=0 )  j = masd - ( (int)masd++/10 ) * 10;
   masd -= 1;
  }
  *mas=masd;
}
//----------------------------------------------------------------------------
void CGraphTemplate::Appr( double &r0,double &r1,double &r2,double *arr1, double *arr2, double* pog )
{
  double b1=0,b2=0,b3=0;
  double am11=10,am12=0,am13=0,am21=0,am22=0,am23=0,
         am31=0,am32=0,am33=0;
  double av11=0,av12=0,av13=0,av21=0,av22=0,av23=0,
         av31=0,av32=0,av33=0;
  double d=0;
   int i;

  for(i=0; i<10; i++)  {
    b1 += (double)arr1[i];
    b2 += (double)arr1[i]*arr2[i];
    b3 += arr1[i]*pow( arr2[i], 2 );
    am12 += arr2[i];
    am13 += pow( arr2[i], 2 );
    am23 += pow( arr2[i], 3 );
    am33 += pow( arr2[i], 4 );
  }
  am21=am12;  am22=am13;
  am31=am13;  am32=am23;

  av11=am22*am33-am32*am23;    av12=-(am21*am33-am31*am23); av13=am21*am32-am31*am22;
  av21=-(am12*am33-am32*am13); av22=am11*am33-am31*am13;    av23=-(am11*am32-am31*am12);
  av31=am12*am23-am22*am13;    av32=-(am11*am23-am21*am13); av33=am11*am22-am21*am12;

  d=am11*am22*am33+am12*am23*am31+am21*am32*am13-am13*am22*am31-am12*am21*am33-am11*am23*am32;
  if (d == 0) {
    AfxMessageBox(_TR("В базе данных не полная информация по характеристикам насоса"),MB_OK | MB_ICONSTOP);
    error_nasos = TRUE;
    return;
  }

  r0=(av11*b1+av12*b2+av13*b3)/d;
  r1=(av21*b1+av22*b2+av23*b3)/d;
  r2=(av31*b1+av32*b2+av33*b3)/d;
  d=0.0;
  for( i=0; i<10; i++ ) if(arr1[i]>d) d=arr1[i];
  b3=200.0;
  for(i=0; i<10; i++)  {
    b1 = r0 + r1*arr2[i] + r2*pow( arr2[i], 2 );
    b2 = (b1 - arr1[i]) * 100 / d;
    if(b2<0.0) b2=-b2;
    if(b3>b2) b3=b2;
  }
  *pog=(double)b3;
}
//----------------------------------------------------------------------------
void CGraphTemplate::CopyRaport(char *cbuf)
{
}
//----------------------------------------------------------------------------
void CGraphTemplate::OnPrint() 
{
  CPrintDialog cp(FALSE);             // выбираем принтер
  if (cp.DoModal()==IDOK)             // если выбрали
  {  
    CDC PrintDC;                // контекст принтера
    PrintDC.Attach(cp.GetPrinterDC());          // получаем контекст принтера
    DOCINFO infStru;              // структура с информацией
    ::ZeroMemory (&infStru, sizeof (DOCINFO));        // обнулить
    LPDEVMODE DM=cp.GetDevMode();
    int xs;
     if( DM->dmOrientation == DMORIENT_LANDSCAPE )
    { 
      xs=GetDeviceCaps(PrintDC,VERTRES);
      xs=xs/(bkgrnd.bottom);
    }
    else
    {
      xs=GetDeviceCaps(PrintDC,HORZRES);
      xs=xs/(bkgrnd.right);
    }
    CString Str;
    Str.LoadString(IDS_STRING_ST_NAS_CHAR);
    Str+=DiamGR.Tip;  
    CString title=Str;
    infStru.cbSize = sizeof (DOCINFO);          // заполняем структуру
    infStru.lpszDocName=title;
    infStru.lpszOutput=NULL;
    if (PrintDC.StartDoc(&infStru )==-1)AfxMessageBox("Error StartDoc") ; // Начало документа
    PrintDC.StartPage();              // начало страницы
    PrintDC.SetMapMode(MM_ANISOTROPIC);
    PrintDC.SetViewportExt(CSize(xs,xs));
    Draw(PrintDC);                  // рисуем
    if (PrintDC.EndPage()<0) AfxMessageBox("Error EndPage", MB_OK|MB_ICONINFORMATION);    // конец страницы
    if (PrintDC.EndDoc()==0) AfxMessageBox("Error EndDoc");   // конец документа
    PrintDC.Detach();             // отсоединить контекст
  } 
}
//----------------------------------------------------------------------------
void CGraphTemplate::OnCopy() 
{
  if ( !OpenClipboard() )
  {
    AfxMessageBox( _TR("Невозможно открыть буфер обмена"),MB_OK | MB_ICONSTOP);
    return;
  }
  if( !EmptyClipboard() )
  {
    AfxMessageBox(_TR("Невозможно очистить буфер обмена для записи в него"),MB_OK | MB_ICONSTOP);
    return;
  }
  if (gr_rap==0)
  {
    CMetaFileDC mfDC;
    mfDC.CreateEnhanced(NULL,NULL,NULL,NULL);
    Draw(mfDC);
    mf=mfDC.CloseEnhanced( );
    if ( ::SetClipboardData(CF_ENHMETAFILE,mf) == NULL )
      {
      AfxMessageBox( _TR("Невозможно скопировать данные в буфер обмена"),MB_OK | MB_ICONSTOP);
      CloseClipboard();
      return;
      }
  }
  else
  {
    HANDLE cbhdl = ::GlobalAlloc(GHND, 10000);
    char far* gbuf = (char far*)::GlobalLock(cbhdl);
    CopyRaport(gbuf);
    ::GlobalUnlock(cbhdl);
    if ( ::SetClipboardData(CF_TEXT,cbhdl) == NULL )
      {
      AfxMessageBox( _TR("Невозможно скопировать данные в буфер обмена"),MB_OK | MB_ICONSTOP);
      CloseClipboard();
      return;
      }
  }
    CloseClipboard();
}
//----------------------------------------------------------------------------
void CGraphTemplate::OnTofile() 
{
  // создание стандартной панели выбора файла SaveAs
  if (gr_rap==0)
  {
    CFileDialog DlgSaveAs(FALSE,(LPCSTR)"emf","graph.emf",
      OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
      (LPCSTR)_TR("Рисунки в виде метафайлов (*.emf) |*.emf||"));

    // отображение стандартной панели выбора файла SaveAs
    if(DlgSaveAs.DoModal()==IDOK)
    {
    CString file=DlgSaveAs.GetPathName();
    CMetaFileDC mfDC;
    mfDC.CreateEnhanced(NULL,file,NULL,NULL); 
    Draw(mfDC);
    mfDC.CloseEnhanced( );
    }
  }
  else
  {
    CFileDialog DlgSaveAs(FALSE,(LPCSTR)"txt","otchet.txt",
      OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
      (LPCSTR)_TR("Текстовые файлы (*.txt) |*.txt||"));

    // отображение стандартной панели выбора файла SaveAs
    if(DlgSaveAs.DoModal()==IDOK)
    {
    CStdioFile File(DlgSaveAs.GetPathName(),
        CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);

      // запись в файл строки
    HANDLE cbhdl = ::GlobalAlloc(GHND, 10000);
      char far* gbuf = (char far*)::GlobalLock(cbhdl);
      CopyRaport(gbuf);
      File.WriteString(gbuf);
      File.Close();


    }

  }
    
}
//----------------------------------------------------------------------------
void CGraphTemplate::SetTitle()
{
  CString Str;
  Str=GetCaptionStrt();
  Str+=DiamGR.Tip;
  Str+=" ( ";
  Str+=DiamGR.Name;
  Str+=" )";
  SetWindowText(Str);
}
//----------------------------------------------------------------------------
CString CGraphTemplate::GetCaptionStrt()
{
  CString Str;
  Str.LoadString(IDS_STRING_ST_NAS_CHARG);
  return Str; 
}
//----------------------------------------------------------------------------


BEGIN_MESSAGE_MAP(CGraphTemplate, CDialog)
  //{{AFX_MSG_MAP(CGraphTemplate)
//  ON_WM_CTLCOLOR()
  ON_WM_PAINT()
  ON_COMMAND(ID_TO_PRINT, OnPrint)
  ON_COMMAND(ID_TO_COPY, OnCopy)
  ON_COMMAND(ID_TO_FILE, OnTofile)  
  ON_BN_CLICKED(IDC_COPY_TO, OnCopyTo)
  ON_BN_DOUBLECLICKED(IDC_COPY_TO, OnCopyTo)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphTemplate message handlers
//----------------------------------------------------------------------------
//HBRUSH CGraphTemplate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
//{
//  return theApp.OnCtlColorDark(pDC,pWnd,nCtlColor);
//}
//----------------------------------------------------------------------------
BOOL CGraphTemplate::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  SetTitle();
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}
//----------------------------------------------------------------------------
void CGraphTemplate::OnPaint() 
{
  CPaintDC dc(this); // device context for painting
  Draw(dc);   
}
//----------------------------------------------------------------------------
void CGraphTemplate::Draw(CDC &dc)
{
  SetMyFont(dc);
  bkgrnd.left=(long)(lX-3*stepX);
  bkgrnd.top=(long)(lY-2*stepY);
  bkgrnd.right=(long)(rX+3*stepX);
  bkgrnd.bottom=(long)(rY+1.6*stepY);
  dc.FillRect(&bkgrnd,&CBrush( RGB( 255, 255, 255 ) ) );
  dc.SetTextColor( RGB(0,0,0)); 
  CString Str;
  Str.LoadString(IDS_GRAPHR_TYPE);
  dc.TextOut(llx+55,lly+5, Str );
  dc.TextOut(llx+145,lly+5, DiamGR.Tip );
}
//----------------------------------------------------------------------------
void CGraphTemplate::PostNcDestroy() 
{
  
  font->DeleteObject();
  DeleteEnhMetaFile(mf);
  CDialog::PostNcDestroy();
}
//----------------------------------------------------------------------------
void CGraphTemplate::OnCopyTo() 
{
  CRect re;
  CPoint poi;
  m_copy_to.GetWindowRect(&re);
  poi.x=re.right;
  poi.y=re.top;

  /*    CCoolMenu menu;   
    menu.InitMenu(IDB_BITMAP1);
    menu.LoadMenu(IDR_PROP);
    menu.TrackPopupMenu(
            TPM_LEFTALIGN|TPM_LEFTBUTTON,
            re.left+x,re.top+y, this);  
            menu.GetSubMenu(0)->ModifyMenu(1,MF_BYPOSITION|MF_OWNERDRAW);
    menu.GetSubMenu(1)->CheckMenuItem(ID_TO_PRINT,MF_GRAYED | MF_OWNERDRAW);
*/    

  CMenu menu;
  menu.LoadMenu(IDR_PROP);
  menu.GetSubMenu(1)->TrackPopupMenu(
          TPM_LEFTALIGN|TPM_LEFTBUTTON,
          poi.x,poi.y, this);   
}
//----------------------------------------------------------------------------
void CGraphTemplate::DrawChart(CDC &dc,TTypeChart type,APPROX_PARAM * apr_par,BOOL prepared)
{
  char buf[100];
  int i;
  CPen * _pen=NULL;
  CPen * def_pen=NULL;
  if (!prepared)
  {
    dc.SetTextColor(_basic_color); //подписи 
    //dc.TextOut( lX-stepX,(long)(lY-1.5*stepY), _TR("Тип насоса :") );
    //dc.TextOut( lX+50,(long)(lY-1.5*stepY), DiamGR.Tip );
      minX = minY = masX = masY = 0;  
    
    maxX = FindMax(DiamGR.Q);   
    MasSh( minX, maxX, nGridX, &masX );
    CRect space;
    space.left=(long)(lX+(double)DiamGR.Q_min/((double)masX/(stepX) ));
    space.top=lY;
    space.right=(long)(lX+(double)DiamGR.Q_max/((double)masX/(stepX) ));
    space.bottom=rY;
    dc.FillRect(&space,&CBrush(_rab_zone_color));
    int i, n = 0;
    CPen p1(PS_SOLID,1,_setka_color);
    dc.SelectObject( &p1 );
    for( i = 0; i<nGridY; i++ ) // сетка
    {
      n = ( i % 2 ) * 5;
      if( i < nGridX )
      {
       dc.MoveTo( lX+stepX+(stepX*i), lY ); dc.LineTo( lX+stepX+(stepX*i), rY+n );
      }
      dc.MoveTo( lX-n, rY-stepY-(stepY*i) );  dc.LineTo( rX+n, rY-stepY-(stepY*i) );
    }
    dc.SetTextColor(_basic_color); //подписи по оси X
    for( i = 0; i<10; i++ ) 
    {
    sprintf( buf, "%4lg", i*masX*2 );
    dc.TextOut( lX-stepX/2+(stepX*2*i), rY+stepY/2, buf );
    }
    dc.TextOut( rX,(long)(rY+stepY/2), "Q, т/ч" );  
  }
  switch (type)
  {
  case H_chart :
    {
      maxY = FindMax(DiamGR.H);
      MasSh( minY, maxY, nGridY-10+1, &masY );
      dc.SetTextColor(_HQ_color);
      for( i = 0; i<8; i++ )  //Значения по оси Н
      {
        sprintf( buf, "%4g", i*masY*2 );
        dc.TextOut( lX-35, rY-stepY/2-(stepY*2*i), buf );
      }
      dc.TextOut((long)(lX-1.5*stepX), rY-16*stepY, "H, м"); //подпись по оси H

      Appr( apr_par->r0 , apr_par->r1 , apr_par->r2 , DiamGR.H, DiamGR.Q, &apr_par->pog );
      ChahgeDrawParams(type,apr_par,_pen);
      def_pen=dc.SelectObject(_pen);
      DrawGraph( apr_par->r0,apr_par->r1, apr_par->r2, &dc, lX, rY );
      //DrawGraphLine(DiamGR.H,DiamGR.Q, &dc, lX, rY );
      break;
    }
  case N_chart :
    {
      maxY=FindMax( DiamGR.N );
      MasSh( minY, maxY, nGridY/2, &masY );//Масштабирование п оои N 
      dc.SetTextColor(_NQ_color); //Значения по оси N
      for( i = 0; i<6; i++ ) 
      {
        sprintf( buf, "%4g", i*masY*2 );
        dc.TextOut( rX+7, rY-stepY/2-(stepY*2*i), buf );
      }
      dc.TextOut( rX+3,(long)( rY-11.9*stepY), "N, кВт");
      
      Appr( apr_par->r0 , apr_par->r1 , apr_par->r2 , DiamGR.N, DiamGR.Q, &apr_par->pog  );
      ChahgeDrawParams(type,apr_par,_pen);
      def_pen=dc.SelectObject(_pen);
      DrawGraph( apr_par->r0 , apr_par->r1 , apr_par->r2 , &dc, lX, rY );     
      //DrawGraphLine(DiamGR.N,DiamGR.Q, &dc, lX, rY );
      break;
    }
  case KPD_chart :
    {
      masY = 10;
      //maxY=FindMax( DiamGR.KPD );
      //MasSh( minY, maxY, 10, &masY );//Масштабирование п оои N 
      dc.SetTextColor(_KPDQ_color);
      for( i = 0; i<=5; i++ ) 
      {
        sprintf( buf, "%3d", i*20 );
        dc.TextOut( rX+7, rY-stepY*14-stepY/2-(stepY*2*i), buf );
      }
      dc.SetTextColor( _KPDQ_color );
      dc.TextOut( rX+3,(long)(lY-1.5*stepY), _TR("КПД, %"));
        
      Appr( apr_par->r0 , apr_par->r1 , apr_par->r2 ,DiamGR.KPD, DiamGR.Q, &apr_par->pog  );
      
      ChahgeDrawParams(type,apr_par,_pen);
      def_pen=dc.SelectObject(_pen);
      DrawGraph( apr_par->r0 , apr_par->r1 , apr_par->r2 , &dc, lX, rY-stepY*14 );      
      //DrawGraphLine(DiamGR.KPD,DiamGR.Q, &dc, lX, rY-stepY*14 );
      break;
    }
  }
  dc.SelectObject(def_pen);
  if (_pen!=NULL) delete _pen;
}
//----------------------------------------------------------------------------
void CGraphTemplate::ChahgeDrawParams(TTypeChart type,APPROX_PARAM * apr_par,CPen * &_pen)
{
  switch (type)
  {
  case H_chart :
    {
      if (_pen==NULL) 
      {
        _pen=new CPen(PS_SOLID,3,_HQ_color);//График HQ         
      }
      break;
    }
  case N_chart :
    {
      if (_pen==NULL) 
      {
        _pen=new CPen(PS_SOLID,3,_NQ_color);//График NQ               
      }
      break;
    }
  case KPD_chart :
    {
      if (_pen==NULL) 
      {
        _pen=new CPen(PS_SOLID,3,_KPDQ_color);//График KPDQ
      }
      break;
    }
  }
}
//----------------------------------------------------------------------------
void CGraphTemplate::DrawEdge(CDC &dc)
{
  CPen p5(PS_SOLID,2,_basic_color);  //окантовка
  dc.SelectObject(&p5);
  dc.MoveTo( lX, lY );  dc.LineTo( lX, rY );
  dc.MoveTo( lX, rY );  dc.LineTo( rX, rY );
  dc.MoveTo( rX, rY-stepY*12 ); dc.LineTo( rX, rY );
  dc.MoveTo( rX, rY-stepY*14 ); dc.LineTo( rX, lY );
}
//----------------------------------------------------------------------------
void CGraphTemplate::inihq(double *n, double *mas, short kol )
{
  for( int j = 0; j < 10; j ++ )   mas[j] = ( *n ++ ) *kol;
}
//----------------------------------------------------------------------------

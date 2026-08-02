// CustomView.cpp : implementation file
//

#include "stdafx.h"
#include "CustomView.h"
#include "..\\resource.h" //Для курсора

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//addon #include "About\zoommake.h"
/////////////////////////////////////////////////////////////////////////////
// CCustomView
CCustomView::CCustomView()
{
  ////////////////////////////////////////////////////////////////
  ///ADD//////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  m_pDocument= new CConsrtuctorDoc(this);
  ////////////////////////////////////////////////////////////////
  ///ADD//////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////

  hGLRC=NULL;
  DebugMode=0;
  Zooming=0;
  ZoomPrepared=0;
  RubberGL.left=RubberGL.top=RubberGL.height=RubberGL.width;
  RubberRect.right=RubberRect.left;
  RubberRect.bottom=RubberRect.top;
  DotPen=NULL;
  DotPen=new CPen(PS_DOT,1,RGB(0,0,0));
  TxtElemPlace.Type=-1;
  TxtElemPlace.Number=-1;
  TxtShift[0]=0;
  TxtShift[1]=0;
  DraggingText=FALSE;
}
//----------------------------------------------------------------------------
CCustomView::~CCustomView()
{
  if (DotPen!=NULL) delete DotPen;
  if (m_pDocument!=NULL) delete m_pDocument;
}
//----------------------------------------------------------------------------
BOOL CCustomView::PreCreateWindow(CREATESTRUCT& cs) 
{
  cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
  return CTemplWnd::PreCreateWindow(cs);
}
//----------------------------------------------------------------------------
int CCustomView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CTemplWnd::OnCreate(lpCreateStruct) == -1) return -1;
  pdc = new CClientDC(this);
  
  if (SetWindowPixelFormat(pdc->m_hDC) == FALSE) return -1;
  hGLRC = wglCreateContext(pdc->m_hDC);
  if (hGLRC == NULL) return -1;
  if (wglMakeCurrent(pdc->m_hDC, hGLRC) == FALSE) return -1;

// glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
   glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

//  glCullFace(GL_BACK); 
//  glEnable(GL_CULL_FACE); 

//    glEnable(GL_BLEND);
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //float pos[4] = {10,20,40,3};//{3,3,3,1};
    //float dir[3] = {0,0,0}; //{-1,-1,-1};
    //glLightfv(GL_LIGHT0, GL_POSITION, pos);
  //  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
  float ambient[4] = {0.7,0.7, 0.7, 1};
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
   //glClearColor(0,0,0.5,1);
   
   old=AfxGetApp()->LoadStandardCursor( IDC_ARROW );
   //old=(HICON)GetClassLong(this->m_hWnd,GCL_HCURSOR);
  cr=AfxGetApp()->LoadCursor(IDC_MY_HAND);
  zoom_in=AfxGetApp()->LoadCursor(IDC_MY_ZOOM_IN);
  zoom_out=AfxGetApp()->LoadCursor(IDC_MY_ZOOM_OUT);
  cross=AfxGetApp()->LoadCursor(IDC_MY_CROSS);
  BuildFont(this->pdc->m_hDC,this->font_data,this->base,this->gmf, &this->font_data.lf);
  CompileList();
  StartMovie();

////  InitDisplay();
  CompileList();

  FitToScreen(); //Add/////////////
  return 0; 
}
//----------------------------------------------------------------------------
void CCustomView::OnDestroy() 
{
  CTemplWnd::OnDestroy();
  KillFont(this->base);
  if(wglGetCurrentContext()!=NULL)
  wglMakeCurrent(NULL, NULL) ;
  
  if(hGLRC!=NULL)
    {
      wglDeleteContext(hGLRC);
      hGLRC = NULL;
    }
  
    delete pdc;
  StopMovie();
  glDeleteLists(listik,0);  
}
//----------------------------------------------------------------------------
int CCustomView::SetWindowPixelFormat(HDC hDC)
{
    int m_GLPixelIndex;
    PIXELFORMATDESCRIPTOR pfd;


    pfd.nSize       = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion    = 1;

    pfd.dwFlags   = PFD_DRAW_TO_WINDOW | 
                    PFD_SUPPORT_OPENGL | 
                    PFD_DOUBLEBUFFER;

    pfd.iPixelType     = PFD_TYPE_RGBA;
    pfd.cColorBits     = 32;
    pfd.cRedBits       = 8;
    pfd.cRedShift      = 16;
    pfd.cGreenBits     = 8;
    pfd.cGreenShift    = 8;
    pfd.cBlueBits      = 8;
    pfd.cBlueShift     = 0;
    pfd.cAlphaBits     = 0;
    pfd.cAlphaShift    = 0;
    pfd.cAccumBits     = 64;    
    pfd.cAccumRedBits  = 16;
    pfd.cAccumGreenBits   = 16;
    pfd.cAccumBlueBits    = 16;
    pfd.cAccumAlphaBits   = 0;
    pfd.cDepthBits        = 32;
    pfd.cStencilBits      = 8;
    pfd.cAuxBuffers       = 0;
    pfd.iLayerType        = PFD_MAIN_PLANE;
    pfd.bReserved         = 0;
    pfd.dwLayerMask       = 0;
    pfd.dwVisibleMask     = 0;
    pfd.dwDamageMask      = 0;



    m_GLPixelIndex = ChoosePixelFormat( hDC, &pfd);
    if(m_GLPixelIndex==0) // Let's choose a default index.
    {
     m_GLPixelIndex = 1;    
     if(DescribePixelFormat(hDC,m_GLPixelIndex,sizeof(PIXELFORMATDESCRIPTOR),&pfd)==0)
       return 0;
    }


    if (SetPixelFormat( hDC, m_GLPixelIndex, &pfd)==FALSE)
        return 0;

    return 1;
}
//----------------------------------------------------------------------------
CConsrtuctorDoc* CCustomView::GetDocument() 
{
  return (CConsrtuctorDoc*)m_pDocument;
}
//----------------------------------------------------------------------------
void CCustomView::UpdateScrollSizes() 
{
  CSize sizeTotal(sc.CX,sc.CY) ;
  CSize sizePage(sc.Step*10,sc.Step*10) ;
  CSize sizeLine(sc.Step,sc.Step) ;
//  SetScrollSizes(MM_TEXT,sizeTotal,sizePage,sizeLine);//add 
}
BOOL CCustomView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{
  OnKeyDown(VK_ESCAPE,0,0);
//  return CTemplWnd::OnScrollBy(sizeScroll, bDoScroll);//add
  return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CCustomView diagnostics

#ifdef _DEBUG
void CCustomView::AssertValid() const
{
  CTemplWnd::AssertValid();
}

void CCustomView::Dump(CDumpContext& dc) const
{
  CTemplWnd::Dump(dc);
}
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CCustomView, CTemplWnd)
  //{{AFX_MSG_MAP(CConsrtuctorView)
  ON_WM_CREATE()
  ON_WM_TIMER()
  ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_WM_KEYDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCustomView message handlers
void CCustomView::StartMovie()
{
  uIDEvent=SetTimer(1,interval,0);
  element=0;
}
//----------------------------------------------------------------------------
void CCustomView::StopMovie()
{ 
  element=-1;
  KillTimer(uIDEvent);
}
//----------------------------------------------------------------------------
void CCustomView::OnInitialUpdate() 
{
  CSize sizeTotal(sc.CX,sc.CY) ;
//  SetScrollSizes(MM_TEXT,sizeTotal);//add
//  CTemplWnd::OnInitialUpdate(); //add
//  InitDisplay();

}
//----------------------------------------------------------------------------
void CCustomView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
//  CTemplWnd::OnPrepareDC(pDC, pInfo);
  origin=pDC->GetViewportOrg();
  if (origin!=tempOrigin)
  {
  InitDisplay();
  tempOrigin=origin;
  }
}
//----------------------------------------------------------------------------
void CCustomView::OnTimer(UINT nIDEvent) 
{
  if (element==0) display();
  CTemplWnd::OnTimer(nIDEvent);
}
//----------------------------------------------------------------------------
void CCustomView::display()
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glNormal3d(0,0,1);
  glColor3ub(sc.ForeColor.R,sc.ForeColor.G,sc.ForeColor.B);
  glCallList(listik); 
  GLfloat amb[4],ambient[4] = {0.0,0.0, 0.0, 1};
  glGetFloatv(GL_LIGHT_MODEL_AMBIENT,amb);    
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glColor3ub(sc.AnimColor.R,sc.AnimColor.G,sc.AnimColor.B);
  AnimElem(&m_pDoc->tubeV);
  AnimElem(&m_pDoc->batV);
  AnimElem(&m_pDoc->turnV);
  AnimElem(&m_pDoc->triV);
  AnimElem(&m_pDoc->ventV);
  AnimElem(&m_pDoc->nasV);
  AnimElem(&m_pDoc->drosV);
  AnimElem(&m_pDoc->airbatV);
  AnimElem(&m_pDoc->vodovodoV);
  AnimElem(&m_pDoc->soploV);
  AnimElem(&m_pDoc->connV);
  AnimElem(&m_pDoc->kranV);
  AnimElem(&m_pDoc->labV);
  AnimElem(&m_pDoc->extermV);
//  AnimElem(&m_pDoc->sopObjV);
  glFinish();
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
  SwapBuffers(wglGetCurrentDC());
}
//----------------------------------------------------------------------------
template <class V>
void CCustomView::AnimElem(V * vect)
{
  V::iterator itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
  itr->Animate();
  }
}
//----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CCustomView -- преобразование координат  
double CCustomView::getGLX(long x)
{
  if(rectClient.right!=rectClient.left) return (-sc.XL2+dx+sc.Wrect.width*x/(float)(rectClient.right-rectClient.left));
  else return 0;
}
//----------------------------------------------------------------------------
double CCustomView::getGLY(long y)
{
  if (rectClient.bottom!=rectClient.top) return (sc.YL2-dy-sc.Wrect.height*y/(float)(rectClient.bottom-rectClient.top));
  else return 0;
}
//-----------------------------------------------------------------------------------
long CCustomView::getX(double x)
{
  return (sc.XL2-dx+x)*(rectClient.right-rectClient.left)/(sc.Wrect.width);
}
//-----------------------------------------------------------------------------------
long CCustomView::getY(double y)
{
  return (sc.YL2-dy-y)*(rectClient.bottom-rectClient.top)/(sc.Wrect.height);
}
//-----------------------------------------------------------------------------------
// CCustomView -- преобразование координат  
/////////////////////////////////////////////////////////////////////////////
void CCustomView::DrawRect(CRect r)
{
 CClientDC dc(this);
 int oldMode=dc.SetROP2(R2_NOT);
 CBrush* oldBrush=(CBrush*)dc.SelectStockObject(NULL_BRUSH);
 CPen* oldPen = (CPen*) dc.SelectStockObject(BLACK_PEN);
 dc.Rectangle(r);
 dc.SelectObject(oldBrush);
 dc.SelectObject(oldPen);
 dc.SetROP2(oldMode);
}
//-----------------------------------------------------------------------------------
void CCustomView::OnFont()
{
  LOGFONT lf;
  memset(&lf, 0, sizeof(LOGFONT));
  CClientDC dc(this);
  lf=font_data.lf;
  CFontDialog dlg(&lf);
  dlg.m_cf.rgbColors=RGB(font_data.color.R,font_data.color.G,font_data.color.B);
  if (dlg.DoModal()==IDOK)
  {
    COLORREF  color= dlg.GetColor();
    memcpy(&lf, dlg.m_cf.lpLogFont, sizeof(LOGFONT));   
    BuildFont(pdc->m_hDC,this->font_data,this->base,this->gmf,&lf,&color);
  } 
  CompileList();
  Invalidate();
}
//-----------------------------------------------------------------------------------
void CCustomView::OnSize(UINT nType, int cx, int cy) 
{
  CTemplWnd::OnSize(nType, cx, cy);
  GetClientRect(&rectClient);
  glViewport(0,0,rectClient.right,rectClient.bottom);
  sc.Wrect.width=(rectClient.right)*2*sc.XL2/(float)sc.CX;
  sc.Wrect.height=(rectClient.bottom)*2*sc.YL2/(float)sc.CY;
  FitToScreen(); //Add/////////////
  InitDisplay();
}
//-----------------------------------------------------------------------------------
void CCustomView::InitDisplay()
{
  StopMovie();
  dx=(-origin.x)*2*sc.XL2/(float)sc.CX;
  dy=(-origin.y)*2*sc.YL2/(float)sc.CY;
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho(-sc.XL2+dx,-sc.XL2+dx+sc.Wrect.width,sc.YL2-dy-sc.Wrect.height,sc.YL2-dy,-1.4*deep,1.4*deep);  
//  glOrtho(-sc.XL2+dx,-sc.XL2+dx+sc.Wrect.width,sc.YL2-dy-sc.Wrect.height,sc.YL2-dy,-1.4*deep,1.4*deep);  
  gluLookAt( 0,0,5, 0,0,0, 0,1,0 );
  glMatrixMode( GL_MODELVIEW );

  GLdouble matrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, matrix);


  CompileList();
  StartMovie();
}
//-----------------------------------------------------------------------------------
void CCustomView::CopyScreen(CCustomView * TemplView)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  wglMakeCurrent(pdc->m_hDC, hGLRC);
  sc=TemplView->sc;
  NodeColor=TemplView->NodeColor;
  worksp=TemplView->worksp;
  CString Str=font_data.lf.lfFaceName;
  CString Str1=TemplView->font_data.lf.lfFaceName;
  if ( (Str!=Str1)||
     (font_data.lf.lfHeight!=TemplView->font_data.lf.lfHeight)||
     (font_data.lf.lfItalic!=TemplView->font_data.lf.lfItalic)||
     (font_data.lf.lfWeight!=TemplView->font_data.lf.lfWeight)||
     (font_data.lf.lfCharSet!=TemplView->font_data.lf.lfCharSet)||
     (font_data.lf.lfStrikeOut!=TemplView->font_data.lf.lfStrikeOut)||
     (font_data.lf.lfUnderline!=TemplView->font_data.lf.lfUnderline) )     
  {
    font_data=TemplView->font_data;
    COLORREF cl=RGB(font_data.color.R,font_data.color.G,font_data.color.B);
    BuildFont(pdc->m_hDC,font_data,base,gmf,&font_data.lf,&cl);
  }
  if  ( (font_data.color.R!=TemplView->font_data.color.R)||
     (font_data.color.G!=TemplView->font_data.color.G)||
     (font_data.color.B!=TemplView->font_data.color.B) )
  {
    font_data.color=TemplView->font_data.color;
  }
  CPoint per;
  per.x=-TemplView->origin.x;
  per.y=-TemplView->origin.y;
  OnSize(0,0,0);
  UpdateScrollSizes();
//  ScrollToPosition(per);  //add
  m_pDoc->RemakeConnection(this);
  ChangeDebugMode(0);
  ::SetClassLong(m_hWnd,GCL_HCURSOR,(long)old);   
  CompileList();

  
}
//-----------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CCustomView -- zooming 
void CCustomView::ZoomIn(const CPoint point)
{
  CRect CorrectedR;
  CorrectedR=RubberRect;
  CorrectRect(CorrectedR);
  if ( (ZoomPrepared)&&
     (point.x>=CorrectedR.left)&&
     (point.x<=CorrectedR.right)&&
     (point.y>=CorrectedR.top)&&
     (point.y<=CorrectedR.bottom) )//Внутри поля увеличения
  {
    ZoomPrepared=0;
    RubberGL.left=getGLX(CorrectedR.left);
    RubberGL.top=getGLY(CorrectedR.top);
    GetClientRect(&rectClient);
    if ((RubberGL.height>1)||(RubberGL.width>1))
    {
      sc.Wrect.height=RubberGL.height;
      sc.Wrect.width=RubberGL.width;
      Zoom_stack.push_back(sc.Step);
      sc.Step=(rectClient.right)/(sc.Wrect.width);      
      sc.CX=sc.Step*2*sc.XL2;
      sc.CY=sc.Step*2*sc.YL2;         
      UpdateScrollSizes();
      sc.Wrect.width=(rectClient.right)/(float)sc.Step;
      sc.Wrect.height=(rectClient.bottom)/(float)sc.Step;
      InitDisplay();          
      CPoint pos=0;
      GetClientRect(&rectClient);
      if (sc.CX>rectClient.right)pos.x=getX(sc.XL2+RubberGL.left)-getX(0);
      if (sc.CY>rectClient.bottom)pos.y=-getY(sc.YL2-RubberGL.top)+getY(0);
//      ScrollToPosition(pos);  //add
    }
    else 
    {
      AfxMessageBox("Выбранное приближение больше максимального\nПоэтому оно не может быть достигнуто", MB_OK);       
    }   
    RubberRect.right=RubberRect.left;
    RubberRect.bottom=RubberRect.top;
    element=0;
    ::SetClassLong(m_hWnd,GCL_HCURSOR,(long)cross);
  }
  else
  {
    SetCapture();
    DrawRubberRect(RubberRect);
    RubberGL.left=getGLX(point.x);
    RubberGL.top=getGLY(point.y);
    RubberRect.left=getX(RubberGL.left);
    RubberRect.top=getY(RubberGL.top);      
    RubberRect.right=RubberRect.left;
    RubberRect.bottom=RubberRect.top;
    Zooming=1;
    element=-1;
  }   
}
//-----------------------------------------------------------------------------------
void CCustomView::ZoomOut(const CPoint point)
{
  RubberRect.right=RubberRect.left; //"резиновый" прямоугольник выделения в оконных координатах
  RubberRect.bottom=RubberRect.top; //"резиновый" прямоугольник выделения в оконных координатах
  RubberGL.height=RubberGL.width=0;
  int step=0;
  if (!Zoom_stack.empty())
  {
    step=Zoom_stack.back();
    Zoom_stack.pop_back();
  }
  else
  {
    step=sc.Step-0.2*sc.OriginalStep;
  }
  if (step>1) ZoomToStepPoint(step,point);        
  else
  {
    AfxMessageBox("Выбранное отдаление больше максимального\nПоэтому оно не может быть достигнуто", MB_OK);
  }   
}
//-----------------------------------------------------------------------------------
void CCustomView::CorrectRect(CRect &Rectangle)
{
  long min=Min(Rectangle.left,Rectangle.right);
  if (min!=Rectangle.left)
  {
    Rectangle.right=Rectangle.left;
    Rectangle.left=min;
  }
  min=Min(RubberRect.top,RubberRect.bottom);
  if (min!=Rectangle.top)
  {
    Rectangle.bottom=Rectangle.top;
    Rectangle.top=min;
  }
}
//-----------------------------------------------------------------------------------
void CCustomView::ZoomCustom()
{
  long minZoom,maxZoom;
  double zoom;
  long maxLenth;
  zoom=100*sc.Step/(double)sc.OriginalStep;
  minZoom=100*2/(double)sc.OriginalStep;
  if (sc.CX>sc.CY)maxLenth=sc.CX;
  else maxLenth=sc.CY;
  maxZoom=100*maxLenth/(double)sc.OriginalStep;
  CPoint point;
  VECTORXY dot;
  dot.X=sc.Wrect.left+sc.Wrect.width/2.0;
  dot.Y=sc.Wrect.top-sc.Wrect.height/2.0;
  point.x=(long)dot.X;
  point.y=(long)dot.Y;
/*  CZoomMake zm(zoom,minZoom,maxZoom,sc.OriginalStep,point);
  if (zm.DoModal()==IDOK)
  {
    long step;
    if (zm.m_check_z)
    {
    point.x=getX(zm.coord.x);//zm.m_x0.GetValue());
    point.y=getY(zm.coord.y);//zm.m_y0.GetValue());
    }
    else
    {
      point.x=getX(dot.X);
      point.y=getY(dot.Y);
    }
    step=sc.OriginalStep*zm.mash/(double)100;
    if ( (zm.mash<=zm.max)||(zm.mash>=zm.min) )
    { 
      ZoomToStepPoint(step,point);      
    }
    else
    {
      AfxMessageBox("Выбранный масштаб не может быть достигнут", MB_OK);
    }   
  }
  */
}
//-----------------------------------------------------------------------------------
void CCustomView::ZoomToStepPoint(long step,CPoint point)
{
  sc.Wrect.left=getGLX(point.x);
  sc.Wrect.top=getGLY(point.y);
  sc.Step=step;
  GetClientRect(&rectClient);
  sc.Wrect.width=(rectClient.right)/(float)sc.Step;
  sc.Wrect.height=(rectClient.bottom)/(float)sc.Step;
  sc.Wrect.left-=sc.Wrect.width/2.0;
  sc.Wrect.top+=sc.Wrect.height/2.0;
  sc.CX=sc.Step*2*sc.XL2;
  sc.CY=sc.Step*2*sc.YL2;         
  UpdateScrollSizes();
  sc.Wrect.width=(rectClient.right)/(float)sc.Step;
  sc.Wrect.height=(rectClient.bottom)/(float)sc.Step;
  InitDisplay();      
  CPoint pos=0;
  GetClientRect(&rectClient);
  if (sc.CX>rectClient.right)pos.x=getX(sc.XL2+sc.Wrect.left)-getX(0);
  if (sc.CY>rectClient.bottom)pos.y=-getY(sc.YL2-sc.Wrect.top)+getY(0);
//  ScrollToPosition(pos);    //add
}
//-----------------------------------------------------------------------------------
void CCustomView::DrawRubberRect(CRect RubberRect)
{
 CClientDC dc(this);
 int oldMode=dc.SetROP2(R2_NOTXORPEN);
 CPen* oldPen = (CPen*) dc.SelectObject(DotPen);
 CBrush* oldBrush=(CBrush*)dc.SelectStockObject(NULL_BRUSH); 
 dc.Rectangle(RubberRect);
 dc.SelectObject(oldBrush);
 dc.SelectObject(oldPen);
 dc.SetROP2(oldMode); 
}
//-----------------------------------------------------------------------------------
void CCustomView::OnZoomInMouseUP()
{
  CRect CorrectedR;
  CorrectedR=RubberRect;
  CorrectRect(CorrectedR);
  if (Zooming) ZoomPrepared=1;
  Zooming=0;
  ReleaseCapture();
}
//-----------------------------------------------------------------------------------
void CCustomView::OnZoomInMouseMove(CPoint point)
{
  if (Zooming)
  {
    DrawRubberRect(RubberRect);
    //слежение за четностью области 
    RubberGL.width=getGLX(point.x)-getGLX(RubberRect.left);
    if (RubberGL.width<0)RubberGL.width=-RubberGL.width;        
    RubberGL.height=RubberGL.width*sc.Wrect.height/sc.Wrect.width;
    if (point.x>RubberRect.left)RubberRect.right=RubberRect.left+getX(RubberGL.width)-getX(0);
    else RubberRect.right=RubberRect.left-getX(RubberGL.width)+getX(0);
    if (point.y>RubberRect.top)RubberRect.bottom=RubberRect.top-getY(RubberGL.height)+getY(0);
    else RubberRect.bottom=RubberRect.top+getY(RubberGL.height)-getY(0);
    DrawRubberRect(RubberRect);
  }
    
  if (ZoomPrepared)
  {
    CRect CorrectedR;
    CorrectedR=RubberRect;
    CorrectRect(CorrectedR);
    if ((point.x>=CorrectedR.left)&&(point.x<=CorrectedR.right)&&
      (point.y>=CorrectedR.top)&&(point.y<=CorrectedR.bottom))
    {
      ::SetClassLong(m_hWnd,GCL_HCURSOR,(long)zoom_in);
    }
    else ::SetClassLong(m_hWnd,GCL_HCURSOR,(long)cross);
  } 
}
//-----------------------------------------------------------------------------------
void CCustomView::FitToScreen()
{
  GetClientRect(&rectClient);
  glViewport(0,0,rectClient.right,rectClient.bottom);
  double step1, step2;
  sc.CX=rectClient.right;
  sc.CY=rectClient.bottom;
  step1=sc.CX/(2.0*sc.XL2);
  step2=sc.CY/(2.0*sc.YL2);
  if (step1<=step2)step2=step1;
  sc.CX=step2*2*sc.XL2;
  sc.CY=step2*2*sc.YL2;
  //sc.CX=sc.OriginalStep*2*sc.XL2;
  //sc.CY=sc.OriginalStep*2*sc.YL2; 
  sc.Wrect.width=(rectClient.right)*2*sc.XL2/(float)sc.CX;
  sc.Wrect.height=(rectClient.bottom)*2*sc.YL2/(float)sc.CY;
  InitDisplay();
  UpdateScrollSizes();
}
//-----------------------------------------------------------------------------------
// CCustomView -- zooming 
/////////////////////////////////////////////////////////////////////////////
void CCustomView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  // TODO: Add your message handler code here and/or call default
  if (nChar==VK_ESCAPE)
    {
    element=0;
    DebugMode=0;
    Zooming=0;
    ZoomPrepared=0;
//ЧТОБЫ МОЖНО БЫЛО СКРОЛЛИТЬ  ReleaseCapture(); 
    RubberRect.right=RubberRect.left; //"резиновый" прямоугольник выделения в оконных координатах
    RubberRect.bottom=RubberRect.top; //"резиновый" прямоугольник выделения в оконных координатах
    RubberGL.height=RubberGL.width=0;
    ::SetClassLong(this->m_hWnd,GCL_HCURSOR,(long)old);   
    }
  CTemplWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
//-----------------------------------------------------------------------------------

bool CCustomView::SwitchShowFont()
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return false;
  worksp.ShowFont=!worksp.ShowFont;   
  ShowFont(&m_pDoc->tubeV,worksp.ShowFont); 
  ShowFont(&m_pDoc->connV,worksp.ShowFont); 
  ShowFont(&m_pDoc->extermV,worksp.ShowFont); 
  CompileList();
  return worksp.ShowFont;
}
//-----------------------------------------------------------------------------------
void CCustomView::ShowFont(bool show)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  worksp.ShowFont=show;   
  ShowFont(&m_pDoc->tubeV,worksp.ShowFont); 
  ShowFont(&m_pDoc->connV,worksp.ShowFont); 
  ShowFont(&m_pDoc->extermV,worksp.ShowFont); 
  CompileList();  
}
//-----------------------------------------------------------------------------------
void CCustomView::ShowBalloons(bool show)
{
  worksp.ShowBalloons=show;   
}
//-----------------------------------------------------------------------------------
template <class V>
void CCustomView::ShowFont(V * vect,bool show)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  V::iterator  itr;
  for (itr=vect->begin();itr!=vect->end();itr++)
  {
    if (itr->HasText())
    {
      itr->ShowText(show);
      duo_selected=m_pDoc->GetDuo(&*itr);     

      if (show)SendData(ONLY_SHOW_TEXT);  
    }
  } 
}
//-----------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CCustomView -- перетаскивание под-элементов
//-----------------------------------------------------------------------------------
bool CCustomView::MouseOnSub(bool with_switch)
{
  CConsrtuctorDoc * m_pDoc=GetDocument();
  CTempl * loop=NULL;
  if (MouseOnSubDevice(&m_pDoc->tubeV,loop)) {StartDragText(loop);return true;}
  if (MouseOnSubDevice(&m_pDoc->connV,loop)) {StartDragText(loop);return true;}
  if (MouseOnSubDevice(&m_pDoc->extermV,loop)) {StartDragText(loop);return true;}
  if (!with_switch) return false;
  if (MouseOnSubDevice(&m_pDoc->ventV,loop)) {StartDragText(loop);return true;}
  if (MouseOnSubDevice(&m_pDoc->nasV,loop)) {StartDragText(loop);return true;}
  if (MouseOnSubDevice(&m_pDoc->airbatV,loop)) {StartDragText(loop);return true;}
  if (MouseOnSubDevice(&m_pDoc->kranV,loop)) {StartDragText(loop);return true;} 
  return false;
}
//-----------------------------------------------------------------------------------
template <class V>
bool CCustomView::MouseOnSubDevice(V * vect,CTempl * &found)
{
  found=NULL;
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return false;
  V::iterator loop;
  for (loop=vect->begin();loop!=vect->end();loop++)
  {
  if (loop->PointInTxtRgn(&m))
    {
      if (loop->HasSubDevice())
      {
        if (loop->GetShowText())
        {
          ::SetClassLong(this->m_hWnd,GCL_HCURSOR,(long)cr);
          found = (CTempl*)&*loop;
          return 1;
        }
      }
    }
  }
  ::SetClassLong(this->m_hWnd,GCL_HCURSOR,(long)old);
  return 0;
}
//-----------------------------------------------------------------------------------
void CCustomView::StartDragText(CTempl * loop)      
{ 
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  RECTAN rett=loop->GetTxtBounds();
  Txtrect.left=getX(rett.left);
  Txtrect.top=getY(rett.top);
  Txtrect.right=Txtrect.left+getX(rett.width)-getX(0);
  Txtrect.bottom=Txtrect.top-getY(rett.height)+getY(0);
  DrawRect(Txtrect);
  TxtShift[0] = m.X-rett.left;
  TxtShift[1] = rett.top-m.Y; 
  TxtElemPlace=(m_pDoc->GetDuo(loop)).place;        
  DraggingText=TRUE;
  StopMovie();
}
//-----------------------------------------------------------------------------------
void CCustomView::EndDragText()     
{ 
  CConsrtuctorDoc * m_pDoc=GetDocument();
  if (m_pDoc==NULL) return;
  if (DraggingText)
  {
    DraggingText=FALSE;
    CDuo dd;
    dd.first=dd.second=-1;
    dd.place=TxtElemPlace;
    CTempl* el=m_pDoc->GetTempl(dd);
    if (el!=NULL)
    {
      RECTAN rett=el->GetTxtBounds();
      rett.left=getGLX(Txtrect.left);
      rett.top=getGLY(Txtrect.top);
      el->MoveText(&rett);
      CompileList();
    }   
    StartMovie();
  }
}
//-----------------------------------------------------------------------------------
void CCustomView::MoveDrag(CPoint point)
{
  if (DraggingText)
  {
    CRgn Reg;
    GetClientRect(&rectClient);   
    #define zazor 10
    CRect Client; 
    Client.left=rectClient.left+zazor;
    Client.top=rectClient.top+zazor;
    Client.bottom=rectClient.bottom-zazor;
    Client.right=rectClient.right-zazor;
    Reg.CreateRectRgnIndirect(&Client);
    if(!Reg.PtInRegion(point))
    {
      DrawRect(Txtrect);
      DrawRect(Txtrect);
      return;
    }
    else 
    {
      DrawRect(Txtrect);
      long w,h;
      w=Txtrect.Width();
      h=Txtrect.Height();
      Txtrect.left=getX(long(m.X-TxtShift[0]));
        //point.x-getX(TxtShift[0])+getX(0);      
      Txtrect.top=getY(long(m.Y+TxtShift[1]));        
      //point.y+getY(TxtShift[1])-getY(0);
      
      Txtrect.bottom=Txtrect.top+h;
      Txtrect.right=Txtrect.left+w;
      
      DrawRect(Txtrect);
    }   
  }
}
//-----------------------------------------------------------------------------------
// CCustomView -- перетаскивание под-элементов
/////////////////////////////////////////////////////////////////////////////
void CCustomView::ChangeDebugMode(unsigned char newDebugMode)
{
  if (DebugMode==newDebugMode) return;
  OnKeyDown(VK_ESCAPE,0,0);
  DebugMode=newDebugMode;
} 
//-----------------------------------------------------------------------------------
